#pragma once


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
		int e[4];
	};
}