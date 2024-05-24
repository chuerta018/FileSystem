#include "Wad.h"

Wad::~Wad()
{// we have file open during the duration of the program ran or delete is called
    file.close(); 
    deleteTree(root);
}

Wad::Wad(const std::string &path)
{
   
    file.open(path, std::ios::in | std::ios::binary | std::ios::out);
    
    if (file.is_open()) 
    {
        // place the first 4 bytes " magic" into our magic buffer
        char mbuffer[4];
        file.read (mbuffer, 4); 
        std::string s (mbuffer);
        //magic ends in WAD,return, s,substring up to D 
        size_t findlast = s.find_last_of("D"); 
        magic = s.substr(0,findlast+1);
    
        // this reads number of descriptors 
        file.read (reinterpret_cast<char*>(&numDescriptors), 4);
       
        // this reads the offset of descriptors
        file.read (reinterpret_cast<char*>(&offsetDescriptor), 4);
        //we change absolute position of file to beginning of descriptors using seekg() form beg
        file.seekg((int)offsetDescriptor, std::ios::beg);

        // here were a creating an root directory as "/" that will contain all directories and and files provided by the wad 
        root = new FileNode(0,0,"/",true);
        addToHash("/",true);
        filePath.push_back("/");
        
        // now we iterate trough number of descriptors and create our filesystem tree via offset  description4
        for(int i =0; i < numDescriptors; i++)
        {
            // we have variables for descriptor offset,length and name then we read them 4 ,4 then 8 bytes
            int eOffset; int eLen; char eName[8];
            file.read (reinterpret_cast<char*>(&eOffset), 4);
            file.read (reinterpret_cast<char*>(&eLen), 4);
            file.read (eName, 8);
           
            std::string temp3 (eName);
             
            // the first if true statement in if can means we found a namespace marker.
            if(temp3.find("_START") != std::string::npos)
            {// this is a directory maker  place either 1st character or the 2 characters into our filepath stack
              // first we add the node to our N-ary tree with the descriptor description and set directory to true. add this directory to filepath after

                if(temp3.length() <= 7)
                {

                    addNode(files[vectorPath()],eOffset,eLen,temp3.substr(0,1),true); 
                    addToHash(temp3.substr(0,1),true);
                    filePath.push_back(temp3.substr(0,1));
                }
                else
                {
                    addNode(files[vectorPath()],eOffset,eLen,temp3.substr(0,2),true); 
                    addToHash(temp3.substr(0,2),true);
                    filePath.push_back(temp3.substr(0,2));
                }
                
            }else if(temp3.find("E") != std::string::npos && temp3.find("M") != std::string::npos && temp3.length() == 4)
            {// this second if statement means we found a map maker directory
                addNode(files[vectorPath()],eOffset,eLen,temp3,true); 
                addToHash(temp3,true);
                filePath.push_back(temp3);

                for(int j=0; j < 10; j++ )
                {
                    int eOff; int eL; char eN[8];
                    file.read (reinterpret_cast<char*>(&eOff), 4);
                    file.read (reinterpret_cast<char*>(&eL), 4);
                    file.read (eN, 8);
                    std::string temp6 (eN);

                    if(temp6.length() <= 8)
                    {
                        addNode(files[vectorPath()],eOff,eL,temp6,false); 
                        addToHash(temp6,false);
                    }
                    else
                    {
                        addNode(files[vectorPath()],eOff,eL,temp6.substr(0,8),false); 
                        addToHash(temp6.substr(0,8),false);
                    }

                    i++; // foorloop of our offset decriptors 
                }

                filePath.pop_back();
                
            }else if(temp3.find("_END") != std::string::npos)
            {// this thrid if statement means we found the  end of the previous found namespace marker
           
                if(filePath.back() == temp3.substr(0,1)||filePath.back() == temp3.substr(0,2))
                filePath.pop_back();
        
            }else
            {// this last if statement means its not a directory and it's a file. 
                if(temp3.length() <= 8)
                {
                    addNode(files[vectorPath()],eOffset,eLen,temp3,false); 
                    addToHash(temp3,false);
                        
                }else
                {
                    addNode(files[vectorPath()],eOffset,eLen,temp3.substr(0,8),false); 
                    addToHash(temp3.substr(0,8),false);
                }
            }
        }//end of descriptor list.   
    }
}

Wad* Wad::loadWad(const std::string &path)
{// call our wad constructor on heap memory
    Wad* wad = new Wad(path);
    return wad;
}

