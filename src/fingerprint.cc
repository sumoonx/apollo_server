#include <math.h>
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

        vector<vector<int>> rssis;
        vector<int> rsum;
        vector<double> sdsum;
        vector<int> rscnt;

        for (int i = 0; i < 10; ++i) {
                vector<int> rss;
                rssis.push_back(rss);
                rsum.push_back(0);
                sdsum.push_back(0);
                rscnt.push_back(0);
	}

        for (int i = 0; i < cnt - 5; ++i) {
                cout << buffer[i];
                if (buffer[i] == HEAD && buffer[i + 5] == TAIL) {
                        int huid = buffer[i + 1] - '0';
                        int luid = buffer[i + 2] - '0';
                        int uid = huid * 10 + luid;
                        int rssi = buffer[i + 4];

                        if (uid < 10 && uid > 0) {
                                rssis[uid].push_back(rssi);
                        }
                        i += 5;
                }
        }

        for (int i = 1; i < 10; ++i) {
                if (!rssis[i].empty()) {
                        //first data of rssis is always incorrect, so j starts with 1
                        for (size_t j = 1; j < rssis[i].size(); ++j) {
                                if (rssis[i][j] > 12 && rssis[i][j] < 225) {
                                        rsum[i] += rssis[i][j];
                                        rscnt[i]++;
                                }
                        }
                }
        }

        for (int i = 1; i < 10; ++i) {
                if (rscnt[i] != 0) {
                        double ave = rsum[i] / rscnt[i];
                        for (size_t j = 1; j < rssis[i].size(); ++j) {
                                if (rssis[i][j] > 12 && rssis[i][j] < 225) {
                                        double err = rssis[i][j] - ave;
                                        sdsum[i] += err * err;
                                }
                                
                        }
                }
        }

        cout << endl;
        for (int i = 1; i < 10; ++i) {
                if (rscnt[i] != 0) {
                        int fcnt = rscnt[i];
                        cout << "LED(" << i
                        << ") has fcnt: " << fcnt << 
                        " and rssi: " << rsum[i] * FACT / fcnt << "mV"
                        << " with variance: " << sqrt(sdsum[i] * FACT / fcnt) << endl;
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
                for (int i = 1; i < 10; ++i) {
                        double rssi = 0;
                        if(rscnt[i] != 0)
                                rssi = rsum[i] * FACT / rscnt[i];
                        fout << " " << i << " " << rssi;
                }
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

