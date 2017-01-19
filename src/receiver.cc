#include <unistd.h>
#include "../include/receiver.h"

RssiInfo::RssiInfo(int uid, std::vector<double> rssis) : uid(uid), rssi(0), variance(0) {
        //ignore first data
        if (rssis.size() <= 1) return;
        double sum = 0;
        for (size_t i = 1; i < rssis.size(); ++i) {
                sum += rssis[i];
        }
        this->rssi = sum / (rssis.size() - 1);

        double vsum = 0;
        for (size_t i = 1; i < rssis.size(); ++i) {
                vsum += (rssis[i] - this->rssi);
        }
        this->variance = vsum / (rssis.size() - 1);
}

const std::string Receiver::PORT = ("/dev/ttyUSB1");
const double Receiver::FACT = 4.0;

Receiver::Receiver(int timeout, const std::string& port, uint32_t buad_rate) :
	ser(port, buad_rate, serial::Timeout(0, timeout, 0, 10, 0)) {
	open();
}

Receiver::~Receiver() {
	close();
}

void Receiver::open() {
        if (ser.isOpen()) ser.close();
        ser.open();
	uint8_t cmd[1];
	cmd[0] = CMD_ON;
	ser.write(cmd, 1);
}

void Receiver::close() {
	if (ser.isOpen()) {
		uint8_t cmd[1];
		cmd[0] = CMD_OFF;
		ser.write(cmd, 1);
		ser.close();
	}
}

void Receiver::silence(bool sile) {
	uint8_t cmd[2];
	cmd[0] = CMD_SILENCE;
	if (sile) cmd[1] = SILENCE_TRUE;
	else cmd[1] = SILENCE_FALSE;
	ser.write(cmd, 2);
	ser.flush();
//	std::cout << "cmd cnt " << cnt << std::endl;
}

std::vector<Rxdata> Receiver::raw_data() {
	silence(false);

        uint8_t buffer[BUF_SIZE];
        for (size_t i = 0; i < BUF_SIZE; ++i)
                buffer[i] = 0;
        int cnt = ser.read(buffer, BUF_SIZE);

 //       std::cout << "cnt " << cnt << std::endl;

        std::vector<Rxdata> ret;

        for (int i = 0; i < cnt - 5; ++i) {
                if (buffer[i] == HEAD && buffer[i + 5] == TAIL) {
                        int huid = buffer[i + 1] - '0';
                        int luid = buffer[i + 2] - '0';
                        int uid = huid * 10 + luid;
                        int cnt = buffer[i + 3];
                        int rssi = buffer[i + 4];

//                        std::cout << "uid " << uid << " cnt " << cnt << " rssi " << std::endl;
                        Rxdata data = {uid, cnt, rssi};
                        ret.push_back(data);
                }
        }

	silence(true);
        return ret;
}

std::vector<Rxdata> Receiver::filtered_data() {
        return raw_data();
}

std::vector<RssiInfo> Receiver::get_rssi() {
        std::vector<RssiInfo> ret;

        std::vector<Rxdata> data = raw_data();//filtered_data();
        std::vector<std::vector<double>> vrssi;

	for (int i = 0; i <= MAX_UID; ++i) {
                std::vector<double> drssi;
                vrssi.push_back(drssi);
        }

        for (size_t i = 0; i < data.size(); ++i) {
                int uid = data[i].uid;
                if (uid > 0 && uid <= MAX_UID)
                        vrssi[data[i].uid].push_back(data[i].rssi * FACT);
        }

        for (size_t i = 1; i < vrssi.size(); ++i) {
                if (!vrssi[i].empty()) {
                        RssiInfo rssi_info = RssiInfo(i, vrssi[i]);
                        ret.push_back(rssi_info);
                }
        }

        return ret;

}
