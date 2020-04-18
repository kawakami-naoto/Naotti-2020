//i行j列目(i>=0, j>=0)をマスi * 6 + jとおく.
//用意するメソッド：終了条件(2種類), 動かす
#pragma once
#include <string>
#include <algorithm>
#include "MoveCommand.h"
using namespace std;

namespace bb {
	int weight1 = 1000;		//青駒の個数の評価の重み(1～1000)
	int weight2 = 1;		//駒の位置の評価の重み(1～1000)
	int bitCountTable[1 << 18];	//bitCountTable[s] = sの2進数表記における桁の和
	int _myGoalDist[1 << 18];	//_myGoalDist[s] = (sのiビット目が1⇔マスiに駒がある)ときのゴールまでのマンハッタン距離の和.
	int _yourGoalDist[1 << 18];
	
	//xは0～2^36 - 1
	inline int bitCount(long long x) {
		return bitCountTable[x & 262143] + bitCountTable[x >> 18];
	}
	
	//前処理. AI.cppで呼び出す.
	void prepare() {
		int i, j;
		
		for (i = 0; i < (1 << 9); i++) {
			int cnt = 0;
			for (j = 0; j < 9; j++) {
				if ((i >> j) & 1) cnt++;
			}
			bitCountTable[i] = cnt;
		}
		for (i = (1 << 9); i < (1 << 18); i++) {
			bitCountTable[i] = bitCountTable[i & 511] + bitCountTable[i >> 9];
		}
		
		for (i = 0; i < (1 << 18); i++) {
			_myGoalDist[i] = 0;
			_yourGoalDist[i] = 0;
			for (j = 0; j < 18; j++) {
				if ((i >> j) % 2 == 0) continue;
				//マスjからゴールまでのマンハッタン距離
				//相手陣はy = 0側に, 自陣はy = 5側にあるとする。
				int y = j / 6;
				int x = j % 6;
				int dist1 = y + min(x, 5 - x);
				int dist2 = 5 - y + min(x, 5 - x);
				_myGoalDist[i] += dist1;
				_yourGoalDist[i] += dist2;
			}
		}
	}
	
	//s = 0～2^36 - 1. (s >> i) & 1 == 1 ⇔ マスiに自駒がある
	inline int myGoalDist(long long s) {
		return _myGoalDist[s & 262143] + _myGoalDist[s >> 18] + 3 * bitCountTable[s >> 18];
	}
	
	//s = 0～2^36 - 1. (s >> i) & 1 == 1 ⇔ マスiに相手駒がある
	inline int yourGoalDist(long long s) {
		return _yourGoalDist[s & 262143] + _yourGoalDist[s >> 18] - 3 * bitCountTable[s >> 18];
	}
}

struct BitBoard
{
	long long existR;	//マスiに自分の赤がある ⇔ (existR >> i) & 1 == 1とする。
	long long existB;
	long long existP;	//マスiに敵の駒がある ⇔ (existP >> i) & 1 == 1とする。Purple駒…取ったら赤に変化し, 脱出するときは青に変化する.
	
	//board[i]…マスiにある駒の種類(R, B, u)
	void toBitBoard(string board) {
		existR = existB = existP = 0;
		for (int i = 0; i < 36; i++) {
			if (board[i] == 'R') { existR |= (1LL << i); }
			if (board[i] == 'B') { existB |= (1LL << i); }
			if (board[i] == 'u') { existP |= (1LL << i); }
		}
	}
	
	//手の生成. 手の個数を返す(teban=0 : 自分手番, teban=1:敵手番). from[], to[]に手を格納. (from, toはサイズ32以上の配列）
	//kiki[5 * i + j] = マスiと隣接するj番目のマスの番号. なければ-1。
	int makeMoves(int teban, int kiki[], int from[], int to[]) {
		int pos, i, cnt = 0;
		for (pos = 0; pos < 36; pos++) {
			if (teban == 0 && !(((existR | existB) >> pos) & 1)) continue;
			if (teban == 1 && !((existP >> pos) & 1)) continue;
			for (i = pos * 5; kiki[i] != -1; i++) {
				int npos = kiki[i];
				//マスpos -> マスnposと駒を動かせるか？（自駒とぶつからないか）
				if (teban == 0 && (((existR | existB) >> npos) & 1)) continue;
				if (teban == 1 && ((existP >> npos) & 1)) continue;
				//動かせる
				from[cnt] = pos;
				to[cnt] = npos;
				cnt++;
			}
		}
		return cnt;
	}
	
	//先手の赤を動かした後の状態に更新する。
	inline void moveR(int from, int to) {
		existR &= ~(1LL << from);
		existR |= (1LL << to);
		existP &= ~(1LL << to);
	}
	
	inline void moveB(int from, int to) {
		existB &= ~(1LL << from);
		existB |= (1LL << to);
		existP &= ~(1LL << to);
	}
	
	inline void moveP(int from, int to) {
		existP &= ~(1LL << from);
		existP |= (1LL << to);
		existR &= ~(1LL << to);
		existB &= ~(1LL << to);
	}
	
	void move(int from, int to) {
		if ((existR >> from) & 1)
			moveR(from, to);
		else if ((existB >> from) & 1)
			moveB(from, to);
		else if ((existP >> from) & 1)
			moveP(from, to);
		else
			assert(0);
	}
	
	//どちらが勝ち状態かを返す. (0…自分, 1…敵, 2…不明). 紫駒がpnum個以下なら敵の勝ち.
	//teban … 0なら自分手番. 1なら敵手番. (手番 … 手を打つ直前のプレイヤー)
	int getWinPlayer(int teban, int pnum) {
		if (existR == 0) return 0;
		if (existB == 0 || bb::bitCount(existP) <= pnum) return 1;
		if (teban == 0 && ((existB & 1LL) || ((existB >> 5) & 1LL))) return 0;
		if (teban == 1 && (((existP >> 30) & 1LL) || ((existP >> 35) & 1LL))) return 1;
		return 2;
	}
	
	//tebanプレイヤーが1手で脱出できるか？ (tebanが0なら自分手番)
	//・できない…MoveCommand(-1, -1, -1)
	//・できる  …動かし方（MoveCommand)を返す
	//深さ0での判定では, こちらを用いる. デバッグ用として, 相手番でも利用可能にする.
	MoveCommand getEscapeCommand(int teban) {
		if (teban == 0) {
			if (existB & 1LL) return MoveCommand(0, 0, 3);
			if ((existB >> 5) & 1LL) return MoveCommand(0, 5, 1);
			return MoveCommand(-1, -1, -1);
		}
		else {
			if ((existP >> 30) & 1LL) return MoveCommand(5, 0, 3);
			if ((existP >> 35) & 1LL) return MoveCommand(5, 5, 1);
			return MoveCommand(-1, -1, -1);
		}
	}
	
	//評価関数. tebanプレイヤーの有利さを返す. teban=0…自分手番.
	int evaluate(int teban) {
		int s0 = bb::weight1 * bb::bitCount(existB) - bb::weight2 * bb::myGoalDist(existB | existR);
		int s1 = -bb::weight2 * bb::yourGoalDist(existP);
		if (teban == 0) return s0 - s1;
		return s1 - s0;
	}
	
	//デバッグ用
	void printBoard() {
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				int id = y * 6 + x;
				if ((existR >> id) & 1) cout << "R";
				else if ((existB >> id) & 1) cout << "B";
				else if ((existP >> id) & 1) cout << "P";
				else cout << ".";
			}
			cout << endl;
		}
		cout << endl;
	}
};