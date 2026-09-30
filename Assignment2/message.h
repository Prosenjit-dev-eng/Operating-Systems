#ifndef MESSAGE_H
#define MESSAGE_H

struct message
{
    long message_type;
    char message_text[256];
};

#endif