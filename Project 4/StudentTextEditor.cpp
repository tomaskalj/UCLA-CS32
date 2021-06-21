#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

TextEditor* createTextEditor(Undo* un) {
    return new StudentTextEditor(un);
}

// Initialize row and col to the first row and first col of the editor
StudentTextEditor::StudentTextEditor(Undo* undo) : TextEditor(undo), m_row(0), m_col(0) {
    // Push an empty line to the editor so that the currentLine iterator has something to point to
    m_lines.push_back("");
    m_currentLine = m_lines.begin();
}

// Clear the list of lines in O(N) time
StudentTextEditor::~StudentTextEditor() {
    m_lines.clear();
}

bool StudentTextEditor::load(std::string file) {
    std::ifstream infile(file);
    if (!infile) {
        return false;
    }

    // Reset the row, col, lines, and undo stack because we're loading a different file
    reset();

    std::string line;
    while (getline(infile, line)) {
        // Remove the carriage character from the line if it is there
        if (line.length() > 0 && line.at(line.length() - 1) == '\r') {
            line = line.substr(0, line.length() - 1);
        }
        m_lines.push_back(line);
    }

    // Have the current line iterator point to the first line in the file
    m_currentLine = m_lines.begin();

    return true;
}

bool StudentTextEditor::save(std::string file) {
    std::ofstream outfile(file);
    if (!outfile) {
        return false;
    }

    // Write each line to the file specified
    for (std::list<std::string>::iterator it = m_lines.begin(); it != m_lines.end(); it++) {
        outfile << *it << std::endl;
    }

    return true;
}

// Reset row and col to 0, clear the lines list, and clear the undo stack
void StudentTextEditor::reset() {
    m_row = 0;
    m_col = 0;

    m_lines.clear();

    getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {
    switch (dir) {
    case UP:
        // If the current row isn't the first row move it up 1
        if (m_row > 0) {
            m_row--;
            m_currentLine--;

            // Have col be the last character in the line the user moved to if col is greater than the line's length
            if (m_col > m_currentLine->length()) {
                m_col = m_currentLine->length();
            }
        }
        break;
    case DOWN:
        // If the current row isn't the last row move it down 1
        if (m_row < m_lines.size() - 1) {
            m_row++;
            m_currentLine++;

            // Have col be the last character in the line the user moved to if col is greater than the line's length
            if (m_col > m_currentLine->length()) {
                m_col = m_currentLine->length();
            }
        }
        break;
    case LEFT:
        // If the user goes left at the first col of a row that's not the first row then place
        // the cursor at the end of the previous line
        if (m_col == 0) {
            if (m_row > 0) {
                m_row--;
                m_currentLine--;
                m_col = m_currentLine->length();
            }
        } else if (m_col > 0) {
            m_col--;
        }
        break;
    case RIGHT:
        // If the user goes right at the last col of a row that's not the last row then place
        // the cursor at the beginning of the next line
        if (m_col == m_currentLine->length()) {
            if (m_row < m_lines.size() - 1) {
                m_row++;
                m_currentLine++;
                m_col = 0;
            }
        } else if (m_col < m_currentLine->length()) {
            m_col++;
        }
        break;
    case HOME:
        m_col = 0;
        break;
    case END:
        m_col = m_currentLine->length();
        break;
    }
}

void StudentTextEditor::del() {
    std::string& line = *m_currentLine;

    // If the user presses del at the last col of a row that's not the last row
    // then that line must be joined by the next line
    if (m_col == line.length()) {
        if (m_row != m_lines.size() - 1) {
            std::list<std::string>::iterator nextLine = m_currentLine;
            nextLine++;

            // Concatenate the current line with the next line
            std::string newLine = line + *nextLine;

            // Erase both the next and current line in the list then insert the newly created line at its correct position
            m_lines.erase(nextLine);
            m_currentLine = m_lines.erase(m_currentLine);
            m_currentLine = m_lines.insert(m_currentLine, newLine);

            // Tell undo that the user has joined two lines
            getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
        }
    } else {
        // Trivially delete the character at the cursor
        char ch = line.at(m_col);
        line.erase(m_col, 1);

        // Tell undo that the user has deleted a character
        getUndo()->submit(Undo::Action::DELETE, m_row, m_col, ch);
    }
}

void StudentTextEditor::backspace() {
    std::string& line = *m_currentLine;

    if (m_col > 0) {
        // Trivially delete the previous character from its line
        m_col--;
        char ch = line.at(m_col);

        line.erase(m_col, 1);

        // Tell undo that the user has deleted a character
        getUndo()->submit(Undo::Action::DELETE, m_row, m_col, ch);
    } else if (m_col == 0) {
        if (m_row > 0) {
            // If the user presses backspace at the first col of a row that's not the first row
            // then that line must join the previous line

            std::list<std::string>::iterator prevLine = m_currentLine;
            prevLine--;

            int oldLen = prevLine->length();

            // Concatenate the previous line with the current line
            std::string newLine = *prevLine + line;

            // Erase both the current and previous line then insert the newly created line at its correct position
            m_lines.erase(m_currentLine);
            prevLine = m_lines.erase(prevLine);
            m_currentLine = m_lines.insert(prevLine, newLine);

            m_row--;
            m_col = oldLen;

            // Tell undo that the user has joined two lines
            getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
        }
    }
}

void StudentTextEditor::insert(char ch) {
    std::string& line = *m_currentLine;

    std::string toInsert;
    if (ch == '\t') {
        toInsert = std::string(4, ' '); // a tab = 4 spaces as per the spec
    } else {
        toInsert = ch;
    }

    // insert the character(s) at the current col
    line.insert(m_col, toInsert);
    m_col += toInsert.length();

    // Tell undo that the user has inserted the character(s)
    getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);
}

