#include "rdxl.hpp"
#include "relation.hpp"

#define N (2000000)

int main()
{
    RHJ::List list;
    for (int i = 0; i < N; i++) {
        RHJ::Result r = {i, i};
        list.append(r);
    }

    return 0;
}