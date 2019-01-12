# Notes on GNU compiler gcc and g++  

According to GCC's online documentation link options and how g++ is invoked, 
g++ is equivalent to gcc -xc++ -lstdc++ -shared-libgcc (the 1st is a compiler option, 
the 2nd two are linker options). This can be checked by running both with the -v option 
it displays the backend toolchain commands being run)

# How to compile this program  

## with gcc  
gcc -xc++ -lstdc++ -shared-libgcc words.c

## with g++
g++ -ggdb -Wall -I. -o output words.cpp

# Algorithm Choice: TRIE over Hash Table or set<string>  
 Algorithm choice: trie 
 ## explanation:  
 If we have a dictionary, and we need to know if a single word is inside of the dictionary 
 the tries are a data structure that can help us. 
 The tries can insert and find strings in O(L) time (where L represent the length of a single word). 
 (1) This is much faster than set <string>, but is it a bit faster than a hash table.
 (2) The set <string> and the hash tables can only find in a dictionary words that match exactly 
 with the single word that we are finding; the trie allow us to find words that have a single character different, 
 or a prefix in common, or a character missing etc.
 
