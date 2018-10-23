#include "rdxl.hpp"
#include "relation.h"

#define N (2000000)

int main()
{
    RDXList list;
    for(int i = 0; i < N; i++) {
        Result r(i, i);
        list.append(r);
    }
    return 0;
}