bool Wad::isContent(const std::string &path)
{ 
  std::string s = removeBackSlash(path);

//after removing the "/" the end, if path exist 
// checks if filenode isdirectory member variable is false return accordingly else flase;
    if (files.find(s) != files.end() )
        if(files[s]->isDirectory == false)
            return true;

        return false;
}

bool Wad::isDirectory(const std::string &path)
{
    std::string s = removeBackSlash(path);

//again but vise versa. 
    if (files.find(s) != files.end() )
        if(files[s]->isDirectory == true)
            return true;
      
        return false;
}

int Wad:: getSize(const std::string &path)
{
//first we check if the path exist and is not a directory 
   if(isContent(path))
        return files[removeBackSlash(path)]->length;
   
   return -1;
}

int Wad::getContents(const std::string &path, char *buffer, int length, int offset)
{
    //First we see if the path is content
    if(!isContent(path))
    return -1;

    std::string s = removeBackSlash(path);

    // check if the length of the lump data is smaller than offset provided
    if(files[s]->length < offset)
    return 0;

    //we set the fstream read pointer at offset of the fileNode returned from our hashmap using absolute path
    // and we add the offset of the function parameter // scoot along wad offset
    file.seekg((int)files[s]->offset + offset, std::ios::beg);
    
        if(files[s]->length < length)
        {
            //first check if lump data lenth is < paramter length then we all lump data.
            file.read (buffer, files[s]->length);
            return  files[s]->length- offset;

        }else if(files[s]->length - offset > length)
        {// if lump data length  - paramter offset(bytes to read after offset)is > length paramter we read up to that length
            file.read (buffer,length);
            return length;
        }else
        { //else we read the same bytes but subtract offset
            file.read (buffer,length);
            return length-offset;
        }
}

int Wad::getDirectory(const std::string &path, std::vector<std::string> *directory)
{
        if(!isDirectory(path))
        return -1;

        std::string s = removeBackSlash(path);

         //Using our file hashmap we retreieve directory node at path 
        //iterate thru directories children and push_back the children's filenames into Directory ptr
        for(int i = 0; i<files[s]->children.size(); i++)
            directory->push_back(files[s]->children.at(i)->fileName);
        
        //return total size of files within directory using vector size() 
        return files[s]->children.size();
}

std::string Wad::getMagic()
{
    return magic;
}

int Wad::writeToFile(const std::string &path, const char *buffer, int length, int offset )
{
    
    std::string s = removeBackSlash(path);
    size_t findlast = s.find_last_of("/");

       // string s1 has a substring of the abolute directory path of our file path 
        std::string s1; 

        // if findlast is 0 then its our root directory else is another directory 
        if(findlast == 0) 
        s1 = s.substr(0,1);
        else  
        s1 = s.substr(0,findlast); 

    // s is our file absolute path 
    if(isContent(s) && files[s]->length == 0)
    {
        // note offset decriptor marks end of lump data. 
        files[s]->offset = offsetDescriptor;
        files[s]->length = length;

        // find decriptor position for our file descriptor using function call 
        std::streampos pos = findDescriptorPostion(files[s]->fileName,s1,true);

        // we substract 16 bytes to overwrite file decriptor length and offset
        file.seekp((int)pos-16, std::ios::beg);
        file.write(reinterpret_cast<char*>(&offsetDescriptor), 4), file.flush(), file.sync();
        file.write(reinterpret_cast<char*>(&length), 4), file.flush(), file.sync();

    if(pos == 0 )// pos should never mark the beginning // error
    return -1;
  
        //reset read pos and endposition 
        file.seekp(0, std::ios::beg);
        file.seekg(0,std::ios::end);

        std::streampos endPosition = file.tellg();
        
        //create a buf of the elements after offset decriptor to end of file 
        std::streampos sizeRemaining = endPosition-offsetDescriptor;
        char *buf= new char[(sizeRemaining)];  

        file.seekg(offsetDescriptor, std::ios::beg);
        file.read(buf,sizeRemaining);
        // after creating buf we set write pointer to offsetdeccriptor over write data with paramter buffer contents
        file.seekp(offsetDescriptor,std::ios::beg);

        file.write(buffer,strlen(buffer)),file.flush(),file.sync();
      
        // that buf we created we rewrite and delete
        file.write(buf,sizeRemaining),file.flush(),file.sync();
        delete[] buf;
        
        // now we update our offset decritpor by paramter length. 
        offsetDescriptor+=length;
        file.seekp(8,std::ios::beg);
        file.write(reinterpret_cast<char*>(&offsetDescriptor), sizeof(offsetDescriptor)), file.flush(), file.sync();    
        
        return length;    

    }else if(getSize(s) > 0 ) // if file already has contents we return 0;
        return 0;
//doesnt exist 
    return -1;
}

