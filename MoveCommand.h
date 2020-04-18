#pragma once
#include <assert.h>

class MoveCommand
{
public:
	int y, x;	//移動する駒の座標 (移動前)
	int dir;	//移動する方向 (↑…0, →…1, ↓…2, ←…3)
	
	MoveCommand() { y = -1; x = -1; dir = -1; }
	MoveCommand(int y, int x, int dir)
	{
		this->y = y;
		this->x = x;
		this->dir = dir;
	}
	bool operator<(const MoveCommand &r) const {
		if (y != r.y) return y < r.y;
		if (x != r.x) return x < r.x;
		return dir < r.dir;
	}
	bool operator==(const MoveCommand &r) const {
		return y == r.y && x == r.x && dir == r.dir;
	}
	static MoveCommand parse(int from, int to) {
		assert(0 <= from && from < 36);
		assert(0 <= to && to < 36);
		int y = from / 6;
		int x = from % 6;
		int dy[4] = {-1, 0, 1, 0};
		int dx[4] = {0, 1, 0, -1};
		int dir;
		for (dir = 0; dir < 4; dir++) {
			int ny = y + dy[dir];
			int nx = x + dx[dir];
			if (ny * 6 + nx == to) break;
		}
		assert(dir < 4);
		return MoveCommand(y, x, dir);
	}
};