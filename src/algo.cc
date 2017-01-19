#include <iostream>
#include <vector>

#include "../include/fingeralgo.h"

using namespace std;

int main() {
	Algorithm* algorithm = new FingerAlgo();
	algorithm->prepare();

        while (true) {
                vector<RssiInfo> rssi_infos;
                Location loc = algorithm->do_work(rssi_infos);
                cout << "timestamp " << loc.timestamp << " x " << loc.x << " y " << loc.y << " z " << loc.z << endl;
        }

        delete algorithm;
	return 0;
}
