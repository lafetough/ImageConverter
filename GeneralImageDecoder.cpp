#include "GeneralImageDecoder.h"
#include "HexLib.h"
#include "BinaryFuncs.h"


int calculateFactor(const string& bin_string) {
	// Если первый бит 1 - то DC-коэфициент равен полученному значению, 
	// если 0 - то DC-коэффициент расчитывается по формуле <значение>-2^<длина значения>+1
	bool is_first_zero = false;
	if (bin_string.at(0) == '0') is_first_zero = true;

	std::bitset<64> rep(bin_string);
	return is_first_zero ? rep.to_ulong() - pow(2, bin_string.length()) + 1 : rep.to_ulong();
}

Matrix buildMatrixZigZagOrder(vector<int>& values) {

	Matrix res(8);

	int c_row = 0;
	int c_col = 0;
	bool checked_n = false;
	bool dir = true;

	for (int i = 0; i < 64; ++i) {
		res[c_row][c_col] = values[i];

		if (c_row == 0) {
			if (!checked_n) {
				c_col == 7 ? ++c_row : ++c_col;
				checked_n = true;
				dir = false;
				continue;
			}
			++c_row;
			if (c_col != 0 && --c_col == 0) {
				checked_n = false;
			}
		}
		else if (c_col == 0) {
			if (!checked_n) {
				c_row == 7 ? ++c_col : ++c_row;
				checked_n = true;
				dir = true;
				continue;
			}
			++c_col;
			if (c_row != 0 && --c_row == 0) {
				checked_n = false;
			}
		}
		else {
			if (dir) {
				if (c_row - 1 == 0) {
					checked_n = false;
				}
				if (c_col == 7) {
					++c_row;
					dir = false;
					continue;
				}
				--c_row;
				++c_col;
			}
			else {
				if (c_col - 1 == 0) {
					checked_n = false;
				}
				if (c_row == 7) {
					++c_col;
					dir = true;
					continue;
				}
				--c_col;
				++c_row;
			}
		}


	}
	return res;
}
Matrix buildMatrixZigZagOrder(vector<uint8_t>& values) {

	Matrix res(8);

	int c_row = 0;
	int c_col = 0;
	bool checked_n = false;
	bool dir = true;

	for (int i = 0; i < 64; ++i) {
		res[c_row][c_col] = values[i];

		if (c_row == 0) {
			if (!checked_n) {
				c_col == 7 ? ++c_row : ++c_col;
				checked_n = true;
				dir = false;
				continue;
			}
			++c_row;
			if (c_col != 0 && --c_col == 0) {
				checked_n = false;
			}
		}
		else if (c_col == 0) {
			if (!checked_n) {
				c_row == 7 ? ++c_col : ++c_row;
				checked_n = true;
				dir = true;
				continue;
			}
			++c_col;
			if (c_row != 0 && --c_row == 0) {
				checked_n = false;
			}
		}
		else {
			if (dir) {
				if (c_row - 1 == 0) {
					checked_n = false;
				}
				if (c_col == 7) {
					++c_row;
					dir = false;
					continue;
				}
				--c_row;
				++c_col;
			}
			else {
				if (c_col - 1 == 0) {
					checked_n = false;
				}
				if (c_row == 7) {
					++c_col;
					dir = true;
					continue;
				}
				--c_col;
				++c_row;
			}
		}


	}
	return res;
}


int computeDCFactor(bitset<BITS_TO_READ>& binary_rep, int& cur_bit, uint8_t& byte, HuffmanTree::Node* tree_node, ifstream& file) {
	//Если не нашли, то где-то оштбка в сигнатуре.
	if (!tree_node) throw;

	//Смотрим значение в узле.
	int length = *tree_node->val;

	// Если значение узла равно 0 - то DC-коэффициент тоже равен 0 (DC - коэффициент в крайнем левом углу матрицы)
	if (length == 0) {
		return 0;
	}
	else {
		return calculateFactor(makeStringOfBinary(binary_rep, cur_bit, length, byte, file));
	}
}
pair<int, int> computeACFactor(bitset<BITS_TO_READ>& binary_rep, int& cur_bit, uint8_t& byte, HuffmanTree::Node* tree_node, ifstream& file) {
	if (cur_bit < 0) {
		if (!validateBitset(binary_rep, byte, file)) {
			return { -1, -1 };
		}
		cur_bit = BITS_TO_READ - 1;
	}

	if (!tree_node) throw;

	uint8_t val = *tree_node->val;

	if (val == 0) {
		return { 0, 0 };
	}
	//Парсим байт на полубайты
	auto [zeros_amount, length] = parseHexToHalfByteVals(val);

	return { zeros_amount,  calculateFactor(makeStringOfBinary(binary_rep, cur_bit, length, byte, file)) };
}


bool isEndImage(uint8_t byte, ifstream& file) {
	if (byte == 0xFF) {
		auto a = readByte(file);
		if (a == 0xD9) return true;
	}
	return false;
}

