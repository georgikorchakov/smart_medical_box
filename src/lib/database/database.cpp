#include "./database.h"

Database::Database(std::string host, std::string user, std::string password, std::string db)
    : __db_connection{ mysql_init(NULL) }
{
    // Check DB initialization
    if (__db_connection == NULL)
    {
        std::cout << mysql_error(__db_connection) << '\n';
        return;
    }

    // Try to connect to mysql server
    if (mysql_real_connect(__db_connection, host.c_str(), user.c_str(), password.c_str(), NULL, 0, NULL, 0) == NULL)
    {
        std::cout << mysql_error(__db_connection) << '\n';
        return;
    }

    // Initiate database if needed and connect to it
    if (__initDatabaseIfNeededAndConnect(db))
    {
        std::cout << mysql_error(__db_connection) << '\n';
        return;
    }
}

Database::~Database()
{
    mysql_close(__db_connection);
}

////////////////////////////////////////////////////////////
// Utils
////////////////////////////////////////////////////////////
std::string Database::__escapeString(std::string str)
{
    char buffer[str.length() * 2 + 1];
    mysql_real_escape_string(__db_connection, buffer, str.c_str(), str.length());
    return buffer;
}

int Database::__errorReporting(int error_code)
{
    if (error_code)
    {
        std::cout << mysql_error(__db_connection) << '\n';
    }
    return error_code;
}

////////////////////////////////////////////////////////////
// Database Initialization
////////////////////////////////////////////////////////////
int Database::__initDatabaseIfNeededAndConnect(std::string db)
{
    std::string create_database;
    create_database = "CREATE DATABASE IF NOT EXISTS " + __escapeString(db) + " CHARACTER SET = 'utf8'";

    if (mysql_query(__db_connection, create_database.c_str()))
    {
        std::cout << mysql_error(__db_connection) << '\n';
    }

    if (mysql_select_db(__db_connection, db.c_str()))
    {
        std::cout << mysql_error(__db_connection) << '\n';
    }

    std::string create_table_time_intervals = 
        "CREATE TABLE IF NOT EXISTS `time_intervals` ("
        "`id` INT PRIMARY KEY AUTO_INCREMENT, "
        "`interval_name` VARCHAR(255), "
        "`active` int, "
        "`day_of_week` VARCHAR(15), "
        "`time_start` TIME, "
        "`time_end` TIME, "
        "UNIQUE INDEX `interval` (`interval_name`, `day_of_week`) )";
    
    if (mysql_query(__db_connection, create_table_time_intervals.c_str()))
    {
        std::cout << mysql_error(__db_connection) << '\n';
    }

    std::string create_table_insulin_time_intervals = 
        "CREATE TABLE IF NOT EXISTS `insulin_time_intervals` ("
        "`id` INT PRIMARY KEY AUTO_INCREMENT, "
        "`interval_number` int, "
        "`insulin_type` int, "
        "`time_start` TIME, "
        "`time_end` TIME, "
        "UNIQUE INDEX `interval` (`interval_number`, `insulin_type`) )";
    
    if (mysql_query(__db_connection, create_table_insulin_time_intervals.c_str()))
    {
        std::cout << mysql_error(__db_connection) << '\n';
    }

    return 0;
}

////////////////////////////////////////////////////////////
// Time Interval Functions
////////////////////////////////////////////////////////////
TimeInterval Database::__extractTimeIntervalFromSqlResult(MYSQL_RES **result)
{
    TimeInterval time_interval;

    unsigned int fields_count = mysql_num_fields(*result);
    MYSQL_ROW interval = mysql_fetch_row(*result);
    MYSQL_FIELD *field;

    for (unsigned int field_number = 0; field_number < fields_count; ++field_number)
    {
        field = mysql_fetch_field(*result);
        if (strcmp("interval_name", field->name) == 0)
        {
            time_interval.interval_name.assign(interval[field_number]);
        }

        if (strcmp("time_start", field->name) == 0)
        {
            // Hours
            char hh[3];
            sprintf(hh, "%c%c", interval[field_number][0], interval[field_number][1]);
            time_interval.time_start_h = std::stoi(hh);

            // Minutes
            char mm[3];
            sprintf(mm, "%c%c", interval[field_number][3], interval[field_number][4]);
            time_interval.time_start_m = std::stoi(mm);
        }

        if (strcmp("active", field->name) == 0)
        {
            // State of the interval
            time_interval.active = std::stoi(interval[field_number]);
        }

        if (strcmp("day_of_week", field->name) == 0)
        {
            // Day of week -> 0 - Monday, 6 - Sunday, 7 - Every day
            if (interval[field_number][0] == '*')
            {
                time_interval.week_day = 7;   
            }
            else
            {
                time_interval.week_day = std::stoi(interval[field_number]);
            }
        }

        if (strcmp("time_end", field->name) == 0)
        {
            // Hours
            char hh[3];
            sprintf(hh, "%c%c", interval[field_number][0], interval[field_number][1]);
            time_interval.time_end_h = std::stoi(hh);

            // Minutes
            char mm[3];
            sprintf(mm, "%c%c", interval[field_number][3], interval[field_number][4]);
            time_interval.time_end_m = std::stoi(mm);
        }
    }

    return time_interval;
}

