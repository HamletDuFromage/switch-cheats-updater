#include "extract.hpp"

std::vector<std::string> getInstalledTitles(std::vector<NcmStorageId> storageId){
    std::vector<std::string> titles;
    NcmContentMetaDatabase metadb;

    for (auto& sId : storageId){
        Result rc = ncmOpenContentMetaDatabase(&metadb, sId);
        if(R_SUCCEEDED(rc)){
            NcmApplicationContentMetaKey *recs = new NcmApplicationContentMetaKey[MaxTitleCount]();
            s32 wrt = 0;
            s32 total = 0;
            rc = ncmContentMetaDatabaseListApplication(&metadb, &total, &wrt, recs, MaxTitleCount, NcmContentMetaType_Application);
            if((R_SUCCEEDED(rc)) && (wrt > 0)){
                titles.reserve(wrt);
                for(s32 i = 0; i < wrt; i++){
                    titles.push_back(formatApplicationId(recs[i].application_id));
                }
            }
            delete[] recs;
            serviceClose(&metadb.s);
        }
    }
    return titles;
}

std::vector<Title> getInstalledTitlesNs(){
    // This function has been cobbled together from the "app_controldata" example in devkitpro.

    // Set the rc variable to begin with
    Result rc = 0;

    // Initialise a vector of tuples, storing the title ID and the title name.
    std::vector<Title> titles;

    // Initialise an application record array, where the size is MaxTitleCount
    NsApplicationRecord *recs = new NsApplicationRecord[MaxTitleCount]();

    // Set the buffer to NULL
    NsApplicationControlData *buf=NULL;
    // Set outsize to 0
    u64 outsize=0;
    // Set the language entry to NULL
    NacpLanguageEntry *langentry = NULL;

    // Create a char array to store the name of the title
    char name[0x201];

    // Set the total records to 0
    s32 total = 0;
    // Set a failed counter to 0
    int totalFailed = 0;
    // Fill the recs array with application records
    rc = nsListApplicationRecord(recs, MaxTitleCount, 0, &total);

    // If filling the recs array was successful
    if (R_SUCCEEDED(rc)){
        // Loop through records
        for (s32 i = 0; i < total; i++){

            // Reset varaibles for accessing memory
            outsize=0;
            buf = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
            if (buf==NULL) {
                rc = MAKERESULT(Module_Libnx, LibnxError_OutOfMemory);
                printf("Failed to alloc mem.\n");
            } else {
                memset(buf, 0, sizeof(NsApplicationControlData));
            }

            if (R_SUCCEEDED(rc)) {
                rc = nsInitialize();
                if (R_FAILED(rc)) {
                    printf("nsInitialize() failed: 0x%x\n", rc);
                }
            }
            
            // Get the application control data for the current record
            rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, recs[i].application_id, buf, sizeof(NsApplicationControlData), &outsize);

            if (R_FAILED(rc)) {
                totalFailed++;
                std::cout << "nsGetApplicationControlData() failed: 0x" << std::hex << rc << " for Title ID: " << formatApplicationId(recs[i].application_id) << std::endl;
            }

            if (outsize < sizeof(buf->nacp)) {
                rc = -1;
                printf("Outsize is too small: 0x%lx.\n", outsize);
            }

            // If application control data was retrieved successfully
            if (R_SUCCEEDED(rc)) {
                rc = nacpGetLanguageEntry(&buf->nacp, &langentry);

                if (R_FAILED(rc) || langentry==NULL) printf("Failed to load LanguageEntry.\n");
            }

            // If nacp language entry was retrieved successfully
            if (R_SUCCEEDED(rc)) {
                memset(name, 0, sizeof(name));
                strncpy(name, langentry->name, sizeof(name)-1); //Don't assume the nacp string is NUL-terminated for safety.
                titles.push_back({formatApplicationId(recs[i].application_id), name});
            }

            nsExit();
        }
    }

    free(buf);
    delete[] recs;
    std::sort(titles.begin(), titles.end());
    if(totalFailed > 0) 
        std::cout << "\n\nFailed " << totalFailed << " titles." << std::endl;
    return titles;
}

std::string formatApplicationId(u64 ApplicationId){
    std::stringstream strm;
    strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << ApplicationId;
    return strm.str();
}

