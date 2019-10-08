/*
 * get_holfuy.c
 *
 * Copyright 2019  <pi@pi4>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 **
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <curl/curl.h>
#include <curl/easy.h>
#include "csv.h"

#include "fanet_struct.h"
#include "fanet_holfuy.h"

sWeather hf711_weather;
sWeather hf795_weather;


int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);
/*
    strcpy(hf711_weather.id_station, "HF711");
    strcpy(hf711_weather.name, "Holfuy Station Hohenberg");
    strcpy(hf711_weather.short_name, "HF711/Hohe");
    strcpy(hf795_weather.id_station, "HF795");
    strcpy(hf795_weather.name, "Holfuy Station Orensberg");
    strcpy(hf795_weather.short_name, "HF795/Ori");
*/
    get_holfuy_weather("711", argv[1], &hf711_weather);
    get_holfuy_weather("795", argv[1], &hf795_weather);

    curl_global_cleanup();
    return 0;
}