int Database::createOrUpdateTimeInterval(std::string interval_name, int time_start_h, int time_start_m, int time_end_h, int time_end_m, int active, int day_of_week)
{
    std::string time_start_str = "'" + std::to_string(time_start_h) + ":" + std::to_string(time_start_m) + "'";
    std::string time_end_str = "'" + std::to_string(time_end_h) + ":" + std::to_string(time_end_m) + "'";
    std::string day_of_week_str = "'" + ((day_of_week == 7) ? "*" : std::to_string(day_of_week)) + "'";
    std::string active_str = "'" + std::to_string(active) + "'";

    std::string insert_or_update_time_interval = 
        "INSERT INTO `time_intervals` (`interval_name`, `active`, `day_of_week`, `time_start`, `time_end`) "
            "VALUES ('" + __escapeString(interval_name) + "', " + active_str + ", " + day_of_week_str + ", "
            + time_start_str + ", " + time_end_str + ") " 
        "ON DUPLICATE KEY UPDATE "
            "`time_start` = " + time_start_str + ", `time_end` = " + time_end_str + ", `active` = " + active_str;

    if (mysql_query(__db_connection, insert_or_update_time_interval.c_str()))
    {
        std::cout << mysql_error(__db_connection) << '\n';
        return -1;
    }

    return 0;
}

TimeInterval Database::getDefaultTimeIntervalByTime(int time_h, int time_m)
{
    TimeInterval time_interval;
    std::string current_time_str = "'" + std::to_string(time_h) + ":" + std::to_string(time_m) + "'";
    
    std::string select_default_interval_sql = 
        "SELECT * FROM `time_intervals` WHERE "
            "CASE WHEN `time_end` < `time_start` THEN "
                "(`time_start` <= "+current_time_str+" AND "+current_time_str+" <= '23:59') OR "
                "('00:00' <= "+current_time_str+" AND "+current_time_str+" <= `time_end`) "
            "ELSE "
                "(`time_start` <= "+current_time_str+" AND "+current_time_str+" <= time_end) "
            "END "
            "AND day_of_week = '*'"
            "AND active = 1 "
            "LIMIT 1";

    __errorReporting( 
            mysql_query(__db_connection, select_default_interval_sql.c_str()) );

    MYSQL_RES *default_interval_data = mysql_store_result(__db_connection);

    if (mysql_num_rows(default_interval_data) == 0)
    {
        mysql_free_result(default_interval_data);
        time_interval.is_empty = 1;
        return time_interval;
    }
    
    time_interval = __extractTimeIntervalFromSqlResult(&default_interval_data);
    time_interval.is_empty = 0;
    mysql_free_result(default_interval_data);
    return time_interval;
}

TimeInterval Database::getSpecificTimeIntervalByTimeAndDay(int time_h, int time_m, int day_of_week)
{
    TimeInterval time_interval;
    std::string current_time_str = "'" + std::to_string(time_h) + ":" + std::to_string(time_m) + "'";
    std::string day_of_week_str = "'" + std::to_string(day_of_week) + "'";

    std::string select_specific_interval_sql = 
        "SELECT * FROM `time_intervals` WHERE "
            "CASE WHEN `time_end` < `time_start` THEN "
                "(`time_start` <= "+current_time_str+" AND "+current_time_str+" <= '23:59') OR "
                "('00:00' <= "+current_time_str+" AND "+current_time_str+" <= `time_end`) "
            "ELSE "
                "(`time_start` <= "+current_time_str+" AND "+current_time_str+" <= time_end) "
            "END "
            "AND day_of_week = "+day_of_week_str+
            "AND active = 1 "
            "LIMIT 1";

    __errorReporting( 
            mysql_query(__db_connection, select_specific_interval_sql.c_str()) );

    MYSQL_RES *specific_interval_data = mysql_store_result(__db_connection);

    if (mysql_num_rows(specific_interval_data) == 0)
    {
        mysql_free_result(specific_interval_data);
        time_interval.is_empty = 1;
        return time_interval;
    }
    
    time_interval = __extractTimeIntervalFromSqlResult(&specific_interval_data);
    time_interval.is_empty = 0;
    mysql_free_result(specific_interval_data);
    return time_interval;
}

