

#include "image_ppm.h"

using namespace std;

bool savePPM(const string& path, const vector<vector<RGB>>& image, int w, int h) {
	
	ofstream out(path, ios::binary);


	out << PPM_SIGN << '\n' << w << ' ' << h << '\n' << PPM_MAX << '\n';

    std::vector<char> buff(w * 3);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            buff[x * 3 + 0] = static_cast<char>(image[y][x].R);
            buff[x * 3 + 1] = static_cast<char>(image[y][x].G);
            buff[x * 3 + 2] = static_cast<char>(image[y][x].B);
        }
        out.write(buff.data(), w * 3);
    }




	//std::vector<char> buff(w * 3);

 //   int blocks_in_row_in_big_block = sqrt(image[0].size());

 //   int big_blocks_in_row = w / (blocks_in_row_in_big_block * 8);
 //   int big_blocks_in_col = h / (blocks_in_row_in_big_block * 8);


 //   for (int big_block_row = 0; big_block_row < big_blocks_in_col; ++big_block_row) { //��������� �� ����� ��� � �������� ���������� (� ��� 1�1)
 //       for (int pixel_row = 0; pixel_row < blocks_in_row_in_big_block * 8; ++pixel_row) { // �� ������ ��� �������� � ������� �������� (16 ������� �� 8 �� ������ ��������� ��������)
 //           for (int big_block_col = 0; big_block_col < big_blocks_in_row; ++big_blocks_in_col) { // ��� ������� �������� �������� � ���� (� ��� 1)
 //               for (int small_block = 0; small_block < blocks_in_row_in_big_block; ++small_block) {
 //                   for (int pixel = 0; pixel < 8; ++pixel) {

 //                   }
 //               }
 //           }
 //       }
 //   }


    //for (int big_block_row = 0; big_block_row < big_blocks_in_col; ++big_block_row) { //������� ����� (� ��� 4�4 RGB) ���
    //    for (int a = 0; a < blocks_in_row_in_big_block; ++a) { // ���������� ����������� ��������� ������ RGB � �������
    //        for (int j = 0; j < 8; ++j) { // ���������� ����� ������� � ��������� ����� RGB
    //            for (int big_block_col = 0; big_block_col < big_blocks_in_row; ++big_block_col) {
    //                for (int i = 0; i < blocks_in_row_in_big_block * 8; ++i) { // ���������� ������� �������� � ������� ����� RGB

    //                    buff[i * 3 + 0] = image[big_blocks_in_row * big_block_row + big_block_col][a * blocks_in_row_in_big_block + i / 8].block[j % 8][i % 8].R;
    //                    buff[i * 3 + 0] = image[big_blocks_in_row * big_block_row + big_block_col][a * blocks_in_row_in_big_block + i / 8].block[j % 8][i % 8].G;
    //                    buff[i * 3 + 0] = image[big_blocks_in_row * big_block_row + big_block_col][a * blocks_in_row_in_big_block + i / 8].block[j % 8][i % 8].B;

    //                }
    //            }
    //            out.write(buff.data(), w * 3);
    //        }

    //    }
    //} 

    return out.good();

}