#pragma once
#include "../include/algorithm.h"
#include "../include/receiver.h"

struct Sample {
	double x;
	double y;
	double z;
	std::vector<double> rssis;
	std::vector<double> dists;
	std::vector<double> adjust;
};

class FingerAlgo : public Algorithm {
public:
	~FingerAlgo();
	void prepare() override;
	Location do_work(const std::vector<RssiInfo>& rssi_infos) const override;

private:
	void load_fingerprint();
	void read_rssi();
	std::vector<double> rough_distance(const std::vector<double>& rssis) const;
	int find_nearest() const;
	void pick_candidates(int nearest);
	void adjust_distance(int nearest);
	int least_error();
        void genertate_coeffs();
        void esti_location();

public:
	const static char FINGERPRINT[100];
	const static double RADIUS;
	const static int K = 12;

private:
	std::vector<Sample> samples;
        std::vector<int> candidates;
	std::vector<double> rssis;
	std::vector<double> rough_dists;
	std::vector<double> esti_dists;
	std::vector<double> errors;
        std::vector<double> coeffs;
        double xe, ye, ze;
};
