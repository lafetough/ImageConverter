#pragma once

#include "structs.h"

using namespace std;

uint8_t readByte(ifstream& file);
string stringHex(uint8_t u);
int decodeLength(ifstream& file);
int decodeCombinedBytes(const vector<uint8_t>& bytes);
int hexToDec(uint8_t hex);
int hexToDec(uint16_t hex);
std::string hexToString(uint8_t hex);
std::string hexToString(uint16_t value);
pair<int, int> parseHexToHalfByteVals(uint8_t byte);