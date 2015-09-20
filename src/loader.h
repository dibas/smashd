#ifndef LOADER_H
#define LOADER_H

#include "../../libwiiu/libwiiu/src/coreinit.h"
#include "../../libwiiu/libwiiu/src/types.h"
#include "../../libwiiu/libwiiu/src/vpad.h"

typedef struct
{
    int		type;
    char	path[0x27F];
} mount_source;

void _start();

void _entryPoint();
#endif /* LOADER_H */