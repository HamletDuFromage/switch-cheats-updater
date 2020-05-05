#include <iostream>

#ifndef TITLE
#define TITLE

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
