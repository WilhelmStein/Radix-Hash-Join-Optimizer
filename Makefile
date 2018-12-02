
CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -g3

PATH_SRC = ./src
PATH_INC = ./inc
PATH_BIN = ./bin
PATH_TST = ./test

.PHONY: all
all:
	mkdir -p $(PATH_BIN)
	@echo
	@echo "*** Compiling object files ***"
	@echo "***"
	make $(OBJS)
	@echo "***"
.PHONY: clean
clean:
	@echo
	@echo "*** Purging binaries ***"
	@echo "***"
	rm -rv $(PATH_BIN)
	@echo "***"

$(PATH_BIN)/%.exe: $(PATH_TST)/%.cpp $(OBJS)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $< $(OBJS) -o $@

EXEC_DEP = $(addprefix $(PATH_INC)/, types.hpp executioner.hpp result.hpp relation.hpp list.hpp query.hpp pair.hpp pair.ipp list.ipp) $(PATH_SRC)/executioner.cpp
HIST_DEP = $(addprefix $(PATH_INC)/, types.hpp histhash.hpp result.hpp relation.hpp list.hpp list.ipp) $(PATH_SRC)/histhash.cpp
INDE_DEP = $(addprefix $(PATH_INC)/, types.hpp histhash.hpp result.hpp relation.hpp list.hpp index.hpp list.ipp) $(PATH_SRC)/index.cpp
QUER_DEP = $(addprefix $(PATH_INC)/, types.hpp query.hpp) $(PATH_SRC)/query.cpp
RELA_DEP = $(addprefix $(PATH_INC)/, types.hpp histhash.hpp result.hpp relation.hpp list.hpp index.hpp list.ipp) $(PATH_SRC)/relation.cpp
RESU_DEP = $(addprefix $(PATH_INC)/, types.hpp result.hpp relation.hpp list.hpp list.ipp) $(PATH_SRC)/result.cpp

$(PATH_BIN)/executioner.o: $(EXEC_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/executioner.cpp -c -o $(PATH_BIN)/executioner.o

$(PATH_BIN)/histhash.o: $(HIST_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/histhash.cpp -c -o $(PATH_BIN)/histhash.o

$(PATH_BIN)/index.o: $(INDE_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/index.cpp -c -o $(PATH_BIN)/index.o

$(PATH_BIN)/query.o: $(QUER_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/query.cpp -c -o $(PATH_BIN)/query.o

$(PATH_BIN)/relation.o: $(RELA_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/relation.cpp -c -o $(PATH_BIN)/relation.o

$(PATH_BIN)/result.o: $(RESU_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/result.cpp -c -o $(PATH_BIN)/result.o


OBJS = $(addprefix $(PATH_BIN)/,  executioner.o histhash.o index.o query.o relation.o result.o)
