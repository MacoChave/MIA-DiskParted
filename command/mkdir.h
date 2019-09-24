#ifndef MKDIR_H
#define MKDIR_H

#include <string.h>
#include "../var/globals.h"
#include "../fileManager/filesystem.h"

void exec_mkdir()
{
    if (session.id_user <= 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }
    if (strlen(values.path) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetro path requerido\n" ANSI_COLOR_RESET);
        return;
    }

    int result = fs_createDirectoryFromPath(values.path, values.recursive, _DIRECTORY_TYPE_, __CREATE__);

    if (result > 0)
    {
        Journal * journal = newJournal();
        journal->command = _MKDIR_;
        journal->owner = session.id_user;
        strcpy(journal->str_1, values.path);
        fs_backup(journal);
        printf(ANSI_COLOR_GREEN "[i] Se ha creado el directorio %s\n" ANSI_COLOR_RESET, values.path);
    }
    else 
        printf(ANSI_COLOR_RED "[i] No se ha creado el directorio %s\n" ANSI_COLOR_RESET, values.path);
}

#endif //MKDIR_H