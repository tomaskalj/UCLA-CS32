#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cctype>

SpellCheck* createSpellCheck() {
    return new StudentSpellCheck;
}

StudentSpellCheck::StudentSpellCheck() {
    // Create our root node and have the value representing it be an asterisk
    m_root = createTrieNode();

    // Create a character array with every letter from the dictionary
    // This includes each letter 'a' through 'z' plus the apostrophe
    for (char c = 'a'; c <= 'z'; c++) {
        dictionaryLetters.push_back(c);
    }
    dictionaryLetters.push_back('\'');
}

StudentSpellCheck::~StudentSpellCheck() {
    destroyRecursively(m_root);
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
    std::ifstream infile(dictionaryFile);
    if (!infile) {
        return false;
    }

    // Insert every word in the dictionary into the trie
    std::string word;
    while (getline(infile, word)) {
        insertWord(word);
    }

    return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions) {
    // If the word is in the dictionary, then it doesn't need to be spell checked
    if (contains(word)) {
        return true;
    }

    suggestions.clear();

    for (size_t i = 0; i < word.length(); i++) {
        // Only a max number of suggestions are allowed in the suggestions vector
        if (suggestions.size() >= maxSuggestions) {
            break;
        }

        std::string prefix = word.substr(0, i);
        std::string suffix = word.substr(i + 1);

        // Substitute 1 char in the word at a time with every valid letter in the dictionary
        // to see if it's a valid spell check suggestion to the word misspelled
        // This is a constant operation because dictionaryLetters has a constant size
        for (const char& c : dictionaryLetters) {
            // Create this possible suggestion
            std::string suggestion = prefix + c + suffix;

            if (contains(suggestion) && suggestions.size() < maxSuggestions) {
                suggestions.push_back(suggestion);
            }
        }
    }

    return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
    problems.clear();

    // Get the positions of words in the line
    std::vector<SpellCheck::Position> positions = getPositions(line);

    // Loop through each position
    for (std::vector<SpellCheck::Position>::iterator it = positions.begin(); it != positions.end(); it++) {
        Position pos = *it;
        // Create the word formed by the start and end indices of a position
        std::string word = line.substr(pos.start, pos.end - pos.start + 1);

        // If it's not a valid word, add it to problems
        if (!contains(word)) {
            problems.push_back(pos);
        }
    }
}

void StudentSpellCheck::destroyRecursively(StudentSpellCheck::TrieNode* node) {
    if (node != nullptr) {
        // Destroy child nodes
        for (TrieNode* child : node->children) {
            destroyRecursively(child);
        }
        // Then destroy node
        delete node;
    }
}

StudentSpellCheck::TrieNode* StudentSpellCheck::createTrieNode() {
    TrieNode* node = new TrieNode;
    node->endsHere = false;

    // Reserve 27 spaces in a node's children vector to account for each character
    // 'a' through 'z' (numbered 0-25) plus the apostrophe character (numbered 26)

    const int NUM_CHARACTERS = 27;
    for (int i = 0; i < NUM_CHARACTERS; i++) {
        node->children.push_back(nullptr);
    }

    return node;
}

void StudentSpellCheck::insertWord(std::string word) {
    TrieNode* temp = m_root;

    for (unsigned int k = 0; k < word.length(); k++) {
        char c = tolower(word.at(k));
        int index = c == '\'' ? 26 : c - 'a';   // get the index 0-26 corresponding to a letter

        // If there is no node at that index, create one
        if (temp->children.at(index) == nullptr) {
            temp->children.at(index) = createTrieNode();
        }

        temp = temp->children.at(index);
    }

    // Indicate that the last character added in a word indicates the end of the word
    temp->endsHere = true;
}

// Returns whether a word is in our trie dictionary
// Time Complexity: O(L) where L is the length of the word
bool StudentSpellCheck::contains(std::string word) {
    TrieNode* temp = m_root;

    for (unsigned int k = 0; k < word.length(); k++) {
        char c = tolower(word.at(k));
        int index = c == '\'' ? 26 : c - 'a';   // get the index 0-26 corresponding to a letter

        // If there is no node at that index, the word isn't in the dictionary
        if (temp->children.at(index) == nullptr) {
            return false;
        }

        temp = temp->children.at(index);
    }

    return temp->endsHere;
}

std::vector<SpellCheck::Position> StudentSpellCheck::getPositions(std::string line) {
    std::vector<SpellCheck::Position> positions;

    int startIndex = 0;
    std::string word;
    for (const char& c : line) {
        // If the letter is alphabetic or an apostrophe then it's ok
        if (isalpha(c) || c == '\'') {
            word += c;
        } else {
            // Increment startIndex by 1 if no characters have been added to word
            if (word.empty()) {
                startIndex++;
                continue;
            }

            SpellCheck::Position pos;
            pos.start = startIndex;
            pos.end = word.length() + startIndex - 1;

            positions.push_back(pos);

            // The new startIndex is just after the word
            startIndex += word.length() + 1;
            // Clear word so we can add to it next iteration
            word.clear();
        }
    }

    return positions;
}