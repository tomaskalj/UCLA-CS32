#ifndef PROJECTS_HISTORY_H
#define PROJECTS_HISTORY_H

#include "globals.h"

class History {
public:
    History(int nRows, int nCols);
    bool record(int r, int c);
    void display() const;

private:
    int m_rows;
    int m_cols;
    char unconverted[MAXROWS][MAXCOLS];
};

#endif //PROJECTS_HISTORY_H
