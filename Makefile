DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

all : ${DIR_BIN}/catch ${DIR_BIN}/fingerprint ${DIR_BIN}/sniff
.PHONY: clean

SRC = $(wildcard ${DIR_SRC}/*.cc)
OBJ = $(patsubst %.cc,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = catch

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = gcc
CFLAGS = -g

CXX = g++
CXXFLAGS = -g -Wall -std=c++11 -Wno-unused-but-set-variable -I${DIR_SRC}

CAT_OBJ = ${DIR_OBJ}/catch.o ${DIR_OBJ}/list_ports_linux.o ${DIR_OBJ}/serial.o ${DIR_OBJ}/unix.o
${DIR_BIN}/catch:${CAT_OBJ}
	$(CXX) ${CXXFLAGS} $(CAT_OBJ) -o $@

FP_OBJ = ${DIR_OBJ}/fingerprint.o ${DIR_OBJ}/list_ports_linux.o ${DIR_OBJ}/serial.o ${DIR_OBJ}/unix.o
${DIR_BIN}/fingerprint:${FP_OBJ}
	$(CXX) ${CXXFLAGS} $(FP_OBJ) -o $@

SNI_OBJ = ${DIR_OBJ}/sniff.o ${DIR_OBJ}/list_ports_linux.o ${DIR_OBJ}/serial.o ${DIR_OBJ}/unix.o
${DIR_BIN}/sniff:${SNI_OBJ}
	$(CXX) ${CXXFLAGS} $(SNI_OBJ) -o $@

${DIR_OBJ}/%.o:${DIR_SRC}/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	find ${DIR_OBJ} -name '*.o' -exec rm -rf '{}' \;
