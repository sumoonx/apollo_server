#include <fstream>
#include <cmath>

#include "../include/fingeralgo.h"

const char FingerAlgo::FINGERPRINT[100] = "./data/fingerprint/fingerprint";
const double FingerAlgo::RADIUS = 12;

FingerAlgo::~FingerAlgo() {}

void FingerAlgo::prepare() {
	Algorithm::prepare();
	load_fingerprint();
}

Location FingerAlgo::do_work(const std::vector<RssiInfo>& rssi_infos) const {
	std::vector<double> recv_rssi;
	for (int i = 0; i < Receiver::MAX_UID; ++i) {
		recv_rssi.push_back(0);
	}

	int n, uid;
	std::cin >> n;
	while (n--) {
		std::cin >> uid;
		double rssi;
		std::cin >> rssi;
		recv_rssi.push_back(rssi);
	}


	return Location{time_stamp(),0,0,0};
}

void FingerAlgo::load_fingerprint() {
	std::ifstream fin(FINGERPRINT);
	samples.clear();
        int x, y, z;
        while(fin >> x >> y >> z) {
                Sample sam;
                sam.x = x; sam.y = y; sam.z = z;
                int uid;
                double rssi;
                for (int i = 0; i < Receiver::MAX_UID; ++i) {
                        fin >> uid >> rssi;
			sam.rssis.push_back(rssi);
			const LedInfo& led_info = get_ledinfos()[uid - 1];
			sam.dists.push_back(distance(x, y, z, led_info.x, led_info.y, led_info.z));
                        sam.adjust.push_back(sqrt(rssi/led_info.P0));
                }
                samples.push_back(sam);
        }
}

void FingerAlgo::read_rssi() {
}

std::vector<double> FingerAlgo::rough_distance(const std::vector<double>& rssis) const {
        std::vector<double> ret;
        for (size_t i = 0; i < rssis.size(); ++i) {
                ret.push_back(sqrt(rssis[i] / get_ledinfos[i + 1].P0));
        }
        return ret;
}

int FingerAlgo::find_nearest() const {
        double min = Algorithm::distance(samples[0], this->rssis);
        int ret = 0;
        for (size_t i = 1; i < samples.size(); ++i) {
                double dis = Algorithm::distance(samples[i], this->rssis);
                if (dis < min) {
                        min = dis;
                        ret = i;
                }
        }
        return ret;
}
