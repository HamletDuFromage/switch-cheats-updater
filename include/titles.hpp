#ifndef TITLES_HPP
#define TITLES_HPP

#define UPDATED_TITLES_PATH "/config/cheats-updater/updated.dat"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <switch.h>

#include "title.h"

void outputUpdatedTitles();

void writeTitlesToFile(std::vector<Title> titles);

std::vector<Title> readTitlesFromFile();

void outputUpdatedTitles();

void printTitles(std::vector<Title> titles);

#endif