#include "StudentUndo.h"

#include <stack>

Undo* createUndo() {
    return new StudentUndo;
}

void StudentUndo::submit(const Undo::Action action, int row, int col, char ch) {
    // If there are no operations add one for the character
    if (m_operations.empty()) {
        m_operations.push(createOperation(action, row, col, ch));
        return;
    }

    Operation*& lastOp = m_operations.top();

    if (action == DELETE) {
        // Batch together delete operations that occur consecutively
        if (lastOp->action == DELETE && row == lastOp->row) {
            if (col == lastOp->col) {
                lastOp->chars.push_front(ch);   // Push delete operations to the front of the last operation's chars
            } else if (col + 1 == lastOp->col) {
                lastOp->col--;
                lastOp->chars.push_back(ch);    // Push backspace operations to the back of the last operation's chars
            }
        } else {
            m_operations.push(createOperation(action, row, col, ch));
        }
    } else if (action == INSERT) {
        // Batch together insert operations that occur consecutively
        if (lastOp->action == INSERT && row == lastOp->row) {
            if (col - 1 == lastOp->col) {
                // Single characters are easy, so just push them to the back of the last operation's chars
                lastOp->col++;
                lastOp->chars.push_back(ch);
            } else if (ch == '\t' && col - 4 == lastOp->col) {
                // Account for tabs because they take up 4 space characters
                lastOp->col += 4;
                for (int i = 0; i < 4; i++) {
                    lastOp->chars.push_back(' ');
                }
            } else {
                m_operations.push(createOperation(action, row, col, ch));
            }
        } else {
            m_operations.push(createOperation(action, row, col, ch));
        }
    } else {
        m_operations.push(createOperation(action, row, col, ch));
    }
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
    if (m_operations.empty()) {
        return Action::ERROR;
    }

    Operation* operation = m_operations.top();
    m_operations.pop();

    row = operation->row;
    col = operation->col;
    count = operation->chars.size();

    Action action;
    switch (operation->action) {
    case INSERT:
        while (!operation->chars.empty()) {
            // Create a string out of the characters that were batched together
            char ch = operation->chars.back();
            operation->chars.pop_back();

            // Account for tabs because they take up 4 space characters
            if (ch == '\t') {
                text += std::string(4, ' ');
                col -= 4;
            } else {
                text += ch;
                col--;
            }
        }

        // The opposite of insertion is deletion, so the action here is to delete the characters that were inserted
        action = DELETE;
        break;
    case SPLIT:
        // The opposite of splitting is joining, so the action here is to join the lines that were split
        action = JOIN;
        break;
    case DELETE:
        // Create a string out of the characters that were batched together
        while (!operation->chars.empty()) {
            char ch = operation->chars.back();
            operation->chars.pop_back();
            text += ch;
        }

        // The opposite of deletion is insertion, so the action here is to insert the characters that were deleted
        action = INSERT;
        break;
    case JOIN:
        // The opposite of joining is splitting, so the action here is to split the lines that were joined
        action = SPLIT;
        break;
    }

    // Remember to delete the operation to free up memory
    delete operation;
    return action;
}

void StudentUndo::clear() {
    // Pop off each Operation* and delete it to free space
    while (!m_operations.empty()) {
        Operation* op = m_operations.top();
        m_operations.pop();

        delete op;
    }
}

// Create a pointer to an Operation to keep track of undo data
StudentUndo::Operation* StudentUndo::createOperation(Undo::Action action, int row, int col, char ch) {
    Operation* op = new Operation;
    op->action = action;
    op->row = row;
    op->col = col;
    op->chars.push_back(ch);
    return op;
}