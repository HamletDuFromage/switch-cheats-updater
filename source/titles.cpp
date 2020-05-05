#include "titles.hpp"

void writeTitlesToFile(std::vector<Title> titles) {
    std::ofstream updatedTitlesFile;
    updatedTitlesFile.open(UPDATED_TITLES_PATH, std::ofstream::out | std::ofstream::trunc);
    if(updatedTitlesFile.is_open()) {
        for(int t = 0; t < (int)titles.size(); t++) {
            updatedTitlesFile << titles.at(t).id << "," << titles.at(t).name << std::endl;
        }
        updatedTitlesFile.close();
    }
}

std::vector<Title> readTitlesFromFile() {
    std::ifstream updatedTitlesFile;
    updatedTitlesFile.open(UPDATED_TITLES_PATH);

    std::string name;
    std::string id;
    std::string line;

    std::vector<Title> titles;

    if(updatedTitlesFile.is_open()) {
        while(getline(updatedTitlesFile, line)) {
            id = line.substr(0, 16);
            name = line.substr(17, line.size());
            titles.push_back({id, name});
        }
        updatedTitlesFile.close();
    }
    return titles;
}

void outputUpdatedTitles() {
    std::vector<Title> titles = readTitlesFromFile();

    for(int i = 0; i < (int)titles.size(); i++) {
        std::cout << "\033[1;37m" << "Title ID: " << "\033[0m" << titles.at(i).id << "\033[1;37m" << " Title Name: " << "\033[0m" << titles.at(i).name << std::endl;
    }
}