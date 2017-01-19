#include <math.h>
#include <iostream>
#include <string>
#include <vector>

#include "../include/receiver.h"

using namespace std;

int main(){
        Receiver receiver;
        receiver.open();

        vector<RssiInfo> rssi_infos = receiver.get_rssi();
        for (auto rssi_info : rssi_infos) {
                cout << "LED(" << rssi_info.uid << " has cnt " << rssi_info.cnt << " and rssi " << rssi_info.rssi
                << "mV with variance " << rssi_info.variance << endl;
        }

        receiver.close();

        return 0;
}

