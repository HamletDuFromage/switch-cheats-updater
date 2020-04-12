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

#include <unzipper.h>

#define CONTENTS_PATH "contents/"
#define TITLES_PATH "contents/"
#define AMS_PATH "/atmosphere/"
#define SXOS_PATH "/sxos/"

std::vector<std::string> getInstalledTitles(std::vector<NcmStorageId> storageId);

std::string formatApplicationId(u64 ApplicationId);

int extractCheats(std::string zipPath, std::vector<std::string> titles, bool sxos, bool credits);

int removeCheats(bool sxos);

bool caselessCompare (const std::string& a, const std::string& b);

static constexpr u32 MaxTitleCount = 64000;

#endif

