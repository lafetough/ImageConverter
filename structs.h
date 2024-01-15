// Kursovaya.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <optional>
#include <bitset>
#include <unordered_map>

#define BITS_TO_READ 8

static const std::unordered_map<char, int> HALF_BYTE_TO_VAL = {
	{'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4},
	{'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
	{'a', 10}, {'b', 11}, {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}
};

struct Matrix
{
	Matrix(int size)
		:matrix(size, std::vector<int>(size, 0))
	{}

	std::vector<int>& operator[](int i) {
		return matrix[i];
	}

	std::vector<std::vector<int>> matrix;
};

struct QuantTable 
{
	QuantTable()
		:table(8)
	{}
	Matrix table;
	bool length_of_vals;
};

struct Channel
{
	Channel() = default;

	Channel(int i, int h, int v, int q_i) 
		:id(i ), H(h), V(v), quant_id(q_i)
	{}

	int id;
	int H;
	int V;
	int quant_id;
};

enum MethodType{
	C0,
	C1,
	C2,
	C3
};

struct MethodInfo
{
	MethodInfo() = default;
	MethodInfo(int c_a)
		:channels(c_a)
	{}

	MethodInfo(int l, int channels_num, int w, int h, int p)
		:length(l), precision(p), height(h), width(w), channels(channels_num)
	{}

	int length = 0;
	int precision = 0;
	int height = 0;
	int width = 0;
	MethodType dct_type;
	std::vector<Channel> channels;

	void addChannel( int quant_id, std::pair<int, int> HV, int id) {
		channels[id] = { id, HV.first, HV.second, quant_id };
		if (HV.first > H_max) H_max = HV.first;
		if (HV.second > V_max) V_max = HV.second;
	}

	int H_max = -1;
	int V_max = -1;

};


struct HuffmanTree
{
	struct Node
	{
		Node() = default;

		Node(Node* p, uint8_t v)
			:parent(p), val(v)
		{}
		Node(Node* p)
			:parent(p)
		{}

		Node* left = nullptr;
		Node* right = nullptr;
		Node* parent = nullptr;
		std::optional<uint8_t> val = std::nullopt;
	};

	HuffmanTree()
		:root(new Node)
	{}

	Node* root;
};

struct HuffmanInfo
{
	std::vector<HuffmanTree> DC;
	std::vector<HuffmanTree> AC;
};

struct StartOfScanInfo
{
	int DC_factor_id;
	int AC_factor_id;
};


struct DecodeInformation
{
	MethodInfo method_info;
	std::vector<QuantTable> quant_tables;
	HuffmanInfo huffman_tables;
	std::vector<StartOfScanInfo> SOS;

};



struct JPEG_Y_Channels
{
	JPEG_Y_Channels(int size)
		:matrixes(size, 8)
	{}
	std::vector<Matrix> matrixes;
};


struct RGB
{
	RGB(int r, int g, int b)
		:R(r), G(g), B(b)
	{}

	RGB() = default;

	int R = 255;
	int G = 255;
	int B = 255;
};


struct BlockChannels
{
	BlockChannels(int amount)
		:y_channels(amount ), cb_channel(8), cr_channel(8)
	{}

	JPEG_Y_Channels y_channels;
	Matrix cb_channel;
	Matrix cr_channel;
};


struct Block
{
	Block()
		:block(8, std::vector<RGB>(8))
	{}

	std::vector<std::vector<RGB>> block;
};


struct Image
{
	
};

// TODO: установите здесь ссылки на дополнительные заголовки, требующиеся для программы.
