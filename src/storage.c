#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "storage.h"


KEYS key[16];
//logic of how the IR data is interpritted
//start: 9ms High, 4.5ms Low
//logic 0: 560us High, 560us Low
//logic 1: 560us High, 1690us Low
//Tail: 560us High


//Hard Coded value of ON/OFF key
//Key 1: Start 0010 0000 1101 1111 0001 0000 1110 1111 Tail
//               2    0    D   F    1     0   E    F


//Functions to store the data into the key array if it was gives as a binry data
void storer(uint8_t Index,uint32_t data)
{
    //Start
    key[Index - 1].array[0] = 9000;
    key[Index - 1].array[1] = 4500;
    //data
    volatile uint8_t i;
    for(i = 0; i < 32; i++)
    {
        key[Index - 1].array[2 + (i * 2)] = 560;
        if(data & (0x80000000 >> i))
        {
            key[Index - 1].array[2 + (i * 2) + 1] = 1690;
        }
        else
        {
            key[Index - 1].array[2 + (i * 2) + 1] = 560;
        }
    }
    //Tail
    key[Index - 1].array[66] = 560;
}

