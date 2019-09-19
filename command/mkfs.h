#ifndef MKFS_H
#define MKFS_H

#include <string.h>
#include <time.h>
#include <math.h>

#include "../var/globals.h"
#include "../fileManager/manager.h"
#include "../fileManager/mpartition.h"
#include "../fileManager/filesystem.h"

#define __USERTXT__ "1,G,root\n1,U,root,root,123\n"
#define __SUPERBLOCK__ sizeof(SuperBlock)
#define __JOURNAL__ sizeof(Journal)
#define __INODE__ sizeof(Inode)
#define __GENBLOCK__ sizeof(DirectoryBlock)

void initFileSystem()
{
    /* CREAR INODE ROOT Y PRIMER BLOQUE */
    Inode * root = newInode(_DIRECTORY_TYPE_);
    root->uid = 2;
    root->gid = 1;
    root->permission = 777;

    DirectoryBlock * rootDir = newDirectoryBlock(0, 0);
    root->block[0] = 0;
    
    updateInode(0, root);
    updateGenericBlock(0, rootDir);
    updateBitmap(0, '1', _INODE_);
    updateBitmap(0, '1', _BLOCK_);

    session.sb->free_blocks -= 1;
    session.sb->free_inodes -= 1;

    int next_inode = getNextFreeBit_Bitmap(_INODE_);
    int next_block = getNextFreeBit_Bitmap(_BLOCK_);
    session.sb->first_inode = next_inode;
    session.sb->first_block = next_block;

    updateSuperBlock();

    int no_file = fs_createFile("users.txt", root, 0);
    Inode * file = getInode(no_file);
    fs_writeFile(__USERTXT__, file, no_file, 0);
}

void settingBitmaps()
{
    for (int i = 0; i < session.sb->inodes_count; i++)
        updateBitmap(i, '0', _BLOCK_);
    for (int i = 0; i < session.sb->blocks_count; i++)
        updateBitmap(i, '0', _INODE_);   
}

void settingSuperBlock(Partition part)
{
    session.sb = (SuperBlock *) calloc(1, sizeof(SuperBlock));
    int initWorkspace = part.part_start;

    session.sb->filesystem = values.fs - '0';
    session.sb->magic = 61267;
    session.sb->mounting_count = 1;
    
    __time_t currentDate = time(NULL);
    struct tm * date = localtime(&currentDate);
    strftime(session.sb->mounted_date, sizeof(session.sb->mounted_date), "%d/%m/%y %H:%M", date);
    strftime(session.sb->unmounted_date, sizeof(session.sb->unmounted_date), "%d/%m/%y %H:%M", date);

    /* CALCULAR n */
    double n = 0;
    double structs_count = 0;
    double dividend = part.part_size - __SUPERBLOCK__;
    double divider = (session.sb->filesystem == 3) ? 
        __JOURNAL__ + __INODE__ + 3 * __GENBLOCK__ + 4 : 
        __INODE__ + 3 * __GENBLOCK__ + 4;
    structs_count = dividend / divider;
    n = floor(structs_count);

    /* SETTING STRUCTS COUNT AND SIZE */
    session.sb->inodes_count = n;
    session.sb->blocks_count = 3 * n;
    session.sb->free_inodes = n;
    session.sb->free_blocks = 3 * n;

    initWorkspace += (session.sb->filesystem == 3) ? 
        __SUPERBLOCK__ + n * __JOURNAL__ + n : 
        __SUPERBLOCK__ + n;
    
    session.sb->bm_inode_start = initWorkspace;
    session.sb->bm_block_start = initWorkspace + n;
    session.sb->inode_start = initWorkspace + n + 3 * n;
    session.sb->block_start = initWorkspace + n + 3 * n + n * __INODE__;

    session.sb->first_inode = 0;
    session.sb->first_block = 0;
    session.sb->inode_size = __INODE__;
    session.sb->block_size = __GENBLOCK__;
}

void exec_mkfs() {
    if (session.id_user >= 0)
    {
        printf(ANSI_COLOR_RED "[e] Una sesión está activa\n" ANSI_COLOR_RESET);
        return;
    }
    if (strlen(values.id) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Se requiere id de partición\n" ANSI_COLOR_RESET);
        return;
    }

    char id_x = values.id[2];
    char id_y = values.id[3] - '0';

    int x = getDiskById(id_x);
    int y = getPartById(id_y, x);

    if (y == _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s no está montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }

    MBR mbr = getMBR(disks_mount[x].path);
    Partition part;
    int a;
    for (a = 0; a < 4; a ++)
    {
        if (mbr.partitions[a].part_type == 'p')
        {
            if (strcmp(mbr.partitions[a].part_name, disks_mount[x].parts_mount[y].mount_name) == 0)
            {
                part = mbr.partitions[a];
                break;
            }
        }
    }

    if (x == 4) return;

    if (values.type != 'n')
        clearPartDisk(disks_mount[x].path, part.part_start, part.part_start + part.part_size);
    
    session.part_start = part.part_start;
    strcpy(session.path, disks_mount[x].path);
    settingSuperBlock(part);
    updateSuperBlock();
    clearJournals(part.part_start + __SUPERBLOCK__, session.sb->inodes_count);
    settingBitmaps();
    initFileSystem();

    session.part_start = 0;
    memset(session.path, 0, 300);
    session.id_user = -1;
}

#endif // MKFS_H}