#ifndef REN_H
#define REN_H

#include <string.h>

#include "../var/globals.h"
#include "../fileManager/filesystem.h"

void exec_ren()
{
    if (session.id_user <= 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }
    if (strlen(values.path) == 0 || strlen(values.name) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    Journal * journal = newJournal();
    journal->command = _REN_;
    strcpy(journal->str_1, values.path);
    strcpy(journal->str_2, values.name);
    journal->owner = session.id_user;

    int no_current = fs_getDirectoryByPath(values.path, __UPDATE__);
    if (no_current <= 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay acceso al directorio\n" ANSI_COLOR_RESET);
        return;
    }
}

#endif //REN_H