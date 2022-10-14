#ifndef DAEDALOS_EXEPARSER_TYPES_H
#define DAEDALOS_EXEPARSER_TYPES_H


#include <stdint.h>
#include <stddef.h>
//#include <types.h>

typedef int64_t address;
typedef unsigned char * ptr_buffer;

typedef enum exeTypes {PE, ELF} exeTypes;

typedef struct {
    address virtualAddress; //Virtual address as if the exe was loaded into memory
    address realAddress; //Real address this exists in our memory
} Address;

typedef struct {
    size_t virtualSize;
    size_t realSize;
} Size;

#endif //DAEDALOS_DE_TYPES_H
