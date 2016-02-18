#include <gtk/gtk.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <map>

using namespace std;

// prototipi delle funzioni per il disegno a mano libera
static gboolean draw_cb (GtkWidget *widget, cairo_t   *cr, gpointer   data);
static void draw_brush (GtkWidget *widget, gdouble    x, gdouble    y);
static gboolean button_press_event_cb (GtkWidget      *widget, GdkEventButton *event, gpointer        data);
static gboolean motion_notify_event_cb (GtkWidget      *widget, GdkEventMotion *event, gpointer        data);
static gboolean configure_event_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
static void clear_surface (void);
static void clear_surface_btn (void);

//prototipi delle funzioni per il funzionamento di GTK
static void activate (GtkApplication *app, gpointer user_data);
static void close_window (void);
static void populateComboBox();

//prototipi delle funzioni per il riconoscimento della lettera
static gboolean readLetterEnter(GtkWidget *widget, GdkEventKey *event);  
static void readLetter(void);


//dichiarazione dei widget GKT globali
GtkWidget *window;
GtkWidget *container;
GtkWidget *drawing_area;
GtkWidget *read_btn;
GtkWidget *clear_btn;
GtkWidget *letterThumbnail;
GdkPixbuf *letterThumbnailPB;
GtkWidget *letterLbl;
GtkWidget *comboBox;
GtkWidget *resFrame;
GtkWidget *testFrame;
GtkWidget *percFrame;
GtkWidget *percScrollW;
GtkWidget *percList;
GtkWidget *percLbl[26];
GtkWidget *percLevelBar[26];

GdkColor greyColor, blackColor;

// area di disegno 
static cairo_surface_t *surface = NULL;

// root dir del progetto
string root = "";

int main (int argc, char **argv)
{

  //si ricava il path da dove è stato lanciato il programma
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  root = string( result, (count > 0) ? count : 0 );
  root=root.substr(0,root.find_last_of("/\\")+1);
  root = root.substr( 0, root.length()-5);

  GtkApplication *app;
  int status;

  //vengono instanziati tutti i widget gtk
  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}


