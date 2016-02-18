CPP = g++
C_FLAGS =
L_FLAGS = -lfann
SRC_DIR = src
BIN_DIR = bin
GTK = `pkg-config --libs --cflags gtk+-3.0`


all: train test bulk-test FANNhcr RTT

train: $(SRC_DIR)/train.cpp
	$(CPP) $(SRC_DIR)/train.cpp $(C_FLAGS) $(L_FLAGS) -o $(BIN_DIR)/train

test: $(SRC_DIR)/test.cpp
	$(CPP) $(SRC_DIR)/test.cpp $(C_FLAGS) $(L_FLAGS) -o $(BIN_DIR)/test 

bulk-test: $(SRC_DIR)/bulk-test.cpp
	$(CPP) $(SRC_DIR)/bulk-test.cpp $(C_FLAGS) $(L_FLAGS) -o $(BIN_DIR)/bulk-test

FANNhcr: $(SRC_DIR)/FANNhcr.cpp
	$(CPP) $(SRC_DIR)/FANNhcr.cpp $(C_FLAGS) $(L_FLAGS) -o $(BIN_DIR)/FANNhcr.cpp

RTT: $(SRC_DIR)/RTT.cpp
	$(CPP) $(SRC_DIR)/RTT.cpp $(C_FLAGS) $(GTK) $(L_FLAGS) -o $(BIN_DIR)/RTT.cpp -w

clean:
	rm -rf test train bulk-test FANNhcr RTT
