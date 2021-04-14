#include "bibli.h"

typedef struct {
    unsigned char marker:8;
    unsigned int add:2;
    unsigned char size:4;
    unsigned char type:4;
    char data[15];
    int seq;
    unsigned char parity:8;
}Message; 
