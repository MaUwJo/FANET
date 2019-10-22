//
// Created by Reinart, Manfred on 08.10.19.
//

//#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <curl/curl.h>
#include <curl/easy.h>

#include "fanet_holfuy.h"
#include "fanet_struct.h"

const char *enbwCockpitFormat = "https://ecockpit2.azurewebsites.net/logic/measurements/park/%s/";

struct MemoryStruct {
    char *memory;
    size_t size;
};

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


int get_enbw_park_json(const char* park_id, const char* token, sWeather *_weather) {
    char url[100];
    sprintf(url, enbwCockpitFormat, park_id);
    //printf(" - fetching JSON from URL: %s", url);

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

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, token);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, chunk);

    res = curl_easy_perform(curl_handle);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    } else {
    //    printf(" : %s\n", hf_chunk.memory);
        float float_value;
        _weather->time = 0; // default with current time?

        _weather->wind = false;
        _weather->wind_speed = 0.0;
        _weather->wind_gusts = 0.0;
        _weather->wind_heading = 0.0;

        _weather->temp = false;
        _weather->temperature = 0.0;

        _weather->humid = false;
        _weather->humidity = 0.0;
        _weather->barom = false;
        _weather->barometric = 0.0;

        // find substring '{"type":"GESCHWINDIGKEIT_WIND","quantity":{"value":'
        const char *speed_tag_substr = "{\"type\":\"GESCHWINDIGKEIT_WIND\",\"quantity\":{\"value\":\"";
        char *speed_value_pos = strstr(hf_chunk.memory, speed_tag_substr);
        if (speed_value_pos) {
            speed_value_pos += strlen(speed_tag_substr);
            char wind_speed_str[20] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
            strncpy(wind_speed_str, speed_value_pos, 17);
            _weather->wind_speed = atof(wind_speed_str);
            _weather->wind = true;
        }
        const char *head_tag_substr = "{\"type\":\"RICHTUNG_WIND\",\"quantity\":{\"value\":\"";
        char *head_value_pos = strstr(hf_chunk.memory, head_tag_substr);
        if (speed_value_pos) {
            head_value_pos += strlen(head_tag_substr);
            char wind_head_str[4] = "\0\0\0\0";
            strncpy(wind_head_str, head_value_pos, 3);
            _weather->wind_heading = atof(wind_head_str);
            _weather->wind = true;
        }

    }
    curl_easy_cleanup(curl_handle);
    free(hf_chunk.memory);
    curl_global_cleanup();
    return error;
}

