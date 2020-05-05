#ifndef TITLES_HPP
#define TITLES_HPP

#define UPDATED_TITLES_PATH "/config/cheats-updater/updated.dat"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "title.h"

void outputUpdatedTitles();

void writeTitlesToFile(std::vector<Title> titles);

std::vector<Title> readTitlesFromFile();

#endif