std::string Wad::removeBackSlash(std::string path)
{
    std::string s;

    if(path.back() == '/' && path != "/")
       return s = path.substr(0,path.length()-1);
    else
        return s = path;
}

void Wad::adjustDescriptorCount()
{//in our wad by redirecting write pos 4 bytes from the beginning 
    numDescriptors++;
    file.seekp(4,std::ios::beg);
    file.write(reinterpret_cast<char*>(&numDescriptors), sizeof(numDescriptors)), file.flush(), file.sync();
}

void Wad::createFile(const std::string &path)
{// from microsoft teams, path cant end in "/"
    // we find the last instance of "/" in path  
    size_t findlast = path.find_last_of("/");

    // if we do not find a "/", then it cant be placed in our root directory 
    if(findlast != std::string::npos)
    {
        // s substring of just the filename
        std::string s = path.substr(findlast+1);

        // string s1 has a substring of the absolute path of parent direcotry
        std::string s1; 

        // if findlast is 0 then its the root else there exist another directory
        if(findlast == 0) 
        s1 = path.substr(0,1);
        else  
        s1 = path.substr(0,findlast); 

       
        if(newFileConstraints(s) && isDirectory(s1) && newDirectoryConstraints(s1))
        { //i know we arent creating a new directory but it needs to pass these constraints
    
        // now we add the newly created file node in both the tree and hashmap in respected directory node
          addNodeHelper(files[s1],0,0,s,false);
        //create a new file absolute path with the newly created tree node
          files.emplace(path,findNode(files[s1],s));

         //now update the wad descriptors and increase decriptor count
          adjustDescriptorFiles(files[s1]->fileName,s,s1);   
          adjustDescriptorCount();   
        }
    }
}
void Wad::adjustDescriptorFiles(std::string parentDirectory,std::string filename, std::string path)
{       //path is absolute path of parent directory
        //parent directory is just the directory name
        //endposition is a pos to end of file        
        file.seekg(0,std::ios::end);
        std::streampos endPosition = file.tellg();
        std::streampos readPosition;
        //has size of the data we are copying if needed
        std::streampos sizeRemaining;
        char *buffer;
        
        if(parentDirectory != "/")
        {
            readPosition = findDescriptorPostion(parentDirectory,path,false);
            if(readPosition == 0 )
            return; // error

            //add 16 to include parents END decriptor 
            sizeRemaining = endPosition - readPosition + 16;
            buffer = new char[(sizeRemaining)];  

            // Read data from the specified position to the end
            //substract -16 because we want to overrite the _END decriptor
            file.seekg(readPosition-16, std::ios::beg);
            //read _end descriptor to end of file and palce into buffer
            file.read(buffer,sizeRemaining);
            //set write pos to begining of parent's END descrptor 
            file.seekp(readPosition-16,std::ios::beg);
        }
        //parent is root directory we still write new file decriptor but at the end 
            int eOffset = 0;
            file.write(reinterpret_cast<char*>(&eOffset), sizeof(eOffset)),file.flush(),file.sync();
       
            int eLen = 0;
            file.write(reinterpret_cast<char*>(&eLen), sizeof(eLen)),file.flush(),file.sync();

            char eName[8];
            filename.copy(eName,sizeof(eName));
            file.write(eName, sizeof(eName)),file.flush(),file.sync();
           
        if(parentDirectory != "/")
        {//after wrtigint new file decriptor we rewrite the buffer that contains parent's END decriptor
        //to end of FILE. delete 
            file.write(buffer,sizeRemaining),file.flush(),file.sync();
            delete[] buffer;
        }
}

