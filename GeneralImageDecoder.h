#pragma once

#include "structs.h"

using namespace std;

int calculateFactor(const string& bin_string);

Matrix buildMatrixZigZagOrder(vector<int>& values);
Matrix buildMatrixZigZagOrder(vector<uint8_t>& values);

int computeDCFactor(bitset<BITS_TO_READ>& binary_rep, int& cur_bit, uint8_t& byte, HuffmanTree::Node* tree_node, ifstream& file);

pair<int, int> computeACFactor(bitset<BITS_TO_READ>& binary_rep, int& cur_bit, uint8_t& byte, HuffmanTree::Node* tree_node, ifstream& file);

bool isEndImage(uint8_t byte, ifstream& file);