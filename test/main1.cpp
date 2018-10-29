#include <histhash.hpp>
#include <relation.hpp>
#include <cstdlib>

#include<iostream>

#define R_SIZE 100
#define S_SIZE 110


int main(void) {

    srand(0);

    RHJ::Relation::Tuple * tArray_R = new RHJ::Relation::Tuple[R_SIZE];


    for (int i = 0; i < R_SIZE; i++) {
        tArray_R[i].key = i;

        tArray_R[i].payload = rand() % 20 + 1;
    }

    RHJ::Relation R(tArray_R, R_SIZE);


    RHJ::Relation::Tuple * tArray_S = new RHJ::Relation::Tuple[S_SIZE];

    for (int i = 0; i < S_SIZE; i++) {
        tArray_S[i].key = i;

        tArray_S[i].payload = rand() % 20 + 1;
    }

    RHJ::Relation S(tArray_S, S_SIZE);
    
    RHJ::List results = RHJ::Relation::RadixHashJoin(R, S);

    delete[] tArray_R;
    delete[] tArray_S;
}