#include <iostream>

#ifndef TITLE
#define TITLE

/* Defines the Title structure
    id: the title ID
    name: the title Name

    This overloads operators too, so that sorting and comparison can be done.
*/
struct Title {
    std::string id;
    std::string name;
    bool operator ==(const Title&x) const {
        return id == x.id;
    }

    bool operator <(const Title&x) const {
        return id < x.id;
    }
};

#endif