static void activate (GtkApplication *app, gpointer user_data)
{

  //istanza della finestra
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Real Time Test:");
  gtk_window_set_resizable(GTK_WINDOW(window), 0);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width (GTK_CONTAINER (window), 0);
  g_signal_connect (window, "destroy", G_CALLBACK (close_window), NULL);
  g_signal_connect(window, "key-release-event", G_CALLBACK(readLetterEnter), NULL);

  //container che permette la visualizzazione di più widget in contemporanea
  container = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(window), container);
  gtk_widget_set_size_request(container, 861, 640); 

  //specifiche per il font del bottone READ
  PangoFontDescription *font_desc;
  font_desc = pango_font_description_from_string ("Sans");
  pango_font_description_set_size(font_desc,14*PANGO_SCALE);

  //bottone READ per leggere l'immagine
  read_btn = gtk_button_new_with_label("READ");
  gtk_widget_set_size_request(read_btn, 175, 50);
  gtk_fixed_put(GTK_FIXED(container), read_btn, 663, 175);
  gtk_widget_modify_font(GTK_WIDGET(read_btn), font_desc);
  g_signal_connect(G_OBJECT(read_btn), "clicked", G_CALLBACK(readLetter), NULL);

  //frame (contenitore con bordi) per contenere la label risultante
  resFrame = gtk_frame_new("");
  gtk_widget_set_size_request(resFrame, 64, 72);
  gtk_fixed_put(GTK_FIXED(container), resFrame, 757, 93);

  //lettera calcolata dalla rete
  letterLbl = gtk_label_new ("");
  gtk_container_add(GTK_CONTAINER(resFrame), letterLbl);
  gtk_widget_modify_font (letterLbl,  pango_font_description_from_string ("Monospace 35"));

  //bottone per pulire l' area di disegno
  clear_btn = gtk_button_new_with_label("clear"); 
  gtk_widget_set_size_request(clear_btn, 125, 25);
  gtk_fixed_put(GTK_FIXED(container), clear_btn, 688, 610);
  g_signal_connect(G_OBJECT(clear_btn), "clicked", G_CALLBACK(clear_surface_btn), NULL);

  //menù a tendina per scegliere le network
  comboBox = gtk_combo_box_text_new();
  populateComboBox();
  gtk_widget_set_size_request(comboBox, 207, 25);
  gtk_fixed_put(GTK_FIXED(container), comboBox, 647, 10);

  //frame per contenere le info sui neuroni di output
  percFrame = gtk_frame_new("Activation (%)");
  gtk_widget_set_size_request(percFrame, 207, 225);
  gtk_fixed_put(GTK_FIXED(container), percFrame, 647, 300);

  //contenitore che rende la lista dei neuroni di output scrollabile
  percScrollW = gtk_scrolled_window_new(NULL,NULL);
  gtk_widget_set_margin_top(percScrollW,10);
  gtk_widget_set_margin_bottom(percScrollW,5);
  gtk_widget_set_size_request(percScrollW, 207, 225);
  gtk_container_add(GTK_CONTAINER(percFrame), percScrollW);

  //lista (2 colonne e 26 righe) dei neuroni di output
  percList = gtk_flow_box_new();
  gtk_widget_set_size_request(percList, 190, 225);
  gtk_container_add(GTK_CONTAINER(percScrollW), percList);
  gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(percList),2);
  gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(percList),2);

  //la prima colonna della lista viene riempita da label  
  int llb=0;
  for(int l=0;l<26;l++)
  { 
    percLbl[l] = gtk_label_new ("");
    gtk_flow_box_insert(GTK_FLOW_BOX(percList),percLbl[l],llb);
    gtk_widget_modify_font (percLbl[l],  pango_font_description_from_string ("Monospace 10"));
    gtk_widget_set_size_request(percLbl[l], 45, 5);
    llb+=2;
  }

  //la seconda colonna della lista va riempita da level bar (barre di progresso)
  llb=1;
  for(int lb=0;lb<26;lb++)
  { 
    percLevelBar[lb] = gtk_level_bar_new_for_interval(0,100);
    gtk_flow_box_insert(GTK_FLOW_BOX(percList),percLevelBar[lb],llb);
    gtk_widget_set_size_request(percLevelBar[lb], 125, 5);
    llb+=2;

  }

  //istanza dell' area di disegno
  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, 640, 640);
  gtk_container_add (GTK_CONTAINER (container), drawing_area);
  g_signal_connect (drawing_area, "draw", G_CALLBACK (draw_cb), NULL);
  g_signal_connect (drawing_area,"configure-event", G_CALLBACK (configure_event_cb), NULL);
  g_signal_connect (drawing_area, "motion-notify-event", G_CALLBACK (motion_notify_event_cb), NULL);
  g_signal_connect (drawing_area, "button-press-event", G_CALLBACK (button_press_event_cb), NULL);
  gtk_widget_set_events (drawing_area, gtk_widget_get_events (drawing_area) | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);


  gtk_widget_show_all(window);
  gtk_widget_hide(resFrame);
  gtk_widget_hide(percFrame);
}


// disegna un quadrato nella posizione specificata
static void draw_brush (GtkWidget *widget, gdouble    x, gdouble    y)
{

  cairo_t *cr;

  // creazione del quadrato da 64 pixel
  cr = cairo_create (surface);
  cairo_rectangle (cr, x-32, y-32, 64 , 64);
  cairo_fill (cr);
  cairo_destroy (cr);

  //viene aggiornata l'area di disegno
  gtk_widget_queue_draw_area (widget, x-32, y-32, 64 , 64);
}

//evento che intercetta la pressione dei tasti del mouse
static gboolean button_press_event_cb (GtkWidget *widget, GdkEventButton *event, gpointer data)
{

  if (surface == NULL)
    return FALSE;

  // se il bottone premuto è il sinistro allora va disegnato il quadrato
  if (event->button == GDK_BUTTON_PRIMARY)
  {
    draw_brush (widget, event->x, event->y);    
  }

  // se il bottone premuto è il destro allora va pulita la superficie
  else if (event->button == GDK_BUTTON_SECONDARY)
  {
    clear_surface ();
    gtk_widget_queue_draw (widget);
  }

  return TRUE;
}

