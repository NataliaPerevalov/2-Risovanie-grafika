// ������ ��������� ��� ������ �� 5.10.2020-6.10.2020
//
/*
	������:
	��������� ������ �������
		z = sinc(sqrt(x*x + y*y))
	��� 1. ��������� ������� ��� �������� ��������� �� ��������� �������
			(x, y, z) � ���������� ����� �� �������� (sx, sy)
			� ������ ���������� ������������� �� ���� ��� ����
	��� 2. ��������� ������ �������, ��������� ���� ���������� � ������� �����,
			� ������ - � ��������� �����.
	��� 2.5. ��������� ������ ��� ���, ������� ������� ��� �� ����.
		(�� ������ ����� � ��� ��� ��������� ����� ������, ��....)
	��� 3. ��� �������� ��������� ����� ������� ����������� ��������
			"���������� ���������"
				1. ������ ������ �������� �� ������� �������� � �������
				2. ���� ��������� ����� ��������� ���� ����� ���������,
					�� � �� ������
				3. ���� ��������� ����� ��������� ���� ����� ���������, ��:
					* ����� ������;
					* ��������� � ���� ������� ����������� �������� �� ������
					  ����� �����
				4. ����� ������ �������� ��������� ������� (� ����������������
					����������) �������� ��������.
		������� ����������: -30 <= x <= 30 � -30 <= y <= 30 (��������)
 */

#include <fstream>
#include <cmath>
#include <cstdint>
#include <vector>

static constexpr auto Pi = acos (-1.);
constexpr double big = 0.5, small = 0.01;
constexpr double  area = 30.0;
constexpr	double Ox = 30.0;
constexpr	double Oy = 30.0;
constexpr	double Oz = 250.0;

// ��� � ��� ��������� ��������� TGA-�����
#pragma pack(push, 1)
struct TGA_Header
{
	uint8_t id_len;			// ����� ��������������
	uint8_t pal_type;		// ��� �������
	uint8_t img_type;		// ��� �����������
	uint8_t pal_desc[5];	// �������� �������
	uint16_t x_pos;			// ��������� �� ��� X
	uint16_t y_pos;			// ��������� �� ��� Y
	uint16_t width;			// ������
	uint16_t height;		// ������
	uint8_t depth;			// ������� �����
	uint8_t img_desc;		// ��������� �����������
};
#pragma pack(pop)

constexpr uint16_t IMG_WIDTH = 1280;
constexpr uint16_t IMG_HEIGHT = 720;
constexpr uint32_t COL_BACKGROUND = 0xff003f3f;
constexpr uint32_t COL_FOREGROUND = 0xffcfcfcf;

// ��� ����� �� ������������ ���� ���������

double sinc(double x)
{
	if (x == 0.0) return 1.0;
	return sin(x) / x;
}

double my_evil_function(double x, double y)
{
	return sinc(hypot(x, y));
}


int main()
{
	// �������� ����������� ����������� � ���� ������� TGA
	TGA_Header hdr {};
	hdr.width = IMG_WIDTH;
	hdr.height = IMG_HEIGHT;
	hdr.depth = 32;
	hdr.img_type = 2;
	hdr.img_desc = 0x28;
	// ��������� ������ ������� � ��������� ������
	std::vector<uint32_t> picture(IMG_WIDTH * IMG_HEIGHT);
	for (auto && p : picture) p = COL_BACKGROUND;
	// ����� ��� �� ������������ ��� ���.
	std::vector<int> horizont(IMG_WIDTH);
	for(auto && value : horizont)
		value = IMG_HEIGHT;
	for (double i = area; i > -area; i -= big) {
		for (double j = area; j > -area; j -= small) {
			double z = (my_evil_function(i, j));
			int x = int( IMG_WIDTH / 2 + cos( Pi/6.0 )*(Ox * i - Oy * j) );
			int y = int( IMG_HEIGHT / 2 + sin( Pi/6.0 )*(Ox * i  + Oy * j) - Oz * z);
			if(0 < x && x < IMG_WIDTH && y * IMG_WIDTH + x <= int(IMG_WIDTH * IMG_HEIGHT)
					&& horizont[x] > y){
				horizont[x] = y;
				if( 0 < y && y <= IMG_HEIGHT )
					picture[y * IMG_WIDTH + x] = COL_FOREGROUND;
			}
		}
	}
	for(auto && value : horizont)
		value = IMG_HEIGHT;
	for (double j = area; j > -area; j -= big) {
		for (double i = area; i > -area; i -= small) {
			double z = my_evil_function(double(i), double(j));
			int x = int( IMG_WIDTH / 2 + cos( Pi/6.0 )*(Ox * i - Oy * j) );
			int y = int( IMG_HEIGHT / 2 + sin( Pi/6.0 )*(Ox * i  + Oy * j) - Oz * z);
			if(0 < x && x < IMG_WIDTH && y * IMG_WIDTH + x <= int(IMG_WIDTH * IMG_HEIGHT)
					&& horizont[x] > y){
				horizont[x] = y;
				if( 0 < y && y <= IMG_HEIGHT )
					picture[y * IMG_WIDTH + x] = COL_FOREGROUND;
			}
		}
	}



	// ������ ����� + ��������� ����� ����
	std::ofstream tga_file { "output.tga", std::ios::binary };
	// ���������� ��������� � ������ ��������
	tga_file.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
	tga_file.write(reinterpret_cast<char*>(&picture[0]),
			picture.size() * 4.0);
	// ��������� ����
	tga_file.close();

	return 0;
}
