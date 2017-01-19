#pragma once
#include <vector>

#include "../include/receiver.h"

struct LedInfo {
        int uid;
	int zid;
	int type;
        double x;
        double y;
        double z;
        double P0;
};

struct Location {
        long long timestamp;
        double x;
        double y;
        double z;
};

class Algorithm {
public:
	static double distance(const std::vector<double>& v1, const std::vector<double>& v2);
	static double distance(double x1, double y1, double z1, double x2, double y2, double z2);

        virtual void prepare();
        virtual Location do_work(const std::vector<RssiInfo>& rssi_infos) const = 0;

protected:
        long long time_stamp() const;
        const std::vector<LedInfo>& get_ledinfos() const;

private: 
	void load_ledinfo();
        long long current_time() const;

public:
        const static int MAX_UID = 9;

private:
        std::vector<LedInfo> ledinfos;
        long long start_time;
};
