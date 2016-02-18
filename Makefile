CPP = g++
C_FLAGS =
L_FLAGS = -lfann
SRC_DIR = src
BIN_DIR = bin

all: train test bulk-test

train: $(SRC_DIR)/train.cpp
	$(CPP) $(SRC_DIR)/train.cpp $(C_FLAGS) $(L_FLAGS) -o $(BIN_DIR)/train

test: $(SRC_DIR)/test.cpp
	$(CPP) $(SRC_DIR)/test.cpp $(C_FLAGS) $(L_FLAGS) -o $(BIN_DIR)/test 

bulk-test: $(SRC_DIR)/bulk-test.cpp
	$(CPP) $(SRC_DIR)/bulk-test.cpp $(C_FLAGS) $(L_FLAGS) -o $(BIN_DIR)/bulk-test

clean:
	rm -rf test train convert bulk-test bulk-test-prerandomed-file
