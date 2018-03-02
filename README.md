# Handwritten Characters Recognition

### Requirements:
1. [FANN library](http://leenissen.dk/fann/wp/download/)
2. Linux OS (we used bash instructions like echo)
3. [JRE (1.7)](http://www.oracle.com/technetwork/java/javase/downloads/index.html)
4. [GTK 3.x](http://www.gtk.org/download/index.php)
5. Make tools (with compiler capable of pkg-config)

### Usage:
1. clone the repo
2. make sure that your directory looks like this (you may have to create some empty dirs):

![ScreenShot](/Screenshots/tree-dirs.png)

3. make

You should now have the binaries and can (probably) use the RTT program. If you want to use one of the other utilities you have to get a dataset that complies to:

* [FANN format for training files](http://leenissen.dk/fann/html/files/fann_train-h.html):
    * the first line (needed only once per file) is composed by:
        * the number of patterns in the training file
        * the number of input nodes
        * the number of output nodes
    * the second line is the encoded image, we chose a binary encoding (threshold):
        * dark pixels --> 1 
        * light pixels --> 0
        * but whichever encoding should be fine (like grayscale with RGB values)
    * the third line is the encoded label represented in a [One-hot like](https://it.wikipedia.org/wiki/One-hot) fashion:
        * off neurons --> 0.1
        * on neurons --> 0.9
        
![ScreenShot](/Screenshots/training_set_file.png)

* FANN format for test files:
    * first line the encoded image
    * second line the label (character)

![ScreenShot](/Screenshots/test_set_file.png)

## Introduction
Using the [FANN library](http://leenissen.dk/fann/wp/) we implemented a multilayer perceptron that can recognize handwritten capital letters.

The network has 4096 input nodes and so has to be trained with 64x64 images. The images were encoded in binary values according to their pixels (0 if the pixel tended to a light color, 1 otherwise). Then they were all stored in a file with their respective labels (according to the [FANN format](http://leenissen.dk/fann/html/files/fann_train-h.html) for training files)

After a fair amount of testing we decided to go for a configuration that has 90 neurons for the hidden layer and to train the network with the QuickProp algorithm, using a learning rate of 1.3 and a momentum of 0.9 that let us reach an accuracy of 80% on the test set.

## Train
The train program is simply a command-line interface that lets the user choose some key-parameters like the training file, the network name etc... and starts to train the network (some parameters like the number of hidden neurons, the learning rate etc... are hardcoded)

![ScreenShot](/Screenshots/train.png)
## Test
To test a single pattern the user must simply run the test program specifying the test-set file and the network to test, the program randomly select a pattern, present it to the net and decode the output comparing it to the respective expected output.

![ScreenShot](/Screenshots/test.png)
## Bulk-test
It is a CLI program that let the user test the accuracy of the network over many examples:
The user simply has to specify the test set, the network to use and the number of examples to check. The program then read randomly from the test file N examples for each letter, submit them to the network, decode the output, compare it with the expected and draws some statistics like the percentage of single letters guessed right etc...

![ScreenShot](/Screenshots/bulk-test.png)
## Real Time Test
This is a GUI program that lets the user choose a (trained) network, draw a letter on the screen and then encodes it according to the [FANN input format](http://leenissen.dk/fann/html/files/fann_train-h.html).
It then present the resulting input to the network, receive the output, order the results in descending order and show them to the user.

![ScreenShot](/Screenshots/RTT.png)
