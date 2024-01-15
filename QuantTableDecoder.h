#pragma once

#include "structs.h"

using namespace std;

namespace QuantTableDecoder {


pair<bool, int> decodeBitLengthID(ifstream& file);
void decode(ifstream& file, DecodeInformation& info);

}