void Wad::createDirectory(const std::string &path)
{
    std::string s = removeBackSlash(path);
    //first if the path provided contains '/' at the end we remove it.
    size_t findlast = s.find_last_of("/");

    // if we do not find a "/", then it cant be placed in our root directory so idk
    if(findlast != std::string::npos)
    {
        //string s1 has a substring of just the directory
        std::string s1 = s.substr(findlast+1);
        
        // string s2 has a substring of the absolute path of parent directory
        std::string s2; 

        // if findlast is 0 then root is parent else its another directory
        if(findlast == 0) 
        s2 = s.substr(0,1);
        else  
        s2 = s.substr(0,findlast); 
      
        // we check if s1 is less than 2 characters long and check if parent directory is a valid path
        // after we check to see if the parent directory follow directory contraints of only adding into namespace makers
        if(s1.size() <= 2 && isDirectory(s2) && newDirectoryConstraints(s2))
            if(newDirectoryConstraints(s1))
            {
                 // now we add the newly created file node in both the tree and hashmap in respected directory node
                addNodeHelper(files[s2],0,0,s1,true);   
                //create a new file absolute path with the newly created tree node
                files.emplace(s,findNode(files[s2],s1));       
                            
                //now update the wad descriptors and increase decriptor count by 2 
                adjustDescriptorDirectory(files[s2]->fileName,s1,s2);  
                adjustDescriptorCount();  
                adjustDescriptorCount();
            }
    }

}

void Wad::adjustDescriptorDirectory(std::string parentDirectory,std::string newDirectory , std::string path)
{       //path is absolute path of parent directory
        //parent directory is just the directory name
        //endposition is a pos to end of file   
      
        file.seekg(0,std::ios::end);
        std::streampos endPosition = file.tellg();
        // these temp string are set newDirectory name 
        std::string temp(newDirectory);
        std::string temp1(newDirectory);
        char *buffer ;
         //has size of the data we are copying if needed
        std::streampos sizeRemaining ;

if(parentDirectory != "/"){
        
        std::streampos readPosition = findDescriptorPostion(parentDirectory,path,false);
    
        if(readPosition == 0 )
        return; // error
         //add 16 to include parents END decriptor 
        sizeRemaining = endPosition - readPosition + 16;
        buffer = new char[(sizeRemaining)];  

        // Read data from the specified position to the end
        //substract -16 because we want to overrite the _END decriptor
        file.seekg(readPosition-16, std::ios::beg);
        //read _end descriptor to end of file and palce into buffer
        file.read(buffer,sizeRemaining);
        //set write pos to begining of parent's END descrptor 
        file.seekp(readPosition-16,std::ios::beg); 
}
            //if parent directory is root we still write these but at end of file
            //now we are creating two new directory descriptors one for START and other for END
            // given bother their offset and length are 0. 

            int eOffset = 0; int eLen = 0; char eName[8]; char eName2[8];
            file.write(reinterpret_cast<char*>(&eOffset), sizeof(eOffset)), file.flush(), file.sync();
            file.write(reinterpret_cast<char*>(&eLen), sizeof(eLen)), file.flush(), file.sync();
           
           //and temp1 temp are new directory name with _START added or _End
            temp+="_START";
            temp.copy(eName,sizeof(eName));
            file.write(eName, 8),file.flush(),file.sync();

            file.write(reinterpret_cast<char*>(&eOffset), sizeof(eOffset)), file.flush(), file.sync();
            file.write(reinterpret_cast<char*>(&eLen), sizeof(eLen)), file.flush(), file.sync();

            temp1+="_END";
            temp1.copy(eName2 ,sizeof(eName));
            file.write(eName2, 8),file.flush(),file.sync();
        
        if(parentDirectory != "/")
        {//after wrtigint new file decriptor we rewrite the buffer that contains parent's END decriptor
        //to end of FILE. delete 
            file.write(buffer,sizeRemaining),file.flush(),file.sync();
            delete[] buffer;
        }
}
std::streampos Wad::findDescriptorPostion(std::string parentDirectory, std::string path, bool isWriteFun)
{       filePath.clear();
        filePath.emplace_back("/");
      //set read pos to offsetdecritpor 
        file.seekg((int)offsetDescriptor, std::ios::beg);
        std::streampos readPosition;

        for(int i =0; i < numDescriptors; i++)
        {
            int eOffset; int eLen; char eName[8];
            file.read (reinterpret_cast<char*>(&eOffset), 4);
            file.read (reinterpret_cast<char*>(&eLen), 4);
            file.read (eName, 8);

            std::string temp3 (eName);
            // while reading our descriptor names we emplace namespace markers into our file path vector similar to load wad
            // we do this to ensure absolute path matches our derciptor location aka repeated names
            if(temp3.find("_START") != std::string::npos)
                if(temp3.length() <= 7)
                    filePath.push_back(temp3.substr(0,1));
                else
                    filePath.push_back(temp3.substr(0,2));
            

            //isWrite boolean is for writeToFile() function as we are looking for a file decriptor  not a directory END decriptor
            if(parentDirectory+"_END" == temp3 && vectorPath() == path && isWriteFun == false)
               return file.tellg();
            //parentdirectory here is not a directory its a filename 
            if(parentDirectory == temp3 && vectorPath() == path && isWriteFun == true)
            return file.tellg();
            
            //pop vector stack for absolute path creation while searching for decriptors 
            if(temp3.find("_END") != std::string::npos)
                if(filePath.back() == temp3.substr(0,1)||filePath.back() == temp3.substr(0,2))
                filePath.pop_back();
        }
    
        return 0;
}

