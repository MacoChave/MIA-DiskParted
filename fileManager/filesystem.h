#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../var/globals.h"

/**
 * @brief Leer archivo de texto en sistema de archivos en apuntadores indirectos
 * 
 * @param current 
 * @param level 
 * @return char* 
 */
char * fs_readFile_Indirect(PointerBlock * current, int level)
{
    char * text = (char *)calloc(279552, sizeof(char));

    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            if (current->pointers[i] == -1) continue;

            PointerBlock * pb = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);
            strcat(text, fs_readFile_Indirect(pb, level -1));
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

/**
 * @brief Leer archivo de texto en sistema de archivos
 * 
 * @param current 
 * @return char* 
 */
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
 * @brief Escribir archivo de texto en sistema de archivos en apuntadores indirectos
 * 
 * @param text 
 * @param current 
 * @param no_current 
 * @param level 
 * @return int 
 */
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

/**
 * @brief Escribir archivo de texto en sistema de archivos
 * 
 * @param text 
 * @param current 
 * @param no_current 
 * @param i 
 */
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
 * @brief Obtener archivo de texto en sistema de archivos desde archivo externo
 * 
 * @param cont 
 * @param current 
 * @param no_current 
 */
void generateContent_cont(char cont[300], Inode * current, int no_current)
{
    int i = 0;
    int size = 0;
    char text[64] = {0};
    FILE * file = fopen(cont, "r");

    if (file != NULL)
    {
        while(!feof(file) || i < 12){
            fgets(text, 64, file);
            size += strlen(text);
            if (strlen(text) > 0)
                fs_writeFile(text, current, no_current, i);
            memset(text, 0, 64);
            i++;
        }
        
        fclose(file);
    }
}

/**
 * @brief Generar archivo de texto en sistema de archivos
 * 
 * @param size 
 * @param current 
 * @param no_current 
 */
void generateContent_size(int size, Inode * current, int no_current)
{
    char text[64] = {0};

    int blocks_count = size / 64;
    if (size % 64 > 0)
        blocks_count++;
    
    for (int i = 0; i < 15; i++)
    {
        if (i < blocks_count)
        {
            for (int j = 0, a = 0; j < 64; j++, a++)
            {
                char aux[3] = {a + '0', '\0'};
                strcat(text, aux);
                if (a == 9) a = -1;
            }

            fs_writeFile(text, current, no_current, i);
            memset(text, 0, 64);
        }
/*         else
            fs_writeFile(text, current, no_current, i); */
            // TODO: Limpiar resto del contenido sin ustilizar
    }
}

/**
 * @brief Buscar directorios por nombre en apuntadores indirectos
 * 
 * @param name 
 * @param bp 
 * @param level 
 * @return int 
 */
int fs_getDirectoryByName_Indirect(char name[], PointerBlock * bp, int level)
{
    int no_inode = -1;
    for (int i = 0; i < 16; i++)
    {
        if (bp->pointers[i] < 0) continue;
        if (level > 1){
            no_inode = fs_getDirectoryByName_Indirect(name, bp, level - 1);
            if (no_inode > 0) return no_inode;
            else continue;
        }
    
        DirectoryBlock * bd = (DirectoryBlock *) getGenericBlock(bp->pointers[i], _DIRECTORY_TYPE_);
        for (int j = 0; j < 4; j++)
        {
            if (bd->content[j].inode < 0) continue;
            if (strcmp(name, bd->content[j].name) == 0)
                return bd->content[j].inode;
        }
    }
    return no_inode;
}

/**
 * @brief Buscar directorios por nombre
 * 
 * @param name 
 * @param current 
 * @return int 
 */
int fs_getDirectoryByName(char name[], Inode * current)
{
    int no_inode = -1;
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (current->block[i] < 0) continue;
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            DirectoryBlock * bd = (DirectoryBlock *) getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0) continue;
                if (strcmp(name, bd->content[j].name) == 0)
                    return bd->content[j].inode;
            }
        }
        else
        {
            /*  BLOQUES INDIRECTOS */
            PointerBlock * bp = (PointerBlock *) getGenericBlock(current->block[i], _POINTER_TYPE_);
            no_inode = fs_getDirectoryByName_Indirect(name, bp, level);
            if (no_inode > 0)
                return no_inode;
            level++;
        }
    }

    return no_inode;
}

