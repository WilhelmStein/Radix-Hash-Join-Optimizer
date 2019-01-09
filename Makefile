
CC = g++
CCFLAGS = -Wall -Wextra -std=c++17 -g3

LIBS = -lpthread

PATH_SRC = ./src
PATH_INC = ./inc
PATH_BIN = ./bin
PATH_TEST = ./test

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
	rm -rvf $(PATH_BIN)
	@echo "***"


EXECUTIONER_DEP = $(addprefix $(PATH_INC)/, statistics.hpp executioner.hpp types.hpp result.hpp relation.hpp list.hpp meta.hpp query.hpp pair.hpp pair.ipp list.ipp) $(PATH_SRC)/executioner.cpp

HISTHASH_DEP = $(addprefix $(PATH_INC)/, types.hpp histhash.hpp result.hpp relation.hpp list.hpp list.ipp) $(PATH_SRC)/histhash.cpp

INDEX_DEP = $(addprefix $(PATH_INC)/, types.hpp histhash.hpp result.hpp relation.hpp list.hpp index.hpp list.ipp) $(PATH_SRC)/index.cpp

META_DEP = $(addprefix $(PATH_INC)/, statistics.hpp types.hpp list.hpp meta.hpp query.hpp list.ipp) $(PATH_SRC)/meta.cpp

QUERY_DEP = $(addprefix $(PATH_INC)/, types.hpp query.hpp) $(PATH_SRC)/query.cpp

RELATION_DEP = $(addprefix $(PATH_INC)/, types.hpp histhash.hpp result.hpp relation.hpp list.hpp index.hpp list.ipp) $(PATH_SRC)/relation.cpp

RESULT_DEP = $(addprefix $(PATH_INC)/, types.hpp result.hpp relation.hpp list.hpp list.ipp) $(PATH_SRC)/result.cpp

SCHEDULER_DEP = $(addprefix $(PATH_INC)/, list.hpp scheduler.hpp list.ipp) $(PATH_SRC)/scheduler.cpp

STATISTICS_DEP = $(addprefix $(PATH_INC)/, statistics.hpp types.hpp meta.hpp query.hpp) $(PATH_SRC)/statistics.cpp


$(PATH_BIN)/executioner.o: $(EXECUTIONER_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $(PATH_SRC)/executioner.cpp -c -o $(PATH_BIN)/executioner.o

$(PATH_BIN)/histhash.o: $(HISTHASH_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $(PATH_SRC)/histhash.cpp -c -o $(PATH_BIN)/histhash.o

$(PATH_BIN)/index.o: $(INDEX_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $(PATH_SRC)/index.cpp -c -o $(PATH_BIN)/index.o

$(PATH_BIN)/meta.o: $(META_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $(PATH_SRC)/meta.cpp -c -o $(PATH_BIN)/meta.o

$(PATH_BIN)/query.o: $(QUERY_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $(PATH_SRC)/query.cpp -c -o $(PATH_BIN)/query.o

$(PATH_BIN)/relation.o: $(RELATION_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $(PATH_SRC)/relation.cpp -c -o $(PATH_BIN)/relation.o

$(PATH_BIN)/result.o: $(RESULT_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $(PATH_SRC)/result.cpp -c -o $(PATH_BIN)/result.o

$(PATH_BIN)/scheduler.o: $(SCHEDULER_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $(PATH_SRC)/scheduler.cpp -c -o $(PATH_BIN)/scheduler.o

$(PATH_BIN)/statistics.o: $(STATISTICS_DEP)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $(PATH_SRC)/statistics.cpp -c -o $(PATH_BIN)/statistics.o


OBJS = $(addprefix $(PATH_BIN)/,  executioner.o histhash.o index.o meta.o query.o relation.o result.o scheduler.o statistics.o)

$(PATH_BIN)/%.exe: $(PATH_TEST)/%.cpp $(OBJS)
	$(CC) -I $(PATH_INC) $(DEFINED) $(CCFLAGS) $< $(OBJS) $(LIBS) -o $@
