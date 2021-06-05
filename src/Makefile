CXX = g++
CXXFLAGS = -Wall -Wextra -Wsign-conversion  -std=c++17 -Wmissing-field-initializers # -Weffc++
MYSQLFLAGS = `mysql_config --cflags` -lmysqlclient
#MYSQLFLAGS = -I/usr/local/mysql-connector-c-6.1.11-macos10.12-x86_64/include -lmysqlclient

OBJECTS = i2c.o pca9685.o pcf2129_rtc.o database.o box.o ws2812b.o smart_medical_box.o settings.o ui_communication.o main.o

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o main -lmysqlclient -lwiringPi -lzmq -lws2811

i2c.o: ./lib/hardware/i2c/i2c.cpp ./lib/hardware/i2c/i2c.h #
	$(CXX) $(CXXFLAGS) -c ./lib/hardware/i2c/i2c.cpp

pca9685.o: ./lib/hardware/pca9685/pca9685.cpp ./lib/hardware/pca9685/pca9685.h #
	$(CXX) $(CXXFLAGS) -c ./lib/hardware/pca9685/pca9685.cpp

pcf2129_rtc.o: ./lib/hardware/pcf2129_rtc/pcf2129_rtc.cpp ./lib/hardware/pcf2129_rtc/pcf2129_rtc.h
	$(CXX) $(CXXFLAGS) -c ./lib/hardware/pcf2129_rtc/pcf2129_rtc.cpp

box.o: ./lib/HAL/box/box.cpp ./lib/HAL/box/box.h ./lib/hardware/pca9685/pca9685.h
	$(CXX) $(CXXFLAGS) -c ./lib/HAL/box/box.cpp

database.o: ./lib/database/database.cpp ./lib/database/database.h
	$(CXX) $(CXXFLAGS) -c ./lib/database/database.cpp $(MYSQLFLAGS)

ui_communication.o: ./lib/ui_communication/ui_communication.cpp ./lib/ui_communication/ui_communication.h
	$(CXX) $(CXXFLAGS) -c ./lib/ui_communication/ui_communication.cpp -lzmq

ws2812b.o: ./lib/hardware/ws2812b/ws2812b.cpp ./lib/hardware/ws2812b/ws2812b.h
	$(CXX) $(CXXFLAGS) -c ./lib/hardware/ws2812b/ws2812b.cpp -lws2811 

smart_medical_box.o: ./smart_medical_box.cpp ./smart_medical_box.h ./lib/database/database.h ./lib/hardware/pcf2129_rtc/pcf2129_rtc.h ./lib/hardware/pca9685/pca9685.h ./lib/HAL/box/box.h ./config/settings.h ./lib/ui_communication/ui_communication.h ./lib/hardware/ws2812b/ws2812b.h
	$(CXX) $(CXXFLAGS) -c smart_medical_box.cpp

settings.o: ./config/settings.cpp ./config/settings.h
	$(CXX) $(CXXFLAGS) -c ./config/settings.cpp

main.o: main.cpp ./smart_medical_box.h ./config/settings.h
	$(CXX) $(CXXFLAGS) -c main.cpp