/**
 * @brief Crear directorio en sistema de archivos en apuntadores indirectos
 * 
 * @param name 
 * @param current 
 * @param no_upper 
 * @param no_current 
 * @param level 
 * @return int 
 */
int fs_createDirectory_Indirect(char name[], PointerBlock * current, int no_upper, int no_current, int level)
{
    int no_inode = -1;

    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            PointerBlock * new_bp = newPointerBlock();
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, new_bp);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                new_bp = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);

            no_inode = fs_createDirectory_Indirect(name, new_bp, no_upper, current->pointers[i], level - 1);
            if (no_inode > 0) return no_inode;
        }
        else
        {
            DirectoryBlock * bd = newDirectoryBlock(_EMPTY_, _EMPTY_);
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, bd);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                bd = (DirectoryBlock *) getGenericBlock(current->pointers[i], _DIRECTORY_TYPE_);
            
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_DIRECTORY_TYPE_);
                    DirectoryBlock * new_bd = newDirectoryBlock(session.sb->first_inode, no_upper);
                    next->block[0] = session.sb->first_block;
                    bd->content[j].inode = session.sb->first_inode;
                    strcpy(bd->content[j].name, name);
                    updateGenericBlock(current->pointers[i], bd);
                    updateInode(_EMPTY_, next);
                    updateGenericBlock(_EMPTY_, new_bd);
                    updateBitmap(bd->content[j].inode, '1', _INODE_);
                    updateBitmap(next->block[0], '1', _BLOCK_);
                    session.sb->free_inodes -= 1;
                    session.sb->free_blocks -= 1;
                    session.sb->first_inode = getNextFreeBit_Bitmap(_INODE_);
                    session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                    updateSuperBlock();
                    return bd->content[j].inode;
                }
            }
        }
    }
    return no_inode;
}

/**
 * @brief Crear directorio en sistema de archivos
 * 
 * @param name 
 * @param current 
 * @param no_current 
 * @return int 
 */
int fs_createDirectory(char name[], Inode * current, int no_current)
{
    int no_next = -1;
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            DirectoryBlock * bd = newDirectoryBlock(_EMPTY_, _EMPTY_);
            if (current->block[i] == -1)
            {
                current->block[i] = session.sb->first_block;
                updateInode(no_current, current);
                updateGenericBlock(_EMPTY_, bd);
                updateBitmap(current->block[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
            }
            else
                bd = (DirectoryBlock *) getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
            
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_DIRECTORY_TYPE_);
                    DirectoryBlock * new_bd = newDirectoryBlock(session.sb->first_inode, no_current);
                    next->block[0] = session.sb->first_block;

                    bd->content[j].inode = session.sb->first_inode;
                    strcpy(bd->content[j].name, name);
                    updateGenericBlock(current->block[i], bd);
                    updateInode(_EMPTY_, next);
                    updateGenericBlock(_EMPTY_, new_bd);
                    updateBitmap(bd->content[j].inode, '1', _INODE_);
                    updateBitmap(next->block[0], '1', _BLOCK_);
                    session.sb->free_inodes -= 1;
                    session.sb->free_blocks -= 1;
                    session.sb->first_inode = getNextFreeBit_Bitmap(_INODE_);
                    session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                    updateSuperBlock();
                    return bd->content[j].inode;
                }
            }
        }
        else
        {
            PointerBlock * bp = newPointerBlock();
            if (current->block[i] == -1)
            {
                current->block[i] = session.sb->first_block;
                updateInode(no_current, current);
                updateGenericBlock(_EMPTY_, bp);
                updateBitmap(current->block[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                bp = (PointerBlock *) getGenericBlock(current->block[i], _POINTER_TYPE_);
            
            no_next = fs_createDirectory_Indirect(name, bp, no_current, current->block[i], level);
            if (no_next > 0)
                return no_next;
            level++;
        }
    }

    return no_next;
}

/**
 * CREAR INODO DE ARCHIVO
 * */

/**
 * @brief Crear archivo en sistema de archivos
 * 
 * @param name 
 * @param current 
 * @param no_current 
 * @param level 
 * @return int 
 */
int fs_createFile_Indirect(char name[], PointerBlock * current, int no_current, int level)
{
    int no_inode = -1;

    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            PointerBlock * new_bp = newPointerBlock();
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, new_bp);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                new_bp = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);

            no_inode = createInodeFile_Indirect(name, new_bp, current->pointers[i], level - 1);
            if (no_inode > 0) return no_inode;
        }
        else
        {
            DirectoryBlock * new_bd = newDirectoryBlock(_EMPTY_, _EMPTY_);
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, new_bd);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                new_bd = (DirectoryBlock *) getGenericBlock(current->pointers[i], _DIRECTORY_TYPE_);
            
            for (int j = 0; j < 4; j++)
            {
                if (new_bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_FILE_TYPE_);
                    new_bd->content[j].inode = session.sb->first_inode;
                    strcpy(new_bd->content[j].name, name);
                    updateGenericBlock(current->pointers[i], new_bd);
                    saveInode(_EMPTY_, next);
                    updateBitmap(new_bd->content[j].inode, '1', _INODE_);
                    session.sb->free_inodes -= 1;
                    session.sb->first_inode = getNextFreeBit_Bitmap(_INODE_);
                    updateSuperBlock();
                    return new_bd->content[j].inode;
                }
            }
        }
    }
    return no_inode;
}

