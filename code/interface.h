#ifndef INTERFACE_H
#define INTERFACE_H

#define CONSOLE_MESSAGE_COUNT 9
#define CONSOLE_MESSAGE_EMPTY "EMPTY"

typedef struct
{
  char msg[256];
  v4_t color;
} message_t;

#endif // INTERFACE_H