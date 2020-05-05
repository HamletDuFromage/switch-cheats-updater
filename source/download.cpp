
#include "download.hpp"

#define API_AGENT           "HamletDuFromage"
#define _1MiB   0x100000

typedef struct
{
    char *memory;
    size_t size;
} MemoryStruct_t;

typedef struct
{
    u_int8_t *data;
    size_t data_size;
    u_int64_t offset;
    FILE *out;
} ntwrk_struct_t;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t num_files, void *userp)
{
    ntwrk_struct_t *data_struct = (ntwrk_struct_t *)userp;
    size_t realsize = size * num_files;

    if (realsize + data_struct->offset >= data_struct->data_size)
    {
        fwrite(data_struct->data, data_struct->offset, 1, data_struct->out);
        data_struct->offset = 0;
    }

    memcpy(&data_struct->data[data_struct->offset], contents, realsize);
    data_struct->offset += realsize;
    data_struct->data[data_struct->offset] = 0;
    return realsize;
}

int download_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
    if (dltotal <= 0.0) return 0;

    struct timeval tv = {0};
    gettimeofday(&tv, NULL);
    int counter = round(tv.tv_usec / 100000);

    if (counter == 0 || counter == 2 || counter == 4 || counter == 6 || counter == 8)
    {
        printf("*** DOWNLOADING: %.2fMB of %.2fMB ***\r", dlnow / _1MiB, dltotal / _1MiB);
        consoleUpdate(NULL);
    }

    return 0;
}

bool downloadFile(const char *url, const char *output, int api)
{
    CURL *curl = curl_easy_init();
    if (curl)
    {
        FILE *fp = fopen(output, "wb");
        if (fp)
        {
            printf("\n");

            ntwrk_struct_t chunk = {0};
            chunk.data = static_cast<u_int8_t *>(malloc(_1MiB));
            chunk.data_size = _1MiB;
            chunk.out = fp;

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            // write calls
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

            if (api == OFF)
            {
              curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
              curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress);
            }

            // execute curl, save result
            CURLcode res = curl_easy_perform(curl);

            // write from mem to file
            if (chunk.offset)
              fwrite(chunk.data, 1, chunk.offset, fp);

            // clean
            curl_easy_cleanup(curl);
            free(chunk.data);
            fclose(chunk.out);

            if (res == CURLE_OK)
            {
                std::cout << std::endl;
                consoleUpdate(NULL);
                return true;
            }
        }
    }
    consoleUpdate(NULL);
    return false;
}

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback2(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = static_cast<char *>(realloc(mem->memory, mem->size + realsize + 1));
  if(ptr == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

//1100 1300
std::string fetchVersion(const char *url, const char *range){
  CURL *curl_handle;
  CURLcode res;
 
  struct MemoryStruct chunk;
 
  chunk.memory = static_cast<char *>(malloc(1));  /* will be grown as needed by the realloc above */ 
  chunk.size = 0;    /* no data at this point */ 
 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* specify URL to get */ 
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
 

  curl_easy_setopt(curl_handle, CURLOPT_RANGE, range);
  /* send all data to this function  */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback2);
 
  /* we pass our 'chunk' struct to the callback function */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
 
  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */ 
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, API_AGENT);

  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
 
  /* get it! */ 
  res = curl_easy_perform(curl_handle);
 
  /* check for errors */ 
    std::string ver = "-1";
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() for page fetching failed: %s\n",
            curl_easy_strerror(res));
  }
  else {
    std::string s = std::string(chunk.memory);
    //std::cout << "start html " << s.substr(0, 199) << std::endl;
    std::regex rgx("<title>.+</title>");
    std::smatch match;
    if (std::regex_search(s, match, rgx)){
        //ver = std::stoi(match[0]);
        //std::cout << match[0].str().substr(match[0].str().find(" ") + 1, 6) << std::endl;
        ver = match[0].str().substr(match[0].str().find(" ") + 1, 5);
    }
    
 
    //printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
  }
 
  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);
 
  free(chunk.memory);
 
  /* we're done with libcurl, so clean it up */ 
  curl_global_cleanup();
 
  return ver;
}