TimeInterval Database::getTimeIntervalByTimeAndDay(int time_h, int time_m, int day_of_week)
{
    if (isSpecificallyDefinedDay(day_of_week))
    {
        return getSpecificTimeIntervalByTimeAndDay(time_h, time_m, day_of_week);
    }
    else
    {
        return getDefaultTimeIntervalByTime(time_h, time_m);
    }
}

int Database::isSpecificallyDefinedDay(int day_of_week)
{
    std::string day_of_week_str = "'" + std::to_string(day_of_week) + "'";

    // Search for intervals with specific day
    std::string search_for_specific_intervals = 
        "SELECT * FROM `time_intervals` WHERE "
            "day_of_week = "+day_of_week_str;

    // There must be at least one interval with a specific day 
    // to count the entire day as specifically defined
    __errorReporting( mysql_query(__db_connection, search_for_specific_intervals.c_str()) );

    MYSQL_RES *matching_time_intervals = mysql_store_result(__db_connection);
    int num_of_intervals_with_matching_day = mysql_num_rows(matching_time_intervals);
    mysql_free_result(matching_time_intervals);

    return num_of_intervals_with_matching_day != 0;
}

TimeInterval Database::getTimeIntervalByIntervalName(std::string interval_name)
{
    std::string select_interval_by_name_sql = 
        "SELECT * FROM `time_intervals` WHERE "
            "interval_name = '" + __escapeString(interval_name) + "' "
            "AND day_of_week = '*'"
            "LIMIT 1";

    __errorReporting( 
            mysql_query(__db_connection, select_interval_by_name_sql.c_str()) );

    MYSQL_RES *interval_data = mysql_store_result(__db_connection);
    TimeInterval time_interval;

    if (mysql_num_rows(interval_data) == 0)
    {
        mysql_free_result(interval_data);
        time_interval.is_empty = 1;
        return time_interval;
    }
    
    time_interval = __extractTimeIntervalFromSqlResult(&interval_data);
    time_interval.is_empty = 0;
    mysql_free_result(interval_data);
    return time_interval;
}

int Database::isTimeIntervalEmpty(TimeInterval time_interval)
{
    return time_interval.is_empty;
}


////////////////////////////////////////////////////////////
// Insulin Time Interval Functions
////////////////////////////////////////////////////////////
int Database::createOrUpdateInsulinTimeInterval(int interval_number, int insulin_type, int time_start_h, int time_start_m, int time_end_h, int time_end_m)
{
    std::string time_start_str = "'" + std::to_string(time_start_h) + ":" + std::to_string(time_start_m) + "'";
    std::string time_end_str = "'" + std::to_string(time_end_h) + ":" + std::to_string(time_end_m) + "'";
    std::string interval_number_str = "'" + std::to_string(interval_number) + "'";
    std::string insulin_type_str = "'" + std::to_string(insulin_type) + "'";

    std::string insert_or_update_insulin_time_interval = 
        "INSERT INTO `insulin_time_intervals` (`interval_number`, `insulin_type`, `time_start`, `time_end`) "
            "VALUES (" + interval_number_str + ", " + insulin_type_str + ", " 
            + time_start_str + ", " + time_end_str + ") " 
        "ON DUPLICATE KEY UPDATE "
            "`time_start` = " + time_start_str + ", `time_end` = " + time_end_str;

    if (mysql_query(__db_connection, insert_or_update_insulin_time_interval.c_str()))
    {
        std::cout << mysql_error(__db_connection) << '\n';
        return -1;
    }

    return 0;
}

InsulinTimeInterval Database::getInsulinTimeIntervalByTime(int time_h, int time_m)
{
    InsulinTimeInterval insulin_time_interval;
    std::string current_time_str = "'" + std::to_string(time_h) + ":" + std::to_string(time_m) + "'";
    
    std::string select_default_interval_sql = 
        "SELECT * FROM `insulin_time_intervals` WHERE "
            "CASE WHEN `time_end` < `time_start` THEN "
                "(`time_start` <= "+current_time_str+" AND "+current_time_str+" <= '23:59') OR "
                "('00:00' <= "+current_time_str+" AND "+current_time_str+" <= `time_end`) "
            "ELSE "
                "(`time_start` <= "+current_time_str+" AND "+current_time_str+" <= time_end) "
            "END "
            "LIMIT 1";

    __errorReporting( 
            mysql_query(__db_connection, select_default_interval_sql.c_str()) );

    MYSQL_RES *default_interval_data = mysql_store_result(__db_connection);

    if (mysql_num_rows(default_interval_data) == 0)
    {
        mysql_free_result(default_interval_data);
        insulin_time_interval.is_empty = 1;
        return insulin_time_interval;
    }
    
    insulin_time_interval = __extractInsulinTimeIntervalFromSqlResult(&default_interval_data);
    insulin_time_interval.is_empty = 0;
    mysql_free_result(default_interval_data);
    return insulin_time_interval;
}

