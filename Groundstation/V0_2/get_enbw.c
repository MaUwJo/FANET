/*
 * get_enbw.c
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
//#include <curl/easy.h>

#include "fanet_struct.h"
#include "fanet_enbw.h"

sWeather enbw_weather;


int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);
    // https://ecockpit2.azurewebsites.net/logic/measurements/park/DERP_.FFD01WN/
    char *en_token = "TTT";
    get_enbw_park_json("DERP_.FFD01WN", en_token, &enbw_weather);
    curl_global_cleanup();
    return 0;
}
