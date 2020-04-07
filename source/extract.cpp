#include "extract.hpp"
#include <iostream>

std::vector<std::string> getInstalledTitles(){
    std::vector<std::string> titles;
    NcmContentMetaDatabase metadb;
    Result rc = ncmOpenContentMetaDatabase(&metadb, NcmStorageId_SdCard);
    if(R_SUCCEEDED(rc))
        {
            NcmApplicationContentMetaKey *recs = new NcmApplicationContentMetaKey[MaxTitleCount]();
            s32 wrt = 0;
            s32 total = 0;
            rc = ncmContentMetaDatabaseListApplication(&metadb, &total, &wrt, recs, MaxTitleCount, NcmContentMetaType_Application);
            if((R_SUCCEEDED(rc)) && (wrt > 0))
            {
                titles.reserve(wrt);
                for(s32 i = 0; i < wrt; i++)
                {
                    titles.push_back(formatApplicationId(recs[i].application_id));
                }
            }
            delete[] recs;
            serviceClose(&metadb.s);
        }
    return titles;
}

std::string formatApplicationId(u64 ApplicationId){
    std::stringstream strm;
    strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << ApplicationId;
    return strm.str();
}

int isSXOS()
{
    Handle tmph = 0;
    SmServiceName TX = {"tx\0"};
    Result rc = smRegisterService(&tmph, TX, false, 1);
    if(R_FAILED(rc)) return true;
    smUnregisterService(TX);
    return 1;
}

struct caselessCompare { 
    bool operator() (const std::string& a, const std::string& b) const {
        return strcasecmp(a.substr(7, 16).c_str(), b.c_str()) < 0;
    }
};

int extractCheats(std::string zipPath, std::vector<std::string> titles, bool sxos){
    zipper::Unzipper unzipper(zipPath);
    std::vector<zipper::ZipEntry> entries = unzipper.entries();

    int offset;
    if(sxos){
        offset = std::string(TITLES_PATH).length();
        chdir(SXOS_PATH);
    }
    else{
        offset = std::string(CONTENTS_PATH).length();
        chdir(AMS_PATH);
    }

    std::vector<std::string> entriesNames;
    std::vector<int> parentIndexes;
    for (size_t i = 1; i < entries.size(); i++){
        entriesNames.push_back(entries[i].name);
    }

    std::sort(entriesNames.begin(), entriesNames.end());
    std::sort(titles.begin(), titles.end());

    std::vector<std::string> parents;
    std::vector<std::vector<std::string>> children;
    std::vector<std::string> tempChildren;

    size_t k = 0;
    while(k < entriesNames.size()){
        if(entriesNames[k].length() == (size_t) (offset + 17)){
            parents.push_back(entriesNames[k]);
            k++;
            while(entriesNames[k].length() != (size_t) (offset + 17) && k < entriesNames.size()){
                tempChildren.push_back(entriesNames[k]);
                k++;
            }
            children.push_back(tempChildren);
            tempChildren.clear (); 
        }
        else{
            k++;
        }
    }

    std::cout << "\033[4;31m"<< "\n*** Extracting cheats (this may take a while) ***" << "\033[0m" <<std::endl;

    size_t lastL = 0;
    for(size_t j = 0; j < titles.size(); j++){
        for(size_t l = lastL; l < parents.size(); l++){
            if(strcasecmp(titles[j].c_str(), parents[l].substr(offset, 16).c_str()) == 0){
                unzipper.extractEntry(parents[l]);
                for(auto& e : children[l]){
                    unzipper.extractEntry(e);
                    std::cout << std::setfill(' ') << std::setw(80) << '\r';
                    std::cout << e.substr(0, 79) << "\r";
                    consoleUpdate(NULL);
                }
                lastL = l;
                break;
            }
        }
    }
    std::cout << std::endl;

    unzipper.close();
    return parents.size();
}


/* 
int extractCheats(std::string zipPath, std::vector<std::string> titles){
    
    zipper::Unzipper unzipper(zipPath);
    std::vector<zipper::ZipEntry> entries = unzipper.entries();
    std::cout << "Found " << entries.size() << " entries" << std::endl;
    std::vector<std::string> entriesNames;
    std::vector<int> parentIndexes;
    for (auto& e : entries){
        if (e.name.size() == 24){
            entriesNames.push_back(e.name.substr(7, 16)); // 16 + titles
            //std :: cout << e.name.substr(7, 22) << std::endl;
        }
    }
    //std::cout << entriesNames[0] << std::endl;
    //std::cout << titles[0] << std::endl;

    std::sort(entriesNames.begin(), entriesNames.end());
    std::sort(titles.begin(), titles.end());

    std::vector<std::string> v;
    //(entriesNames.size() + titles.size());
    //std::vector<std::string>::iterator it;

    std::set_intersection(entriesNames.begin(), entriesNames.end(), 
                                begin(titles), titles.end(), 
                                std::back_inserter(v), caselessCompare());
                                //std::back_inserter(v));

    //switch(IsSXOS()){
    switch(0){
        case 0:
            for(auto& t : v){

                //unzipper.extractEntry("titles/" + t + "/", "/atmosphere/contents/");}
                unzipper.extractEntry("titles/" + t + "/", "./test/");
            }
            break;
        case 1:
            for(auto& t : v){
            unzipper.extractEntry("titles/" + t + "/", "sxos/titles/");}
            break;
    }
    //unzipper.extract("./test/");

    unzipper.close();
    return (int) v.size();
} */