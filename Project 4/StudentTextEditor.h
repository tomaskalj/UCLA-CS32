#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"

#include <string>
#include <list>

class Undo;

class StudentTextEditor : public TextEditor {
public:
    StudentTextEditor(Undo* undo);
    ~StudentTextEditor();
    bool load(std::string file);
    bool save(std::string file);
    void reset();
    void move(Dir dir);
    void del();
    void backspace();
    void insert(char ch);
    void enter();
    void getPos(int& row, int& col) const;
    int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
    void undo();
private:
    int m_row;  // row of current editing position
    int m_col;  // col of current editing position
    std::list<std::string> m_lines;     // STL list container for all lines in the text editor
    std::list<std::string>::iterator m_currentLine;     // list iterator pointing to current line being edited
};

#endif // STUDENTTEXTEDITOR_H_