void StudentTextEditor::enter() {
    std::string& line = *m_currentLine;

    // Get the string that are after the col at which the user pressed enter
    std::string postEnter = line.substr(m_col);

    // Erase the remaining characters from the current line
    line.erase(m_col);

    // Insert the rest of line at its correct position
    m_currentLine++;
    m_currentLine = m_lines.insert(m_currentLine, postEnter);

    // Tell undo that the user has split two lines
    getUndo()->submit(Undo::Action::SPLIT, m_row, m_col);

    m_row++;
    m_col = 0;
}

void StudentTextEditor::getPos(int& row, int& col) const {
    row = m_row;
    col = m_col;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
    if (startRow < 0 || numRows < 0 || startRow > m_lines.size()) {
        return -1;
    }

    lines.clear();

    // Have an iterator keep track of the line that startRow corresponds to
    std::list<std::string>::const_iterator it = m_currentLine;
    int numToAdvance = startRow - m_row;
    std::advance(it, numToAdvance);

    // Visit numRows (if possible) at and after startRow and count how many have been visited
    int numLinesVisited = 0;
    for (int i = 0; i < numRows && it != m_lines.end(); i++) {
        lines.push_back(*it);
        it++;
        numLinesVisited++;
    }

    return numLinesVisited;
}

void StudentTextEditor::undo() {
    int row, col, count;
    std::string text;
    Undo::Action action = getUndo()->get(row, col, count, text);
    if (action == Undo::ERROR) {
        return;
    }

    // Change the current line to point to wherever the undo needs to occur
    int numToAdvance = row - m_row;
    advance(m_currentLine, numToAdvance);
    std::string& line = *m_currentLine;

    if (action == Undo::INSERT) {
        // Insert the characters back that were previously removed
        line.insert(col, text);
    } else if (action == Undo::SPLIT) {
        // Split two lines that were previously joined together
        std::string postEnter = line.substr(col);
        line.erase(col);

        std::list<std::string>::iterator nextLine = m_currentLine;
        nextLine++;

        m_lines.insert(nextLine, postEnter);
    } else if (action == Undo::DELETE) {
        // Erase the characters that were previously inserted
        line.erase(col, count);
    } else if (action == Undo::JOIN) {
        // Join two lines that were previously split
        std::list<std::string>::iterator nextLine = m_currentLine;
        nextLine++;

        std::string newLine = line + *nextLine;

        m_lines.erase(nextLine);
        m_currentLine = m_lines.erase(m_currentLine);
        m_currentLine = m_lines.insert(m_currentLine, newLine);
    }

    // Update the editing row and col
    m_row = row;
    m_col = col;
}