InsulinTimeInterval Database::getInsulinTimeIntervalByNumberAndType(int interval_number, int insulin_type)
{
    InsulinTimeInterval insulin_time_interval;
    std::string interval_number_str = "'" + std::to_string(interval_number) + "'";
    std::string insulin_type_str = "'" + std::to_string(insulin_type) + "'";

    std::string select_insulin_interval_by_number_and_type_sql = 
        "SELECT * FROM `insulin_time_intervals` WHERE "
            "interval_number = " + interval_number_str +
            " AND insulin_type = " + insulin_type_str +
            " LIMIT 1";

    __errorReporting( 
            mysql_query(__db_connection, select_insulin_interval_by_number_and_type_sql.c_str()) );

    MYSQL_RES *interval_data = mysql_store_result(__db_connection);

    if (mysql_num_rows(interval_data) == 0)
    {
        mysql_free_result(interval_data);
        insulin_time_interval.is_empty = 1;
        return insulin_time_interval;
    }
    
    insulin_time_interval = __extractInsulinTimeIntervalFromSqlResult(&interval_data);
    insulin_time_interval.is_empty = 0;
    mysql_free_result(interval_data);
    return insulin_time_interval;
}

int Database::deleteInsulinTimeIntervalByNumberAndType(int interval_number, int insulin_type)
{
    std::string interval_number_str = "'" + std::to_string(interval_number) + "'";
    std::string insulin_type_str = "'" + std::to_string(insulin_type) + "'";

    std::string delete_insulin_interval_by_number_and_type_sql = 
        "DELETE FROM `insulin_time_intervals` WHERE "
            "interval_number = " + interval_number_str +
            " AND insulin_type = " + insulin_type_str;

    __errorReporting( 
            mysql_query(__db_connection, delete_insulin_interval_by_number_and_type_sql.c_str()) );

    return 1;
}

InsulinTimeInterval Database::__extractInsulinTimeIntervalFromSqlResult(MYSQL_RES **result)
{
    InsulinTimeInterval insulin_time_interval;

    unsigned int fields_count = mysql_num_fields(*result);
    MYSQL_ROW interval = mysql_fetch_row(*result);
    MYSQL_FIELD *field;

    for (unsigned int field_number = 0; field_number < fields_count; ++field_number)
    {
        field = mysql_fetch_field(*result);

        if (strcmp("interval_number", field->name) == 0)
        {
            insulin_time_interval.interval_number = std::stoi(interval[field_number]);
        }

        if (strcmp("insulin_type", field->name) == 0)
        {
            insulin_time_interval.insulin_type = std::stoi(interval[field_number]);
        }

        if (strcmp("time_start", field->name) == 0)
        {
            // Hours
            char hh[3];
            sprintf(hh, "%c%c", interval[field_number][0], interval[field_number][1]);
            insulin_time_interval.time_start_h = std::stoi(hh);

            // Minutes
            char mm[3];
            sprintf(mm, "%c%c", interval[field_number][3], interval[field_number][4]);
            insulin_time_interval.time_start_m = std::stoi(mm);
        }

        if (strcmp("time_end", field->name) == 0)
        {
            // Hours
            char hh[3];
            sprintf(hh, "%c%c", interval[field_number][0], interval[field_number][1]);
            insulin_time_interval.time_end_h = std::stoi(hh);

            // Minutes
            char mm[3];
            sprintf(mm, "%c%c", interval[field_number][3], interval[field_number][4]);
            insulin_time_interval.time_end_m = std::stoi(mm);
        }
    }

    return insulin_time_interval;
}

int Database::isInsulinTimeIntervalEmpty(InsulinTimeInterval insulin_time_interval)
{
    return insulin_time_interval.is_empty;
}

////////////////////////////////////////////////////////////
// Debug Functions
////////////////////////////////////////////////////////////
void Database::printTimeIntervalStruct(TimeInterval time_interval)
{
    if (time_interval.is_empty == 1)
    {
        std::cout << "Empty interval\n";
        return; 
    }

    std::cout << "****************************************" << std::endl;
    std::cout << time_interval.interval_name << std::endl;
    std::cout << "Active: " << time_interval.active << std::endl;
    std::cout << "Day: " << time_interval.week_day << std::endl;
    std::cout << "From: " << time_interval.time_start_h << ":" << time_interval.time_start_m << std::endl;
    std::cout << "To: " << time_interval.time_end_h << ":" << time_interval.time_end_m << std::endl;
    std::cout << "****************************************" << std::endl;
}