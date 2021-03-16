// g++ -Wall -Weffc++ -Wextra -Wsign-conversion  -std=c++17 ./lib/database/database.cpp db_test.cpp -I/usr/local/mysql-connector-c-6.1.11-macos10.12-x86_64/include -lmysqlclient -o db_test
// g++ -Wall -Weffc++ -Wextra -Wsign-conversion  -std=c++17 ./lib/database/database.cpp db_test.cpp `mysql_config --cflags` -lmysqlclient -o db_test
#include <iostream>
#include "./config/settings.h"
#include "./lib/database/database.h"

int main()
{
    Database myDb(settings::db_host, settings::db_user, settings::db_password, settings::db_name);
    myDb.createOrUpdateTimeInterval("morning", 9, 0, 11, 15);
    myDb.createOrUpdateTimeInterval("morning", 9, 15, 11, 15, 1);
    myDb.createOrUpdateTimeInterval("morning", 10, 15, 11, 15, 2);

    TimeInterval interval = myDb.getTimeIntervalByTimeAndDay(9, 0, 1);
    std::cout << "Is empty: " << myDb.isTimeIntervalEmpty(interval) << '\n';

    if (myDb.isTimeIntervalEmpty(interval) == 0)
        std::cout << interval.interval_name << " " << interval.time_start_h << ":" << interval.time_start_m << " " << interval.time_end_h << ":" << interval.time_end_m << "\n";

    return 0;
}