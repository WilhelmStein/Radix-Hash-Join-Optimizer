
CC = g++
CFLAGS = -Wall -Wextra -std=c++14 -g3

PATH_SRC = ./src
PATH_INC = ./inc
PATH_BIN = ./bin
PATH_TST = ./test

.PHONY: all
all:
	mkdir -p $(PATH_BIN)
	
	@echo "*** Compiling tests ***\n"
	@echo "***"
	make $(PATH_BIN)/test1
	make $(PATH_BIN)/test2
	make $(PATH_BIN)/test3
	@echo "***"

.PHONY: clean
clean:
	@echo "*** Purging binaries ***\n"
	@echo "***"
	rm -rv $(PATH_BIN)
	@echo "***"

HIST_DEP = $(addprefix $(PATH_INC)/, histhash.hpp) $(PATH_SRC)/histhash.cpp
INDX_DEP = $(PATH_INC)/index.hpp $(PATH_SRC)/index.cpp
RDXL_DEP = $(addprefix $(PATH_INC)/, result.hpp) $(PATH_SRC)/result.cpp
RLTN_DEP = $(addprefix $(PATH_INC)/, relation.hpp histhash.hpp) $(PATH_SRC)/relation.cpp

$(PATH_BIN)/histhash.o: $(HIST_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/histhash.cpp -c -o $(PATH_BIN)/histhash.o

$(PATH_BIN)/index.o: $(INDX_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/index.cpp -c -o $(PATH_BIN)/index.o

$(PATH_BIN)/result.o: $(RDXL_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/result.cpp -c -o $(PATH_BIN)/result.o

$(PATH_BIN)/relation.o: $(RLTN_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_SRC)/relation.cpp -c -o $(PATH_BIN)/relation.o

OBJS = $(addprefix $(PATH_BIN)/, histhash.o index.o result.o relation.o)

###############################################################################################
# TESTS #
#########

# (1)
T1ST_DEP = $(OBJS) $(PATH_BIN)/main1.o

$(PATH_BIN)/test1: $(T1ST_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(T1ST_DEP) -o $(PATH_BIN)/test1

$(PATH_BIN)/main1.o: $(PATH_INC)/relation.hpp
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_TST)/main1.cpp -c -o $(PATH_BIN)/main1.o

# (2)
T2ST_DEP = $(OBJS) $(PATH_BIN)/main2.o

$(PATH_BIN)/test2: $(T2ST_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(T2ST_DEP) -o $(PATH_BIN)/test2

$(PATH_BIN)/main2.o: $(PATH_INC)/relation.hpp
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_TST)/main2.cpp -c -o $(PATH_BIN)/main2.o

# (3)
T3ST_DEP = $(OBJS) $(PATH_BIN)/main3.o

$(PATH_BIN)/test3: $(T3ST_DEP)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(T3ST_DEP) -o $(PATH_BIN)/test3

$(PATH_BIN)/main3.o: $(addprefix $(PATH_INC)/, result.hpp relation.hpp)
	$(CC) -I $(PATH_INC) $(CFLAGS) $(PATH_TST)/main3.cpp -c -o $(PATH_BIN)/main3.o

###############################################################################################
