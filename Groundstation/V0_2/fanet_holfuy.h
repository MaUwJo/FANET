//
// Created by Reinart, Manfred on 08.10.19.
//

#ifndef FANET_HOLFUY_H
#define FANET_HOLFUY_H

#include "fanet_struct.h"

int get_holfuy_weather(const char* holfuy_id, const char* token, sWeather *_weather);
int get_holfuy_weather_json(const char* holfuy_id, const char* token, sWeather *_weather);
int get_holfuy_puget_json(const char* holfuy_id, const char* token, sWeather *_weather);

#endif //FANET_HOLFUY_H
