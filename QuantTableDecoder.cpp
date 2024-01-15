#include "QuantTableDecoder.h"
#include "HexLib.h"
#include "GeneralImageDecoder.h"


namespace QuantTableDecoder {

	pair<bool, int> decodeBitLengthID(ifstream& file) {
		uint8_t marker;
		file.read(reinterpret_cast<char*>(&marker), 1);

		stringstream ss;

		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(marker);
		string str = ss.str();

		return { str[0] - '0', str[1] - '0' };
	}

	void decode(ifstream& file, DecodeInformation& info) {
		int length = decodeLength(file);
		auto byte_id = parseHexToHalfByteVals(readByte(file));

		QuantTable quant_table;

		quant_table.length_of_vals = byte_id.first;

		vector<uint8_t> vals(64);
		file.read(reinterpret_cast<char*>(&(vals[0])), 64);

		quant_table.table = buildMatrixZigZagOrder(vals);
		

		if (info.quant_tables.size() <= byte_id.second) {
			info.quant_tables.resize(byte_id.second + 1);
		}
		info.quant_tables[byte_id.second] = std::move(quant_table);

	}
}