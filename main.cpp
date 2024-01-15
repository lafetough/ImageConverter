// Kursovaya.cpp: определяет точку входа для приложения.
//

#include "structs.h"
#include "HexLib.h"
#include "QuantTableDecoder.h"
#include "MethodDecoder.h"
#include "HuffmanTableDecoder.h"
#include "StartOfScanDecoder.h"
#include "GeneralImageDecoder.h"
#include "image_ppm.h"

#include <cmath>


using namespace std;

bool isJPEG(ifstream& file) {
	uint8_t marker1, marker2;
	file.read(reinterpret_cast<char*>(&marker1), 1);
	file.read(reinterpret_cast<char*>(&marker2), 1);

	return marker1 != 0xFF || marker2 != 0xD8;
}

void skipComentSection(ifstream& file) {
	uint8_t marker1, marker2;
	while (!file.eof()) {
		file.read(reinterpret_cast<char*>(&marker1), 1);
		file.read(reinterpret_cast<char*>(&marker2), 1);
		if (marker1 == 0xFF && marker2 == 0xDB) {
			break;
		}
	}
}

void printM(const vector<vector<uint8_t>>& m) {
	for (const auto& v : m) {
		for (const auto& n : v) {
			std::cout << stringHex(n) << " ";
		}
		std::cout << endl;
	}
}
void printM(const vector<vector<int>>& m) {
	for (const auto& v : m) {
		for (const auto& n : v) {
			std::cout << n << " ";
		}
		std::cout << endl;
	}
}


enum class ChannelType
{
	Y,
	Cb,
	Cr
};

std::optional<Matrix> MakeChannelMatrix(bitset<BITS_TO_READ>& binary_rep, int& cur_bit, uint8_t cur_byte, const DecodeInformation& decode_info, ChannelType channel, ifstream& file) {
	// текущая позиция, с которой работаем и вектор матрицы 8х8, распаложенной в ряд
			// для дальнейшего построения матрицы в зиг-загообразном порядке
	int vals_pos = 0;
	vector<int> values(64, 0);
	int channel_id = 0;

	switch (channel)
	{
	case ChannelType::Y:
		channel_id = 1; 
		break;
	case ChannelType::Cb:
		channel_id = 2;
		break;
	case ChannelType::Cr:
		channel_id = 3;
		break;
	default:
		break;
	}


	//Считывание DC-коэффициента
	{
		HuffmanTree::Node* tree_val = findNode(binary_rep, cur_bit, cur_byte, decode_info.huffman_tables.DC[decode_info.SOS[channel_id].DC_factor_id].root, file);

		if (!tree_val->val) {
			return nullopt;
		}

		values[vals_pos++] = computeDCFactor(
			binary_rep,
			cur_bit,
			cur_byte,
			tree_val,
			file
		);
	}

	//Считываем AC-коэффициенты
	while (true)
	{

		HuffmanTree::Node* node = findNode(binary_rep, cur_bit, cur_byte, decode_info.huffman_tables.AC[decode_info.SOS[channel_id].AC_factor_id].root, file);

		if (!node->val) {
			return nullopt;
		}

		auto [zeros_to_add, factor] = computeACFactor(
			binary_rep,
			cur_bit,
			cur_byte,
			node,
			file);

		if (zeros_to_add < 0) {
			throw;
		}
		if (factor == 0 && zeros_to_add == 0) {
			break;
		}

		vals_pos += zeros_to_add;
		values[vals_pos++] = factor;

	}

	return buildMatrixZigZagOrder(values);

}


void Quantition(const QuantTable& quant_table, Matrix& channel) {
	for (int m = 0; m < BITS_TO_READ; ++m) {
		for (int v = 0; v < BITS_TO_READ; ++v) {
			channel[m][v] *= quant_table.table.matrix[m][v];
		}
	}
}


Matrix InverseDiscreteCosineTransformation(const Matrix& S) {
	Matrix res(8);

	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {

			double r = 0;

			for (int u = 0; u < 8; ++u) {
				for (int v = 0; v < 8; ++v) {

					double Cu = u == 0 ? 1 / sqrt(2) : 1.0;
					double Cv = v == 0 ? 1 / sqrt(2) : 1.0;

					r += 
						Cu *
						Cv *
						S.matrix.at(v).at(u) *
						cos(((2 * x + 1) * u * 3.14) / 16) *
						cos(((2 * y + 1) * v * 3.14) / 16);
				}
			}

			res[y][x] = static_cast<int>(r / 4);

			res[y][x] = min(max(0, res[y][x] + 128), 255);
		}
	}

	return res;
}

