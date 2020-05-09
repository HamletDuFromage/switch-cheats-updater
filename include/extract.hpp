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
#include "titles.hpp"

#define CONFIG_PATH     "/config/cheats-updater/"
#define CONTENTS_PATH   "contents/"
#define TITLES_PATH     "titles/"
#define AMS_PATH        "/atmosphere/"
#define SXOS_PATH       "/sxos/"
#define REINX_PATH      "/ReiNX/"

enum CFW{
    ams,
    rnx,
    sxos,
};

std::vector<std::string> getInstalledTitles(std::vector<NcmStorageId> storageId);

std::vector<Title> getInstalledTitlesNs();

std::string formatApplicationId(u64 ApplicationId);

std::vector<Title> excludeTitles(const char* path, std::vector<Title> listedTitles);

int extractCheats(std::string zipPath, std::vector<Title> titles, int cfw, bool credits);

int removeCheats(int cfw);

bool caselessCompare (const std::string& a, const std::string& b);

static constexpr u32 MaxTitleCount = 64000;

#endif

