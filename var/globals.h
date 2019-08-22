#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "struct.h"

/* TIPO DE COMANDO */
#define _EXIT_ -1
#define _EXEC_ 0
#define _MKDISK_ 1
#define _RMDISK_ 2
#define _FDISK_ 3
#define _MOUNT_ 4
#define _UNMOUNT_ 5
#define _REP_ 6
#define _PAUSE_ 7

/* TIPO DE PARAMETRO */
#define _SIZE_ 0
#define _PATH_ 1
#define _UNIT_ 2
#define _NAME_ 3
#define _TYPE_ 4
#define _FIT_ 5
#define _DELETE_ 6
#define _ADD_ 7
#define _ID_ 8

#define _ERROR_ -2
#define _COMMAND_ 0
#define _PARAM_ 1
#define _VALUE_ 2

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

Mount disks_mount[10];
Values values;
int command;

void initDisksMount()
{
    for (int i = 0; i < 10; i++)
    {
        disks_mount[i].letter = '0';
        memset(&disks_mount[i].path, 0, 300);
        for (int j = 0; j < 20; j++)
        {
            disks_mount[i].parts_mount[j].mount_type = '0';
            disks_mount[i].parts_mount[j].mount_start = 0;
            disks_mount[i].parts_mount[j].mount_size = 0;
            disks_mount[i].parts_mount[j].mount_id = 0;
            
            memset(disks_mount[i].parts_mount[j].mount_name, 0, 16);
        }
    }
}

void clearValues()
{
    memset(values.path, 0, 300);
    memset(values.del, 0, 300);
    memset(values.name, 0, 300);
    memset(values.id, 0, 300);
    values.fit = '0';
    values.unit = '0';
    values.type = '0';
    values.size = 0;
    values.add = 0;
    command = -1;
}

void clearDiskMounted (int i)
{
    for (int j = 0; j < 20; j++)
    {
        if (disks_mount[i].parts_mount[j].mount_id != 0)
            return;
    }
    disks_mount[i].letter = '0';
    memset(disks_mount[i].path, 0, 300);
}


void clearPartMounted (int i, int j)
{
    disks_mount[i].parts_mount[j].mount_type = '0';
    disks_mount[i].parts_mount[j].mount_start = 0;
    disks_mount[i].parts_mount[j].mount_size = 0;
    disks_mount[i].parts_mount[j].mount_id = 0;
    memset(disks_mount[i].parts_mount[j].mount_name, 0, 16);   
}

void clearAllPartMounted (int i)
{
    for (int j = 0; j < 30; j++)
        clearPartMounted(i, j);    
}
#endif