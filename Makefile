
CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -g3

PATH_SRC = ./src
PATH_INC = ./inc
PATH_BIN = ./bin
PATH_TST = ./test

.PHONY: all
all:
	mkdir -p $(PATH_BIN)
	
	@echo "\n*** Compiling object files ***"
	@echo "***"
	make $(OBJS)
	@echo "***"

.PHONY: clean
clean:
	@echo "*** Purging binaries ***"
	@echo "***"
	rm -rv $(PATH_BIN)
	@echo "***"

$(PATH_BIN)/%.exe: $(PATH_TST)/%.cpp $(OBJS) $(PATH_INC)/benchmark.hpp
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $< $(OBJS) -o $@

HIST_DEP = $(addprefix $(PATH_INC)/, histhash.hpp) $(PATH_SRC)/histhash.cpp
INDX_DEP = $(PATH_INC)/index.hpp $(PATH_SRC)/index.cpp
RDXL_DEP = $(addprefix $(PATH_INC)/, result.hpp) $(PATH_SRC)/result.cpp
RLTN_DEP = $(addprefix $(PATH_INC)/, relation.hpp histhash.hpp) $(PATH_SRC)/relation.cpp
QUER_DEP = $(addprefix $(PATH_INC)/, types.hpp query.hpp) $(PATH_SRC)/query.cpp

$(PATH_BIN)/histhash.o: $(HIST_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/histhash.cpp -c -o $(PATH_BIN)/histhash.o

$(PATH_BIN)/index.o: $(INDX_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/index.cpp -c -o $(PATH_BIN)/index.o

$(PATH_BIN)/result.o: $(RDXL_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/result.cpp -c -o $(PATH_BIN)/result.o

$(PATH_BIN)/relation.o: $(RLTN_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/relation.cpp -c -o $(PATH_BIN)/relation.o

$(PATH_BIN)/query.o: $(QUER_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/query.cpp -c -o $(PATH_BIN)/query.o

OBJS = $(addprefix $(PATH_BIN)/, histhash.o index.o result.o relation.o query.o)
