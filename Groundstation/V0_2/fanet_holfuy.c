//
// Created by Reinart, Manfred on 08.10.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <curl/curl.h>
#include <curl/easy.h>

#include "fanet_holfuy.h"
#include "fanet_struct.h"

const char *holfuyUrlFormat = "http://api.holfuy.com/live/?s=%s&pw=%s";

//sWeather hf711_weather;
//sWeather hf795_weather;


struct MemoryStruct {
    char *memory;
    size_t size;
};

// Function to remove all spaces from a given string
// https://www.geeksforgeeks.org/remove-spaces-from-a-given-string/
void removeSpaces(char *str)
{
    // To keep track of non-space character count
    int count = 0;

    // Traverse the given string. If current character
    // is not space, then place it at index 'count++'
    for (int i = 0; str[i]; i++)
        if (str[i] != ' ')
            str[count++] = str[i]; // here count is
    // incremented
    str[count] = '\0';
}

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
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

int get_holfuy_weather(const char* holfuy_id, const char* token, sWeather *_weather) {
    char url[72];
    sprintf(url, holfuyUrlFormat, holfuy_id, token);
    printf(" - fetching from URL: %s", url);

    CURL *curl_handle;
    CURLcode res;
    int error = 0;
    struct MemoryStruct hf_chunk;
    hf_chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    hf_chunk.size = 0;    /* no data at this point */
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&hf_chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl_handle);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    } else {
        printf(" : %s\n", hf_chunk.memory);
        char *parsed[20];
        int numToken = 0;
        char *token = strtok(hf_chunk.memory, ",");
        while (token != NULL)
        {
            parsed[numToken++] = token;
            token = strtok(NULL, ",");
        }
        parsed[numToken] = NULL;
        float float_value;
        removeSpaces(parsed[0]); // leading space in result string
        if (0 == strcmp(holfuy_id, parsed[0])) {
            // matching station data returned
            sprintf(_weather->id_station, "%s", parsed[0]);
            sprintf(_weather->name, "HF-%s - %s", parsed[0], parsed[1]);
            sprintf(_weather->short_name, "HF-%s", parsed[0]);
            // Time
            _weather->time = 0;
            if ((NULL != parsed[2]) && (NULL != parsed[3])) {
                char time_str[25];
                sprintf(time_str, "%s %s", parsed[2], parsed[3]);
                struct tm tm;
                strftime(time_str, 100, "%Y-%m-%d %H:%M:%S", &tm);
                _weather->time = mktime(&tm);
            }
            // Wind - default: m/s
            _weather->wind = false;
            _weather->wind_speed = 0.0;
            _weather->wind_gusts = 0.0;
            _weather->wind_heading = 0.0;
            if (NULL != parsed[4]) {
                _weather->wind_speed = atof(parsed[4]);
                _weather->wind = true;
            }
            if (NULL != parsed[5]) {
                _weather->wind_gusts = atof(parsed[5]);
                _weather->wind = true;
            }
            if (NULL != parsed[7]) {
                _weather->wind_heading = atof(parsed[7]);
                _weather->wind = true;
            }
            // Temperature - default: C
            _weather->temp = false;
            _weather->temperature = 0.0;
            if (NULL != parsed[8]) {
                _weather->temp = true;
                _weather->temperature = atof(parsed[8]);
            } else {
            }
            // No other sensor so far in 711/795
            _weather->humid = false;
            _weather->humidity = 0.0;
            _weather->barom = false;
            _weather->barometric = 0.0;
        } else {
            error = 1;
        }
    }
    curl_easy_cleanup(curl_handle);
    free(hf_chunk.memory);
    curl_global_cleanup();
    return error;
}
