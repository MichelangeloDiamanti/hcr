#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <string.h>
#include "floatfann.h"

using namespace std;

#define print(x) cout << x << endl;

/* global variables definitions */
string images_file = "test_1.data";
string network_filename = "64hidden_network.nn";

// stampa l'help
void help(const char* cmd)
{
	print("Usage: " << cmd << " [options]")
	print("Options:")
	print("  --network <network-file>  Network file")
	print("  --test <test-file>        The test file")
	print("  --help                    Display this information")
	print("")
}

int main(int argc, char* argv[])
{
	srandom((unsigned)time(0));

	/* header */
	cout << endl << "Network tester" << endl <<
	                "==============" << endl << endl;
	/* parameter parsing */
	if (argc < 2)
	{
		print("Usage: " << argv[0] << " [options]")
		print("For more informations, use: " << argv[0] << " --help")
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
			images_file = argv[i+1];
			i++;
		}
		else if (strcmp(argv[i], "--help")==0)
		{
			help(argv[0]);
			return(0);
		}
	}
	// definisce la topologia della rete e la importa dal file
	fann_type *calc_out;
	fann_type input[64*64];
	struct fann *ann;
	ann = fann_create_from_file(network_filename.c_str());

	// conta il numero di righe del file
	ifstream infile_count( images_file.c_str( ) , std::ios::in );
	string line_count;
	int number_of_lines = 0;
    while (getline(infile_count, line_count))
        ++number_of_lines;

	// genera un numero random (la linea da leggere dal file di input)
	int item = (random()%number_of_lines)+1;
	// le righe del file di input contengono il pattern di input sulle righe dispari
	// e la label su quelle pari -> bisogna leggere prima il pattern
	if (item%2==0)
	{
		item--;
	}

	ifstream infile( images_file.c_str( ) , std::ios::in ) ;

	int lines_read = 0;
	string line;
	string imgLine;
	string lblLine;
	if ( infile.is_open( ) ) {
		// scorre tutto il file
		while ( infile ) {
			getline( infile , line );
			lines_read++;
			if ( lines_read == item ) {
				// quando ci troviamo alla linea desiderata
				// copio il pattern 
				imgLine = line;
				// copio la label
				getline( infile , line );
				lblLine = line;
				break; 
			}
		}
		infile.close( ) ;
	}
	else {
		cerr << "Could not find file " << infile << " !\n";
		return 1;
	}	

	// avendo preso la linea che ci interessa dal file dobbiamo estrapolarne
	// i valori, mediante tokenizzazione (spazi), e caricarli sul vettore input
	string delimiter = " ";
	size_t pos = 0;
	string token;
	int i = 0;
	while ((pos = imgLine.find(delimiter)) != string::npos) {
	    token = imgLine.substr(0, pos);
	    // conversione del valore ASCII in valore intero
	    input[i] = (int)(token[0]) - 48;
	    // cout << input[i];
	    imgLine.erase(0, pos + delimiter.length());
	    i++;
	}

	cout << "+----------------------------------------------------------------+" << endl;
	for(int i=0; i<64*64;i++)
	{
		if(i%64==0)
			cout<<"|";

		if(input[i]==0)
			cout<<"█";
		
		if(input[i]==1)
			cout<<" ";

		if(i%64==63)
			cout<<"|"<<endl;

	}
	cout << "+----------------------------------------------------------------+" << endl;

	calc_out = fann_run(ann, input);


	float out[2] = {-1, -1000};
	for (int i=0; i<26; i++)
	{
		if (out[1] <= calc_out[i])
		{
			out[0] = i;
			out[1] = calc_out[i];
		}

		cout << "O(" << i << ") = " << calc_out[i] << "; " << endl;
	}
	cout << endl << endl;


	if (out[0]==(int)lblLine[0]-65)
		cout << "  *** CORRECT ***";
	else
		cout << "  --- WRONG ---";
	cout << endl << endl;

	cout << "Example number " << item << endl <<
	        "Expected  : " << (int)lblLine[0]-65 << " = " << lblLine << endl <<
	        "Calculated: " << out[0] << " = " << (char)(out[0]+65) << endl;
	return 0;
}
