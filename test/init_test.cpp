#include <iostream>
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <climits>
#include <sys/mman.h>
#include <fstream>
#include <relation.hpp>

using std::cout;
using std::endl;
using std::cin;
using std::cerr;

using RHJ::Relation;

struct Meta
{
    std::size_t rowSize, columnSize;

    RHJ::Relation::Tuple * columns;
} * meta;

int main()
{
    char* str = new char[PATH_MAX + 1];
    size_t inputSize = 0;

    while(true) {
        cin.getline(str, PATH_MAX);
        if (!strcmp(str, "Done"))
            break;
        inputSize++;
    }

    cin.seekg(0);
    
    meta = new Meta[inputSize];

    size_t currentFileNo = 0;

    do {
        // Get each file path
        cin.getline(str, PATH_MAX);
        

        FILE* inputFile = nullptr;

        if( !( inputFile = fopen(str, "r") ) ){
            cerr<<"Could not open file "<<str<<"\nErrorno: "<<errno<<endl;
            return -1;
        }

        // Get length of file
        fseek(inputFile, 0, SEEK_END);
        size_t file_size = (size_t) ftell(inputFile);
        fseek(inputFile, 0, SEEK_END);

        // Load whole file in memory
        void* p = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(inputFile), 0);
        fclose(inputFile);

        uint64_t tuples(0UL), columns(0UL);

        // Copy Metadata
        memmove( (void *) &tuples, p, sizeof(uint64_t));
        p = (char*)p + sizeof(uint64_t);
        memmove( (void *) &columns, p, sizeof(uint64_t));
        p = (char*)p + sizeof(uint64_t);

        // Create Index        
        for(uint64_t j = 0UL; j < columns; j++) {
            memmove(&meta[currentFileNo].columns[j], (void*) ((char*)p + j*tuples), sizeof(uint64_t));
        }
        
        currentFileNo++;
    } while( true );


    delete[] meta;
    delete[] str;
    return 0;
}
