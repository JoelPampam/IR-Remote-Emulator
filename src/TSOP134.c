#include "tm4c123gh6pm.h"
#include "TSOP134.h"
#include <stdint.h>
#include "storage.h"

#define PE2 0x4

volatile uint32_t t0 = 0;
volatile uint8_t KeyIndex = 0;
volatile uint32_t arrayi = 0;

//Port E ISR: Used to handle the edge interrupt of the IR waveform
//If Theres is an edge interrupt store the elapsed times into an array in the struct named key for that specific key
void PortE_ISR(void)
{
    if(arrayi >= 68) 
    {
        GPIO_PORTE_IM_R &= ~PE2;
        return;
    }
    if(GPIO_PORTE_MIS_R & PE2)
    {
        volatile uint32_t t1 = TIMER0_TAV_R;
        volatile uint32_t elapsed = (t1 - t0) / 40;
        if(arrayi > 0) 
        {
            key[KeyIndex - 1].array[arrayi - 1] = elapsed;
        }
        arrayi++;
        t0 = t1;
    }
    GPIO_PORTE_ICR_R |= PE2;
}


//Function to Handle setting up and configuring GPIO E AND TIMER 0
/*
    GPIO PORT E
    -Enables PORTE Pin 2
    -Makes the direction be input
    -Enables the Internal Pull up resistor
    -Enables it to be edge sensitive
    -Enables Gpio interrupt mask
    -Used for TSOP134

    Timer 0
    -32 bit timer
    -Periodic
    -Count down
    -Loads value of 0 to t0
    -Used to calculate elapsed time
*/
void recv(uint8_t num)
{
    KeyIndex = num;
    arrayi = 0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;
    _delay_cycles(3);

    GPIO_PORTE_DEN_R |= PE2;
    GPIO_PORTE_DIR_R &= ~PE2;
    GPIO_PORTE_PUR_R |= PE2;
    GPIO_PORTE_IS_R &= ~PE2;
    GPIO_PORTE_IBE_R |= PE2;

    GPIO_PORTE_ICR_R |= PE2;
    GPIO_PORTE_IM_R |= PE2;


    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;
    TIMER0_CFG_R |= TIMER_CFG_32_BIT_TIMER;
    TIMER0_TAMR_R |= TIMER_TAMR_TAMR_PERIOD;
    TIMER0_TAMR_R |= TIMER_TAMR_TACDIR;
    TIMER0_TAILR_R = 0xFFFFFFFF;
    TIMER0_TAV_R = 0;
    TIMER0_CTL_R |= TIMER_CTL_TAEN;

    t0 = TIMER0_TAV_R;
    //Enables Port E Interrupt
    NVIC_EN0_R |= 0x10;
}
