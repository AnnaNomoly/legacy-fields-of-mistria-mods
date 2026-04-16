#include "Utils.h"

int euclidean_modulo(int a, int m)
{
	int r = a % m;
	return (r < 0) ? r + m : r;
}

std::vector<double> linspace(double start, double end, std::size_t num)
{
	std::vector<double> result;
	result.reserve(num);

	if (num == 0)
		return result;

	if (num == 1)
	{
		result.push_back(start);
		return result;
	}

	double step = (end - start) / static_cast<double>(num - 1);
	for (std::size_t i = 0; i < num; ++i)
		result.push_back(start + step * static_cast<double>(i));

	return result;
}

double round_n(double value, int decimals)
{
	double factor = std::pow(10.0, decimals);
	return std::round(value * factor) / factor;
}

std::complex<double> round_complex(const std::complex<double>& z, int decimals)
{
	return {
		round_n(z.real(), decimals),
		round_n(z.imag(), decimals)
	};
}

double GetDistance(int x1, int y1, int x2, int y2)
{
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

std::pair<int, int> GetVector(int x1, int y1, int x2, int y2)
{
	return { x2 - x1, y2 - y1 };
}

std::pair<int, int> GetCenterOffset(int screen_center_x, int screen_center_y, int image_width, int image_height)
{
	int offset_x = screen_center_x - image_width / 2;
	int offset_y = screen_center_y - image_height / 2;
	return { offset_x, offset_y };
}

std::vector<double> GetCenter(std::vector<double> topLeft, std::vector<double> bottomRight)
{
	double x = (topLeft[0] + bottomRight[0]) / 2.0;
	double y = (topLeft[1] + bottomRight[1]) / 2.0;
	std::vector<double> center = { x, y };
	return center;
}

uint64_t GetCurrentSystemTime() {
	return duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}
