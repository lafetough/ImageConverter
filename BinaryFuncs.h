#pragma once

#include "structs.h"

using namespace std;



bool validateBitset(bitset<BITS_TO_READ>& binary_rep, uint8_t& byte, ifstream& file);

string makeStringOfBinary(bitset<BITS_TO_READ>& binary_rep, int& cur_bit, int length, uint8_t& byte, ifstream& file);
