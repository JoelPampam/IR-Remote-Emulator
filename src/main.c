#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "wait.h"
#include "uart0.h"
#include "commands.h"
#include "storage.h"

int main(void)
{
    //Intialize Timer for Uart 0
    initSystemClockTo40Mhz();
    initUart0();
    _delay_cycles(3);
    USER_DATA data;
    //Used to store the hard coded ON/OFF button IR data
    storer(1,0x20DF10EF);

    //Common Terminal Interface for communicating with the user
    while(1)
    {
        getsUart0(&data);
        commands(&data);
    }
	return 0;
}
