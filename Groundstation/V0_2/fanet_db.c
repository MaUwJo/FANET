/*
 * fanet_db.c
 * 
 * Copyright 2018  <pi@pi42>
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
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "fanet_struct.h"
#include "fanet_global.h"
#include "fanet_db.h"

#define KNRM  "\x1B[0m"		// Color: Normal
#define KRED  "\x1B[31m"	// Color: Red
#define KGRN  "\x1B[32m"	// Color: Green
#define KYEL  "\x1B[33m"	// Color: Yellow
#define KBLU  "\x1B[34m"	// Color: Blue
#define KMAG  "\x1B[35m"	// Color: Mangenta
#define KCYN  "\x1B[36m"	// Color: Cyan
#define KWHT  "\x1B[37m"	// Color: White

char db_query_debug = 0;
char function_debug  = 0;
char message_debug   = 0;

char* dfl_db_name = "FANET_Station.db";
sqlite3 *db_conn;

//---------------------------------------------------------------------
static int db_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

/***********************************************************************
 * Init SQLite DB
 *
 * *********************************************************************
 * Ver | Date     | Sign | Description
 * ---------------------------------------------------------------------
 * 0.1 |29.09.2019|  mr  | SQLite3 Init Version
 **********************************************************************/
void db_init (char *_db_name)
{
    if (_db_name != NULL) {
        _db_name = dfl_db_name; // fallback/default
    }
    if (function_debug) printf("[%sFUNC%s]  db_init()\n",KCYN, KNRM);

    char *zErrMsg = 0;
    int rc;
    char *sql;

    rc = sqlite3_open(_db_name, &db_conn);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_conn));
        return;
    }
    fprintf(stderr, "Opened database successfully\n");

    /* Create SQL statement */
    sql = "CREATE TABLE FNS_PARAMS("  \
      "PARAM_NAME     CHAR(17)  PRIMARY KEY  NOT NULL," \
      "PARAM_VALUE    CHAR(42) );";

    /* Execute SQL statement */
    rc = sqlite3_exec(db_conn, sql, db_callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }

    sql = "INSERT INTO FNS_PARAMS (PARAM_NAME, PARAM_VALUE) "  \
            "VALUES ('STATION_ID', 'GER-1742' ); " \
          "INSERT INTO FNS_PARAMS (PARAM_NAME, PARAM_VALUE) "  \
            "VALUES ('STATION_NAME', 'GER/Palz' ); " \
          "INSERT INTO FNS_PARAMS (PARAM_NAME, PARAM_VALUE) "  \
            "VALUES ('STATION_LAT', '49.131702' ); " \
          "INSERT INTO FNS_PARAMS (PARAM_NAME, PARAM_VALUE) "  \
            "VALUES ('STATION_LON', '8.224818' );";

    /* Execute SQL statement */
    rc = sqlite3_exec(db_conn, sql, db_callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db_conn);
}

/***********************************************************************
 * Login into SQLite DB
 *
 * *********************************************************************
 * Ver | Date     | Sign | Description
 * ---------------------------------------------------------------------
 * 0.1 |24.09.2018|  bet | Init Version
 * 0.2 |29.09.2019|  mr  | SQLite3
 **********************************************************************/
void db_login (char *_db_name)
{
    if (_db_name != NULL) {
        _db_name = dfl_db_name; // fallback/default
    }
    if (function_debug) printf("[%sFUNC%s]  db_login()\n",KCYN, KNRM);

    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open(_db_name, &db_conn);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_conn));
        return;
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
}

/*
static int get_param_callback(void *data, int argc, char **argv, char **azColName){
    int i;
    fprintf(stderr, "%s: ", (const char*)data);

    for(i = 0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
}
*/
int get_parameter(const char *_name, char *_value, const char *_default)
{
    if (db_conn != NULL) {
        char *zErrMsg = 0;
        int rc;
        char *sql = "SELECT PARAM_VALUE FROM FNS_PARAMS WHERE PARAM_NAME = ?";
        sqlite3_stmt *res;

        /*
        const char* db_data = "Callback function called";

        sprintf(sql,
                "SELECT %s FROM %s WHERE %s = '%s'",
                "PARAM_VALUE", "FNS_PARAMS", "PARAM_NAME", _name);

        rc = sqlite3_exec(db_conn, sql, get_param_callback, (void*)db_data, &zErrMsg);

        if( rc != SQLITE_OK ) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        */

        rc = sqlite3_prepare_v2(db_conn, sql, -1, &res, 0);
        sqlite3_bind_text(res, 1, _name, -1, 0);

        if (rc == SQLITE_OK) {

            int step = sqlite3_step(res);
            if (step == SQLITE_ROW) {
                strcpy(_value, sqlite3_column_text(res, 0));
            }

        } else {

            fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db_conn));
        }

        sqlite3_finalize(res);

    } else {
        // get default value passed in
        strcpy(_value, _default);
    }

}

// Get own station parameters
void get_station_parameters (sWeather *_station_data)
{
    if (db_conn != NULL) {
        get_parameter("STATION_ID", _station_data->id_station, "TTT");
        char param_value[42];
        float float_value;
        get_parameter("STATION_LAT", param_value, "49.94");
        float_value = atof(param_value);
        _station_data->latitude = atof(param_value);
        get_parameter("STATION_LON", param_value, "8.8");
        float_value = atof(param_value);
        _station_data->longitude = atof(param_value);
    } else {
        get_weather_station("JWD", _station_data);
    }
}


