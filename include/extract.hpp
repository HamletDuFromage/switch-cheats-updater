#ifndef EXTRACT_HPP
#define EXTRACT_HPP

#include <string>
#include <vector>
#include <switch.h>
#include <sstream>
#include <iomanip>
#include <iostream> 
#include <algorithm>
#include <iterator>
#include <string.h>
#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <fstream>

#include <unistd.h>

#include <unzipper.h>
#include "title.h"

#define CONTENTS_PATH "contents/"
#define TITLES_PATH "titles/"
#define AMS_PATH "/atmosphere/"
#define SXOS_PATH "/sxos/"

std::vector<std::string> getInstalledTitles(std::vector<NcmStorageId> storageId);

std::vector<std::tuple<std::string, std::string>> getInstalledTitlesNs();

std::string formatApplicationId(u64 ApplicationId);

std::vector<std::tuple<std::string, std::string>> excludeTitles(const char* path, std::vector<std::tuple<std::string, std::string>> listedTitles);

bool isExcluded (std::tuple<std::string, std::string> t1, std::tuple<std::string, std::string> t2);

int extractCheats(std::string zipPath, std::vector<std::tuple<std::string, std::string>> titles, bool sxos, bool credits);

int removeCheats(bool sxos);

bool caselessCompare (const std::string& a, const std::string& b);

static constexpr u32 MaxTitleCount = 64000;

#endif

