#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../include/serial.h"

using namespace std;

const static char FINGERPRINT[100] = "./data/fingerprint/fingerprint";

const static string PORT = "/dev/ttyUSB1";
const static uint32_t BUAD_RATE = 115200;
const static serial::Timeout TIMEOUT(1000, 1000, 0, 0, 0);

const static uint8_t HEAD = 0xCA;
const static uint8_t TAIL = 0xFE;

const static size_t BUF_SIZE = 8 * 1024;

const static double FACT = 4;//7.8125;

void read_rssi() {
        serial::Serial ser(PORT, BUAD_RATE, TIMEOUT);
        if (ser.isOpen()) ser.close();
        ser.open();

        uint8_t buffer[BUF_SIZE];
        for (size_t i = 0; i < BUF_SIZE; ++i)
                buffer[i] = 0;
        int cnt = ser.read(buffer, BUF_SIZE);
        if (cnt <= 0) {
                cout << "No data was received!" << endl;
                return;
        }
        cout << "----------------------------------------------------" << endl;
        cout << "Totally " << cnt << " bytes of data are received:" << endl;

        int fcnt[10];
        int sum[10];
        for (int i = 0; i < 10; ++i) {
                fcnt[i] = 0;
                sum[i] = 0;
        }

        for (int i = 0; i < cnt - 7; ++i) {
                cout << buffer[i];
                if (buffer[i] == HEAD && buffer[i + 7] == TAIL) {
                        int huid = buffer[i + 1] - '0';
                        int luid = buffer[i + 2] - '0';
                        int uid = huid * 10 + luid;

                        if (uid < 10 && uid > 0) {
                                fcnt[uid]++;
                                sum[uid] += buffer[i + 6];
                        }
                        i += 7;
                }
        }

        cout << endl;
        for (int i = 1; i < 10; ++i) {
                if (fcnt[i] != 0) {
                        cout << "LED(" << i
                        << ") has fcnt: " << fcnt[i] << 
                        " and rssi: " << sum[i] * FACT / fcnt[i] << "mV" << endl;
                }
        }
        cout << "----------------------------------------------------" << endl;
        
        cout << "Do you want take this into fingerprint database Y/N?" << endl;
        char c;
        cin >> c;
        if (c == 'Y' || c == 'y') {
                cout << "Type in x y z:" << endl;
                int x, y, z;
                cin >> x >> y >> z;
                ofstream fout(FINGERPRINT, ios::app);
                fout << x << " " << y << " " << z;
                for (int i = 1; i < 10; ++i)
                        if(fcnt[i] != 0)
                                fout << " " << i << " " << sum[i] * FACT / fcnt[i];
                fout << endl;
        }

        ser.close();
}

int main(){

        ifstream fin(FINGERPRINT);
        if (fin) {
                cout << "fingerprint already exists, do you want to overwrite it Y/N?" << endl;
                char c;
                cin >> c;
                if (c == 'Y') ofstream fout(FINGERPRINT);
        }
        fin.close();

        while(1) {
                read_rssi();
        }

        return 0;
}

