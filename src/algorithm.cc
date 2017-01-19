#include <sys/time.h>
#include <sqlite3.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <cmath>

#include "../include/algorithm.h"

Algorithm::~Algorithm() {}

double Algorithm::distance(const std::vector<double>& v1, const std::vector<double>& v2) {
	if (v1.empty()) return 0;
	int len = v1.size();
        double ret = 0;
        for (int i = 0; i < len; ++i) {
                double s = v1[i] - v2[i];
                ret += s * s;
        }
        return sqrt(ret) / len;

}

double Algorithm::distance(double x1, double y1, double z1, double x2, double y2, double z2) {
        double d1 = x1 - x2;
        double d2 = y1 - y2;
        double d3 = z1 - z2;
        return sqrt(d1 * d1 + d2 * d2 + d3 * d3);
}


void Algorithm::prepare() {
	this->start_time = current_time();
	load_ledinfo();
}

long long Algorithm::current_time() const {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

long long Algorithm::time_stamp() const {
	return current_time() - this->start_time;
}

const std::vector<LedInfo>& Algorithm::get_ledinfos() const{
	return this->ledinfos;
}

void Algorithm::load_ledinfo() {
	ledinfos.clear();

	sqlite3 *db = nullptr;
	char dbname[255] = ".data.db";
	int rc = sqlite3_open(dbname, &db);
	if (rc == SQLITE_OK) {
		char sql[256] = "select * from led_info";
		char** p_result;
		int n_row;
		int n_col;
		rc = sqlite3_get_table(db, sql, &p_result, &n_row, &n_col, NULL);
		if (rc == SQLITE_OK) {
			if (n_row >= 1) {
				for (int i = 1; i < n_row; ++i) {
					int uid, zid, type;
					double x, y, z, P0;
					sscanf(p_result[i * 7 + 0], "%d", &uid);
					sscanf(p_result[i * 7 + 1], "%d", &zid);
					sscanf(p_result[i * 7 + 2], "%lf", &x);
					sscanf(p_result[i * 7 + 3], "%lf", &y);
					sscanf(p_result[i * 7 + 4], "%lf", &z);
					sscanf(p_result[i * 7 + 5], "%d", &type);
					sscanf(p_result[i * 7 + 6], "%lf", &P0);
					LedInfo info = {uid, zid, type, x, y, z, P0};
					ledinfos.push_back(info);
				}
			}
		}
	}
}
