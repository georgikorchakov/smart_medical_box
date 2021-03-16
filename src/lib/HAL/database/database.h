#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include <cstring>
#include <mariadb/mysql.h>

typedef struct {
    std::string interval_name;
    int is_empty;
    int time_start_h;
    int time_start_m;
    int time_end_h;
    int time_end_m;
} TimeInterval;

class Database
{
private:
    MYSQL *m_db_connection;

    // Utils
    std::string escapeString(std::string str);
    int errorReporting(int error_code);

    // Database Initialization
    int initDatabaseIfNeededAndConnect(std::string db);
    
    // Time Intervals functions
    TimeInterval extractTimeIntervalFromSqlResult(MYSQL_RES **result);
    
public:
    Database(std::string host, std::string user, std::string password, std::string db);
    ~Database();

    // Time Intervals functions
    int createOrUpdateTimeInterval(std::string interval_name, int time_start_h, int time_start_m, 
                               int time_end_h, int time_end_m, int day_of_week = 0);
    TimeInterval getDefaultTimeIntervalByTime(int time_h, int time_m);
    TimeInterval getSpecificTimeIntervalByTimeAndDay(int time_h, int time_m, int day_of_week);
    TimeInterval getTimeIntervalByTimeAndDay(int time_h, int time_m, int day_of_week);
    int isSpecificallyDefinedDay(int day_of_week);
    int isTimeIntervalEmpty(TimeInterval time_interval);
};

#endif // DATABASE_H