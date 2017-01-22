#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../include/receiver.h"
#include "../include/fingeralgo.h"

using namespace std;

int main(){

        Receiver receiver;
        receiver.open();

        while (true) {
        FingerAlgo algo;
        algo.prepare();
        
        vector<RssiInfo> rssi_infos = receiver.get_rssi();
        vector<Location> result = algo.debug(rssi_infos);

//	cout << "size" << result.size() << endl;

        cout << "The estimated coordinates is " << result[2].x << " " << result[2].y << " " << result[2].z << endl;

        cout << "The nearest coordinates is " << result[0].x << " " << result[0].y << " " << result[0].z << endl;

        cout << "The dist-estimated coordinates is " << result[1].x << " " << result[1].y << " " << result[1].z << endl;

        cout << "write to the result Y/N?" << endl;

        char c;
        cin >> c;
        if (c != 'Y' && c != 'y')
                continue;

        double x, y, z;
        cout << "type x y:" << endl;
        cin >> x >> y;
        z = 12;
        ofstream fout("./data/result", ios::app);
        fout << x << " " << y << " " << z << " "
                << result[0].x << " " << result[0].y << " " << result[0].z << " "
                << result[1].x << " " << result[1].y << " " << result[1].z << " "
                << result[2].x << " " << result[2].y << " " << result[2].z << " " << endl;
        }
        
        receiver.close();
        return 0;
}

