#ifndef STORAGE_H_
#define STORAGE_H_

#include <stdint.h>

//struct containing the elapsed time between each bit of data on the IR sensor
typedef struct
{
    uint32_t array[67];
} KEYS;

extern KEYS key[16];

void storer(uint8_t Index,uint32_t data);

#endif
