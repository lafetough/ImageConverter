
#include "MethodDecoder.h"
#include "HexLib.h"
namespace MethodDecoder {

	MethodInfo decode(ifstream& file) {

		int sec_length = decodeLength(file) - 2;
		vector<uint8_t> data(sec_length);
		file.read(reinterpret_cast<char*>(&(data[0])), sec_length);

		int i = 0;

		MethodInfo dct(sec_length + 2, data[i++] + 1, decodeCombinedBytes({ data[i++], data[i++] }), decodeCombinedBytes({ data[i++], data[i++] }), data[i++]);

		for (int j = 1; j < dct.channels.size(); ++j) {
			dct.addChannel(data[i++], parseHexToHalfByteVals(data[i++]), data[i++]);
		}

		return dct;
	}



}