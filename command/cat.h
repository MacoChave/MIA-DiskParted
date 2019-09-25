#ifndef CAT_H
#define CAT_H

#include <string.h>

#include "../var/globals.h"
#include "../fileManager/filesystem.h"

void exec_cat()
{
    if (session.id_user <= 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }
    if (strlen(values.file) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetro file requerido\n" ANSI_COLOR_RESET);
        return;
    }
    
    int no_current = fs_getDirectoryByPath(values.file, __READ__);
    Inode * current = getInode(no_current);
    char * text = fs_readFile(current);

    printf("%s\n", text);
}

#endif //CAT_H