#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "uart0.h"

void commands(USER_DATA *data);
void sendxmit(char *message);
void sendrecv(char *message);

#endif
