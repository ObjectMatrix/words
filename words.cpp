/**
* gcc or  g++
* According to GCC's online documentation link options and how g++ is invoked, 
* g++ is equivalent to gcc -xc++ -lstdc++ -shared-libgcc (the 1st is a compiler option, 
* the 2nd two are linker options). This can be checked by running both with the -v option 
* it displays the backend toolchain commands being run)
*/

// gcc -xc++ -lstdc++ -shared-libgcc words.c
// g++ -ggdb -Wall -I. -o output words.cpp

/**
 * Algorithm choice: trie 
 * explanation: 
 * If we have a dictionary, and we need to know if a single word is inside of the dictionary 
 * the tries are a data structure that can help us. 
 * The tries can insert and find strings in O(L) time (where L represent the length of a single word). 
 * (1) This is much faster than set <string>, but is it a bit faster than a hash table.
 * (2) The set <string> and the hash tables can only find in a dictionary words that match exactly 
 * with the single word that we are finding; the trie allow us to find words that have a single character different, 
 * or a prefix in common, or a character missing etc.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <set>
#include <iterator>

using namespace std;

// define character size
// Each trie node can only contains 'a'-'z' characters. 
// So we can use a small cache to store the character.
#define CHAR_SIZE   26

// A Trie node
typedef struct Trie {
    bool isLeaf;
    struct Trie *character[CHAR_SIZE];
}trie;

// create an empty Trie
trie* create(trie *node)
{
    if (node == NULL)
        node = (trie *)malloc(sizeof(trie));
    
    node->isLeaf = false;
    for (int i=0; i < CHAR_SIZE; i++)
        node->character[i] = NULL;
    return node;
}

// destroy the Trie
void trieDestroy(trie* &node)
{
    if (node == NULL)
        return;
    for (int i=0; i < CHAR_SIZE; i++) {
        trieDestroy(node->character[i]);
    }
    free(node);
    node = NULL;
}

// add a word into Trie
trie* insertWord(trie *node, const char *str)
{
    if(str[0] == '\0') {
        node->isLeaf = true;
    } else {
        int ch = str[0] - 'a';
        trie* &edge = node->character[ch];
        if ( edge == NULL)
            edge = create(edge);
        str++;
        edge = insertWord(edge, str);
    }
    return node;
}

// search a break in the leaf (word-break) until found
bool isLeafBreak(trie *node, const char *str, int start, int end, int &mid)
{
    trie *subnode = node;
    const char *pch = str + start;
    int i;
    for (i=start; i<=end; i++) { 
        int ch = *pch - 'a';
        if (subnode->character[ch] == NULL) {
            // no match found
            return false;
        }
        pch++;
        subnode = subnode->character[ch];
        // first position where >= mid)
        if (i >= mid && subnode->isLeaf) {
            mid = i;
            // found a match
            return true; 
        }
    }
    mid = end;
    return subnode->isLeaf;
}

// decide a word whether are made of other words
// return count of subwords
int concatWord(trie *node, const char *str, int start, int end, bool &result)
{
    result = false;

    // validate input
    if (start > end) {
        return 0;
    }
    for (int i = start; i <= end; i++) {
        bool bPartOne = isLeafBreak(node, str, start, end, i);
        
        if (i == end) {
            result = bPartOne;
            return (bPartOne ? 1 : 0);
        }
        
        // start the second part match
        bool bPartTwo = false;
        int cntWords = concatWord(node, str, i+1, end, bPartTwo);
        if (bPartOne && bPartTwo) {
            result = true;
            return 1 + cntWords;
        }
    }
    return 0;
}

// read words from file
// into Trie
// assume input lowercase words, nospaces, on word perline
typedef list<string> StringList;
int ReadWordFile(const char *filename, trie* &root, map<size_t, StringList> &wordsWithSameLen, set<size_t> &LengthSet)
{
    root = NULL;
    root = create(root);
    int cntWords = 0;
    // ifstream: stream class used for file handling
    ifstream ifs(filename, ifstream::in);
    istream_iterator<string> itr_word(ifs), itr_word_end;
    for (; itr_word != itr_word_end; itr_word++, cntWords++ ) {
        // c_str returns a const char* 
        // that points to a null-terminated string
        insertWord(root, (*itr_word).c_str());  
        size_t len = itr_word->size();
        wordsWithSameLen[len].push_back(*itr_word);
        LengthSet.insert(len);
    }
    ifs.close();
    return cntWords;
}

int main(int argc, const char * argv[])
{
    if (argc <= 1) {
        cout << "default name: wordsforproblem.txt\n";
    }
    // default file name with words (input file)
    const char *filename = "wordsforproblem.txt";
    
    if (argc > 1) {
        filename = argv[1];
    }

    // this is the root of Trie
    trie *root = NULL;
    // map words by grouping with the same length
    // and sort by length of words
    map<size_t, StringList> mapWordsWithSameLen;
    set<size_t> LengthSet;
    int cntWords = ReadWordFile(filename, root, mapWordsWithSameLen, LengthSet);
    cout << "Input words: " << cntWords << endl;
    
    int foundWords = 0;
    int cntConcat = 0;
    set<size_t>::reverse_iterator rit;
    // output file (result)
    const char* foundWordsFileName = "output_wordsforproblem.txt";
    ofstream foundWordsFile(foundWordsFileName);

    // first begin with the longest length
    // rbegin: returns a reverse iterator pointing to the last element in the container
    // rend: returns a reverse iterator pointing to the theoretical element 
    // right before the first element in the array 
    for (rit = LengthSet.rbegin(); rit != LengthSet.rend(); rit++) { 
        size_t len = *rit;
        StringList& dict = mapWordsWithSameLen[len];

        // loop all of the same length
        for (StringList::const_iterator it=dict.begin(); it!=dict.end(); it++) { 
            bool found = false;
            cntConcat = concatWord(root, it->c_str(), 0, (int)it->size()-1, found);

            // output this
            if (found && cntConcat>1) { 
                if(foundWords==0)cout << "The longest output: " << *it << endl;
                else if(foundWords==1)cout << "The second longest longest output: " << *it << endl;
                foundWords++;
                foundWordsFile << *it << endl;
            }
        }
    }

    cout << "Total words given: " << foundWords << endl;
    
    trieDestroy(root);
    return 0;
}

