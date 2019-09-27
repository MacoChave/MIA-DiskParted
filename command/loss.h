#ifndef LOSS_H
#define LOSS_H

#include <string.h>

#include "../var/globals.h"
#include "../fileManager/filesystem.h"
#include "../fileManager/manager.h"

void lossPartition()
{
    int size = session.part_size;
    size -= __SUPERBLOCK__ - session.sb->inodes_count * __JOURNAL__;

    clearPartDisk(session.path, session.sb->bm_inode_start, size);
}

void exec_loss()
{
    if (strcmp(permissions[session.id_user].group, "root") != 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }

    lossPartition();
    printf(ANSI_COLOR_GREEN "[i] Se ha simulado la pérdida con éxito\n" ANSI_COLOR_RESET);
}

#endif //LOSS_H