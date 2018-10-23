#include "histhash.h"
#include "relation.h"
#include <stdlib.h>

#include<iostream>

#define R_SIZE 100
#define S_SIZE 110


int main(void) {

    srand(0);

    Relation * R; 
    Relation * S;

    Tuple * tArray_R = new Tuple[R_SIZE];


    for (int i = 0; i < R_SIZE; i++) {
        tArray_R[i].key = i;

        tArray_R[i].payload = rand() % 20 + 1;
    }

    R = new Relation(R_SIZE, tArray_R);


    Tuple * tArray_S = new Tuple[S_SIZE];

    for (int i = 0; i < S_SIZE; i++) {
        tArray_S[i].key = i;

        tArray_S[i].payload = rand() % 20 + 1;
    }

    S = new Relation(S_SIZE, tArray_S);
    
    RadixHashJoin(R, S);

    delete[] tArray_R;
    delete[] tArray_S;

    delete S;
    delete R;
}