std::vector<Title> excludeTitles(const char* path, std::vector<Title> listedTitles){
    // Initialise a vector of titles
    std::vector<Title> titles;
    // Open a file stream
    std::ifstream file(path);
    // Declare a total variable and set to 0
    int total = 0;
    // Declare a name variable
    std::string name;


    if (file.is_open()) {
        std::string line;
        // Read each line of the file in
        while (std::getline(file, line)) {
            name = "No name provided";
            // Change the line to uppercase
            std::transform(line.begin(), line.end(), line.begin(), ::toupper);

            for(int i = 0; i < (int)listedTitles.size(); i++) {
                // Iterate through the listedTitles, and compare the id's in the file, to the installed title ids
                // When a match is found, it sets the name of the excluded title to the name of the installed title
                if(listedTitles.at(i).id == line) {
                    name = listedTitles.at(i).name;
                    break;
                }
            }
            // Push a new title to the titles vector and increment the total number of excluded titles
            titles.push_back({line, name});
            total++;
        }
        // Close the file
        file.close();
    }

    // Sort the titles list (uses an overloaded < operator to sort based on ID's)
    std::sort(titles.begin(), titles.end());

    if (total > 0)
        std::cout << "Found " << total << " titles to exclude: " << std::endl;

    printTitles(titles);

    std::cout << std::endl << std::endl;

    // Create a vector to store the difference between the excluded titles list, and the installed title list
    std::vector<Title> diff;
    // Populate the difference vector
    std::set_difference(listedTitles.begin(), listedTitles.end(), titles.begin(), titles.end(), 
                         std::inserter(diff, diff.begin()));
    return diff;
}

bool caselessCompare (const std::string& a, const std::string& b){
    return strcasecmp(a.c_str(), b.c_str()) < 0;
}

int extractCheats(std::string zipPath, std::vector<Title> titles, bool sxos, bool credits){
    zipper::Unzipper unzipper(zipPath);
    std::vector<zipper::ZipEntry> entries = unzipper.entries();
    std::vector<Title> extractedTitles;
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

    std::sort(entriesNames.begin(), entriesNames.end(), caselessCompare);

    std::vector<std::string> parents;
    std::vector<std::vector<std::string>> children;
    std::vector<std::string> tempChildren;

    size_t k = 0;
    while(k < entriesNames.size()){
        if(entriesNames[k].length() == (size_t) (offset + 17)){
            parents.push_back(entriesNames[k]);
            k++;
            while(entriesNames[k].length() != (size_t) (offset + 17) && k < entriesNames.size()){
                if(credits == false){ 
                    if(strcasecmp(entriesNames[k].substr(offset + 16, 7).c_str(), "/cheats") == 0){
                        tempChildren.push_back(entriesNames[k]);
                    }
                }
                else{
                    tempChildren.push_back(entriesNames[k]);
                }
                k++;
            }
            children.push_back(tempChildren);
            tempChildren.clear (); 
        }
        else{
            k++;
        }
    }

    std::cout << "\033[1;4;31m"<< "\n*** Extracting cheats (this may take a while) ***" << "\033[0m" <<std::endl;

    int count = 0;
    size_t lastL = 0;
    std::string name;
    std::string id;
    bool alreadyExtractedTitle;
    for(size_t j = 0; j < titles.size(); j++){
        for(size_t l = lastL; l < parents.size(); l++){
            if(strcasecmp((titles.at(j).id).c_str(), parents[l].substr(offset, 16).c_str()) == 0){
                unzipper.extractEntry(parents[l]);
                for(auto& e : children[l]){
                    unzipper.extractEntry(e);
                    std::cout << std::setfill(' ') << std::setw(80) << '\r';

                    name = "No name retrieved.";
                    id = e.substr(7, 16);
                    std::transform(id.begin(), id.end(), id.begin(), ::toupper);

                    for(int i = 0; i < (int)titles.size(); i++) {
                        if(titles.at(i).id == id) {
                            name = titles.at(i).name;
                            break;
                        }
                    }

                    alreadyExtractedTitle = false;
                    for(int i = 0; i < (int)extractedTitles.size(); i++) {
                        if(extractedTitles.at(i).id == id) {
                            alreadyExtractedTitle = true;
                            break;
                        }
                    }
                    if(!alreadyExtractedTitle)
                        extractedTitles.push_back({id, name});

                    std::cout << e.substr(0, 79) << "\r";
                    count++;
                    consoleUpdate(NULL);
                }
                lastL = l;
                break;
            }
        }
    }
    std::cout << std::endl;

    writeTitlesToFile(extractedTitles);

    unzipper.close();
    return count;
}

int removeCheats(bool sxos){
    std::string path;
    if(sxos){
        path = std::string(SXOS_PATH) + std::string(TITLES_PATH);
        std::filesystem::remove("./titles.zip");
    } else {
        path = std::string(AMS_PATH) + std::string(CONTENTS_PATH);
        std::filesystem::remove("./contents.zip");
    }
    int c = 0;
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        std::string cheatsPath =  entry.path().string() + "/cheats";
        if(std::filesystem::exists(cheatsPath)){
            for (const auto & cheat : std::filesystem::directory_iterator(cheatsPath)){
                std::filesystem::remove(cheat);
                c++;
            }
            //std::filesystem::remove(cheatsPath);
            rmdir(cheatsPath.c_str());
            if(std::filesystem::is_empty(entry)){
                rmdir(entry.path().string().c_str());
            }
        }
    }
    return c;
}