bool Wad::newDirectoryConstraints(std::string path)
{
    if(path.find("E") != std::string::npos && path.find("M") != std::string::npos)
        return false;

    return true;
}

bool Wad::newFileConstraints(std::string path)
{
    if(path.length() > 8)
        return false;
    
    if(path == "/" )
        return false;

    if(path.find("_END") != std::string::npos)
        return false;

    if(path.find("_START") != std::string::npos)
        return false;

    if(path.find("E") != std::string::npos && path.find("M") != std::string::npos)
        return false;

        return true;
}

// Hash Map insertion  // 

void Wad::addToHash(std::string filename, bool isDirectory)
{ // here we add a filepath in the form of /x/xy/xyz where filename is xyz and the rest comes from the vector"stack" when building the tree
    std::string temp = vectorPath();
    // this is for initiliation. 
    if(temp.empty())
    {
        files.emplace("/",findNode(root,filename));
        return;
    }
    else if(temp != "/" )
    temp.append("/"+filename);
    else if(filename != "/")
    temp.append(filename);

    files.emplace(temp,findNode(files[vectorPath()],filename));
}

void Wad::printHash()
{
    for (const auto& iter : files) 
    {// just to print filepath and its node 
        std::cout << iter.first << ": " << iter.second->fileName << std::endl;
    }
}

// N - ARY TREE BUILDER FUNCTIONS // DOES NOT MEAN INSERTION 
void Wad::addNode(FileNode* parent,unsigned int offset, unsigned int length,std::string fileName, bool isDirectory)
{
    if (root != nullptr) // root should not be null as we create it during wad construction 
    {
        FileNode* parentNode = findNode(parent,filePath.back());
         // we find the parent node aka a directory at from the top of a stack
            if (parentNode !=nullptr)
             { // if we find it , we add the the new file/directory into this directory.
                addNodeHelper(parentNode, offset,length,fileName,isDirectory);
            } else {
                 // means my root is unintialized lol which should be the case
                std::cout << "Parent node not found!" << std::endl;
                 
            }      
    }
}

void Wad::addNodeHelper(FileNode* parent,unsigned int offset, unsigned int length,std::string fileName, bool isDirectory) 
{// add a file at said directoy into its childrens fileNodes
        FileNode* newNode = new FileNode(offset,length,fileName,isDirectory);
        parent->children.push_back(newNode);
}

Wad::FileNode* Wad::findNode( FileNode*  currentNode, std::string directory)
{// we find a node based on the directory of the vector "stack' filePath 
        if (currentNode->fileName == directory)
        {
            // if the current node  file name matches directory
            return currentNode;
        } else 
        {
            // other wise we recusrively call findNode on the child of the current Node which starts at the root
            for (FileNode* child : currentNode->children) 
            {
                FileNode* foundNode = findNode(child, directory);
                if (foundNode != nullptr) 
                    return foundNode;
                
            }
            return nullptr;
        }
}

std::string Wad::vectorPath()
{
//returns a string of the absolute path of vector(stack) member variable.
    std::string temp;
    
    for(int i = 0 ; i <filePath.size() ; i++)
    {
        if(i == 0)
        {
            temp.append(filePath[i]);
        }else
        { 
            temp.append(filePath[i]);
            temp.append("/");
        }
    }
    return removeBackSlash(temp);
}

void Wad::deleteTree(FileNode*& _root) 
{// recursively call to delete the tree. 
    if (_root == nullptr)
        return;
    
    //recursivly call to delete the root children
    for (FileNode* child : _root->children) 
    deleteTree(child);
    
    delete _root;
    _root = nullptr; 
}