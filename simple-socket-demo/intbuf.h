#include <stdint.h>

#define INTBUFSZ sizeof(int32_t)

union INTBUF {
    int32_t number;
    char data[INTBUFSZ];
};
typedef union INTBUF INTBUF;

int random1_500();