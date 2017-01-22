#include <iostream>
#include <stdio.h>

#include "../include/receiver.h"

using namespace std;

const static int UID = 5;

int main(int argc, char** argv) {
	int uid = UID;
	if (argc == 2) {
		sscanf(argv[1], "%d", &uid);
	}
	
	Receiver receiver(10000);
	receiver.open();

	vector<Rxdata> rx_data = receiver.raw_data();
	int last = -1;
	int cnt = 0;
	int missing = 0;
	for (auto data : rx_data) {
		if (data.uid == uid) {
			cnt++;
			if (last >= 0) {
				if (last >= data.cnt) {
					last -= 256;
				}
				missing += (data.cnt - last - 1);
			}
			last = data.cnt;
		}	
	}

        int all = cnt + missing;
        cout << "totally " << cnt << " frames of data are received, " << missing << " frames are missing." << endl;
        cout << "fer is " << missing * 1.0 / all * 100 << "%" << endl;

	receiver.close();
	return 0;
}
