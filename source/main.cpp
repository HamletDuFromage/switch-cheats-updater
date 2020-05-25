#include <iostream>
#include <tuple>
#include <fstream>
#include <filesystem>

#include "unzipper.h"
#include "title.h"
#include <switch.h>

#include "extract.hpp"
#include "download.hpp"
#include "titles.hpp"

#define VERSION "1.2.1"
#define RELEASE_URL "https://github.com/HamletDuFromage/switch-cheats-db/releases/tag/v1.0"
#define ARCHIVE_URL "https://github.com/HamletDuFromage/switch-cheats-db/releases/download/v1.0/"

#define WIDTH 80 // The width of the console
#define HEIGHT 45 // Height of the console


void initServices(){
    consoleInit(NULL);
    //ncmInitialize();
    nsInitialize();
    socketInitializeDefault();
}

void exitServices(){
    socketExit();
    nsExit();
    //ncmExit();
    consoleExit(NULL);
}

std::string readVersion(std::string path){
    std::fstream versionFile;
    std::string version = "0";
    if(std::filesystem::exists(CONFIG_PATH + path)){
        versionFile.open(CONFIG_PATH + path, std::fstream::in);
        versionFile >> version;
        versionFile.close();
    }
    return version;
}

void saveVersion(std::string path, std::string version){
    std::fstream newVersion;
    newVersion.open(CONFIG_PATH + path, std::fstream::out | std::fstream::trunc);
    newVersion << version << std::endl;
    newVersion.close();
}

bool isServiceRunning(const char *serviceName) {
  Handle handle;
  SmServiceName service_name = smEncodeName(serviceName);
  bool running = R_FAILED(smRegisterService(&handle, service_name, false, 1));

  svcCloseHandle(handle);

  if (!running)
    smUnregisterService(service_name);

  return running;
}

CFW getCFW(){
    if(isServiceRunning("rnx"))         return rnx;
    else if(isServiceRunning("tx"))     return sxos;
    else                                return ams;
}

bool run(){    

    int cfw = getCFW();
    u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
    bool credits = kHeld & KEY_L;
    std::string filename = "";
        switch(cfw){
            case ams:
                filename = "contents.zip";
                std::filesystem::create_directory("/atmosphere");
                std::filesystem::create_directory("/atmosphere/contents");
                break;
            case rnx:
                filename = "contents.zip";
                std::filesystem::create_directory("/ReiNX");
                std::filesystem::create_directory("/ReiNX/contents");
                break;
            case sxos:
                filename = "titles.zip";
                std::filesystem::create_directory("/sxos");
                std::filesystem::create_directory("/sxos/titles");
                break;
    }

    std::vector<Title> titles;

    titles = getInstalledTitlesNs();

    titles = excludeTitles((std::string(CONFIG_PATH) + "exclude.txt").c_str(), titles);

    //std::cout << std::endl;

    int total = titles.size();

    std::cout << "\033[0;32m" << "Found " << total << " installed titles" << "\033[0m" << std::endl << std::endl;

    consoleUpdate(NULL);

    std::string ver = fetchVersion(RELEASE_URL, "1100-1110");
    std::string oldVersion = readVersion("version.dat");

    std::cout << "Current cheats revision: v" << "\033[31m" << oldVersion << "\033[0m" << "\nLatest cheats revision: v" << "\033[0;32m" << ver << "\033[0m";
    switch(cfw){
        case ams : 
            std::cout << " for AMS" << std::endl;
            break;
        case rnx : 
            std::cout << " for ReiNX" << std::endl;
            break;
        case sxos :
            std::cout << " for SXOS" << std::endl;
            break;
    }
    std::cout << std::endl;

    if(ver == oldVersion){
        std::cout << "Already up to date. There's nothing else to do." << std::endl;
    }
    else if(ver == "-1"){
        std::cout << "Couldn't reach the download page. Please check your internet connection" << std::endl;
    }
    else{
        std::cout << "Downloading v" << ver << "..." << std::endl;
        std::string url = std::string(ARCHIVE_URL) + filename;
        std::string path = std::string(CONFIG_PATH) + filename;
        if(downloadFile(url.c_str(), path.c_str(), OFF)){
        //if(false){
            extractCheats(path.c_str(), titles, cfw, credits);
            saveVersion("version.dat", ver);
        }
        else{
            std::cout << "Couldn't download the cheats archive" << std::endl;
        }
    }
    consoleUpdate(NULL);

    // If the versions are the same, return false (indicating that the files were not updated)
    // Otherwise, return true (indicating that they were updated)
    if(ver == oldVersion) 
        return false;
    else
        return true;
}