RGB YCbCrToRGB(int Y, int Cb, int Cr) {

	RGB rgb;

	rgb.R = (Y + 1.402 * (Cr - 128));
	rgb.G = (Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128));
	rgb.B = (Y + 1.772 * (Cb - 128));


	rgb.R = min(max(0, rgb.R), 255);
	rgb.G = min(max(0, rgb.G), 255);
	rgb.B = min(max(0, rgb.B), 255);

	return rgb;

}



int main(int argc, char* argv[])
{

	argc = 4;

	setlocale(LC_ALL, "ru");

	if (argc < 2) {
		cerr << "Нет аргументов"s << endl;
		return -1;
	}

	ifstream file("C:/dev/OP/Kursovaya/out/build/x64-debug/Test.jpeg"s, std::ios::binary);

	if (!file) {
		cerr << "Не удалось открыть файл JPEG."s << endl;
		return 1;
	}

	if (isJPEG(file)) {
		cerr << "Файл не является действительным JPEG." << endl;
		return 2;
	}
	else {
		std::cout << "Файл успешно открыт"s << endl;
	}

	DecodeInformation decode_info;
	bool is_image_started = false;

	//vector<DKP8> dkps;
	while (!file.eof() && !is_image_started) {
		uint8_t marker1;
		file.read(reinterpret_cast<char*>(&marker1), 1);

		if (marker1 == 0xFF) {
			uint8_t marker2;
			file.read(reinterpret_cast<char*>(&marker2), 1);

			switch (marker2)
			{
			case 0xDB:
				QuantTableDecoder::decode(file, decode_info);
				break;
			case 0xC2:
				decode_info.method_info = MethodDecoder::decode(file);
				decode_info.method_info.dct_type = MethodType::C2;
				break;
			case 0xC0:
				decode_info.method_info = MethodDecoder::decode(file);
				decode_info.method_info.dct_type = MethodType::C0;
				break;
			case 0xC4:
				HuffmanTableDecoder::decode(file, decode_info);
				break;
			case 0xDA:
				StartOfScanDecoder::decode(file, decode_info);
				is_image_started = true;
				break;
			default:
				break;
			}

		}

	}


	int amount_of_Y_channels = decode_info.method_info.channels[1].H * decode_info.method_info.channels[1].V;
	int amount_of_Cr_channels = decode_info.method_info.channels[2].H * decode_info.method_info.channels[2].V;
	int amount_of_Cb_channels = decode_info.method_info.channels[3].H * decode_info.method_info.channels[3].V;


	uint8_t byte = readByte(file);
	if (isEndImage(byte, file)) {
		std::cout << "Done" << endl;
	}
	std::bitset<BITS_TO_READ> binary_rep(byte); // двоичное представление байта			
	int cur_bit = BITS_TO_READ - 1; // позиция бита в байте с которым работаем

	vector<BlockChannels> channelsByBlock;

 	while (!file.eof() && !isEndImage(byte, file)) {
		
		BlockChannels curr_block(amount_of_Y_channels);

		bool is_end = false;

		//создаем матрицы каналов
		for (int y = 0; y < amount_of_Y_channels; ++y) {


			auto m = MakeChannelMatrix(
				binary_rep,
				cur_bit,
				byte,
				decode_info,
				ChannelType::Y,
				file
			);

			if (!m) {
				if (isEndImage(readByte(file), file)) {
					is_end = true;
					break;
				}
				throw;
			}

			curr_block.y_channels.matrixes[y] = *m;

			if (y > 0) {
				curr_block.y_channels.matrixes[y].matrix[0][0] = curr_block.y_channels.matrixes[y - 1].matrix[0][0] + curr_block.y_channels.matrixes[y].matrix[0][0];
			}		
		}

		if (is_end) {
			break;
		}

		

		//Cb_канал
		{
			
			auto m = MakeChannelMatrix(
				binary_rep,
				cur_bit,
				byte,
				decode_info,
				ChannelType::Cb,
				file
			);

			if (!m) {
				throw;
			}

			curr_block.cb_channel = *m;

		}


		//Сr_канал

		{
			
			auto m = MakeChannelMatrix(
				binary_rep,
				cur_bit,
				byte,
				decode_info,
				ChannelType::Cr,
				file
			);

			if (!m) {
				throw;
			}

			curr_block.cr_channel = *m;
		}

		for (int i = 0; i < amount_of_Y_channels; ++i) {
			auto& quant_matrix = decode_info.quant_tables[decode_info.method_info.channels[1].quant_id];

			auto& channel = curr_block.y_channels.matrixes[i];

			Quantition(quant_matrix, channel);

			channel =  InverseDiscreteCosineTransformation(channel);

		}


		{
			auto& quant_matrix = decode_info.quant_tables[decode_info.method_info.channels[2].quant_id];

			Quantition(quant_matrix, curr_block.cb_channel);
			curr_block.cb_channel = InverseDiscreteCosineTransformation(curr_block.cb_channel);
		}

		{
			auto& quant_matrix = decode_info.quant_tables[decode_info.method_info.channels[3].quant_id];

			Quantition(quant_matrix, curr_block.cr_channel);
			curr_block.cr_channel = InverseDiscreteCosineTransformation(curr_block.cr_channel);

		}


		channelsByBlock.push_back(std::move(curr_block));
			
		
	}

	//vector<vector<Block>> image;

	vector<vector<RGB>> image(decode_info.method_info.width, vector<RGB>(decode_info.method_info.height)); 


	int blocks_col_amount = decode_info.method_info.width / (decode_info.method_info.V_max * 8);
	int blocks_row_amount = channelsByBlock.size() / blocks_col_amount;

	for (int block_row = 0; block_row < blocks_row_amount; ++block_row) {
		for (int block_col = 0; block_col < blocks_col_amount; ++block_col) {
			int y_channels_in_row = channelsByBlock[0].y_channels.matrixes.size() / decode_info.method_info.V_max;
			int y_channels_in_col = channelsByBlock[0].y_channels.matrixes.size() / decode_info.method_info.H_max;

			for (int y = 0; y < y_channels_in_row * 8; ++y) {
				for (int x = 0; x < y_channels_in_col * 8; ++x) {

					auto& channels = channelsByBlock[block_row * blocks_col_amount + block_row];

					image[block_row * y_channels_in_row + y][block_col * y_channels_in_col + x] = YCbCrToRGB(
						channels.y_channels.matrixes[y / 8 + x / 8].matrix[y % 8][x % 8],
						channels.cb_channel.matrix[y / 2][x / 2],
						channels.cr_channel.matrix[y / 2][x / 2]
					);
				}
			}
		}
	}

	for (int i = 0; i < image.size(); ++i) {
		for (int j = 0; j < image[i].size(); ++j) {
			cout << image[i][j].R << " ";
		}
		cout << endl;
	}

	cout << endl;
	

	for (int i = 0; i < image.size(); ++i) {
		for (int j = 0; j < image[i].size(); ++j) {
			cout << image[i][j].G << " ";
		}
		cout << endl;
	}

	cout << endl;

	for (int i = 0; i < image.size(); ++i) {
		for (int j = 0; j < image[i].size(); ++j) {
			cout << image[i][j].B << " ";
		}
		cout << endl;
	}

	cout << endl;

	/*for (auto channels : channelsByBlock) {

		int y_channels_in_row = channels.y_channels.matrixes.size() / decode_info.method_info.V_max;
		int y_channels_in_col = channels.y_channels.matrixes.size() / decode_info.method_info.H_max;

		vector<Block> blocks(y_channels_in_col* y_channels_in_row);

 		for (int y = 0; y < y_channels_in_row * 8; ++y) {
			for (int x = 0; x < y_channels_in_col * 8; ++x) {

				blocks[y / 8 + x / 8].block[y % 8][x % 8] = YCbCrToRGB(
					channels.y_channels.matrixes[y / 8 + x / 8].matrix[y % 8][x % 8],
					channels.cb_channel.matrix[y / 2][x / 2],
					channels.cr_channel.matrix[y / 2][x / 2]
				);

			}

		}

		for (int i = 0; i < blocks[0].block.size(); ++i) {
			for (int j = 0; j < blocks[0].block.size(); ++j) {
				std::cout << blocks[0].block[i][j].R << " ";
			}
			std::cout << endl;
		}
		std::cout << endl;
		for (int i = 0; i < blocks[0].block.size(); ++i) {
			for (int j = 0; j < blocks[0].block.size(); ++j) {
				std::cout << blocks[0].block[i][j].G << " ";
			}
			std::cout << endl;
		}
		std::cout << endl;
		for (int i = 0; i < blocks[0].block.size(); ++i) {
			for (int j = 0; j < blocks[0].block.size(); ++j) {
				std::cout << blocks[0].block[i][j].B << " ";
			}
			std::cout << endl;
		}

		image.push_back(std::move(blocks));
		

	}*/

	savePPM("C:/dev/OP/Kursovaya/out/build/x64-debug/Test3Out.ppm", image, decode_info.method_info.width, decode_info.method_info.height);


}
