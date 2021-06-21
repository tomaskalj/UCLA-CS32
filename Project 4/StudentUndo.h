#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"

#include <stack>
#include <list>

class StudentUndo : public Undo {
public:
    void submit(Undo::Action action, int row, int col, char ch = 0);
    Action get(int& row, int& col, int& count, std::string& text);
    void clear();
private:
    // We need to keep track of the things that are stored when a user decides to undo something
    struct Operation {
        Undo::Action action;
        int row;
        int col;
        std::list<char> chars;  // Popping these off yields the text that was undone
    };
    std::stack<Operation*> m_operations;

    Operation* createOperation(Undo::Action action, int row, int col, char ch);
};

#endif // STUDENTUNDO_H_