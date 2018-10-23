
CC = g++
CFLAGS = -Wall -Wextra -std=c++14 -g3

PATH_SRC = ./src
PATH_INC = ./inc
PATH_BIN = ./bin

.PHONY: all
all:
	mkdir -p $(PATH_BIN)
	
	make $(PATH_BIN)/test1
	make $(PATH_BIN)/test3

.PHONY: clean
clean:
	@echo "*** Purging binaries ***\n"
	@echo "***"
	rm -rv $(PATH_BIN)
	@echo "***"

HIST_DEP = $(addprefix $(PATH_INC)/, histhash.hpp) $(PATH_SRC)/histhash.cpp
INDX_DEP = $(addprefix $(PATH_INC)/, index.hpp list.hpp) $(PATH_SRC)/index.cpp
RDXL_DEP = $(addprefix $(PATH_INC)/, rdxl.hpp) $(PATH_SRC)/rdxl.cpp
RLTN_DEP = $(addprefix $(PATH_INC)/, relation.hpp histhash.hpp) $(PATH_SRC)/relation.cpp

$(PATH_BIN)/histhash.o: $(HIST_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/histhash.cpp -c -o $(PATH_BIN)/histhash.o

$(PATH_BIN)/index.o: $(INDX_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/index.cpp -c -o $(PATH_BIN)/index.o

$(PATH_BIN)/rdxl.o: $(RDXL_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/rdxl.cpp -c -o $(PATH_BIN)/rdxl.o

$(PATH_BIN)/relation.o: $(RLTN_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/relation.cpp -c -o $(PATH_BIN)/relation.o

OBJS = $(addprefix $(PATH_BIN)/, histhash.o index.o rdxl.o relation.o)

###############################################################################################
# TESTS #
#########

# (1)
T1ST_DEP = $(OBJS) $(PATH_BIN)/main1.o

$(PATH_BIN)/test1: $(T1ST_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(T1ST_DEP) -o $(PATH_BIN)/test1

$(PATH_BIN)/main1.o: $(PATH_INC)/relation.hpp
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/main1.cpp -c -o $(PATH_BIN)/main1.o

# (3)
T3ST_DEP = $(OBJS) $(PATH_BIN)/main3.o

$(PATH_BIN)/test3: $(T3ST_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(T3ST_DEP) -o $(PATH_BIN)/test3

$(PATH_BIN)/main3.o: $(addprefix $(PATH_INC)/, rdxl.hpp relation.hpp)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/main3.cpp -c -o $(PATH_BIN)/main3.o

###############################################################################################
