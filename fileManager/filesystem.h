#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../var/globals.h"

/**
 * LEER ARCHIVO DE TEXTO
 * */
char * fs_readFile_Indirect(PointerBlock * current, int level)
{
    char * text = (char *)calloc(279552, sizeof(char));

    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            if (current->pointers[i] == -1) continue;

            PointerBlock * pb = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);
            strcat(text, fs_readFile_Indirect(pb, level -1);
        }
        else 
        {
            if (current->pointers[i] == -1) continue;

            FileBlock * fb = (FileBlock *)getGenericBlock(current->pointers[i], _FILE_TYPE_);
            strcat(text, fb->content);
        }
    }
    return text;
}

char * fs_readFile(Inode * current)
{
    int level = 1;
    char * text = (char *)calloc(280320, sizeof(char));

    for (int i = 0; i < 15; i++)
    {
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            if (current->block[i] == -1) continue;
            
            FileBlock * fb = (FileBlock *) getGenericBlock(current->block[i], _FILE_TYPE_);
            strcat(text, fb->content);
        }
        else 
        {
            /* BLOQUES INDIRECTOS */
            if (current->block[i] == -1) continue;

            PointerBlock * pb = NULL;
            pb = (PointerBlock *) getGenericBlock(current->block[i], _POINTER_TYPE_);
            strcat(text, fs_readFile_Indirect(pb, level));

            level++;
        }
    }
    return text;
}
/**
 * ESCRIBIR ARCHIVO DE TEXTO
 * */
int fs_writeFile_Indirect(char text[], PointerBlock * current, int no_current, int level)
{
    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            PointerBlock * pb = NULL;
            if (current->pointers[i] == -1)
            {
                pb = newPointerBlock();
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, pb);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else 
                pb = (PointerBlock *)getGenericBlock(current->pointers[i], _POINTER_TYPE_);
            
            int result = fs_writeFile_Indirect(text, pb, current->pointers[i], level -1);
            if (result) return result;
        }
        else 
        {
            FileBlock * fb = NULL;
            if (current->pointers[i] == -1)
            {
                fb = (FileBlock *) newFileBlock();
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, fb);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
            }
            else 
            {
                fb = (FileBlock *)getGenericBlock(current->pointers[i], _FILE_TYPE_);
                memset(fb->content, 0, 64);
                updateGenericBlock(current->pointers[i], fb);
            }

            strcpy(fb->content, text);
            updateGenericBlock(current->pointers[i], fb);

            return 1;
        }
        return 0;
    }
}

void fs_writeFile(char text[], Inode * current, int no_current, int i)
{
    int level = 1;

    if (i < 12)
    {
        /* BLOQUES DIRECTOS */
        FileBlock * bf = NULL;
        if (current->block[i] == -1)
        {
            bf = newFileBlock();
            current->block[i] = session.sb->first_block;
            updateInode(no_current, current);
            updateGenericBlock(_EMPTY_, bf);
            updateBitmap(current->block[i], '1', _BLOCK_);
            session.sb->free_blocks -= 1;
            session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
            updateSuperBlock();
        }
        else
        {
            bf = (FileBlock *)getGenericBlock(current->block[i], _BLOCK_);
            memset(bf->content, 0, 64);
        }
        strcpy(bf->content, text);
        updateGenericBlock(current->block[i], bf);
    }
    else 
    {
        /* BLOQUES INDIRECTOS */
        PointerBlock * pb = NULL;
        if (current->block[i] == -1)
        {
            pb = newPointerBlock();
            current->block[i] = session.sb->first_block;
            updateInode(no_current, current);
            updateGenericBlock(current->block[i], pb);
            updateBitmap(current->block[i], '1', _BLOCK_);
            session.sb->free_blocks -= 1;
            session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
            updateSuperBlock();
        }
        else 
            pb = (PointerBlock *)getGenericBlock(current->block[i], _POINTER_TYPE_);
        
        fs_writeFile_Indirect(text, pb, current->block[i], level);
        level++;
    }
}
/**
 * OBTENER O GENERAR CONTENIDO PARA ARCHIVO DE TEXTO
 * */

/**
 * BUSCAR INODO POR NOMBRE
 * */

/**
 * CREAR INODO DE DIRECTORIO
 * */

/**
 * CREAR INODO DE ARCHIVO
 * */

/**
 * VERIFICAR PERMISOS
 * */

/**
 * CREAR INODO DESDE UNA RUTA
 * */

/**
 * BUSCAR INODO POR RUTA
 * */

/**
 * 
 * */

#endif // FILESYSTEM_H