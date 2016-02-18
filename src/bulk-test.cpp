#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>
#include "floatfann.h"

using namespace std;

// Il file che contiene gli esempi da testare:
// 	- righe pari -> immagine codificata
// 	- righe dispari -> label
// le lettere devono comparire in ordine alfabetico
string test_set = "test.data";
string network_filename = "network.nn";	// network da usare per il test
const int max_tests = 1400; // massimo numero di campioni presenti (per ogni lettera) nel file test_set
int tests_number = 0;	// numero di istanze da testare per ogni lettera
string command = "";	// comandi system da eseguire

void help(const char* cmd)
{
	cout << "Usage: " << cmd << " [options]" << endl;
	cout << "Options:" << endl;
	cout << "  --network <network-file>  Network filename" << endl;
	cout << "  --test <test-set>         Test set filename" << endl;
	cout << "  --tot <tests-no>          Number of test to do" << endl;
	cout << "  --help                    Display this information" << endl;
	cout << "" << endl;
}


int main(int argc, char* argv[])
{

	srand(time(0));	// seed per per lo shuffle random del vettore delle righe da prendere

	/* intestazione */
	cout << endl << "┌─────────────────────┐" << endl <<
	                "│ Network Bulk Tester │" << endl <<
				    "└─────────────────────┘" << endl;


	/* parsing parametri di input */
	if (argc < 2)
	{
		cout << "Usage: " << argv[0] << " [options]" << endl;
		cout << "For more informations, use: " << argv[0] << " --help" << endl;
		return(1);
	}
	for (int i = 1; i < argc; i++)
	{
		if ((strcmp(argv[i], "--network")==0) && (i < argc-1))
		{
			network_filename = argv[i+1];
			i++;
		}
		else if ((strcmp(argv[i], "--test")==0) && (i < argc-1))
		{
			test_set = argv[i+1];
			i++;
		}
		else if ((strcmp(argv[i], "--tot")==0) && (i < argc-1))
		{
			stringstream ss;
			ss << argv[i+1];
			ss >> tests_number;
			i++;
		}
		else if (strcmp(argv[i], "--help")==0)
		{
			help(argv[0]);
			return(0);
		}
	}

	// mappa che contiene la quantità di campioni per ogni lettera
	map<char,int> rows;

	// apriamo il file del test_set
	ifstream infile( test_set.c_str( ) , std::ios::in ) ;

	string line;	// linea corrente letta dal file
	if (infile.is_open())
	{
		system("echo -n \"Reading the test set... \"");
		// scorre le linee dispari del file per contare ogni lettera
		while (infile)
		{
			getline(infile, line);
			getline(infile, line);

			// evita di prenere l'ultma riga (vuota) del file
			if(line != "")
			{
				// controlla se la entry esiste nella mappa
				if(rows.find(line[0]) == rows.end())
					// se non esiste viene creata e inizializzata a 0
					rows.insert(pair<char,int>(line[0],0));
				// viene incrementato il contatore della label
				rows[line[0]]++;
			}
		}
		// resetta e chiude il file degli input
		infile.clear();
		infile.seekg(0, ios::beg);
		infile.close();
	}
	else
	{
		cerr << "Could not find file " << infile << " !\n";
		return 1;
	}

	pair <char, int> least_samples_pair;	// lettera con il minor numero di samples
	// consideriamo la prima coppia della mappa come quella che ha il minor numero di samples
	least_samples_pair = pair<char, int>(rows.begin()->first, rows.begin()->second);
	// scorriamo tutte le coppie della mappa e cerchiamo quella con il minor numero di samples
	for (map<char, int>::iterator it=rows.begin(); it!=rows.end(); ++it)
	{
		if (least_samples_pair.second > it->second)
		{
			least_samples_pair.first = it->first;
			least_samples_pair.second = it->second;
		}
	}

	// controlliamo che il numero di test da effettuare non sia minore di 0
	if (tests_number<=0)
	{
		cerr << endl << endl << "Immetti un numero di test (--tot) maggiore di 0!" << endl;
		return(0);
	}
	// o maggiore del numero di campioni della lettera che ne ha di meno
	else if (tests_number>least_samples_pair.second)
	{
		cerr << endl << endl << "La lettera con il minor numero di campioni presente" << endl;
		cerr << "nel test set è: " << least_samples_pair.first << " -> " << least_samples_pair.second << endl << endl;
		cerr << "Immetti un numero di test (--tot) minore di " << least_samples_pair.second << endl;
		return(0);
	}

  	// mappa che contiene le righe da leggere per ogni lettera
	map<char,vector<int> > random_rows;

	// vettore con gli intervalli per ogni lettera es -> A da 0 a 1469
	int R[26][2];

	// definisce gli intervalli in cui scegliere le lettere
  	for(int i=0; i<26; i++)
  	{
  		char current_letter = (char)(i+65);

  		// caso base: le A vanno da 0 al numero di 'A' presenti
  		// nel test set
  		if(i==0)
  		{
  			R[0][0]=0;
  			R[0][1]=rows['A'];
  		}
  		// le altre lettere vanno dal limite superiore della precedente + 1 (N)
  		// fino al limite superiore della precedente + numero di occorrenze di tale lettera
  		else
  		{
	  		R[i][0] = R[i-1][1] + 1;
	  		R[i][1] = R[i-1][1] + rows[(char)(i+65)];
  		}

  		// una volta definito l'intervallo delle righe su cui è possibile pescare dal test set
  		// vengono sorteggiati N numeri random che saranno le righe
  		// che effettivamente verranno testate per la lettera corrente

  		// viene riempito un vettore con numeri che vanno dal limite inferiore a quello superiore
  		// per la lettera attuale
  		vector<int> row_numbers;
  		for (int j = R[i][0]; j < R[i][1]; ++j)
  			row_numbers.push_back(j*2);


  		// il vettore viene mischiato e inserito nella mappa
  		random_shuffle(row_numbers.begin(), row_numbers.end());
  		// ridimensioniamo il vettore per contenere il numero di test specificati dall'utente
  		row_numbers.resize(tests_number);
  		// riordiniamo i numeri sorteggiati in modo da porterli leggere sequenzialmente dal file
  		sort(row_numbers.begin(), row_numbers.end());
  		random_rows.insert(pair<char,vector<int> >(current_letter, row_numbers));
  	}

  	system("echo \" OK\"");
  	system("echo -n \"Shuffling the examples...\"");

	vector<string>  codedImgs;	// contiene le immagine codificate lette dal test set
	vector<char>  	lbls;		// contiene le labels lette dal test set

	float correctLetters = 0;	// lettere corrette
	int correctLetter = 0;		// quante istanze della lettera corrente sono state riconosciute

	infile.open(test_set.c_str(), std::ios::in);	// apre il file test set

	if (infile.is_open())
	{
		// scorre tutte le lettere
		for (map<char,vector<int> >::iterator itrr=random_rows.begin(); itrr!=random_rows.end(); ++itrr)
		{

			vector<int> row_numbers = itrr->second;	// vettore contenente le righe da leggere per la lettera corrente
			vector<int>::iterator itrn = row_numbers.begin();	// scorre le righe da leggere per la lettera corrente

			int lines_read = 0;	// righe del file  lette
			string line;	// contenuto della riga corrente
			// scorre tutto il file
			while (infile)
			{
				// se non abbiamo esaurito le righe da leggere
				if (itrn != row_numbers.end())
				{
					int current_row_number = *itrn;	// punta al numero della prossima riga da leggere
					getline(infile, line);
					if(lines_read == current_row_number) // se dobbiamo leggere la riga dove ci troviamo
					{
						// salva l'immagine codificata (se non è vuota)
						if (line!="") codedImgs.push_back(line);

						// salva la rispettiva label (se non è vuota)
						getline( infile , line );
						lines_read++;
						if (line!="") lbls.push_back(line[0]);
						++itrn;
					}
					lines_read++;
				}
				// se abbiamo esaurito le righe da leggere usciamo dal file
				else break;
			}
			// resetta il puntatore del file
			infile.clear();
			infile.seekg(0, ios::beg);
		}
	}
	else
	{
		cerr << "Could not find file " << infile << " !\n";
		return 1;
	}
	infile.close();

	system("echo \" OK\"");
	cout << endl;

	// definisce la topologia della rete e la importa dal file
	fann_type *calc_out;
	fann_type input[64*64];
	struct fann *ann;
	ann = fann_create_from_file(network_filename.c_str());

	int count=0;	// tiene traccia di quanti pattern sono scorsi
	int perc=0;		// percentuale di esempi scorsi per la lettera attuale (parte intera)
	int last_perc=0;// ultima percentuale stampata

	vector<char>::iterator itlbl = lbls.begin();	// iteratore che contiene la label dell'immagine corrente
	// scorre tutto il vettore delle immagini codificate
	for (vector<string>::iterator itimg = codedImgs.begin(); itimg != codedImgs.end(); ++itimg)
	{
		// ogni posizione del vettore contiene una stringa che rappresenta la lettera
		// codificata in 0->pixel bianchi e 1->pixel neri es.-> "0 0 0 0 1 0 0 1 0 0 0"
		// dobbiamo ricavare i valori dalla stringa, usiamo lo spazio " " come delimitatore
		string delimiter = " ";	// delimitatore
		size_t pos = 0;			// ci posizioniamo all'inizio della stringa
		string token;			// conterra il valore 0 o 1
		string imgLine=*itimg;	// stringa alla posizione corrente del vettore
		char lblLine=*itlbl;	// label corrente
		++itlbl;				// label successiva

		int k = 0;	// scorre il vettore di input FANN

		// scorriamo la stringa corrente finchè troviamo token, ne estrapoliamo
		// i valori interi e li carichiamo nel vettore di input FANN
		while ((pos = imgLine.find(delimiter)) != string::npos) {
			token = imgLine.substr(0, pos);
			// conversione del valore ASCII in valore intero
			input[k] = (int)(token[0]) - 48;
			imgLine.erase(0, pos + delimiter.length());
			k++;
		}

		// diamo l'input alla rete neurale e salviamo l'output
		calc_out = fann_run(ann, input);

		// out è un array che contiene i neuroni d'output che sono coppie del tipo
		// <indice_neurone, valore_attivazione> dobbiamo trovare il neurone con il
		// valore di attivazione più alto
		float out[2] = {-1, -1000};
		for (int l=0; l<26; l++)
		{
			if (out[1] <= calc_out[l])
			{
				out[0] = l;
				out[1] = calc_out[l];
			}
		}

		// se il neurone con valore di attivazione massimo corrisponde
		// alla label allora la classificazione ha avuto esito positivo
		if (out[0]==(int)lblLine-65)
		{
			// aumentiamo il numero di lettere indovinate
			correctLetters++;
			correctLetter++;
		}
		count++;

		// calcola la percentuale di esempi scorsi per la lettera attuale
		// e stampa l'inizio della progress bar
		perc = (count * 100) / tests_number;
		if (count%tests_number==1) {
			command = "echo -n \"";
			command += lblLine;
			command += " \"";
			system(command.c_str());
		}
		// ogni volta che avanziamo del 10% aggiorniamo la progress bar
		else if (perc % 10 == 0 && perc - 10 == last_perc)
		{
			system("echo -n \"■■\"");
			last_perc = perc;
		}
		// quando l'iteratore arriva al numero di test prefissati
		// per ogni lettera passiamo a quella successiva
		if(count%tests_number==0)
		{
			command = "echo -n \" ";
			ostringstream convert;
			convert << correctLetter*100/tests_number;
			command += convert.str();
			command += "%\"";
			system(command.c_str());
			command = "echo";
			system(command.c_str());
			correctLetter=0;	// azzeriamo il contatore delle lettere indovinate
		}
	}

	 // percentuale di match di tutte le lettere
  	cout << endl << "Total precision: " << correctLetters*100/(tests_number*26) << "%" << endl;
	return 0;
 }
