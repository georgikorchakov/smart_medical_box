CXX = g++
CXXFLAGS = -Wall -Wextra -Wsign-conversion  -std=c++17 # -Weffc++
MYSQLFLAGS = `mysql_config --cflags` -lmysqlclient
#MYSQLFLAGS = -I/usr/local/mysql-connector-c-6.1.11-macos10.12-x86_64/include -lmysqlclient

OBJECTS = i2c.o pca9685.o pcf2129_rtc.o database.o box_opening.o smart_medical_box.o settings.o main.o

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o main -lmysqlclient -lwiringPi

i2c.o: ./lib/hardware/i2c/i2c.cpp ./lib/hardware/i2c/i2c.h #
	$(CXX) $(CXXFLAGS) -c ./lib/hardware/i2c/i2c.cpp

pca9685.o: ./lib/hardware/pca9685/pca9685.cpp ./lib/hardware/pca9685/pca9685.h #
	$(CXX) $(CXXFLAGS) -c ./lib/hardware/pca9685/pca9685.cpp

pcf2129_rtc.o: ./lib/hardware/pcf2129_rtc/pcf2129_rtc.cpp ./lib/hardware/pcf2129_rtc/pcf2129_rtc.h
	$(CXX) $(CXXFLAGS) -c ./lib/hardware/pcf2129_rtc/pcf2129_rtc.cpp

box_opening.o: ./lib/HAL/box_opening/box_opening.cpp ./lib/HAL/box_opening/box_opening.h ./lib/hardware/pca9685/pca9685.h
	$(CXX) $(CXXFLAGS) -c ./lib/HAL/box_opening/box_opening.cpp

database.o: ./lib/HAL/database/database.cpp ./lib/HAL/database/database.h
	$(CXX) $(CXXFLAGS) -c ./lib/HAL/database/database.cpp $(MYSQLFLAGS)

smart_medical_box.o: ./smart_medical_box.cpp ./smart_medical_box.h ./lib/HAL/database/database.h ./lib/hardware/pcf2129_rtc/pcf2129_rtc.h ./lib/hardware/pca9685/pca9685.h ./lib/HAL/box_opening/box_opening.h ./config/settings.h
	$(CXX) $(CXXFLAGS) -c smart_medical_box.cpp

settings.o: ./config/settings.cpp ./config/settings.h
	$(CXX) $(CXXFLAGS) -c ./config/settings.cpp

main.o: main.cpp ./smart_medical_box.h ./config/settings.h
	$(CXX) $(CXXFLAGS) -c main.cpp
