#include <iostream>
#include <fstream>
#include <filesystem>

#include "unzipper.h"
#include "extract.hpp"
#include "download.hpp"

#include <switch.h>

#define VERSION "1.0"
#define RELEASE_URL "https://github.com/HamletDuFromage/switch-cheats-db/releases/tag/v1.0"
#define ARCHIVE_URL "https://github.com/HamletDuFromage/switch-cheats-db/releases/download/v1.0/"



void initServices(){
    consoleInit(NULL);
    ncmInitialize();
    socketInitializeDefault();
}

void exitServices(){
    socketExit();
    ncmExit();
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
    std::filesystem::create_directory("/config");
    std::filesystem::create_directory("/config/cheats-updater");
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

void run(){    

    bool sxos = !(isServiceRunning("dmnt:cht") && !(isServiceRunning("tx") && !isServiceRunning("rnx")));
    std::string filename;
    if(sxos){
        filename = "titles.zip";
        std::filesystem::create_directory("/sxos/titles");
    }
    else{
        filename = "contents.zip";
        std::filesystem::create_directory("/atmosphere/contents");
    }

    std::vector<std::string> titles = getInstalledTitles();
    std::cout << "Found " << titles.size() << " installed titles" << std::endl;
    consoleUpdate(NULL);

    std::string ver = fetchVersion(RELEASE_URL, "1100-1110");
    std::string oldVersion = readVersion("version.dat");
    if(sxos) std::cout << "Current cheats revision: v" << oldVersion << ", downloading v" << ver << " for SXOS" <<std::endl;
    else std::cout << "Current cheats revision: v" << oldVersion << ", downloading v" << ver << " for AMS" <<std::endl;
    std::cout << std::endl;
    if(ver == oldVersion){
        std::cout << "Already up to date. There's nothing else to do." << std::endl;
    }
    else if(ver == "-1"){
        std::cout << "Couldn't reach the download page. Please check your internet connection" << std::endl;
    }
    else{
        std::string url = std::string(ARCHIVE_URL) + filename;
        if(downloadFile(url.c_str(), filename.c_str(), OFF)){
            int upd = extractCheats(filename.c_str(), titles, sxos);
            std::cout << "Successfully extacted " << upd << " cheat files" << std::endl;
            saveVersion("version.dat", ver);
        }
        else{
            std::cout << "Couldn't download the cheats archive" << std::endl;
        }
    }
    consoleUpdate(NULL);
}

int main(int argc, char* argv[])
{
    initServices();

    std::cout << "\033[1;31m" <<"Cheats Updater v" << VERSION << " by HamletDuFromage" << "\033[0m" <<std::endl;
    std::cout << "Press [A] to download and update cheat files" << std::endl;
    std::cout << "Press [+] to quit" << std::endl;

    consoleUpdate(NULL);

    bool done = false;

    while (appletMainLoop())
    {
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_A){
            if(!done){
                run();
                done = true;
                std::cout << "\033[7;37m"<< "\nPress [+] to quit" << "\033[0m" <<std::endl;
                consoleUpdate(NULL);

            }
        } 

        if (kDown & KEY_PLUS)
            break; 
    }

    exitServices();
    return 0;
}
