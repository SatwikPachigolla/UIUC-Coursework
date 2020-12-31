/**
 * finding_filesystems
 * CS 241 - Fall 2020
 */
#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

//forward declarations
void* helper(file_system* fs, inode* parent, uint64_t index);

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string, "Free blocks: %zd\n"
                            "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    // Thar she blows!
    inode* node;
    if ( !(node = get_inode(fs, path)) ) {
        //set no entity error
        errno = ENOENT;
        return -1;
    }
    node->mode = ((node->mode >> RWX_BITS_NUMBER) << RWX_BITS_NUMBER) | new_permissions;//RWX 9 in .h
    //https://stackoverflow.com/questions/3523442/difference-between-clock-realtime-and-clock-monotonic
    clock_gettime(CLOCK_REALTIME, &(node->ctim));
    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    inode* node;
    if ( !(node = get_inode(fs, path)) ) {
        //set no entity error
        errno = ENOENT;
        return -1;
    }
    if (owner != (uid_t)-1){
        node->uid = owner;
    }
    if (group != (gid_t)-1){
        node->gid = group;
    }
    clock_gettime(CLOCK_REALTIME, &(node->ctim));
    return 0;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    inode* node;
    if( (node = get_inode(fs, path)) ){
        clock_gettime(CLOCK_REALTIME, &(node->ctim));
        return NULL;
    }
    const char* filename;
    inode* parent = parent_directory(fs, path, &filename);//undefined for trailing slash
    if (!parent || !is_directory(parent) || !valid_filename(filename)){
        return NULL;
    }
    inode_number fui = first_unused_inode(fs);
    if ( fui == -1 ){
        return NULL;
    }
    inode* result = fs->inode_root + fui;
    init_inode(parent, result);
 
    minixfs_dirent init;
    init.inode_num = first_unused_inode(fs);
    init.name = (char*)filename;
    uint64_t index = parent->size/sizeof(data_block);
    if ( index >= NUM_DIRECT_BLOCKS ){
        return NULL;
    }
    int mod = parent->size % sizeof(data_block);
    if ( !mod && add_data_block_to_inode(fs, parent) == -1 ) {
        return NULL;
    }
    void* block = helper(fs, parent, index) + mod;//come back to this
    memset(block, 0, sizeof(data_block));
    make_string_from_dirent(block, init);

    parent->size += MAX_DIR_NAME_LEN;
    clock_gettime(CLOCK_REALTIME, &(parent->mtim));
    return result;
}

/**
 * The data map records used blocks and is stored at the end of the filesystem.
 * It is an array that stores 1 if a particular block is used and 0 otherwise.
 * The data map is stored at the end so that the filesystem can be easily
 * resized in the future - currently this behavior is not supported
 */
ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
        ssize_t counter = 0;
        char* dataMap = GET_DATA_MAP(fs->meta);
        for (uint64_t i = 0; i < fs->meta->dblock_count; i++) {
            if (dataMap[i]) {
                counter+=1;
            }
        }
        char* block_str = block_info_string(counter);
        long blockStringLength = strlen(block_str);
        if ((*off) > blockStringLength)  {
            return 0;
        }
        size_t result = count;
        if ((long)count > blockStringLength - *off) {
            result = blockStringLength - *off;
        }
        memmove(buf, block_str + *off, result);
        *off += result;
        return result;
    }
    errno = ENOENT;
    return -1;
}
/*
*  Note: You are responsible for allocating blocks of memory for the nodes. You
 *  might find the following functions useful:
 *    minixfs_min_blockcount
 *    add_data_block_to_inode
 *    add_single_indirect_block
 *    add_data_block_to_indirect_block
 */
ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    // X marks the spot
    uint64_t maxCap = (NUM_DIRECT_BLOCKS + NUM_INDIRECT_BLOCKS)*sizeof(data_block);
    if (maxCap < count + *off) {
        errno = ENOSPC;
        return -1;
    }

    int blocks = (sizeof(data_block) + count + *off - 1)/sizeof(data_block);
    if (minixfs_min_blockcount(fs, path, blocks) == -1) {
        errno = ENOSPC;
        return -1;
    }
    inode* writeFile = get_inode(fs, path);
    
    size_t mod = *off % sizeof(data_block);
    uint64_t size = count;
    if(count > sizeof(data_block) - mod){
        size = sizeof(data_block) - mod;
    }

    uint64_t index = *off / sizeof(data_block);
    void* block = helper(fs, writeFile, index) + mod;
    memcpy(block, buf, size);
    uint64_t counter = size;
    *off += size;
    while (counter < count) {
        index+=1;
        if(sizeof(data_block) < count - counter){
            size = sizeof(data_block);
        }
        else {
            size = count - counter;
        }
        block = helper(fs, writeFile, index);
        memcpy(block, buf + counter, size);
        *off += size;
        counter += size;
    }

    if( writeFile->size < count + *off ){
        writeFile->size  = count + *off;
    }

    clock_gettime(CLOCK_REALTIME, &(writeFile->atim));
    clock_gettime(CLOCK_REALTIME, &(writeFile->mtim));
    return counter;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);
    // 'ere be treasure!
    if(!buf){
        errno = ENOENT;
        return -1;
    }
    inode* readFile;
    if( !(readFile = get_inode(fs, path)) ){
        errno = ENOENT;
        return -1;
    }

    if ( readFile->size < (uint64_t)(*off) ) {
        return 0;
    }
    if( count > readFile->size-(*off) ){
        count = readFile->size-(*off);
    }
    
    size_t mod = (*off)%sizeof(data_block);
    uint64_t size = count;
    if( sizeof(data_block) - mod < count ){
        size =  sizeof(data_block) - mod;
    }

    uint64_t index = (*off)/sizeof(data_block);
    void* block = helper(fs, readFile, index) + mod;
    memcpy(buf, block, size);
    uint64_t counter = size;
    *off += size;
    while (counter < count) {
        index+=1;
        if( sizeof(data_block) < count-counter) {
            size = sizeof(data_block);
        }
        else {
            size = count - counter;
        }
        block = helper(fs, readFile, index);
        memcpy(buf+counter, block, size);
        counter+=size;
        (*off)+=size;
    }

    clock_gettime(CLOCK_REALTIME, &(readFile->atim));
    return counter;
}

void* helper(file_system* fs, inode* param, uint64_t index){
    if(index < NUM_DIRECT_BLOCKS){
        return (fs->data_root + param->direct[index]);
    } 
    else {
        return (fs->data_root + ((data_block_number*)(fs->data_root + param->indirect))[index-NUM_DIRECT_BLOCKS]);
    }
}