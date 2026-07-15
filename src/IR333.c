#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "storage.h"
#include "wait.h"

#define PB0 0x1

//Function to handle the transmission of the IR back to the TV
//Essentially uses GPTM Timer in PWM mode and uses the stored values to transmit back to the TV
/*
    Enables PORTB and uses AFSEL to pick the TIMER2

    TIMER2 is a pwm timer thats 16 bit periodic and with a 50% duty cycle
    TIMER2 is configured to generate a pwm specific for the IR LED

    TIMER3 is a 32 bit periodic timer that is a 1 shot and is used to wait for the 
    elapsed time store in the key structs array
*/
void xmit(uint8_t num)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R2;
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3;
    _delay_cycles(3);

    GPIO_PORTB_DEN_R |= PB0;
    GPIO_PORTB_DIR_R |= PB0;
    GPIO_PORTB_DR2R_R |= PB0;
    GPIO_PORTB_AMSEL_R &= ~PB0;
    GPIO_PORTB_AFSEL_R |= PB0;
    GPIO_PORTB_PCTL_R &= ~0xFFFFFFFF;
    GPIO_PORTB_PCTL_R |= 0x00000007;

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;
    TIMER2_CFG_R |= TIMER_CFG_16_BIT;
    TIMER2_TAMR_R |= TIMER_TAMR_TAAMS | TIMER_TAMR_TAMR_PERIOD;
    TIMER2_CTL_R &= ~TIMER_CTL_TAPWML;
    TIMER2_TAMR_R &= ~TIMER_TAMR_TACMR;
    TIMER2_TAILR_R = 1052;
    TIMER2_TAMATCHR_R = 526;

    TIMER3_CTL_R &= ~TIMER_CTL_TAEN;
    TIMER3_CFG_R |= TIMER_CFG_32_BIT_TIMER;
    TIMER3_TAMR_R |= TIMER_TAMR_TAMR_1_SHOT;

    volatile uint8_t i;
    for(i = 0; i < 67; i++)
    {
        volatile uint32_t Time = key[num - 1].array[i];
        if(i % 2 == 0)
        {
            TIMER2_CTL_R |= TIMER_CTL_TAEN;
        }
        else
        {
            TIMER2_CTL_R &= ~TIMER_CTL_TAEN;
        }

        // wait using Timer3
        TIMER3_CTL_R  &= ~TIMER_CTL_TAEN;
        TIMER3_TAILR_R = 40 * Time - 1;
        TIMER3_ICR_R   = TIMER_ICR_TATOCINT;
        TIMER3_CTL_R  |= TIMER_CTL_TAEN;
        while ((TIMER3_RIS_R & TIMER_RIS_TATORIS) == 0);
    }
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;
}
