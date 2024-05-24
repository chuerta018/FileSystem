#define FUSE_USE_VERSION 26
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <fuse.h>
#include <string.h>
#include <iostream>
#include <cerrno>
#include <filesystem>
#include "../libWad/Wad.h"


static int getattr_callback(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));
// copied from https://engineering.facile.it/blog/eng/write-filesystem-fuse/
//replace with my function calls
    if(((Wad*)fuse_get_context()->private_data)->isDirectory(path)) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }
    if(((Wad*)fuse_get_context()->private_data)->isContent(path)) {
        stbuf->st_mode = S_IFREG | 0644;
        stbuf->st_nlink = 1;
        int size = ((Wad*)fuse_get_context()->private_data)->getSize(path);
        if(size == -1) {
            //File not found
            return 0;
        }
        stbuf->st_size = size;
        return 0;
    }
    return -ENOENT;

}
static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    if(!((Wad*)fuse_get_context()->private_data)->isDirectory(path)) {
        return -ENOENT;
    }
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

//directory children
    auto* children= new std::vector<std::string>();
    //call our getDirectory 
    int numC = ((Wad*)fuse_get_context()->private_data)->getDirectory(path,children);
    if(numC == -1) {
        return -ENOENT;
    }
    for(auto child : *children) {
        filler(buf, child.c_str(), NULL, 0);
    }
    return 0;
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
    return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {

//check if file has size
    size_t len = ((Wad*)fuse_get_context()->private_data)->getSize(path);
    if(len == -1) {
        return -ENOENT;
    }
//check if path is valid
    if(!((Wad*)fuse_get_context()->private_data)->isContent(path)) {
        return -ENOENT; 
    }

    // call get contents 
    int a = ((Wad*)fuse_get_context()->private_data)->getContents(path, buf, len - offset, offset);
    
    if(a == -1) { // error 
        return -ENOENT;
    }
    //return # of bytes
    return a;
}

static int mknod_callback(const char* path, mode_t mode, dev_t rdev){

        ((Wad*)fuse_get_context()->private_data)->createFile(path);

return 0;
}

static int write_callback(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
   
    size_t len = ((Wad*)fuse_get_context()->private_data)->getSize(path);
    if(len != 0 ) {
        return -ENOENT;
    }
    if(!((Wad*)fuse_get_context()->private_data)->isContent(path)) {
        return -ENOENT; //not a file or file not found
    }

    int a = ((Wad*)fuse_get_context()->private_data)->writeToFile(path, buf, size, offset);
    
    if(a == -1) {
        return -ENOENT;
    }
    return a;
}

static int mkdir_callback(const char * path, mode_t mode){

        ((Wad*)fuse_get_context()->private_data)->createDirectory(path);
        return 0;
}

static struct fuse_operations operations = {
        .getattr = getattr_callback,
        .mknod = mknod_callback,
        .mkdir = mkdir_callback,
        .open = open_callback,
        .read = read_callback,
        .write = write_callback,
        .readdir = readdir_callback,
    
};

int main(int argc, char *argv[])
{
    if(argc<3)
    {
        std::cout<<"Not enough paramters"<<std::endl;
        exit(EXIT_SUCCESS);
    }

    std::string wadPath = argv[argc - 2];

    //relative path!
    if(wadPath.at(0)!='/')
    {
        wadPath = std::string(get_current_dir_name()) + "/" + wadPath;
    }

    Wad* myWad = Wad::loadWad(wadPath);

    argv[argc - 2] = argv[argc - 1];
    argc--;

    return fuse_main(argc,argv,&operations,myWad);
}