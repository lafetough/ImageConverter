#include "HexLib.h"

uint8_t readByte(ifstream& file) {
	uint8_t m;
	file.read(reinterpret_cast<char*>(&m), 1);
	return m;
}

string stringHex(uint8_t u) {
	stringstream ss;
	ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(u);
	string str = ss.str();
	return str;
}

int decodeLength(ifstream& file) {
	uint8_t marker[2];
	file.read(reinterpret_cast<char*>(&marker), 2);

	stringstream ss;
	ss << std::hex << static_cast<int>(marker[0]) << static_cast<int>(marker[1]);

	string hexStr = ss.str();

	int dec;
	istringstream(hexStr) >> hex >> dec;

	return dec;


}
int decodeCombinedBytes(const vector<uint8_t>& bytes) {
	stringstream ss;
	ss << hex;
	for (const uint8_t byte : bytes) {
		ss << static_cast<int>(byte);
	}

	int dec;
	istringstream(ss.str()) >> hex >> dec;
	return dec;
}

int hexToDec(uint8_t hex) {
	int dec = 0;
	for (int i = 0; hex > 0; ++i) {
		dec += (hex % 10) * (1 << (4 * i));
		hex /= 10;
	}
	return dec;
}
int hexToDec(uint16_t hex) {
	int dec = 0;
	for (int i = 0; hex > 0; ++i) {
		dec += (hex % 10) * (1 << (4 * i));
		hex /= 10;
	}
	return dec;
}

std::string hexToString(uint8_t hex) {
	std::stringstream ss;
	ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hex);
	return ss.str();
}

std::string hexToString(uint16_t value) {
	std::stringstream ss;
	ss << std::hex << std::setw(4) << std::setfill('0') << value;
	return ss.str();
}

pair<int, int> parseHexToHalfByteVals(uint8_t byte) {
	string str = hexToString(byte);
	return { HALF_BYTE_TO_VAL.at(str[0]), HALF_BYTE_TO_VAL.at(str[1]) };
	//return { stoi(str.substr(0, 1)), stoi(str.substr(1)) };
}