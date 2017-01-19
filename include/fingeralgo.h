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
	std::vector<int> get_candidates(int nearest) const;
	void adjust_distance(int nearest);
	void least_error();

public:
	const static char FINGERPRINT[100];
	const static double RADIUS;
	const static int K = 12;

private:
	std::vector<Sample> samples;
	std::vector<double> rssis;
	std::vector<double> rough_dists;
	std::vector<double> esti_dists;
	std::vector<double> errors;
};
