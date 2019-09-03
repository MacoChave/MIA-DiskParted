/**
 * @file manager.h
 * @author Marco Chávez (macochave.github.io)
 * @brief Contiene los métodos necesarios para el manejo de los archivos de discos
 * @version 0.1
 * @date 2019-08-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef MANAGER_H
#define MANAGER_H

#include <stdio.h>
#include "../var/globals.h"

/**
 * @brief Crea el directorio contenedor de un fichero
 * 
 * @param path 
 * @return int 
 */
int createDirectory(char path[])
{
    int i = 0;
    char cmd[300] = {0};
    char file_dir[300] = {0};
    char temp[300] = {0};
    strcat(cmd, "mkdir -m 777 -p \"");
    while (path[i] != '\0')
    {
        char directory[300] = {path[i], '\0'};
        if (path[i] != '/' && path[i] != '.')
            strcat(temp, directory);
        else if (values.path[i] == '/')
        {
            strcat(file_dir, temp);
            memset(temp, 0, 300);
            strcat(temp, "/");
        }
        else break;

        i++;
    }
    strcat(cmd, file_dir);
    strcat(cmd, "\"");
    return (system(cmd) == 0) ? 1 : 0;
}

/**
 * @brief Verifica la existencia de un archivo de disco
 * 
 * @param path 
 * @return int 
 */
int existDisk(char path[])
{
    FILE * file;
    file = fopen(path, "r");
    
    if (file != NULL)
    {
        fclose(file);
        return 1;
    }
    
    return 0;
}

/**
 * @brief Crea un archivo de disco
 * 
 * @param path 
 * @return int 
 */
int createDisk(char path[])
{
    FILE * file;
    file = fopen(path, "wb");
    
    if (file != NULL)
    {
        char buffer[1024];
        memset(buffer, 0, 1024);

        for (int i = 0; i < values.size / 1024; i++)
        {
            fwrite(buffer, sizeof(buffer), 1, file);
            fflush(file);
        }

        fclose(file);
        return 1;
    }

    return 0;
}

/**
 * @brief Crea una partición en un archivo de disco
 * 
 * @param path 
 * @param start 
 * @param size 
 * @return int 
 */
int clearPartDisk(char path[], int start, int size)
{
    FILE * file;
    file = fopen(path, "rb+");

    if (file != NULL)
    {
        char buffer[1024];
        memset(buffer, 0, 1024);

        for (int i = start; i < size / 1024; i++)
        {
            fwrite(buffer, sizeof(buffer), 1, file);
            fflush(file);
        }
        fclose(file);
        return 1;
    }

    return 0;
}

/**
 * @brief Elimina un archivo de disco
 * 
 * @param path 
 * @return int 
 */
int removeDisk(char path[])
{
    return (remove(path) == 0) ? 1 : 0;
}

/**
 * @brief Actualiza el MBR de un archivo de disco
 * 
 * @param path 
 * @param mbr 
 * @return int 
 */
int updateMBR(char path[], MBR mbr)
{
    FILE * file;
    file = fopen(path, "rb+");
    
    if (file != NULL)
    {
        fwrite(&mbr, sizeof(MBR), 1, file);
        
        fclose(file);
        return 1;
    }

    return 0;
}

/**
 * @brief obtiene el MBR de un archivo de disco
 * 
 * @param path 
 * @return MBR 
 */
MBR getMBR(char path[])
{
    MBR mbr;
    mbr.size = 0;
    FILE * file;
    file = fopen(path, "rb");

    if (file != NULL)
    {
        fread(&mbr, sizeof(MBR), 1, file);
        fclose(file);
        return mbr;
    }

    return mbr;
}

/**
 * @brief Actualiza el EBR de un archivo de disco
 * 
 * @param path 
 * @param ebr 
 * @param start 
 * @return int 
 */
int updateEBR(char path[], EBR ebr, int start)
{
    FILE * file;
    file = fopen(path, "rb+");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(&ebr, sizeof(EBR), 1, file);
        fclose(file);
        return 1;
    }

    return 0;
}