//gestione dello spostamento del mouse mentre si preme il bottone
static gboolean motion_notify_event_cb (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{

  if (surface == NULL)
    return FALSE;

  //se il bottone è il sinistro allora va disegnato il quadrato
  if (event->state & GDK_BUTTON1_MASK)
  {
    draw_brush (widget, event->x, event->y);      
  }

  return TRUE;
}


//funzione per pulire l' area di disegno
static void clear_surface (void)
{
  cairo_t *cr;

  //viene creata una nuova superficie che sovrascrive quella vecchia "sporca"
  cr = cairo_create (surface);

  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);

  cairo_destroy (cr);
}

//funzione da bindare al bottone CLEAR per pulire la superficie
static void clear_surface_btn (void)
{
  clear_surface ();
  gtk_widget_queue_draw (drawing_area);
}


// creazione della area di disegno
static gboolean configure_event_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  if (surface)
    cairo_surface_destroy (surface);

  //istanza della nuova area di disegno
  surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget), CAIRO_CONTENT_COLOR, gtk_widget_get_allocated_width (widget), gtk_widget_get_allocated_height (widget));

  // la superficie viene pulita
  clear_surface ();

  return TRUE;
}

//disegna sulla superficie
static gboolean draw_cb (GtkWidget *widget, cairo_t   *cr, gpointer   data)
{
  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);

  return FALSE;
}

//evento di chiusura della finestra
static void close_window (void)
{
  //distrugge la superficie di disegno
  if (surface)
    cairo_surface_destroy (surface);

  //se esistono i file temporanei questi vengono eliminati
  if(access( (root + "/tmp/realTimeTestLetter.txt").c_str(), F_OK ) != -1)
    remove((root + "/tmp/realTimeTestLetter.txt").c_str());

  if(access( (root + "/tmp/realTimeTestLetter.png").c_str(), F_OK ) != -1)
    remove((root + "/tmp/realTimeTestLetter.png").c_str());

  if(access( (root + "/tmp/FANNresult.txt").c_str(), F_OK ) != -1)
    remove((root + "/tmp/FANNresult.txt").c_str());

  gtk_main_quit ();
}


//funzione per la pressione del tasto invio (viene richiamata la funzione per leggere la lettera)
static gboolean readLetterEnter(GtkWidget *widget, GdkEventKey *event)
{
  // viene controllato che il tasto premuto è invio
  if(!strcmp(gdk_keyval_name (event->keyval),"Return"))
    readLetter();

  return FALSE;
}

