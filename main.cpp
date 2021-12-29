// The library has been modified to always create a borderless window
#include "CImg.h"
using namespace cimg_library;

#include <functional>
#include <numbers>
#include <chrono>

using stripFunc = std::function<void(double, uint32_t*, int)>;
const int Height = 2;

double csin(double x) {
	return sin(x * 2 * std::numbers::pi);
}

double ccos(double x) {
	return cos(x * 2 * std::numbers::pi);
}

uint32_t from_rgb(double r, double g, double b) {
	uint32_t z = 0;
	z += (int)std::clamp(r * 255.0, 0.0, 255.0);
	z += (int)std::clamp(g * 255.0, 0.0, 255.0) << 8;
	z += (int)std::clamp(b * 255.0, 0.0, 255.0) << 16;
	return z;
}

uint32_t scale_color(uint32_t base, double factor) {
	double r = base & 0xff;
	double g = (base >> 8) & 0xff;
	double b = (base >> 16) & 0xff;
	r *= factor;
	g *= factor;
	b *= factor;
	return from_rgb(r / 255, g / 255, b / 255);
}

void rainbow(double t, uint32_t* a, int w) {
	const double period = 10.0;
	double ang_base = t / period;
	const double extra = 0.2;
	for (int i = 0; i < w; i++) {
		double ang = ang_base + i * 2.0 / w;
		double r = csin(ang + 0.0 / 3.0) + extra;
		double g = csin(ang + 1.0 / 3.0) + extra;
		double b = csin(ang + 2.0 / 3.0) + extra;
		a[i] = from_rgb(r, g, b);
	}
}

namespace christmas_lights {
	const uint32_t colors[4] = { 0x0000ee, 0x00bbbb, 0x00cc00, 0xff4400 };

	int get_space_state(int i, int w) {
		int x = (i * 160 / w) % 8;
		if (x % 2 == 0) {
			return x / 2;
		} else {
			return -1;
		}
	}

	int get_time_state(double period, double t, int states) {
		double cycle_t = t / period;
		return std::clamp((int)floor((cycle_t - floor(cycle_t)) * states), 0, states - 1);
	}

	void mask_cycle(double t, uint32_t* a, int w, int mask) {
		const double period = 0.8;
		const int time_state = get_time_state(period, t, 4);

		for (int i = 0; i < w; i++) {
			int space_state = get_space_state(i, w);
			if (space_state == -1) {
				a[i] = 0;
			} else if (mask & (1 << ((space_state + 4 - time_state) % 4))) {
				a[i] = colors[space_state];
			}
		}
	}

	void ascending(double t, uint32_t* a, int w) {
		return mask_cycle(t, a, w, 0b0001);
	}

	void alternating(double t, uint32_t* a, int w) {
		return mask_cycle(t, a, w, 0b0101);
	}

	void ascending_double(double t, uint32_t* a, int w) {
		return mask_cycle(t, a, w, 0b0011);
	}

	void slow_pulse_single(double t, uint32_t* a, int w) {
		const double period = 20;
		const double cycle_t = t / period;
		const int time_state = get_time_state(period, t, 4);

		for (int i = 0; i < w; i++) {
			int space_state = get_space_state(i, w);
			if (space_state == time_state) {
				a[i] = scale_color(colors[time_state], (1 - ccos(cycle_t * 4)) / 2);
			} else {
				a[i] = 0;
			}
		}
	}

	void slow_pulse_all(double t, uint32_t* a, int w) {
		const double period = 5;
		const double cycle_t = t / period;

		for (int i = 0; i < w; i++) {
			int space_state = get_space_state(i, w);
			if (space_state != -1) {
				a[i] = scale_color(colors[space_state], (1 - ccos(cycle_t)) / 2);
			} else {
				a[i] = 0;
			}
		}
	}

	void flash_single(double t, uint32_t* a, int w) {
		const double period = 3.2;
		const double cycle_t = t / period;
		const int time_state = get_time_state(period, t, 32);

		for (int i = 0; i < w; i++) {
			int space_state = get_space_state(i, w);
			if (time_state % 2 == 0 && space_state == time_state / 8) {
				a[i] = colors[space_state];
			}
			else {
				a[i] = 0;
			}
		}
	}

	void meshuggah(double t, uint32_t* a, int w) {
		const double period_base = 48.0 / 23.0;
		const int time_state_base = get_time_state(period_base, t, 64);

		const double period_kick = 87.0 / 23.0;
		const int time_state_kick = get_time_state(period_kick, t, 116);
		
		const auto kicks   = 0b0000000101011101011101011101011100000001010111010111010111LL;
		const auto changes = 0b0000001000000010000010000010000000000010000000100000100000LL;

		static int primary_color = 3;
		static int last_kick = -1;

		if (time_state_kick % 2 == 0) {
			int kick = time_state_kick / 2;
			if (kick != last_kick && (changes & (1LL << kick))) {
				primary_color = (0x2130 >> (4 * primary_color)) & 0xf;
			}
			last_kick = kick;
		}

		for (int i = 0; i < w; i++) {
			int space_state = get_space_state(i, w);
			if (space_state == 0) {
				if (time_state_base % 16 < 2) {
					if (time_state_base / 16 == 2) {
						a[i] = colors[space_state];
					} else {
						a[i] = scale_color(colors[space_state], 0.5);
					}
				} else {
					a[i] = 0;
				}
			} else {
				if (time_state_kick % 2 == 0) {
					int kick = time_state_kick / 2;
					if ((kicks & (1LL << kick)) && primary_color == space_state) {
						a[i] = colors[space_state];
					} else {
						a[i] = 0;
					}
				}
			}
		}
	}

	void mix(double t, uint32_t* a, int w) {
		const stripFunc funcs[7] = {
			slow_pulse_single,
			ascending,
			slow_pulse_all,
			alternating,
			flash_single,
			ascending_double,
			meshuggah,
		};
		const int n = std::size(funcs);
		const double period = n * 60.0;
		const int time_state = get_time_state(period, t, n);
		double cropped_t = (t / 60.0 - floor(t / 60.0)) * 60.0;
		funcs[time_state](cropped_t, a, w);
	}
}

CImg<uint8_t> from_func(stripFunc f, int w, double t) {
	CImg<uint8_t> img(w, Height, 1, 3);
	std::vector<uint32_t> a(w);
	f(t, a.data(), w);
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < Height; j++) {
			*img.data(i, j, 0, 0) = a[i] & 0xff;
			*img.data(i, j, 0, 1) = (a[i] >> 8) & 0xff;
			*img.data(i, j, 0, 2) = (a[i] >> 16) & 0xff;
		}
	}
	return img;
}

int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	const stripFunc func = christmas_lights::mix;
	const int w = CImgDisplay::screen_width();
	const auto start_time = std::chrono::steady_clock::now();
	
	CImg<uint8_t> img = from_func(func, w, 0.0);
	CImgDisplay display(img);
	display.move(0, 0);
	
	while (!display.is_closed()) {
		auto curr_time = std::chrono::steady_clock::now();
		display.display(from_func(func, w, (curr_time - start_time).count() / 1e9));
		display.wait(4);
	}
	
	return 0;
}
