#include <iostream>
#include <string>
#include <vector>

#include "../include/serial.h"

using namespace std;

const static string PORT = "/dev/ttyUSB1";
const static uint32_t BUAD_RATE = 115200;
const static serial::Timeout TIMEOUT(1000, 1000, 0, 0, 0);

const static uint8_t HEAD = 0xCA;
const static uint8_t TAIL = 0xFE;

const static size_t BUF_SIZE = 8 * 1024;

const static double FACT = 7.8125;

int main(){
        serial::Serial ser(PORT, BUAD_RATE, TIMEOUT);
        if (ser.isOpen()) ser.close();
        ser.open();

        uint8_t buffer[BUF_SIZE];
        for (size_t i = 0; i < BUF_SIZE; ++i)
                buffer[i] = 0;
        int cnt = ser.read(buffer, BUF_SIZE);

        int fcnt = 0;
        double sum = 0;
        for (size_t i = 0; i < BUF_SIZE - 7; ++i) {
                cout << buffer[i];
                if (buffer[i] == HEAD && buffer[i + 7] == TAIL) {
                        cout << "uid: " << buffer[i + 1] - '0' << buffer[i + 2] - '0'
                             << ", zid: " << buffer[i + 3] - 0
                             << ", cnt: " << buffer[i + 4] - 0
                             << ", type: " << buffer[i + 5] - 0
                             << ", rssi: " << buffer[i + 6] * FACT << "mV" << endl;
                        fcnt++;
                        sum += buffer[i + 6] * FACT;
                        i += 7;
                }
        }
        cout << "cnt is " << cnt << endl;
        cout << "fcnt is " << fcnt << endl;
        if (fcnt != 0)
                cout << "average rssi is " << sum / fcnt << "mV" << endl;
        ser.close();
        return 0;
}

