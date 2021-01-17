#pragma once

#include <stdint.h>


namespace cotwin
{
	union Vector2
	{
		struct {
			int x, y;
		};
		struct {
			int w, h;
		};
		int e[2];
	};

	union Vector3
	{
		struct {
			int x, y, z;
		};
		struct {
			int r, g, b;
		};
		int e[3];
	};

	union Vector4
	{
		struct {
			int x, y, z, w;
		};
		struct {
			int r, g, b, a;
		};
		struct {
			int left, top, width, height;
		};
		int e[4];
	};

	union Vector4ui8
	{
		struct {
			uint8_t x, y, z, w;
		};
		struct {
			uint8_t r, g, b, a;
		};
		struct {
			uint8_t left, top, width, height;
		};
		uint8_t e[4];
	};
}