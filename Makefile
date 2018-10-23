
CC = g++
CFLAGS = -Wall -Wextra -std=c++14 -g3

PATH_SRC = ./src
PATH_INC = ./inc
PATH_BIN = ./bin

HIST_DEP = $(addprefix $(PATH_INC)/, histhash.h) $(PATH_SRC)/histhash.cpp
INDX_DEP = $(addprefix $(PATH_INC)/, index.hpp list.h) $(PATH_SRC)/index.cpp
RDXL_DEP = $(addprefix $(PATH_INC)/, rdxl.hpp) $(PATH_SRC)/rdxl.cpp
RLTN_DEP = $(addprefix $(PATH_INC)/, relation.h histhash.h) $(PATH_SRC)/relation.cpp

$(PATH_BIN)/histhash.o: $(HIST_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/histhash.cpp -c -o $(PATH_BIN)/histhash.o

$(PATH_BIN)/index.o: $(INDX_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/index.cpp -c -o $(PATH_BIN)/index.o

$(PATH_BIN)/rdxl.o: $(RDXL_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/rdxl.cpp -c -o $(PATH_BIN)/rdxl.o

$(PATH_BIN)/relation.o: $(RLTN_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/relation.cpp -c -o $(PATH_BIN)/relation.o

.PHONY: clean
clean:
	@echo "*** Purging binaries ***\n"
	@echo "***"
	rm -rvi $(PATH_BIN)
	@echo "***"
