#include <iostream>
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <climits>
#include <sys/mman.h>
#include <fstream>
#include <relation.hpp>
#include <list.hpp>

using std::cout;
using std::endl;
using std::cin;
using std::cerr;

using RHJ::Relation;

struct Meta
{
    uint64_t rowSize, columnSize;
    tuple_key_t** columns;
} * meta;


int main(void)
{
    utility::list<char*> paths;
    char* str = nullptr;

    while(true) {
        str = new char[PATH_MAX + 1];
        cin.getline(str, PATH_MAX);

        if (!strcmp(str, "Done"))
        {
            delete[] str;
            break;
        }
        paths.push_back(str);
    }
    
    if(!paths.size())
    {
        cerr<<"Error: no paths"<<endl;
        return -1;
    }

    meta = new Meta[paths.size()];

    size_t currentFileNo = 0;

    while( !paths.empty() ) {
        // Get each file path
        FILE* inputFile = nullptr;

        if( !( inputFile = fopen(paths.back(), "r+b") ) ){
            cerr<<"Could not open file "<<paths.back()<<"\nErrorno: "<<errno<<endl;
            return -1;
        }

        delete[] paths.front();
        paths.erase(paths.begin());

        // Get length of file
        fseek(inputFile, 0, SEEK_END);
        size_t file_size = (size_t) ftell(inputFile);
        fseek(inputFile, 0, SEEK_END);

        // Load whole file in memory
        void* p = mmap(NULL, file_size, PROT_READ | PROT_WRITE , MAP_SHARED, fileno(inputFile), 0);
        if( p == MAP_FAILED )
        {
            cerr<<"Error: Mmap failed.\nErrorno: "<<errno<<endl;

            return -1;
        }

        fclose(inputFile);

        // Copy Metadata
        memmove( (void *) &meta[currentFileNo].rowSize, p, sizeof(uint64_t));
        p = (char*)p + sizeof(uint64_t);
        memmove( (void *) &meta[currentFileNo].columnSize, p, sizeof(uint64_t));
        p = (char*)p + sizeof(uint64_t);

        // Create Index
        meta[currentFileNo].columns = new tuple_key_t*[meta[currentFileNo].columnSize]; 
        for(uint64_t j = 0UL; j < meta[currentFileNo].columnSize; j++) {
            meta[currentFileNo].columns[j] = ((tuple_key_t*)p + j*meta[currentFileNo].rowSize);
        }
        
        currentFileNo++;
    }

    for(size_t i = 0; i < currentFileNo; i++) {
        size_t ret = munmap( (void*) (  *meta[i].columns - 2 ) , (meta[i].columnSize * meta[i].rowSize + 2) * sizeof(uint64_t) );
        if(ret)
        {
            cerr<<"Error: Munmap failed\nErrorno: "<<errno<<endl;
            return -1;
        }
        delete[] meta[i].columns;
    }

    delete[] meta;
    return 0;
}