void cleanUp(){
    int cfw = getCFW();
    int c = removeCheats(cfw);
    saveVersion("version.dat", "0");
    std::cout << "Removed " << c << " cheat files" << std::endl;
}

void viewTitles() {
    std::vector<Title> titles;

    titles = getInstalledTitlesNs();

    titles = excludeTitles((std::string(CONFIG_PATH) + "exclude.txt").c_str(), titles);
    //std::cout << std::endl;

    int total = titles.size();

    printTitles(titles);

    std::cout << std::endl << "\033[0;32m" << "Found " << total << " installed titles" << "\033[0m" << std::endl;
    
    std::cout << std::endl;
    
}

void viewHeader() {
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << "\033[1;31m" << "Cheats Updater v" << VERSION << " by HamletDuFromage & Acta" << "\033[0m" <<std::endl;
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << std::endl;
    consoleUpdate(NULL);
}
void viewMain() {
    viewHeader();

    std::cout << "\033[36m" << "Title IDs listed in \"/config/cheats-updater/exclude.txt\" won't get cheat updates" << "\033[0m" << std::endl;

    std::cout << "Press [A] to download and update cheat files" << std::endl;
    std::cout << "Press [B] to view installed titles" << std::endl;
    std::cout << "Hold  [L] and press [A] to also download cheat credits and instructions" << std::endl;
    std::cout << "Press [X] to delete ALL existing cheat files\n" << std::endl;

    std::cout << "Press [+] to quit" << std::endl << std::endl;

    consoleUpdate(NULL);
}

void clearConsole() {
    consoleClear();
    //consoleSetWindow(NULL, 0, 0, WIDTH, HEIGHT);
    consoleUpdate(NULL);
}

int main(int argc, char* argv[])
{
    initServices();

    viewMain();

    std::filesystem::create_directory("/config");
    std::filesystem::create_directory(CONFIG_PATH);

    bool done = false;
    bool updated = false;
    bool deleteCheatsConfirm = false;
    while (appletMainLoop())
    {
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_A){
            if(!deleteCheatsConfirm) {
                if(!done) {
                    clearConsole();
                    viewHeader();
                    updated = run();
                    done = true;

                    if(updated) {
                        std::cout << "\nPress [Y] to view updated files.\n" << std::endl;
                    }
                    std::cout << "\033[7;37m"<< "\nPress [-] to return to main menu" << "\033[0m" <<std::endl;
                    std::cout << "\033[7;37m"<< "\nPress [+] to quit" << "\033[0m" <<std::endl;
                    consoleUpdate(NULL);

                }
            } else {
                clearConsole();
                viewHeader();
                cleanUp();
                updated = false;
                done = true;
                deleteCheatsConfirm = false;
                std::cout << "\033[7;37m"<< "\nPress [-] to return to main menu" << "\033[0m" <<std::endl;
                std::cout << "\033[7;37m"<< "\nPress [+] to quit" << "\033[0m" <<std::endl;
                consoleUpdate(NULL);
            }
            
        } 

        if (kDown & KEY_X){
            if(!done) {
                deleteCheatsConfirm = true;
                clearConsole();
                viewHeader();
                std::cout << "\033[1;31m" << "Are you sure? this will delete every cheat from your /contents/ folder." << std::endl;
                std::cout << "Press [A] to delete ALL existing cheat files" << std::endl;
                std::cout << "Press [-] to return to main menu and abort" << "\033[0m" <<std::endl;
                done = true;
                consoleUpdate(NULL);
            }
        }

        if (kDown & KEY_B){
            if(!done){
                clearConsole();
                viewHeader();
                viewTitles();
                updated = false;
                done = true;
                std::cout << "\033[7;37m"<< "\nPress [-] to return to main menu" << "\033[0m" <<std::endl;
                std::cout << "\033[7;37m"<< "\nPress [+] to quit" << "\033[0m" <<std::endl;
                consoleUpdate(NULL);

            }
        }

        if (kDown & KEY_MINUS) {
            if(done) {
                clearConsole();
                viewMain();
                updated = false;
                done = false;
                deleteCheatsConfirm = false;
                consoleUpdate(NULL);
            }
        }
        if (kDown & KEY_Y) {
            if(updated) {
                clearConsole();
                viewHeader();
                outputUpdatedTitles();
                consoleUpdate(NULL);
                updated = false;
            }
        }

        if (kDown & KEY_PLUS)
            break; 
    }

    exitServices();
    return 0;
}
