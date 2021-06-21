#ifndef PROJECTS_FLATULAN_H
#define PROJECTS_FLATULAN_H

class City;

class Flatulan {
public:
    // Constructor
    Flatulan(City* cp, int r, int c);

    // Accessors
    int row() const;
    int col() const;

    // Mutators
    void move();
    bool possiblyGetConverted();

private:
    City* m_city;
    int m_row;
    int m_col;
};

#endif //PROJECTS_FLATULAN_H
