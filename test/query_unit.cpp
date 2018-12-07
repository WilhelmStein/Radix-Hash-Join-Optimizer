
#include <query.hpp>
#include <iostream>
#include <executioner.hpp>
#include <vector>
#include <string>
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

RHJ::Meta * RHJ::meta;

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
        paths.emplace_back(str);
    }
    
    if(!paths.size())
    {
        cerr<<"Error: no paths"<<endl;
        return -1;
    }

    RHJ::meta = new RHJ::Meta[paths.size()];

    size_t currentFileNo = 0;

    while( !paths.empty() ) {
        // Get each file path
        FILE* inputFile = nullptr;

        if( !( inputFile = fopen(paths.front(), "r+b") ) ){
            cerr<<"Could not open file "<<paths.front()<<"\nErrorno: "<<errno<<endl;
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
        memmove( (void *) &RHJ::meta[currentFileNo].rowSize, p, sizeof(uint64_t));
        p = (char*)p + sizeof(uint64_t);
        memmove( (void *) &RHJ::meta[currentFileNo].columnSize, p, sizeof(uint64_t));
        p = (char*)p + sizeof(uint64_t);
        

        // Create Index
        RHJ::meta[currentFileNo].columns = new tuple_key_t*[RHJ::meta[currentFileNo].columnSize]; 
        for(uint64_t j = 0UL; j < RHJ::meta[currentFileNo].columnSize; j++) {
            RHJ::meta[currentFileNo].columns[j] = ((tuple_key_t*)p + j*RHJ::meta[currentFileNo].rowSize);
        }
  
        currentFileNo++;
    }

    std::ifstream infile("./test_data/small/small.work");
    std::ofstream outfile("./test_data/results/results");

    std::string line;
    while ( getline(infile, line) ) {
        if (line != "F") {
            char *q = new char[line.length() + 1];
            strcpy(q, line.c_str());

            cout << endl << "Executing..  " << q << endl << endl;

            RHJ::Query query = RHJ::Query(q);
            RHJ::Executioner demios;

            std::vector<std::string> checkSums = demios.execute(query);
            
            for (auto &c : checkSums) {
                outfile << c << " ";
            }
            outfile << endl;
            delete[] q;
        }
        // break;
    }

    outfile.close();
    infile.close();



    for(size_t i = 0; i < currentFileNo; i++) {
        size_t ret = munmap( (void*) (  *RHJ::meta[i].columns - 2 ) , (RHJ::meta[i].columnSize * RHJ::meta[i].rowSize + 2) * sizeof(uint64_t) );
        if(ret)
        {
            cerr << "Error: Munmap failed" << endl << "Errorno: " << errno << endl;
            return -1;
        }
        delete[] RHJ::meta[i].columns;
    }

    delete[] RHJ::meta;
    return 0;
}