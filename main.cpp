#include "histhash.h"
#include "relation.h"
#include <stdlib.h>

#include<iostream>

int main(void) {
    srand(0);
    Relation * R; 
    Relation * S;
    std::cout << "Started1" << std::endl;

    Tuple * tArray = new Tuple[10];

    std::cout << "Started2" << std::endl;
    for (int i = 0; i < 10; i++) {
        tArray[i].key = i;

        tArray[i].payload = rand() % 10 + 1;
    }
    std::cout << "Started3" << std::endl;

    R = new Relation(10, tArray);
    std::cout << "Started4" << std::endl;

    for (int i = 0; i < 10; i++) {
        tArray[i].key = i;

        tArray[i].payload = rand() % 10 + 1;
    }
    std::cout << "Started5" << std::endl;

    S = new Relation(10, tArray);
    
    RadixHashJoin(R, S);
}