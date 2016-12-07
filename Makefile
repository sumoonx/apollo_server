DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

SRC = $(wildcard ${DIR_SRC}/*.cc)
OBJ = $(patsubst %.cc,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = monitor

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = gcc
CFLAGS = -g

CXX = g++
CXXFLAGS = -g -Wall -Wno-unused-but-set-variable -I${DIR_SRC}

${BIN_TARGET}:${OBJ}
	$(CXX) ${CXXFLAGS} $(OBJ) -o $@

${DIR_OBJ}/%.o:${DIR_SRC}/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	find ${DIR_OBJ} -name '*.o' -exec rm -rf '{}' \;
