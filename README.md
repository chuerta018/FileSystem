File Format
The WAD file format contains information in three sections: the header, which gives basic layout information,
the descriptors, which describe elements in the file, and the lumps, which contain the data themselves. NOTE:
all numbers are in little-Endian format and, where applicable, are designated in bytes! Since Reptilian stores
its variables in memory in little-Endian format as well, it is not necessary to perform any byte-order inversions
when reading in or writing data, but this is still important information to know.
File Header
The header contains the file magic, descriptor count, and location (offset) of the descriptors in the file:
The magic for a wad file is usually ASCII and always ends in the suffix "WAD" (e.g., "IWAD" or "PWAD").
It is also important to note that the descriptor list, beginning at the position indicated by the descriptor
offset, is always situated at the end of the WAD file.
Descriptors
The file’s descriptors contain information about elements in the WAD file – its file offset, length, and
name:
Some elements will have specific naming conventions that will differentiate them from regular content
files. These “marker” elements will be interpreted by the daemon as directories and should be displayed
accordingly in the filesystem (see below).
Lumps
Elements in the WAD format are stored as “lumps” described by the descriptors. These lumps will be
represented in the filesystem by the daemon as individual files that can be opened, read, and closed.
You cannot write to existing lumps, but you will be creating empty files whose lumps you will have
to write to.
Marker Elements
There are two primary types of marker elements in WAD files, each of which should be interpreted as
directories by our daemon. The type includes map markers and namespace markers.
Map marker names are of the format "E#M#", where # represents a single decimal digit (e.g., "E1M9").
They are followed by ten (10) map element descriptors. The elements for the next 10 descriptors should be
placed inside of a directory with the map’s name. Map marker directories cannot have files or directories
added to them.
Namespace markers come in pairs. A namespace’s beginning is marked with a descriptor whose name has
the suffix "_START" (e.g., "F1_START"), and its ending is marked with a descriptor whose name has the
suffix "_END" (e.g., "F1_END"). Any descriptors for elements falling between the beginning and ending
markers for a namespace should be placed within a directory with the namespace’s name (e.g., "F1").
The namespace marker's name, excluding the suffixes, will never exceed two characters. These will be
the kind of directories you will be responsible for creating.
As an example, the following descriptors, in order, in the descriptor list, should result in this organization:
Library
Your library will contain a class to represent WAD data as described in this section.
Wad Class
The Wad class is used to represent WAD data and should have the following functions. The root of all paths
in the WAD data should be "/", and each directory should be separated by '/' (e.g., "/F/F1/LOLWUT").
public static Wad* loadWad(const string &path)
Object allocator; dynamically creates a Wad object and loads the WAD file data from path into memory.
Caller must deallocate the memory using the delete keyword.
public string getMagic()
Returns the magic for this WAD data.
public bool isContent(const string &path)
Returns true if path represents content (data), and false otherwise.
public bool isDirectory(const string &path)
Returns true if path represents a directory, and false otherwise.
public int getSize(const string &path)
If path represents content, returns the number of bytes in its data; otherwise, returns -1.
public int getContents(const string &path, char *buffer, int length, int offset = 0)
If path represents content, copies as many bytes as are available, up to length, of content's data into the preexisting buffer. If offset is provided, data should be copied starting from that byte in the content. Returns
number of bytes copied into buffer, or -1 if path does not represent content (e.g., if it represents a directory).
public int getDirectory(const string &path, vector<string> *directory)
If path represents a directory, places entries for immediately contained elements in directory. The elements
should be placed in the directory in the same order as they are found in the WAD file. Returns the number of
elements in the directory, or -1 if path does not represent a directory (e.g., if it represents content).
Offset Length Name
0 0 F_START
0 0 F1_START
67500 0 E1M1
67500 1380 THINGS
68880 6650 LINEDEFS
75532 19440 SIDEDEFS
94972 1868 VERTEXES
96840 8784 SEGS
105624 948 SSECTORS
106572 6608 NODES
113180 2210 SECTORS
115392 904 REJECT
116296 6922 BLOCKMAP
42 9001 LOLWUT
0 0 F1_END
0 0 F_END
F
 F1
 E1M1
 THINGS  LINEDEFS  SIDEDEFS  VERTEXES  SEGS  SSECTORS  NODES  SECTORS  REJECT  BLOCKMAP LOLWUTDirectory Structure

public void createDirectory(const string &path)
path includes the name of the new directory to be created. If given a valid path, creates a new directory
using namespace markers at path. The two new namespace markers will be added just before the “_END”
marker of its parent directory. New directories cannot be created inside map markers.
public void createFile(const string &path)
path includes the name of the new file to be created. If given a valid path, creates an empty file at path,
with an offset and length of 0. The file will be added to the descriptor list just before the “_END” marker
of its parent directory. New files cannot be created inside map markers.
public int writeToFile(const string &path, const char *buffer, int length, int offset = 0)
If given a valid path to an empty file, augments file size and generates a lump offset, then writes length amount
of bytes from the buffer into the file’s lump data. If offset is provided, data should be written starting from that
byte in the lump content. Returns number of bytes copied from buffer, or -1 if path does not represent content
(e.g., if it represents a directory).
Daemon Command & Parameters
Your daemon should have name wadfs and should accept at a minimum three parameters – the single-threaded
flag "-s", the target WAD file, and the mount directory. For example, this command should mount TINY.WAD
in /home/reptilian/mountdir…
$ ./wadfs -s TINY.WAD /home/reptilian/mountdir
$
…and this should result from executing the ls command to show part of its contents:
$ ls /home/reptilian/mountdir/F/F1 -al
total 0
drwxrwxrwx. 2 root root 0 Jan 1 1970 .
drwxrwxrwx. 2 root root
drwxrwxrwx. 2 root root
 0 Jan 1 1970 ..
 0 Jan 1 1970 E1M1
-rwxrwxrwx. 2 root root 9001 Jan 1 1970 LOLWUT
Your daemon should run in the background. Do not hard-code the debug (-d) or foreground (-f) flags!
