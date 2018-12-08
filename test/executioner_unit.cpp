
#include <executioner.hpp>
#include <iostream>
#include <cstring>

int main()
{
    RHJ::Executioner::createMetadata();

    std::size_t query_max = 256UL; char * query_str = new char[query_max];

    for (;;)
    {
        ssize_t query_len;
        if ((query_len = getline(&query_str, &query_max, stdin)) < 0)
        {
            if (errno == EINVAL || errno == ENOMEM)
            {
                std::perror(query_str); std::exit(errno);
            }

            break;
        }

        if (query_str[query_len - 1] == '\n')
            query_str[--query_len] = '\0';

        if (!std::strcmp(query_str, "F"))
            continue;

        RHJ::Executioner demios;

        #if !defined (__QUIET__)
            std::cerr << "\nExecuting..  " << query_str << std::endl << std::endl;
        #endif

        std::vector<std::string> checksums = demios.execute(RHJ::Query(query_str));
        
        for (const auto &checksum : checksums)
        {
            std::cout << checksum;
            if (&checksum != &checksums.back())
                std::cout << ' ';
        }

        std::cout << std::endl;
    }

    delete[] query_str;

    RHJ::Executioner::deleteMetadata();

    return 0;
}
