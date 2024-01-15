
#include "StartOfScanDecoder.h"
#include "HexLib.h"

namespace StartOfScanDecoder {

	void decode(ifstream& file, DecodeInformation& info) {

		int l = decodeLength(file) - 2;

		vector<uint8_t> data(l);
		file.read(reinterpret_cast<char*>(&(data[0])), l);

		int i = 0;
		int channels_amount = data[i++];

		info.SOS.resize(channels_amount + 1);

		for (int j = 0; j < channels_amount; ++j) {
			int id = data[i++];
			auto p = parseHexToHalfByteVals(data[i++]);
			info.SOS[id] = { p.first, p.second };
		}
	}

}
