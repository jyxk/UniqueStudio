
INC =  -I ./src/
SRC = ./src/*.cpp
CFLAG = -lmagic -g -O3 -Wall -Wno-deprecated -std=c++0x #-DSHOW_DEBUG #-pipe  -D_NEW_LIC -D_GNU_SOURCE -D_REENTRANT  -z defs
CC = g++
EXE = ./bin/search

all:
	$(CC) -o $(EXE) $(CFLAG) $(SRC) $(INC)
clean: 
	rm -rf *.o
	rm -rf *.so
	rm -rf $(EXE)