#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck();
    virtual ~StudentSpellCheck();
    bool load(std::string dict_file);
    bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
    void spellCheckLine(const std::string& line, std::vector<Position>& problems);
private:
    struct TrieNode {
        bool endsHere;  // Denotes whether this TrieNode is the end of a word
        std::vector<TrieNode*> children;    // The children of any given TrieNode
    };
    TrieNode* m_root;
    std::vector<char> dictionaryLetters;

    // Destroy each node in a trie recursively
    void destroyRecursively(TrieNode* node);
    // Create a node to be contained in a trie
    TrieNode* createTrieNode();
    // Insert a word into the trie
    void insertWord(std::string word);
    // Check whether a word is in a tree
    bool contains(std::string word);
    // Get the positions of words in a line
    std::vector<SpellCheck::Position> getPositions(std::string line);
};

#endif  // STUDENTSPELLCHECK_H_