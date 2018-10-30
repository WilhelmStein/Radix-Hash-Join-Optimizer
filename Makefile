
CC = g++
CFLAGS = -Wall -Wextra -std=c++14 -g3

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

HIST_DEP = $(addprefix $(PATH_INC)/, histhash.hpp) $(PATH_SRC)/histhash.cpp
INDX_DEP = $(PATH_INC)/index.hpp $(PATH_SRC)/index.cpp
RDXL_DEP = $(addprefix $(PATH_INC)/, result.hpp) $(PATH_SRC)/result.cpp
RLTN_DEP = $(addprefix $(PATH_INC)/, relation.hpp histhash.hpp) $(PATH_SRC)/relation.cpp

$(PATH_BIN)/histhash.o: $(HIST_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/histhash.cpp -c -o $(PATH_BIN)/histhash.o

$(PATH_BIN)/index.o: $(INDX_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/index.cpp -c -o $(PATH_BIN)/index.o

$(PATH_BIN)/result.o: $(RDXL_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/result.cpp -c -o $(PATH_BIN)/result.o

$(PATH_BIN)/relation.o: $(RLTN_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CFLAGS) $(PATH_SRC)/relation.cpp -c -o $(PATH_BIN)/relation.o

OBJS = $(addprefix $(PATH_BIN)/, histhash.o index.o result.o relation.o)
