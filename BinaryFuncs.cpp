
#include "BinaryFuncs.h"
#include "HexLib.h"
#include "GeneralImageDecoder.h"

void endFunction() {
	std::cout << "Done" << endl;
}

bool validateBitset(bitset<BITS_TO_READ>& binary_rep, uint8_t& byte, ifstream& file) {
	byte = readByte(file);
	std::cout << stringHex(byte) << endl;
	if (isEndImage(byte, file)) {
		endFunction();
		return false;
	}
	binary_rep = std::bitset<8>(byte);
	return true;
}


string makeStringOfBinary(bitset<BITS_TO_READ>& binary_rep, int& cur_bit, int length, uint8_t& byte, ifstream& file) {
	string bin_string = "";
	for (int j = 0; j < length; ++j) {
		// ≈сли текуща€ позици€ байта, с которым работаем меньше 0, считываем и провер€ем на конченость следующий байт.
		if (cur_bit < 0) {
			if (!validateBitset(binary_rep, byte, file)) {
				throw;
			}
			cur_bit = BITS_TO_READ - 1; //сбрасываем счетчик
		}
		bin_string += (binary_rep[cur_bit] & 1) ? '1' : '0';
		--cur_bit;
	}
	return bin_string;
}