/**
 * @brief Obtiene el EBR de un archivo de disco
 * 
 * @param path 
 * @param start 
 * @return EBR 
 */
EBR getEBR (char path[], int start)
{
    EBR ebr;
    ebr.part_next = -1;
    ebr.part_size = -1;
    FILE * file;
    file = fopen(path, "rb");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fread(&ebr, sizeof(EBR), 1, file);
        fclose(file);
        return ebr;
    }

    return ebr;
}

int updateSuperBlock(char path[], SuperBlock * sb, int start)
{
    FILE * file;
    file = fopen(path, "rb+");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(sb, sizeof(SuperBlock), 1, file);
        fclose(file);
        return 1;
    }

    return 0;
}

SuperBlock * getSuperBlock(char path[], int start)
{
    SuperBlock * sb = newSuperBlock();
    FILE * file;
    file = fopen(path, "rb");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fread(sb, sizeof(SuperBlock), 1, file);
        fclose(file);

        return sb;
    }

    return sb;
}

int updateInode(char path[], Inode * in, int start)
{
    FILE * file;
    file = fopen(path, "rb+");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(in, sizeof(Inode), 1, file);
        fclose(file);
        return 1;
    }

    return 0;
}

Inode * getInode(char path[], int start)
{
    Inode * in = newInode(_DIRECTORY_TYPE_);
    FILE * file;
    file = fopen(path, "rb");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fread(in, sizeof(Inode), 1, file);
        fclose(file);

        return in;
    }

    return in;
}

int updateGenericBlock(char path[], void * block, int start)
{
    FILE * file;
    file = fopen(path, "rb+");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(block, sizeof(DirectoryBlock), 1, file);
        fclose(file);
        return 1;
    }

    return 0;
}

void * getGenericBlock(char path[], int type, int start)
{
    void * block = NULL;

    switch(type)
    {
        case _DIRECTORY_TYPE_:
            block = newDirectoryBlock();
            break;
        case _FILE_TYPE_:
            block = newFileBlock();
            break;
        case _POINTER_TYPE_:
            block = newPointerBlock();
            break;
    }

    FILE * file;
    file = fopen(path, "rb");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fread(block, sizeof(DirectoryBlock), 1, file);
        fclose(file);

        return block;
    }

    return block;
}

int clearJournals(char path[], int start, int count)
{
    FILE * file;
    file = fopen(path, "rb+");

    if (file != NULL)
    {
        Journal * jounal = newJournal();
        fseek(file, start, SEEK_SET);
        for (int i = start; i < count; i++)
        {
            fwrite(jounal, sizeof(Journal), 1, file);
            fclose(file);
        }
        return 1;
    }

    return 0;
}

int updateJournal(char path[], Journal * j, int start)
{
    FILE * file;
    file = fopen(path, "rb+");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(j, sizeof(Journal), 1, file);
        fclose(file);
        return 1;
    }

    return 0;
}

Journal * getJournal(char path[], int start)
{
    Journal * journal = newJournal();
    FILE * file;
    file = fopen(path, "rb");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fread(journal, sizeof(Journal), 1, file);
        fclose(file);

        return journal;
    }

    return journal;
}

int clearBitmaps(char path[], int start, int count)
{
    FILE * file;
    file = fopen(path, "rb+");

    if (file != NULL)
    {
        char state = (char *) calloc(1024, sizeof(char));
        fseek(file, start, SEEK_SET);

        for (int i = start; i < count; i++)
        {
            fwrite(state, sizeof(char), 1024, file);
            fclose(file);
        }
        return 1;
    }
    return 0;
}

int updateBitmap(char path[], char state, int start)
{
    FILE * file;
    file = fopen(path, "rb+");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(state, sizeof(char), 1, file);
        fclose(file);
        return 1;
    }

    return 0;
}

char getBitmap(char path[], int start)
{
    char state = '0';
    FILE * file;
    file = fopen(path, "rb");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fread(state, sizeof(char), 1, file);
        fclose(file);

        return state;
    }

    return state;
}
#endif