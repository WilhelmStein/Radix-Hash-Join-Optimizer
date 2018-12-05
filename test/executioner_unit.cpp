
#include <iostream>
#include <cstring>
#include <climits>
#include <cassert>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/mman.h>

#include <fcntl.h>

#include <relation.hpp>
#include <list.hpp>

// Generate a list of the full paths of the relations and store it in files.txt
// for file in "$(ls ./test_data/small/r*)"; do echo "$(realpath $file)"; done > ./test_data/files.txt &&  echo "Done" >> ./test_data/files.txt

// Macro used in order to exit if condition is met
#define exit_if(condition, message)                 \
do                                                  \
{                                                   \
    if (condition)                                  \
    {                                               \
        std::perror(message); std::exit(errno);     \
    }                                               \
} while (false)                                     \

// Macro definition for consistency with above declaration
#define exit_if_not(condition) assert(condition)

int main()
{
    utility::list<char *> paths;

    // Throw exception if any of the following flags are set for std::cin
    std::cin.exceptions(std::ios_base::badbit | std::ios_base::failbit);

    while(true)
    {
        char buff[PATH_MAX + 1UL];
        
        // Atempt to read the current line
        try
        {
            std::cin.getline(buff, sizeof(buff));
        }
        catch (std::ios_base::failure& f)
        {
            std::cerr
            << "iteration: "  << paths.size()
            << " exception: " << f.what()
            << std::endl;

            std::exit(f.code().value());
        }

        if (!std::strcmp(buff, "Done"))
            break;
        
        paths.push_back(new char[std::strlen(buff) + 1UL]);
        
        std::strcpy(paths.back(), buff);
    }
    
    std::size_t total = paths.size(); exit_if_not(total > 0UL);

    RHJ::meta = new RHJ::Meta[total];

    for (std::size_t i = 0UL; i < total; i++)
    {
        // Open file
        int fd = open(paths.front(), O_RDONLY);
        exit_if(fd < 0, paths.front());

        // Retrieve its size
        struct stat st;
        exit_if(fstat(fd, &st) < 0, paths.front());

        // Map file to memory
        RHJ::meta[i].mapping = mmap(nullptr, (RHJ::meta[i].mappingSize = st.st_size), PROT_READ, MAP_PRIVATE, fd, 0);
        exit_if(RHJ::meta[i].mapping == MAP_FAILED, paths.front());

        // Close file
        exit_if(close(fd) < 0, paths.front());

        // Deallocate uneccessary memory
        delete[] paths.front(); paths.erase(paths.begin());

        // Copy Metadata
        void * mapping_clone = RHJ::meta[i].mapping;

        void * rsptr = reinterpret_cast<void *>(&RHJ::meta[i].rowSize);
        std::memmove(rsptr, mapping_clone, sizeof(tuple_key_t));
        reinterpret_cast<tuple_key_t *&>(mapping_clone)++;

        void * csptr = reinterpret_cast<void *>(&RHJ::meta[i].columnSize);
        std::memmove(csptr, mapping_clone, sizeof(tuple_key_t));
        reinterpret_cast<tuple_key_t *&>(mapping_clone)++;
        
        // Create Index
        RHJ::meta[i].columns = new tuple_payload_t*[RHJ::meta[i].columnSize]; 
        for(tuple_key_t j = 0UL; j < RHJ::meta[i].columnSize; j++)
        {
            const tuple_key_t index = j * RHJ::meta[i].rowSize;

            tuple_payload_t * const mapping = &(reinterpret_cast<tuple_payload_t*>(RHJ::meta[i].mapping)[index]);

            RHJ::meta[i].columns[j] = mapping;
        }
    }

    for(std::size_t i = 0; i < total; i++)
    {
        exit_if(munmap(RHJ::meta[i].mapping, RHJ::meta[i].mappingSize) < 0, ("munmap No." + std::to_string(i)).c_str());
        
        delete[] RHJ::meta[i].columns;
    }

    delete[] RHJ::meta;

    return 0;
}
