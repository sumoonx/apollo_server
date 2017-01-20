#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../include/receiver.h"

using namespace std;

const static char FINGERPRINT[100] = "./data/fingerprint/fingerprint";

const static double XMAX = 35;
const static double YMAX = 35;
const static double ZMAX = 24;
const static double STEP = 5;
const static double ZSTEP = 8;

void read_rssi(double x, double y, double z) {
	Receiver receiver;
        receiver.open();

	while(true) {
                cout << "recording location (" << x << ", " << y << ", " << z << ") >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
		vector<RssiInfo> rssi_infos = receiver.get_rssi();
		for (auto rssi_info : rssi_infos) {
			cout << "LED(" << rssi_info.uid << ") has cnt " << rssi_info.cnt << "\t and rssi " << rssi_info.rssi
			<< "mV \t\t with variance " << rssi_info.variance << endl;
		}
		char buf[100];
		cout << "press N to test again or Enter to continue" << endl;
		cin.getline(buf, 100);
		if (buf[0] == '\0') {
			ofstream fout(FINGERPRINT, ios::app);
			fout << x << " " << y << " " << z;
			for (auto rssi_info : rssi_infos)
				fout << " " << rssi_info.uid << " " << rssi_info.rssi;
			fout << endl;
			return;
		}
		cout << "try again >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;

	}
        receiver.close();
}

int main(int argc, char* argv[]) {
	double step = STEP;
	if (argc == 2)
		sscanf(argv[1], "%lf", &step);

	double x_beg = 0;
	double y_beg = 0;
	double z_beg = 0;
	cout << "Please type x_beg, y_beg and z_beg" << endl;
	cin >> x_beg >> y_beg >> z_beg;
        char buf[100];
        cin.getline(buf, 100);

	double x_end = XMAX;
	double y_end = YMAX;
	double z_end = ZMAX;
	cout << "Please type x_end, y_end and z_end" << endl;
	cin >> x_end >> y_end >> z_end;
        cin.getline(buf, 100);

        if (x_beg > x_end || y_beg > y_end || z_beg > z_end) {
                cout << "wrong argument, please try again!" << endl;
                return 0;
        }

	cout << "------------------------------------------------------------------" << endl;
	cout << "start recording fingerprint from (" << x_beg << ", " << y_beg << ", " << z_beg << ")"
		<< " with xy step of " << step << " and z step of " << ZSTEP << endl;

        for (double z = z_beg; z <= z_end; z += ZSTEP) {
		for (double y = y_beg; y <= y_end; y += step) {
                        for (double x = x_beg; x <= x_end; x += step) {
				read_rssi(x, y, z);
			}
		}
	}

	cout << "stop recording fingerprint from (" << x_beg << ", " << y_beg << ", " << z_beg << ")" << endl;
	cout << "------------------------------------------------------------------" << endl;
        return 0;
}

