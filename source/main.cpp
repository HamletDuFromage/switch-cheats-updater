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

#define VERSION "1.1.7"
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
    if(std::filesystem::exists("/config/cheats-updater/" + path)){
        versionFile.open("/config/cheats-updater/" + path, std::fstream::in);
        versionFile >> version;
        versionFile.close();
    }
    return version;
}

void saveVersion(std::string path, std::string version){
    std::fstream newVersion;
    newVersion.open("/config/cheats-updater/" + path, std::fstream::out | std::fstream::trunc);
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

bool run(){    

    bool sxos = !(isServiceRunning("dmnt:cht") && !(isServiceRunning("tx") && !isServiceRunning("rnx")));
    u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
    bool credits = kHeld & KEY_L;
    std::string filename;
    if(sxos){
        filename = "titles.zip";
        std::filesystem::create_directory("/sxos");
        std::filesystem::create_directory("/sxos/titles");
    }
    else{
        filename = "contents.zip";
        std::filesystem::create_directory("/atmosphere");
        std::filesystem::create_directory("/atmosphere/contents");
    }
    std::filesystem::create_directory("/config");
    std::filesystem::create_directory("/config/cheats-updater");

    std::vector<Title> titles;

    titles = getInstalledTitlesNs();

    titles = excludeTitles("/config/cheats-updater/exclude.txt", titles);

    //std::cout << std::endl;

    int total = titles.size();

    std::cout << "\033[0;32m" << "Found " << total << " installed titles" << "\033[0m" << std::endl << std::endl;

    consoleUpdate(NULL);

    std::string ver = fetchVersion(RELEASE_URL, "1100-1110");
    std::string oldVersion = readVersion("version.dat");

    std::cout << "Current cheats revision: v" << "\033[31m" << oldVersion << "\033[0m" << "\nLatest cheats revision: v" << "\033[0;32m" << ver << "\033[0m";
    if(sxos) std::cout << " for SXOS" << std::endl;
    else std::cout << " for AMS" << std::endl;
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
        if(downloadFile(url.c_str(), filename.c_str(), OFF)){
        //if(false){
            int upd = extractCheats(filename.c_str(), titles, sxos, credits);
            std::cout << "Successfully extacted " << upd << " cheat files" << std::endl;
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
    bool sxos = !(isServiceRunning("dmnt:cht") && !(isServiceRunning("tx") && !isServiceRunning("rnx")));
    int c = removeCheats(sxos);
    saveVersion("version.dat", "0");
    std::cout << "Removed " << c << " cheat files" << std::endl;
}

void viewTitles() {
    std::vector<Title> titles;

    titles = getInstalledTitlesNs();

    titles = excludeTitles("/config/cheats-updater/exclude.txt", titles);
    //std::cout << std::endl;

    int total = titles.size();

    std::cout << "\033[0;32m" << "Found " << total << " installed titles" << "\033[0m" << std::endl << std::endl;

    for(int t = 0; t < total; t++) {
        std::cout << "\033[1;37m" << "Title ID: " << "\033[0m" << titles.at(t).id << "\033[1;37m" << " Title Name: " << "\033[0m" << titles.at(t).name << std::endl;
    }
    std::cout << std::endl;
    
}

void viewHeader() {
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << "\033[1;31m" << "Cheats Updater v" << VERSION << " by HamletDuFromage & Acta" << "\033[0m" <<std::endl;
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << std::endl;
}
void viewMain() {
    viewHeader();

    std::cout << "\033[36m" << "Title ids listed in \"/config/cheats-updater/exclude.txt\" won't get cheat updates" << "\033[0m" << std::endl;

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

    bool done = false;
    bool updated = false;
    while (appletMainLoop())
    {
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_A){
            if(!done){
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
        } 

        if (kDown & KEY_X){
            if(!done){
                clearConsole();
                viewHeader();
                cleanUp();
                updated = false;
                done = true;
                std::cout << "\033[7;37m"<< "\nPress [-] to return to main menu" << "\033[0m" <<std::endl;
                std::cout << "\033[7;37m"<< "\nPress [+] to quit" << "\033[0m" <<std::endl;
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
                updated = false;
                clearConsole();
                viewMain();
                done = false;
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
