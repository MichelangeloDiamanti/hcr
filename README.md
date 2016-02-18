# Handwritten Characters Recognition
Using the [FANN library](http://leenissen.dk/fann/wp/) we implemented a multilayer perceptron that can recognize handwritten capital letters.

The network has 4096 input nodes and so has to be trained with 64x64 images. The images were encoded in binary values according to their pixels (0 if the pixel tended to a light color, 1 otherwise). Then they were all stored in a file with their respective labels (according to the [FANN format](http://leenissen.dk/fann/html/files/fann_train-h.html) for training files)

After a fair amount of testing we decided to go for a configuration that has 90 neurons for the hidden layer and to train the network with the QuickProp algorithm, using a learning rate of 1.3 and a momentum of 0.9 that let us reach an accuracy of 80% on the test set.

# Train
The train program is simply a command-line interface that lets the user choose some key-parameters like the training file, the network name etc... and starts to train the network (some parameters like di number of hidden neurons, the learning rate etc... are hardcoded)

# Test
To test a single pattern the user must simply run the test program passing some parameters (which are specified in the help), the program randomly select a pattern, present it to the net and decode the output comparing it to the respective expected output.

![alt tag](https://raw.githubusercontent.com/MichelangeloDiamanti/hcr/master/Screenshots/test.png)
# Bulk-test
It is a CLI program that let the user test the accuracy of the network over many examples:
The user simply has to specify the test set, the network to use and the number of examples to check. The program then read randomly from the test file N examples for each letter, giv them to the network, decode the output, compare it with the expected and draws some statistics like the percentage of single letters guessed right etc...

![alt tag](https://raw.githubusercontent.com/MichelangeloDiamanti/hcr/master/Screenshots/bulk-test.png)
# Real Time Test
This is a GUI program that lets the user choose a (trained) network, draw a letter on the screen and then encodes it according to the [FANN input format](http://leenissen.dk/fann/html/files/fann_train-h.html).
It then present the resulting input to the network, receive the output, order the results in descending order and show them to the user.

![alt tag](https://raw.githubusercontent.com/MichelangeloDiamanti/hcr/master/Screenshots/RTT.png)
