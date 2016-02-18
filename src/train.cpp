#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <limits.h>
#include <unistd.h>   // current path
#include "fann.h"

using namespace std;

#define PROG_VERSION 1.0
#define print(x) cout << x << endl;

/* variabili globali */
string root = "";	// root dir del progetto

string error_data_log = "";
string network_file = "";
string network_name = "";
string training_file = "";

/* topologia della rete */
const unsigned int num_input = 64*64;
const unsigned int num_output = 26;
const unsigned int num_layers = 3;
const unsigned int num_neurons_hidden_1 = 64;


// Questa è la funzione che viene chiamata ogni "n" epoche dove n->epochs_between_reports
// la funzione deve ritornare un valore intero, se ritorna -1 il training termina
int FANN_API test_callback(struct fann *ann, struct fann_train_data *train,
                           unsigned int max_epochs, unsigned int epochs_between_reports,
                           float desired_error, unsigned int epochs)
{
	// scrive nel file "error_data.dat" l'attuale MSE (errore quadratico medio)
	ofstream outfile;
	outfile.open(error_data_log.c_str(), ios::app);
	outfile << fann_get_MSE(ann) << endl;
	outfile.close();

	// // ogni 10 epoche salva la neural network
	// if (epochs % 10 == 0)
	// {
		print(" > Saving network... ");
		fann_save(ann, network_file.c_str());
	// }

	// stampa a video l'attuale MSE (errore quadratico medio)
	print("Epoch " << epochs << ", current mse: "<< fann_get_MSE(ann));
	return 0;
}


// stampa l'help
void help(const char* cmd)
{
	print("Usage: " << cmd << " [options]")
	print("Options:")
	print("  --mse <error>             Mean square error to reach")
	print("  --epoch <max>             Maximum epochs number")
	print("  --name <network-name>     Name for the new network")
	print("  --network <network-file>  Network file")
	print("  --train <train-file>      The training file")
	print("  --log <log-file>          The file where the mse are recorded")
	print("  --backprop-batch          Learning algorithm")
	print("  --backprop-incr           Learning algorithm")
	print("  --quickprop               Learning algorithm")
	print("  --help                    Display this information")
	print("")
}



int main(int argc, char* argv[])
{

	//si ricava il path da dove è stato lanciato il programma
	char res[ PATH_MAX ];
	ssize_t count = readlink( "/proc/self/exe", res, PATH_MAX );
	root = string( res, (count > 0) ? count : 0 );
	root=root.substr(0,root.find_last_of("/\\")+1);
	root = root.substr( 0, root.length()-5);

	network_file = root + "/nets/network.nn";
	training_file = root + "/training_set/training.data";

	/* header */
	cout << endl << "Neural network trainer v. " << PROG_VERSION << endl <<
	                "=============================" << endl << endl;

	float desired_error;
	int max_epochs;
	int epochs_between_reports = 1;
	struct fann *ann;
	fann_train_enum learning_algorithm;

	/* parameter parsing */
	if (argc < 2)
	{
		print("Usage: " << argv[0] << " [options]")
		print("For more informations, use: " << argv[0] << " --help")
		return(1);
	}
	for (int i = 1; i < argc; i++)
	{
		if ((strcmp(argv[i], "--mse")==0) && (i < argc-1))
		{
			stringstream ss;
			ss << argv[i+1];
			ss >> desired_error;
			i++;
		}
		else if ((strcmp(argv[i], "--epoch")==0) && (i < argc-1))
		{
			stringstream ss;
			ss << argv[i+1];
			ss >> max_epochs;
			i++;
		}
		else if ((strcmp(argv[i], "--network")==0) && (i < argc-1))
		{
			network_file = argv[i+1];
			i++;
		}
		else if ((strcmp(argv[i], "--name")==0) && (i < argc-1))
		{
			network_name = argv[i+1];
			i++;
		}
		else if ((strcmp(argv[i], "--train")==0) && (i < argc-1))
		{
			training_file = argv[i+1];
			i++;
		}
		else if ((strcmp(argv[i], "--log")==0) && (i < argc-1))
		{
			error_data_log = argv[i+1];
			i++;
		}
		else if (strcmp(argv[i], "--backprop-batch")==0)
		{
			learning_algorithm = FANN_TRAIN_BATCH;
		}
		else if (strcmp(argv[i], "--backprop-incr")==0)
		{
			learning_algorithm = FANN_TRAIN_INCREMENTAL;
		}
		
		else if (strcmp(argv[i], "--quickprop")==0)
		{
			learning_algorithm = FANN_TRAIN_QUICKPROP;
		}
		else if (strcmp(argv[i], "--help")==0)
		{
			help(argv[0]);
			return(0);
		}
	}

	print("Training parameters:")
	print("   Desired error: " << desired_error)
	print("   Max epochs: " << max_epochs)
	// se è stato specificato il nome della network da creare
	if (network_name != "")
	{
		cout << "   Creating new network: " << root + "/nets/" + network_name << endl;
		ann = fann_create_standard(num_layers, num_input, num_neurons_hidden_1, num_output);
		network_file = root + "/nets/" + network_name;
	}
	// se non è stato specificato
	else
	{
		// proviamo ad importare la net dal file che ci è stato passato (default = network.nn)
		if(access( network_file.c_str(), F_OK ) != -1)
		{
			print("   Using network file: " << network_file)
			ann = fann_create_from_file(network_file.c_str());
		}
		// se non esiste creiamo una nuova net
		else
		{
			cout << "   Creating new network: " << network_file << endl;
			ann = fann_create_standard(num_layers, num_input, num_neurons_hidden_1, num_output);
		}
	}

	// se NON ci è stato specificato un file di log da usare
	// default = "/error_data.dat"
	if (error_data_log == "")
	{
		error_data_log = root + "/error_data.dat";
		print("   Using log file: " << error_data_log)		
	}
	else
	{
		error_data_log = root + "/" + error_data_log;
		// controlliamo che il file da usare esiste
		if(access( error_data_log.c_str(), F_OK ) != -1)
			print("   Using log file: " << error_data_log)
		// altrimenti lo creiamo
		else
			print("   Creating log file: " << error_data_log)
	}

	if (learning_algorithm == FANN_TRAIN_BATCH)
		print("   Learning algorithm: Backpropagation - batch")
	if (learning_algorithm == FANN_TRAIN_INCREMENTAL)
		print("   Learning algorithm: Backpropagation - incremental")
	if (learning_algorithm == FANN_TRAIN_QUICKPROP)
		print("   Learning algorithm: Quickprop")
	print("")

	// setta la funzione di attivazione per TUTTI gli hidden layers
	fann_set_activation_function_hidden(ann, FANN_SIGMOID);
	fann_set_activation_function_output(ann, FANN_SIGMOID);

	fann_set_training_algorithm(ann, learning_algorithm);	// setta l'algoritmo di training

	fann_set_learning_momentum(ann, 0.9);
	fann_set_learning_rate(ann, 0.3);

	fann_set_callback(ann, *(test_callback));	// specifica l'algoritmo di training

	fann_train_on_file(ann, training_file.c_str(), max_epochs, epochs_between_reports, desired_error);
	fann_save(ann, network_file.c_str());

	fann_destroy(ann);

	return 0;
}