/**
 * @brief Crear archivo en sistema de archivos
 * 
 * @param name 
 * @param current 
 * @param no_current 
 * @return int 
 */
int fs_createFile(char name[], Inode * current, int no_current)
{
    int no_next = -1;
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            DirectoryBlock * bd = NULL;
            if (current->block[i] == -1)
            {
                bd = newDirectoryBlock(_EMPTY_, _EMPTY_);
                current->block[i] = session.sb->first_block;
                updateInode(no_current, current);
                updateGenericBlock(_EMPTY_, bd);
                updateBitmap(current->block[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
            }
            else
                bd = (DirectoryBlock *) getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
            
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_FILE_TYPE_);
                    bd->content[j].inode = session.sb->first_inode;
                    strcpy(bd->content[j].name, name);
                    updateGenericBlock(current->block[i], bd);
                    updateInode(_EMPTY_, next);
                    updateBitmap(bd->content[j].inode, '1', _INODE_);
                    session.sb->free_inodes -= 1;
                    int next_inode = getNextFreeBit_Bitmap(_INODE_);
                    session.sb->first_inode = next_inode;
                    updateSuperBlock();
                    return bd->content[j].inode;
                }
            }
        }
        else
        {
            PointerBlock * bp = newPointerBlock();
            if (current->block[i] == -1)
            {
                current->block[i] = session.sb->first_block;
                updateInode(no_current, current);
                updateGenericBlock(current->block[i], bp);
                updateBitmap(current->block[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                bp = (PointerBlock *) getGenericBlock(current->block[i], _POINTER_TYPE_);
            
            no_next = fs_createFile_Indirect(name, bp, current->block[i], level);
            if (no_next > 0)
                return no_next;
            level++;
        }
    }

    return no_next;
}

/**
 * VERIFICAR PERMISOS
 * */

/**
 * @brief Verificar permisos dentro del sistema de archivos
 * 
 * @param uid 
 * @param gid 
 * @param inode_permission 
 * @param operation 
 * @return int 
 */
int fs_checkPermission(int uid, int gid, int inode_permission, char operation)
{
    if (strcmp(permissions[session.id_user].group, "root") == 0) return 1;
    
    char str_perm[4];
    sprintf(str_perm, "%d", inode_permission);
    int u = str_perm[0] - '0';
    int g = str_perm[1] - '0';
    int o = str_perm[2] - '0';
    int sameGroup = (session.id_group == gid) ? 1 : 0;
    int userIsOwner = (session.id_user == uid) ? 1 : 0;

    switch (operation)
    {
        case _CREATE_: // > 6
            if (o >= 6) return 1;
            if (sameGroup && g >= 6) return 1;
            if (userIsOwner && u >= 6) return 1;
            break;
        case _READ_: // > 4
            if (o >= 4) return 1;
            if (sameGroup && g >= 4) return 1;
            if (userIsOwner && u >= 4) return 1;
            break;
        case _UPDATE_: // >= 6
            if (o >= 6) return 1;
            if (sameGroup && g >= 6) return 1;
            if (userIsOwner && u >= 6) return 1;
            break;
        case _DELETE_: // >= 6
            if (o >= 6) return 1;
            if (sameGroup && g >= 6) return 1;
            if (userIsOwner && u >= 6) return 1;
            break;
        default: 
            break;
    }
    return 0;
}
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