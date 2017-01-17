#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#include "../include/serial.h"

struct 	Rxdata {
	int uid;
	int cnt;
	int rssi;
};

class RssiInfo {
public:
        RssiInfo(int uid, std::vector<double> rssis);
public:
	int uid;
	double rssi;
	double variance;
};

class Receiver {
public:
        Receiver(int timeout = TIMEOUT, const std::string& port = PORT, uint32_t buad_rate = BUAD_RATE);
	~Receiver();
	void open();
	void close();
	void silence(bool silen);
        std::vector<Rxdata> raw_data();
        std::vector<Rxdata> filtered_data();
        std::vector<RssiInfo> get_rssi();
public:
        const static std::string PORT;
	const static uint32_t BUAD_RATE = 115200;
	const static int TIMEOUT = 1000;

	const uint8_t CMD_ON = 2;
	const uint8_t CMD_OFF = 3;
	const uint8_t CMD_SILENCE = 5;
	const uint8_t CMD_LEVEL = 6;

	const uint8_t SILENCE_TRUE = 1;
	const uint8_t SILENCE_FALSE = 2;

	const static uint8_t HEAD = 0xCA;
	const static uint8_t TAIL = 0xFE;

	const static size_t BUF_SIZE = 8 * 1024;

	const static double FACT;


private:
	serial::Serial ser;
};


