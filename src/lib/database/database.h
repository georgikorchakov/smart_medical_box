#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include <cstring>
#include <mariadb/mysql.h>

typedef struct {
    std::string interval_name;
    int is_empty;
    int active;
    int week_day;
    int time_start_h;
    int time_start_m;
    int time_end_h;
    int time_end_m;
} TimeInterval;

typedef struct {
    int is_empty;
    int interval_number;
    int insulin_type;
    int time_start_h;
    int time_start_m;
    int time_end_h;
    int time_end_m;
} InsulinTimeInterval;

#define INSULIN_TYPE_DAY 0
#define INSULIN_TYPE_NIGHT 1

class Database
{
private:
    MYSQL *__db_connection;

    // Utils
    std::string __escapeString(std::string str);
    int __errorReporting(int error_code);

    // Database Initialization
    int __initDatabaseIfNeededAndConnect(std::string db);
    
    // Time Intervals functions
    TimeInterval __extractTimeIntervalFromSqlResult(MYSQL_RES **result);
    InsulinTimeInterval __extractInsulinTimeIntervalFromSqlResult(MYSQL_RES **result);
    
public:
    Database(std::string host, std::string user, std::string password, std::string db);
    ~Database();

    // Time Intervals functions 
    // Every day(7)
    int createOrUpdateTimeInterval(std::string interval_name, int time_start_h, int time_start_m, 
                               int time_end_h, int time_end_m, int active = 0, int day_of_week = 7);
    TimeInterval getDefaultTimeIntervalByTime(int time_h, int time_m);
    TimeInterval getSpecificTimeIntervalByTimeAndDay(int time_h, int time_m, int day_of_week);
    TimeInterval getTimeIntervalByTimeAndDay(int time_h, int time_m, int day_of_week);
    TimeInterval getTimeIntervalByIntervalName(std::string interval_name);
    int isSpecificallyDefinedDay(int day_of_week);
    int isTimeIntervalEmpty(TimeInterval time_interval);

    //Insulin Time Intervals Functions
    int createOrUpdateInsulinTimeInterval(int interval_number, int insulin_type, int time_start_h, 
                                int time_start_m, int time_end_h, int time_end_m);
    InsulinTimeInterval getInsulinTimeIntervalByTime(int time_h, int time_m);
    InsulinTimeInterval getInsulinTimeIntervalByNumberAndType(int interval_number, int insulin_type);
    int deleteInsulinTimeIntervalByNumberAndType(int interval_number, int insulin_type);
    int isInsulinTimeIntervalEmpty(InsulinTimeInterval insulin_time_interval);

    // Debug
    void printTimeIntervalStruct(TimeInterval time_interval);
};

#endif // DATABASE_H