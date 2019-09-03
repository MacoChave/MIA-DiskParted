#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../var/globals.h";

Inode * createUsersFile()
{
    Inode * in = newInode(_FILE_TYPE_);
}

Inode * createRootDirectory()
{
    Inode * in = newInode(_DIRECTORY_TYPE_);
}

#endif // FILESYSTEM_H