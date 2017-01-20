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
	Location do_work(const std::vector<RssiInfo>& rssi_infos) override;
	std::vector<Location> debug(const std::vector<RssiInfo> rssi_infos);

private:
	void load_fingerprint();
        void read_rssi();       //read rssis from std::cin and initlize rough_dists
	std::vector<double> rough_distance(const std::vector<double>& rssis) const;     //generate rough distances from given rssis
	int find_nearest() const;       //find index of nearest Sample
	void pick_candidates(int nearest);
	void adjust_distance(int nearest);
	void sort_candidates(std::vector<double>& errors);
	int least_error();
        void generate_coeffs(const std::vector<double>& errors);
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
