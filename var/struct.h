#ifndef STRUCT_H
#define STRUCT_H

typedef struct Partition Partition;
typedef struct MBR MBR;
typedef struct EBR EBR;

typedef struct Superblock SuperBlock;
typedef struct Inode Inode;
typedef struct DirectoryBlock DirectoryBlock;
typedef struct ContentDirectory ContentDirectory;
typedef struct FileBlock FileBlock;
typedef struct PointerBlock PointerBlock;

typedef struct Mount Mount;
typedef struct PartMount PartMount;

typedef struct Values Values;
typedef struct SpaceDisk SpaceDisk;

struct Partition 
{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];
};

struct MBR 
{
    int size;
    char mbr_creation[16];
    int mbr_disk_signature;
    char fit;
    Partition partitions[4];
};

struct EBR 
{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char ebr_name[16];
};

struct SuperBlock
{
    int filesystem;
    int inodes_count;
    int blocks_count;
    int free_blocks;
    int free_inodes;
    char mounted_date[16];
    char unmounted_date[16];
    int mounting_count;
    int magic;
    int inode_size;
    int block_size;
    int first_inode;
    int first_block;
    int bm_inode_start;
    int bm_block_start;
    int inode_start;
    int block_start;
};

struct Inode
{
    int uid;
    int gid;
    int size;
    char last_date[16];
    char create_date[16];
    char modified_date[16];
    int block[15];
    char type;
    int permission;
};

struct ContentDirectory
{
    char name[12];
    int inode;
};

struct DirectoryBlock
{
    ContentDirectory content[4];
};

struct FileBlock
{
    char content[64];
};

struct PointerBlock
{
    int pointers[16];
};

struct PartMount
{
    char mount_type;
    int mount_start;
    int mount_size;
    int mount_id;
    char mount_name[16];
};

struct Mount
{
    char path[300];
    char letter;
    PartMount parts_mount[30];
};

struct Values
{
    char path[300];
    char del[300];
    char name[300];
    char id[300];
    char fit;
    char unit;
    char type;
    int size;
    int add;
};

struct SpaceDisk
{
    int start;
    int space;
    char type;
    int next;
    int prev;
};

#endif