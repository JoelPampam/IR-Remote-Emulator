#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "commands.h"
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "wait.h"
#include "TSOP134.h"
#include "IR333.h"
#include "storage.h"


//Gets the command and send them to the corresponding command function
void commands(USER_DATA *data)
{
    char str[64];
    strcpy(str,data->buffer);
    char *token = strtok(str," ");
    char *datatoken = strtok(NULL,"");
    if(!token)
    {
        return;
    }
    if(strcmp(token,"xmit")== 0)
    {
        sendxmit(datatoken);
    }
    if(strcmp(token,"recv")== 0)
    {
        sendrecv(datatoken);
    }
}

//function for xmit and also recieve the key number entered by the user
void sendxmit(char *message)
{
    uint8_t num = atoi(message);
    putsUart0("Transmitting key - ");
    putsUart0(message);
    putsUart0("\r\n");
    xmit(num);
}

//Function to recv and also used to recieve the key number entered by the user
void sendrecv(char *message)
{
    uint8_t num = atoi(message);
    putsUart0("Recieved key - ");
    putsUart0(message);
    putsUart0("\r\n");
    recv(num);
}

