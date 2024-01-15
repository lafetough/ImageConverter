#pragma once

#include "structs.h"

using namespace std;

namespace HuffmanTableDecoder {

HuffmanTree buildTree(const std::vector<std::vector<std::uint8_t>>& H_Table);
void decode(ifstream& file, DecodeInformation& info);
}

HuffmanTree::Node* findNode(std::bitset<BITS_TO_READ>& binary_rep, int& cur_bit, uint8_t& byte, HuffmanTree::Node* node, ifstream& file);