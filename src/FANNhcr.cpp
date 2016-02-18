#include <fstream>
#include <string>
#include <iostream>
#include <unistd.h>   // current path
#include <limits.h>
#include "floatfann.h"


using namespace std;


string network_filename = "../nets/network.nn";
struct fann *ann;

// root dir del progetto
string root = "";

int main (int argc, char **argv)
{

  //si ricava il path da dove è stato lanciato il programma
  char res[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", res, PATH_MAX );
  root = string( res, (count > 0) ? count : 0 );
  root=root.substr(0,root.find_last_of("/\\")+1);
  root = root.substr( 0, root.length()-5);

  //se gli argomenti sono 2 (programma e network)
  if(argc == 2)
  {
    //viene salvato il nome della network
    network_filename = root + "/nets/";
    network_filename += argv[1]; 

    //se la network passata non è davvero una network (non finisce con .nn) allora si esce dal programma
    if (network_filename.substr(network_filename.find_last_of("."), network_filename.length()-network_filename.find_last_of(".")) != ".nn")
      exit(0);

  }
  // se gli argomenti non sono due si esce dal programma
  else
    exit(0);


  //viene creata la neural network dal file specificato negli argomenti 
  ann = fann_create_from_file(network_filename.c_str());
  fann_type *calc_out;
  fann_type input[64*64];

  //apre il file contenente l'immagine codificata
  string codedImg;
  ifstream infile( (root + "/tmp/realTimeTestLetter.txt").c_str() , ios::in );
   if ( infile.is_open( ) ) 
    getline( infile , codedImg );
  infile.close( ) ;

  // avendo preso la linea che ci interessa dal file dobbiamo estrapolarne
  // i valori, mediante tokenizzazione (spazi), e caricarli sul vettore input
  string delimiter = " ";
  size_t pos = 0;
  string token;
  int i = 0;
  while ((pos = codedImg.find(delimiter)) != string::npos) 
  {
      token = codedImg.substr(0, pos);
      input[i] = (int)(token[0]) - 48; // conversione del valore ASCII in valore intero
      codedImg.erase(0, pos + delimiter.length());
      i++;
  }

  //una volta settato l' input layer viene fatta partire la network
  calc_out = fann_run(ann, input);

  //il risultato della network viene salvato nell' apposito file txt
  ofstream result;
  result.open ((root + "/tmp/FANNresult.txt").c_str());

  //ogni riga del file corrisponde al valore di un neurone di output
  for (int j=0; j<26; j++)
    result << calc_out[j] << endl;

  result.close();

  return 1;

}