//funzione che legge la lettera disegnata a mano
static void readLetter(void)
{

  //salva l' immagine in png dall' area di disegno
  cairo_surface_t *finalSurf = surface;
  cairo_surface_write_to_png (finalSurf, (root + "/tmp/realTimeTestLetter.png").c_str());

  //l' eseguibile java codifica l' immagine nel file txt
  //(viene richiamato java in quanto rende molto più semplici le manipolazioni delle immagini)
  system(("java -jar " + root + "/bin/HealInput.jar").c_str());

  //prima di proseguire viene controllata l'esistenza del file
  while( access( (root + "/tmp/realTimeTestLetter.txt").c_str(), F_OK ) == -1 ) 
    usleep(100000); //aspetta che il file venga creato

  //viene controllato che l' immagine non sia vuota 
  string chkImg;
  ifstream chkfile( (root + "/tmp/realTimeTestLetter.txt").c_str() , ios::in );
  if ( chkfile.is_open( ) )
    getline( chkfile , chkImg );
  chkfile.close( );
  //se l'immagine è vuota allora nel file è presente solo il valore -1, quindi la funzione viene interrotta
  if(chkImg=="-1")
    return;    

  //viene preparata una thumbnail dell' immagine originale
  letterThumbnailPB = gdk_pixbuf_new_from_file_at_size ((root + "/tmp/realTimeTestLetter.png").c_str(), 64, 64, NULL);
  letterThumbnail = gtk_image_new_from_pixbuf(letterThumbnailPB);
  gtk_fixed_put(GTK_FIXED(container), letterThumbnail, 680, 100);

  //viene ruchiamato il programma che grazie alla libreria FANN può calcolare il risultato
  string FANNcmd = root + "/bin/FANNhcr ";
  FANNcmd += gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comboBox)); //la network viene passata dinamicamente
  system(FANNcmd.c_str());

  //prima di proseguire viene controllata l'esistenza del file
  while( access( (root + "/tmp/FANNresult.txt").c_str(), F_OK ) == -1 ) 
  usleep(100000); //aspetta che il file venga creato


  //mappa che collega ogni lettera con il valore del proprio neurone di output
  map <char, float> letterAccuracy;
  float total=0;
  char letterResult;

  //apre il file e legge il valore di ogni neurone 
  char letter = 'A';
  string accuracy;
  ifstream infile( (root + "/tmp/FANNresult.txt").c_str() , ios::in );
  if ( infile.is_open( ) ) 
  {
    while (infile)
    {
      getline( infile , accuracy );

      //salta eventuali linee vuote      
      if (accuracy!="")
      {
        stringstream ss;
        float ftmp;
        ss << accuracy;
        ss >> ftmp;
        //inserisce la nuova posizione nella mappa (lettera e valore conventito)
        letterAccuracy.insert(pair<char,float>(letter,ftmp));      
        //mano a mano viene calcolato il totale 
        total += ftmp;
        letter++;
      }
    }
  }
  infile.close( ) ;


  //ciclo che setta le label e le progress bar
  for(int q=0; q<26; q++)
  {
    pair <char, float> max = *letterAccuracy.begin();

    //trova il neurone con il valore maggiore nella mappa
    for(map< char, float >::iterator it=letterAccuracy.begin(); it!=letterAccuracy.end(); ++it)
    {
      if (max.second < it->second)
      {
        max.first = it->first;
        max.second = it->second;
      }

      //salva la prima lettera calcolata (quella corrispondente al disegno)
      if(q==0)
        letterResult=max.first;
    }

    //la lettera ricavata viene eliminata dalla mappa per oridinare i neuroni con le eguenti iterazioni
    letterAccuracy.erase(max.first);

    //viene calcolata la percentuale di attivazione del neurone e creata la stringa da scrivere nelle label
    int percentage = max.second * 100 / total;
    string percStr;
    string percentageStr, maxfirstStr;
    stringstream ss, ss2;
    ss << max.first;
    ss >> maxfirstStr;
    ss2 << percentage;
    ss2 >> percentageStr;
    percStr = maxfirstStr + " " + percentageStr + "%";

    //aggiornamento delle label e delle progress bar
    gtk_label_set_text(GTK_LABEL(percLbl[q]), percStr.c_str());
    gtk_level_bar_set_value(GTK_LEVEL_BAR(percLevelBar[q]),percentage);

  }

  //aggiorna la label con la lettera calcolata alla prima iterazione
  string computedLetterStr = "0";
  computedLetterStr[0] = letterResult;    
  gtk_label_set_text(GTK_LABEL(letterLbl),computedLetterStr.c_str());

  gtk_widget_show_all (window); 

}

//funzione che popola il menù a tendina con tutte le network salvate
static void populateComboBox()
{

  // cartella dove sono contenute le networks
  string netsPath = root + "/nets/";
  DIR *dir;
  struct dirent *ent;

  //numero di network trovate
  int nets = 0; 

  //scorre tutti i file nella directory specificata
  if ((dir = opendir (netsPath.c_str())) != NULL) 
  {
    while ((ent = readdir (dir)) != NULL) 
    {
      string edn = ent->d_name;
      //controlla se il file corrente è una network
      if( (edn.length() > 4) &&  //deve essere almeno lunga 4 caratteri (a.nn) 
      (edn.find(".")!=string::npos) && //deve esserci almeno un punto (l' estensione)
      (edn.substr(edn.find_last_of("."),edn.length()-edn.find_last_of(".")) == ".nn")) //deve avere l'estensione .nn
      {
        //se il file è una neural network allora viene aggiunta alla combobox
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboBox), NULL, edn.c_str());
        nets++;   
      } 
    } 

    closedir (dir);
  } 

  //se non venono trovate network viene mostrata una notifica e rende il programma inagibile
  if(nets <= 0)
  {
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboBox), NULL, "no network found");
    gtk_widget_set_sensitive(read_btn, FALSE);
    gtk_widget_set_sensitive(comboBox, FALSE);
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(comboBox),0);

}


//g++ -std=c++11 RTT.cpp -o ../bin/RTT `pkg-config --libs --cflags gtk+-3.0` -w
