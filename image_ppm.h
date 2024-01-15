#pragma once


#include "structs.h"
#include <string_view>

using namespace std; 

static const string_view PPM_SIGN = "P6"sv;
static const int PPM_MAX = 255;

bool savePPM(const string& path, const vector<vector<RGB>>& image, int w, int h);