/***********************************************************************
 * SQL error handler
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |24.09.2018|  bet | Init Version
 * 0.2 |29.09.2019|  mr  | SQLite3
 **********************************************************************/
void sql_finish_with_error(sqlite3 *_db_conn)
{
  fprintf(stderr, "[%sERR%s]   MySQL error: %s\n",KRED, KNRM, sqlite3_errmsg(_db_conn));
  sqlite3_close(_db_conn);
  exit(1);        
}

void get_weather_data(sWeather *_weather_data)
{
//	sql_get_weather_values (conn, res, row, _weather_data);
// get dynamic weather data or dummy
    if (0 == strcmp(_weather_data->id_station, "4217")) {
        _weather_data->wind = true;
        _weather_data->wind_speed   = 42.0;
        _weather_data->wind_gusts   = 88.0;
        _weather_data->wind_heading = 271.17;

        _weather_data->temp         = true;
        _weather_data->temperature  = 17.42;

        _weather_data->barom        = false;
        _weather_data->humid        = false;
        // Todo:    _weather_data->time
    } else if (0 == strcmp(_weather_data->id_station, "H711")) {
        _weather_data->wind         = true;
        _weather_data->wind_speed   = 11.0;
        _weather_data->wind_gusts   = 19.11;
        _weather_data->wind_heading = 300.11;

        _weather_data->temp         = true;
        _weather_data->temperature  = 7.11;
    } else if (0 == strcmp(_weather_data->id_station, "H795")) {
        _weather_data->wind         = true;
        _weather_data->wind_speed   = 7.95;
        _weather_data->wind_gusts   = 21.0;
        _weather_data->wind_heading = 195.91;

        _weather_data->temp         = true;
        _weather_data->temperature  = 17.95;
    } else {
        _weather_data->wind         = true;
        _weather_data->wind_speed   = 10.0;
        _weather_data->wind_gusts   = 20.0;
        _weather_data->wind_heading = 360.0;

        _weather_data->temp         = true;
        _weather_data->temperature  = 42.17;
    }
    _weather_data->barom        = false;
    _weather_data->humid        = false;

}

void get_weather_station(const char* _station_id, sWeather *_weather_data)
{
//	sql_get_weather_station (conn, res, row, _weather_data);
    if (0 == strcmp(_station_id, "4217")) {
        // GER - Dummy
        strcpy (_weather_data->name, "GS:Germersche/Palz");
        strcpy (_weather_data->short_name, "GS-G");
        _weather_data->longitude =  8.37;
        _weather_data->latitude  = 49.22;
        _weather_data->altitude  = 117;
    } else if (0 == strcmp(_station_id, "H711")) {
        // https://holfuy.com/de/map/la=49.20255&lo=8.00506&z=14
        strcpy (_weather_data->name, "Holfuy-WS 711 - Hohenberg/Pfalz");
        strcpy (_weather_data->short_name, "W711:Hohe");
        _weather_data->longitude =  8.00506;
        _weather_data->latitude  = 49.20255;
        _weather_data->altitude  = 565;
    } else if (0 == strcmp(_station_id, "H795")) {
        // https://holfuy.com/de/map/la=49.23716&lo=8.02495&z=14
        strcpy (_weather_data->name, "Holfuy-WS 795 - Orensberg/Pfalz");
        strcpy (_weather_data->short_name, "W795:Ori");
        _weather_data->longitude =  8.02495;
        _weather_data->latitude  = 49.23716;
        _weather_data->altitude  = 555;
    } else {
        printf("ID: %s\n", _station_id);
        strcpy (_weather_data->id_station, _station_id);
        sprintf(_weather_data->name, "Unknown:%s", _station_id);
        sprintf(_weather_data->short_name, "UNK:%s", _station_id);
        _weather_data->longitude = 17.0;
        _weather_data->latitude  = 42.0;
        _weather_data->altitude  = 1742;
    }
}

void write_system_data_1min (sSystem *_system_data)
{
//	sql_write_system_data_1min (conn, res, row, _system_data);
}


void write_system_data_15min (sSystem *_system_data)
{
//	sql_write_system_data_15min (conn, res, row, _system_data);
}


void write_object_tracking (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sTRACKING *_tracking)
{
//	sql_write_tracking_data (conn, res, row, _radiodata, _fanet_mac, _tracking);
}

void write_object_ground_tracking (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sGroundTracking *_tracking)
{
//	sql_write_tracking_data (conn, res, row, _radiodata, _fanet_mac, _tracking);
}


void write_object_name (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sName *_name)
{
//	sql_write_object_name (conn, res, row, _radiodata, _fanet_mac, _name);
}

void update_routing_table (sRouting *_routing_data)
{
//	sql_update_routing_table (conn, res, row, _routing_data);
}

void write_message_send (sMessage *_message)
{
//	sql_write_message_send (conn, res, row, _message);
}

void search_last_welcome_message (sMessage *_message)
{
//	sql_search_last_welcome_message (conn, res, row, _message);
}

void search_online_address (sOnline *_online)
{
//	sql_search_online_address (conn, res, row, _online);
    _online->online = 1;
}

void delete_old_data (void)
{
//	sql_delete_old_data (conn, res, row);
}

