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

const static double RADIUS = 12;

const static double X1 = 5;
const static double Y1 = 10;
const static double Z1 = 225;

const static double X2 = 35;
const static double Y2 = 10;
const static double Z2 = 225;

const static double X3 = 35;
const static double Y3 = 40;
const static double Z3 = 225;

const static double X4 = 5;
const static double Y4 = 40;
const static double Z4 = 225;

const static double P1 = 19000000;
const static double P2 = 12900000;
const static double P3 = 14000000;
const static double P4 = 16000000;

const static int K = 12;

typedef struct Sample {
        double x, y, z;
        vector<int> uid;
        vector<double> rssi;
} Sam;

vector<Sam> load_fingerprint() {
        ifstream fin(FINGERPRINT);
        vector<Sam> ret;
        int x, y, z;
        while(fin >> x >> y >> z) {
                Sam sam;
                sam.x = x; sam.y = y; sam.z = z;
                int uid;
                double rssi;
                for (int i = 0; i < 4; ++i) {
                        fin >> uid >> rssi;
                        sam.uid.push_back(uid);
                        sam.rssi.push_back(rssi);
                }
                ret.push_back(sam);
        }
        return ret;
}

Sam read_rssi() {
        serial::Serial ser(PORT, BUAD_RATE, TIMEOUT);
        if (ser.isOpen()) ser.close();
        ser.open();

        Sam ret;
        uint8_t buffer[BUF_SIZE];
        for (size_t i = 0; i < BUF_SIZE; ++i)
                buffer[i] = 0;
        int cnt = ser.read(buffer, BUF_SIZE);
        if (cnt <= 0) {
                cout << "No data was received!" << endl;
                return ret;
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

                        if (uid < 5 && uid > 0) {
                                rssis[uid].push_back(rssi);
                        }
                        i += 5;
                }
        }

        for (int i = 1; i < 5; ++i) {
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

        for (int i = 1; i < 5; ++i) {
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
        for (int i = 1; i < 5; ++i) {
                if (rscnt[i] != 0) {
                        int fcnt = rscnt[i];
                        cout << "LED(" << i
                        << ") has fcnt: " << fcnt << 
                        " and rssi: " << rsum[i] * FACT / fcnt << "mV"
                        << " with variance: " << sqrt(sdsum[i] * FACT / fcnt) << endl;
                        ret.uid.push_back(i);
                        ret.rssi.push_back(rsum[i] * FACT / fcnt);
                }
        }
        cout << "----------------------------------------------------" << endl;

        ser.close();
        return ret;
}

double dis2_rssi(vector<double> &r1, vector<double> &r2) {
        int len = r1.size();
        double ret = 0;
        for (int i = 0; i < len; ++i) {
                double s = r1[i] - r2[i];
                ret += s * s;
        }
        return ret;
}

int find_nearest(vector<Sam> sams, Sam online) {
        vector<double> dis2;
        for (size_t i = 0; i < sams.size(); ++i)
                dis2.push_back(dis2_rssi(sams[i].rssi, online.rssi));

        int t = 0;
        double min = dis2[0];
        for (size_t i = 1; i < dis2.size(); ++i) {
                if (dis2[i] < min) {
                        min = dis2[i];
                        t = i;
                }
        }
        return t;
}

double dis2_space(double x1, double y1, double z1, double x2, double y2, double z2) {
        double d1 = x1 - x2;
        double d2 = y1 - y2;
        double d3 = z1 - z2;
        return d1 * d1 + d2 * d2 + d3 * d3;
}

vector<int> get_candidates(vector<Sam> sams, Sam nearest, double radius) {
        vector<int> ret;
        radius = radius * radius;
        for (size_t i = 0; i < sams.size(); ++i) {
                Sam sam = sams[i];
                double dis2 = dis2_space(sam.x, sam.y, sam.z, nearest.x, nearest.y, nearest.z);
                if (dis2 < radius) ret.push_back(i);
        }
        return ret;
}
                

int main(){
        vector<Sam> sams = load_fingerprint();
        if (sams.empty()) {
                cout << "No fingerprint database was found in" << FINGERPRINT << endl;
                return 0;
        }

        vector<vector<double>> sdis;
        for (auto sam : sams) {
                vector<double> dis;
                dis.push_back(dis2_space(sam.x, sam.y, sam.z, X1, Y1, Z1));
                dis.push_back(dis2_space(sam.x, sam.y, sam.z, X2, Y2, Z2));
                dis.push_back(dis2_space(sam.x, sam.y, sam.z, X3, Y3, Z3));
                dis.push_back(dis2_space(sam.x, sam.y, sam.z, X4, Y4, Z4));
                sdis.push_back(dis);
        }

        vector<vector<double>> rdis;
        for (auto sam : sams) {
                vector<double> dis;
                dis.push_back(sqrt(sam.rssi[0]/P1));
                dis.push_back(sqrt(sam.rssi[1]/P2));
                dis.push_back(sqrt(sam.rssi[2]/P3));
                dis.push_back(sqrt(sam.rssi[3]/P4));
                rdis.push_back(dis);
        }

        vector<vector<double>> adjust;
        for (size_t i = 0; i < sams.size(); ++i) {
                vector<double> coe;
                coe.push_back(sdis[i][0]/rdis[i][0]);
                coe.push_back(sdis[i][1]/rdis[i][1]);
                coe.push_back(sdis[i][2]/rdis[i][2]);
                coe.push_back(sdis[i][3]/rdis[i][3]);
                adjust.push_back(coe);
        }

        Sam online = read_rssi();

        int t = find_nearest(sams, online);

        cout << "the nearest sample is " << sams[t].x << " " << sams[t].y << " " << sams[t].z << endl;

        vector<int> candidates = get_candidates(sams, sams[t], RADIUS);

        for (size_t i = 0; i < candidates.size(); ++i) {
                cout << "candidate " << i << " is " << sams[candidates[i]].x << " " << sams[candidates[i]].y << " " << sams[candidates[i]].z << endl;
        }


        double onedis[4];
        double onrdis[4];

        onrdis[0] = sqrt(online.rssi[0]/P1);
        onrdis[1] = sqrt(online.rssi[1]/P2);
        onrdis[2] = sqrt(online.rssi[2]/P3);
        onrdis[3] = sqrt(online.rssi[3]/P4);

        int finger_t = t;
        int last_t = t;
        do {
                last_t = t;
                onedis[0] = adjust[t][0] * onedis[0];
                onedis[1] = adjust[t][1] * onedis[1];
                onedis[2] = adjust[t][2] * onedis[2];
                onedis[3] = adjust[t][3] * onedis[3];

                vector<double> dis_4dis;
                for (size_t i = 0; i < candidates.size(); ++i) {
                        double d1 = onedis[0] - sdis[candidates[i]][0];
                        double d2 = onedis[1] - sdis[candidates[i]][1];
                        double d3 = onedis[2] - sdis[candidates[i]][2];
                        double d4 = onedis[3] - sdis[candidates[i]][3];
                        dis_4dis.push_back(d1 * d1 + d2 * d2 + d3 * d3 + d4 * d4);
                }

                int min_t = 0;
                double min_dis = dis_4dis[0];
                for (size_t i = 1; i < dis_4dis.size(); ++i) {
                        if (dis_4dis[i] < min_dis) {
                                min_dis = dis_4dis[i];
                                min_t = candidates[i];
                        }
                }
                t = min_t;
        }while (last_t != t);
        
        onedis[0] = adjust[t][0] * onedis[0];
        onedis[1] = adjust[t][1] * onedis[1];
        onedis[2] = adjust[t][2] * onedis[2];
        onedis[3] = adjust[t][3] * onedis[3];

        vector<double> dis_4dis;
        for (size_t i = 0; i < candidates.size(); ++i) {
                double d1 = onedis[0] - sdis[candidates[i]][0];
                double d2 = onedis[1] - sdis[candidates[i]][1];
                double d3 = onedis[2] - sdis[candidates[i]][2];
                double d4 = onedis[3] - sdis[candidates[i]][3];
                dis_4dis.push_back(d1 * d1 + d2 * d2 + d3 * d3 + d4 * d4);
        }

        for (size_t i = 1; i < candidates.size(); ++i) {
                if (dis_4dis[i] < dis_4dis[i - 1]) {
                        int j = i - 1;
                        double tmp = dis_4dis[i];
                        int index = candidates[i];
                        dis_4dis[i] = dis_4dis[i - 1];
                        candidates[i] = candidates[i - 1];
                        while (tmp < dis_4dis[j]) {
                                dis_4dis[j + 1] = dis_4dis[j];
                                candidates[j + 1] = candidates[j];
                                j--;
                        }
                        dis_4dis[j + 1] = tmp;
                        candidates[j + 1] = index;
                }
        }
        
        double csum = 0;
        for (int i = 0; i < K; ++i) {
                csum += 1 / dis_4dis[i];
        }

        vector<double> k_coe;
        for (int i = 0; i < K; ++i)
                k_coe.push_back((1 / dis_4dis[i]) / csum);

        double xe, ye, ze;
        for (int i = 0; i < K; ++i) {
                xe += (k_coe[i] * sams[candidates[i]].x);
                ye += (k_coe[i] * sams[candidates[i]].y);
                ze += (k_coe[i] * sams[candidates[i]].z);
        }
        cout << "The estimated coordinates is " << xe << " " << ye << " " << ze << endl;


        cout << "write to the result Y/N?" << endl;
        char c;
        cin >> c;
        if (c != 'Y' && c != 'y') return 0;
        double x, y, z;
        cout << "type x y z:" << endl;
        cin >> x >> y >> z;
        ofstream fout("./data/result", ios::app);
        fout << x << " " << y << " " << z << " "
                << sams[finger_t].x << " " << sams[finger_t].y << " " << sams[finger_t].z << " "
                << xe << " " << ye << " " << ze << endl;
        
        return 0;
}

