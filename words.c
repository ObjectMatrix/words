/**
* gcc or  g++
* According to GCC's online documentation link options and how g++ is invoked, 
* g++ is equivalent to gcc -xc++ -lstdc++ -shared-libgcc (the 1st is a compiler option, 
* the 2nd two are linker options). This can be checked by running both with the -v option 
* it displays the backend toolchain commands being run)
*
* with GNU gcc: 
* --------------
* gcc -xc++ -lstdc++ -shared-libgcc words.c
*
* with GNU g++
* -------------
* g++ -ggdb -Wall -I. -o output words.cpp
*/

/**
 * Algorithm choice: trie (also known as a prefix tree)
 * 
 * Why Trie?
 * ----------
 * A Trie is a data structure designed for rapid reTRIEval of objects.
 * This data structure allows you to map sentences/words to objects, 
 * allowing rapid indexing and searching of massive dictionaries by partial matches.
 * Tries are space and time efficient structures for text storage and search
 * 
 * What about Complexity in naive approach?
 * ----------------------------------------
 * A naive approach will be to match all words of the input with each word of the map and maintain a count
 * so number of words in map and maintain a count of the number of occurence of all words in the map. Complexity
 * would be O(n*m)  
 * m: words in the sentence and 
 * n: words in the map/dictionary
 * 
 * Why not a set<string> or Hash Table?
 * ------------------------------------- 
 * If we have a dictionary, and we need to know if a single word is inside of the dictionary 
 * the tries are a data structure that can help us. 
 * The tries can insert and find strings in O(L) time (where L represent the length of a single word). 
 * (1) This is much faster than set <string>, but is it a bit faster than a hash table.
 * (2) The set <string> and the hash tables can only find in a dictionary words that match exactly 
 * with the single word that we are finding; the trie allow us to find words that have a single character different, 
 * or a prefix in common, or a character missing, etc.
 * ------------------------------------------
 * Design Decisions with Trie data structure:
 * (followed and used some online design and coding guidelines and approaches)
 * ------------------------------------------
 * (1) Start with the first string and loop over sorted strings
 * (2) Check if it can be made of other words by dividing strings 
 *      into all possible combinations and doing same thing for each splits.
 * (3) Retrun true if it is made of other strings and Save that string onto cache.
 * (4) Return the top string because that would be the longest string.
 * (5) The size of array list will give you the total number of words that can be made of other words.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <set>
#include <iterator>
#include <time.h>
#include <pthread.h>

using namespace std;

/**
 * define character size
 * Each trie node can only contains 'a'-'z' characters. 
 * So we can use a small cache to store the character.
 */

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
    // deallocate memory block
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
        if ( edge == NULL) {
            edge = create(edge);
        }
        str++;
        edge = insertWord(edge, str);
    }
    return node;
}

/**
 * search a break in the leaf (word break) until found
 * returns true if string can be segmented into space separated
 * words, otherwise returns false
 */ 
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
/**
 * decide a word whether are made of other words
 * return count of subwords
 * sorted by length, began with the longest length
 * used recursive function and used "isLeafBreak()" to search subword break
 * position for better performance.
 */ 
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

/**
 * read words from input file
 * into trie data structure
 * assume input lowercase words, nospaces, on word perline
 * read the input text file line by line, while building a Trie tree.
 */

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
        /**
         * c_str returns a const char* 
         * that points to a null-terminated string
         * caution: assume input lowercase words, 
         * nospaces, and word perline
         */
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
    
    // params to calculate execution time
    clock_t start, end;
    double cpu_time_used;
    
    // this is the root of Trie
    trie *root = NULL;

    /**
     * map words by grouping with the same length
     * and sort by length of words
     */

    map<size_t, StringList> mapWordsWithSameLen;
    set<size_t> LengthSet;

    start = clock();
    int cntWords = ReadWordFile(filename, root, mapWordsWithSameLen, LengthSet);
    cout << "Input words: " << cntWords << endl;
    // block following lines to include perf for fn: ReadWordFile 
    start = clock();

    int foundWords = 0;
    int cntConcat = 0;
    set<size_t>::reverse_iterator rit;

    // output file: result
    const char* foundWordsFileName = "output_wordsforproblem.txt";
    ofstream foundWordsFile(foundWordsFileName);

    /**
     * first begin with the longest length
     * rbegin: returns a reverse iterator pointing to the last element in the container
     * rend: returns a reverse iterator pointing to the theoretical element 
     * right before the first element in the array
     */

    for (rit = LengthSet.rbegin(); rit != LengthSet.rend(); rit++) { 
        size_t len = *rit;
        StringList& dict = mapWordsWithSameLen[len];

        // loop all of the same length
        for (StringList::const_iterator it=dict.begin(); it!=dict.end(); it++) { 
            bool found = false;
            cntConcat = concatWord(root, it->c_str(), 0, (int)it->size()-1, found);

            // output this
            if (found && cntConcat > 1) { 
                if(foundWords==0)cout << "The longest output: " << *it << endl;
                else if(foundWords==1)cout << "The second longest longest output: " << *it << endl;
                foundWords++;
                foundWordsFile << *it << endl;
            }
        }
    }

    /**
     * The output will show following things:
     * (1) Total number of words in the input file
     * (2) The longest output, The second longest output:
     * (3) Total words found
     * (4) And total execution time
     */

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    cout << "Seconds to execute: " << cpu_time_used << endl; 
    cout << "Total Found words: " << foundWords << endl;

    // deallocate memory block
    trieDestroy(root);
    
    return 0;
}

