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

Location FingerAlgo::do_work(const std::vector<RssiInfo>& rssi_infos) {
	read_rssi();
	int t = find_nearest();
	pick_candidates(t);

	int last_t;
	do {
		last_t = t;
		adjust_distance(last_t);
		t = least_error();
	}while (last_t != t);

	generate_coeffs();
	esti_location();
	return Location{time_stamp(), this->xe, this->ye, this->ze};
}

void FingerAlgo::load_fingerprint() {
	std::ifstream fin(FINGERPRINT);
	this->samples.clear();
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
                this->samples.push_back(sam);
        }
}

void FingerAlgo::read_rssi() {
	//initlize rssis
	this->rssis.clear();
	for (int i = 0; i < Receiver::MAX_UID; ++i) {
		this->rssis.push_back(0);
	}

	//read rssis from std::cin
	int n, uid;
	std::cin >> n;
	while (n--) {
		std::cin >> uid;
		double rssi;
		std::cin >> rssi;
		this->rssis.push_back(rssi);
	}

	//initlize rough_dists with rssis
	this->rough_dists = rough_distance(this->rssis);
}

std::vector<double> FingerAlgo::rough_distance(const std::vector<double>& rssis) const {
        std::vector<double> ret;
        for (size_t i = 0; i < this->rssis.size(); ++i) {
                ret.push_back(sqrt(this->rssis[i] / get_ledinfos()[i + 1].P0));
        }
        return ret;
}

int FingerAlgo::find_nearest() const {
        int ret = 0;
        double min = Algorithm::distance(this->samples[0].rssis, this->rssis);
        for (size_t i = 1; i < this->samples.size(); ++i) {
                double dis = Algorithm::distance(this->samples[i].rssis, this->rssis);
                if (dis < min) {
                        min = dis;
                        ret = i;
                }
        }
        return ret;
}

void FingerAlgo::pick_candidates(int nearest) {
	this->candidates.clear();
	const Sample& near_sam = this->samples[nearest];
	for (size_t i = 0; i < this->samples.size(); ++i) {
		const Sample& sam = this->samples[i];
		double dist = distance(near_sam.x, near_sam.y, near_sam.z, sam.x, sam.y, sam.z);
		if (dist < RADIUS)
			this->candidates.push_back(i);
	}
}

void FingerAlgo::adjust_distance(int nearest) {
	this->esti_dists.clear();
	const std::vector<double>& adjust = this->samples[nearest].adjust;
	for (size_t i = 0; i < this->rough_dists.size(); ++i) {
		this->esti_dists.push_back(this->rough_dists[i] * adjust[i]);
	}
}

int FingerAlgo::least_error() {
	this->errors.clear();
	for (auto index : candidates) {
		double error = Algorithm::distance(this->esti_dists, this->samples[index].dists);
		this->errors.push_back(error);
	}

	double min = this->errors[0];
	int ret = this->candidates[0];
	for (size_t i = 1; i < this->errors.size(); ++i) {
		if (this->errors[i] < min) {
			min = this->errors[i];
			ret = this->candidates[i];
		}
	}
	return ret;
}

void FingerAlgo::generate_coeffs() {
	double sum = 0;
	std::vector<double> temps;
	for (auto error : this->errors) {
		double temp = 1 / error;
		sum += temp;
		temps.push_back(temp);
	}

	this->coeffs.clear();
	for (auto temp : temps) {
		this->coeffs.push_back(temp / sum);
	}
}

void FingerAlgo::esti_location() {
	this->xe = 0; this->ye = 0; this->ze = 0;
	for (size_t i = 0; i < candidates.size(); ++i) {
		this->xe += samples[candidates[i]].x * coeffs[i];
		this->ye += samples[candidates[i]].y * coeffs[i];
		this->ze += samples[candidates[i]].z * coeffs[i];
	}
}
