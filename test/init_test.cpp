#include <iostream>
#include <cstdint>
#include <cstring>
#include <climits>
#include <fstream>
#include <relation.hpp>
#include <vector>

using std::cout;
using std::endl;
using std::cin;
using std::cerr;

using std::vector;

using RHJ::Relation;

struct Table {
    Relation r;
    Relation::Tuple** index;
    uint64_t rows, columns;

    Table(uint64_t rows, uint64_t columns): rows(rows),
                                            columns(columns),
                                            r( (size_t) (rows * columns), "L" ) 
                                            { index = new Relation::Tuple*[columns]; }
    ~Table() { delete[] index; }
};

int main()
{
    vector<Table> relations;
    relations.reserve(40);
    char* str = new char[PATH_MAX + 1];

    do {
        //get each file path and load it to memory
        cin.getline(str, PATH_MAX);

        if (!strcmp(str, "Done"))
            break;

        std::ifstream inputFile(str, std::ios::in | std::ios::binary);

        if(!inputFile.is_open()){
            cerr<<"Could not open file\n";
            return -1;
        }

        // get length of file
        inputFile.seekg(0, inputFile.end);
        size_t buff_size = inputFile.tellg();
        inputFile.seekg(0, inputFile.beg);

        //load whole file in memory
        char* buffer = new char[buff_size + 1];
        inputFile.read( buffer, buff_size); // H read isws na skaei an einai megalo to file
        inputFile.close();

        uint64_t tuples(0UL), columns(0UL);
        void* p = (void*) buffer;

        //copy metadata
        memmove( (void *) &tuples, p, sizeof(uint64_t));
        p += sizeof(uint64_t);
        memmove( (void *) &columns, p, sizeof(uint64_t));
        p += sizeof(uint64_t);

        //create relation
        //Table* tmp = new Table(tuples, columns);
        relations.emplace_back(tuples, columns);   
        
        for(uint64_t j = 0UL; j < columns; j++) {
            relations[relations.size() - 1].index[j] = &(relations[relations.size() - 1].r.tuples[j*tuples]);
            for(uint64_t i = 0UL; i < tuples; i++) {
                relations[relations.size() - 1].r.tuples[j*tuples + i].key = i;
                memmove(&relations[relations.size() - 1].r.tuples[j*tuples + i].payload, p, sizeof(uint64_t) );
                p += sizeof(uint64_t);
            }
        }
        
        delete[] buffer;
    } while( true );
    
    while(relations.size()){
        #ifdef __VERBOSE__
            cout<<relations[relations.size() - 1].r;
        #endif
        //delete relations[relations.size() - 1];
        relations.pop_back();
    }

    delete[] str;
    return 0;
}
