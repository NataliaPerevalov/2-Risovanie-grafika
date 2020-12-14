// Шаблон программы для задачи от 5.10.2020-6.10.2020
//
/*
	Задача:
	Построить график функции
		z = sinc(sqrt(x*x + y*y))
	Шаг 1. Придумать формулы для перевода координат из трёхмерной системы
			(x, y, z) в координаты точки на картинке (sx, sy)
			с учётом масштабных коэффициентов по всем трём осям
	Шаг 2. Построить график функции, перебирая одну координату с большим шагом,
			а вторую - с маленьким шагом.
	Шаг 2.5. Построить график ещё раз, поменяв местами шаг по осям.
		(на данном этапе у нас уже получится некий график, но....)
	Шаг 3. Для сокрытия невидимых линий графика реализовать алгоритм
			"плавающего горизонта"
				1. График всегда рисуется от ближних объектов к дальним
				2. Если очередная точка находится ниже линии горизонта,
					то её не рисуем
				3. Если очередная точка находится выше линии горизонта, то:
					* точку рисуем;
					* поднимаем в этой колонке изображения горизонт до уровня
					  новой точки
				4. Перед вторым проходом рисования графика (в перпендикулярных
					плоскостях) горизонт опускаем.
		Область построения: -30 <= x <= 30 и -30 <= y <= 30 (например)
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

// Это у нас структура заголовка TGA-файла
#pragma pack(push, 1)
struct TGA_Header
{
	uint8_t id_len;			// Длина идентификатора
	uint8_t pal_type;		// Тип палитры
	uint8_t img_type;		// Тип изображения
	uint8_t pal_desc[5];	// Описание палитры
	uint16_t x_pos;			// Положение по оси X
	uint16_t y_pos;			// Положение по оси Y
	uint16_t width;			// Ширина
	uint16_t height;		// Высота
	uint8_t depth;			// Глубина цвета
	uint8_t img_desc;		// Описатель изображения
};
#pragma pack(pop)

constexpr uint16_t IMG_WIDTH = 1280;
constexpr uint16_t IMG_HEIGHT = 720;
constexpr uint32_t COL_BACKGROUND = 0xff003f3f;
constexpr uint32_t COL_FOREGROUND = 0xffcfcfcf;

// Тут могли бы разместиться Ваши константы

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
	// Записать построенное изображение в файл формата TGA
	TGA_Header hdr {};
	hdr.width = IMG_WIDTH;
	hdr.height = IMG_HEIGHT;
	hdr.depth = 32;
	hdr.img_type = 2;
	hdr.img_desc = 0x28;
	// Построить график функции в некотором буфере
	std::vector<uint32_t> picture(IMG_WIDTH * IMG_HEIGHT);
	for (auto && p : picture) p = COL_BACKGROUND;
	// Здесь мог бы разместиться Ваш код.
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



	// Создаём поток + открываем сразу файл
	std::ofstream tga_file { "output.tga", std::ios::binary };
	// Записываем заголовок и данные картинки
	tga_file.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
	tga_file.write(reinterpret_cast<char*>(&picture[0]),
			picture.size() * 4.0);
	// Закрываем файл
	tga_file.close();

	return 0;
}
