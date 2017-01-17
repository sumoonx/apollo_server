#include <stdio.h>
#include "../include/receiver.h"

using namespace std;

int main(int argc, char* argv[]) {
        int timeout = Receiver::TIMEOUT;
        if (argc == 2)
                sscanf(argv[1], "%d", &timeout);
        Receiver receiver(timeout);

        while(true) {
                vector<RssiInfo> rssi_infos = receiver.get_rssi();
                cout << rssi_infos.size() << endl;
                for (size_t i = 0; i < rssi_infos.size(); ++i) {
                        cout << rssi_infos[i].uid << " " << rssi_infos[i].rssi << endl;
                }
        }

        return 0;
} 
