

#include "HuffmanTableDecoder.h"
#include "HexLib.h"
#include "BinaryFuncs.h"


namespace HuffmanTableDecoder {


	HuffmanTree buildTree(const std::vector<std::vector<std::uint8_t>>& H_Table) {

		HuffmanTree ht;

		int lev = 1;
		int cur_level = lev;
		HuffmanTree::Node* cur_node = ht.root;
		for (const auto& vals : H_Table) {
			if (vals.size() > 0) {
				for (const auto val : vals) {
					bool added = false;
					while (!added) {
						if (cur_level == lev) {
							if (!cur_node->left) {
								cur_node->left = new HuffmanTree::Node(cur_node, val);
								added = true;
							}
							else if (!cur_node->right) {
								cur_node->right = new HuffmanTree::Node(cur_node, val);
								added = true;
							}
							else {
								cur_node = cur_node->parent;
								--cur_level;
							}
						}
						else {
							if (!cur_node->left) {
								cur_node->left = new HuffmanTree::Node(cur_node);
								cur_node = cur_node->left;
								++cur_level;
							}
							else if (!cur_node->right) {
								cur_node->right = new HuffmanTree::Node(cur_node);
								cur_node = cur_node->right;
								++cur_level;
							}
							else {
								cur_node = cur_node->parent;
								--cur_level;
							}
						}
					}
				}

			}
			++lev;
		}
		return ht;
	}

	void decode(ifstream& file, DecodeInformation& info) {

		int l = decodeLength(file) - 2;
		auto p = parseHexToHalfByteVals(readByte(file));

		std::vector<std::vector<std::uint8_t>> H_Table(16);

		HuffmanInfo& dht = info.huffman_tables;

		int cur_byte = 1;
		int i = 0;
		for (; i < 16; ++i, ++cur_byte) {
			H_Table[i].resize(readByte(file));
		}

		int j = 0;
		for (; cur_byte < l;) {
			while (H_Table[j].size() < 1 && j < 16) {
				++j;
			}
			for (int code_pos = 0; code_pos < H_Table[j].size(); ++cur_byte, ++code_pos) {
				if (cur_byte >= l) throw;
				H_Table[j][code_pos] = readByte(file);
			}
			++j;
		}

 		if (p.first) {
			if (dht.AC.size() <= p.second) dht.AC.resize(p.second + 1);
			dht.AC[p.second] = buildTree(H_Table);
		}
		else {
			if (dht.DC.size() <= p.second) dht.DC.resize(p.second + 1);
			dht.DC[p.second] = buildTree(H_Table);
		}

	}
}

HuffmanTree::Node* findNode(std::bitset<BITS_TO_READ>& binary_rep, int& cur_bit, uint8_t& byte, HuffmanTree::Node* node, ifstream& file) {
	while (true) {
		if (cur_bit < 0) {
			if (!validateBitset(binary_rep, byte, file)) {
				return nullptr;
			}
			cur_bit = 7;
		}
		uint8_t sign = binary_rep[cur_bit] & 1;
		if (sign == 1) {
			if (node->right) {
				node = node->right;
			}
			else {
				break;
			}
		}
		else {
			if (node->left) {
				node = node->left;
			}
			else {
				break;
			}
		}
		--cur_bit;
	}
	return node;
}

