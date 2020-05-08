#include "titles.hpp"

/**
 * writeTitlesToFile
 * Writes the input vector to the updated.dat file in config
 * Stores the id and name of each title seprated by a comma
**/
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

/**
 * readTitlesFromFile
 * Reads titles from the updated.dat file in config
 * Stores them as a new titles vector, and returns this to the function calling.
**/
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

/**
 * outputUpdatedTitles
 * Displays the contents of the updated.dat file in /config/
**/
void outputUpdatedTitles() {
    std::vector<Title> titles = readTitlesFromFile();
    std::cout << "Updated Titles:" << std::endl << std::endl;
    printTitles(titles);

    std::cout << "\033[7;37m"<< "\nPress [-] to return to main menu" << "\033[0m" <<std::endl;
    std::cout << "\033[7;37m"<< "\nPress [+] to quit" << "\033[0m" <<std::endl;
}

/**
 * printTitles
 * Display the titles vector parsed in
 * Truncates the name of the titles when necessary
 */ 
void printTitles(std::vector<Title> titles) {
    for(int i = 0; i < (int)titles.size(); i++) {
        if(titles.at(i).name.size() > 35) {
            titles.at(i).name = titles.at(i).name.substr(0, 35);
            titles.at(i).name.append("...");
        }
        std::cout << "\033[1;37m" << "Title ID: " << "\033[0m" << titles.at(i).id << "\033[1;37m" << " Title Name: " << "\033[0m" << titles.at(i).name << std::endl;
    }
}