#include <iostream>
#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <string>
#include <vector>
#include <stack>
#include <fstream>
#include <unordered_map>
#include <bits/stdc++.h>


class Wad // Wad is what u said a file system, here wad is an N-ary tree representing and updating the wad file 
{

    class FileNode { // file node for our n-ary tree aka our Wad calss (a file system representation )
        public: 
        unsigned int offset;
        unsigned int length;
        bool isDirectory;
        std::string fileName; 
        std::vector<FileNode*> children; 

        FileNode(){
            this->offset = 0;
            this->length = 0;
            this->isDirectory = false;

        }
  
        FileNode(unsigned int offset, unsigned int length,std::string fileName, bool isDirectory) 
        { 
            this->offset = offset;
            this->length = length;
            this->fileName = fileName;
            this->isDirectory = isDirectory;
        }    
    };

    FileNode* root; // the root directory of our file system
    std::vector<std::string> filePath; // this vector is used for the creation of n-ary tree aka a stack
    std::string magic;  //  4 bytes for magic 
    unsigned int numDescriptors;
    unsigned int offsetDescriptor;
    Wad* ourWad; 
    std::fstream file;
    std::unordered_map<std::string,FileNode*> files; // a hash map that contains all files in our wad. 

    Wad(const std::string& path);

    public:
    ~Wad();
    static Wad* loadWad(const std::string &path);
    std::string getMagic();
    bool isContent(const std::string &path);
    bool isDirectory(const std::string &path);
    int getSize(const std::string &path);
    int getContents(const std::string &path, char *buffer, int length, int offset = 0);
    int getDirectory(const std::string &path, std::vector<std::string> *directory);
    void createDirectory(const std::string &path);
    void createFile(const std::string &path);
    int writeToFile(const std::string &path, const char *buffer, int length, int offset = 0);

    //Helper functions 
    bool newFileConstraints(std::string path);
    bool newDirectoryConstraints(std::string path);
    void adjustDescriptorFiles(std::string parentDirectory,std::string filename,std::string path);
    void adjustDescriptorDirectory(std::string parentDirectory,std::string newDirectory,std::string path);
    std::streampos findDescriptorPostion(std::string parentDirectory, std::string path,bool isWriteToFile);
    std::string removeBackSlash(std::string path);
    void adjustDescriptorCount();


    // additonal functions to build n-ary tree  and delete tree. 
    std::string vectorPath();
    void addNode(FileNode* parent,unsigned int offset, unsigned int length,std::string fileName, bool isDirectory); // we are creating nodes at "directory" which is the top element of the stack as we create the tree.
    FileNode* findNode(FileNode* currentNode, std::string directory);
    void addNodeHelper(FileNode* parent,unsigned int offset, unsigned int length,std::string fileName, bool isDirectory) ;
    void deleteTree(FileNode*& _root);
 
    // function to add to hash
    void addToHash(std::string filename,bool isDirectory);
    void addToHashHelper(std::string parent, std::string child);
    void printHash();
  

};

