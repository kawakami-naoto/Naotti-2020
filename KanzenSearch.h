//ガイスターの探索
//不完全情報部分, 完全情報ガイスターの2段階からなる。
//i行j列目(i>=0, j>=0)をマスi * 6 + jとおく。先手の脱出口はマス0, 5。
//cornerId…左上0, 右上1, 左下2, 右下3
#pragma once
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include "KanzenBoard.h"
#include "Game.h"
using namespace std;

class KanzenSearch
{
	int kiki[36 * 5];				//kiki[i * 5 + j] = マスiから行けるj番目のマスの番号. (なければ-1)
	int INF;						//INF         = 評価値の上限 (下限は-INF以上)
	int maxDepth;					//maxDepth    = 探索の深さの最大値
	
public:

	KanzenSearch()	//ゲームが始まる前の処理
	{
		INF = 100000000;
		int y, x, dir, i, j;
		int dy[4] = {-1, 0, 1, 0};
		int dx[4] = {0, 1, 0, -1};
		
		for (i = 0; i < 180; i++) kiki[i] = -1;
		for (y = 0; y < 6; y++) {
			for (x = 0; x < 6; x++) {
				i = y * 6 + x;
				j = 0;
				for (dir = 0; dir < 4; dir++) {
					int ny = y + dy[dir];
					int nx = x + dx[dir];
					if (0 <= ny && ny < 6 && 0 <= nx && nx < 6) {
						kiki[5 * i + j] = ny * 6 + nx;
						j++;
					}
				}
			}
		}
	}
	
	//board[i] = マスiの状態. ([i / 6]行, i % 6列目）
	//完全情報。相手赤はK個で、他は全部青！みたいな盤面が与えられる。
	pair<MoveCommand, int> think(string board, int maxDepth) {
		KanzenBoard bb;
		
		//探索の設定
		this->maxDepth = maxDepth;
		
		//1手で脱出できるか？
		bb.toBitBoard(board);
		MoveCommand escapeTe = bb.getEscapeCommand(0);
		if (escapeTe.y >= 0) return pair<MoveCommand, int>(escapeTe, INF);
		
		//相手の駒を紫駒とした完全情報探索 (深さ0探索時に最善手を格納）
		int eval = negamax(bb, 0, -INF - 1, INF + 1);
		return pair<MoveCommand, int>(bestMove, eval);
	}
	
private:
	MoveCommand bestMove;	//深さ0（R,Bを動かす手番）における最良手
	
	//探索部分 (自分必勝：INF, 自分必負-INF), 戻り値が(alpha, beta)の範囲を超えたら適当に返す
	int negamax(KanzenBoard bb, int depth, int alpha, int beta) {
		int player = depth % 2;
		int winPlayer = bb.getWinPlayer(player);
		if (winPlayer <= 1) return player == winPlayer ? INF - depth : -INF + depth;
		if (depth == maxDepth) return bb.evaluate(player);
		
		int from[32], to[32];
		int moveNum = bb.makeMoves(player, kiki, from, to);
		
		for (int i = 0; i < moveNum; i++) {
			KanzenBoard nextbb = bb;
			nextbb.move(from[i], to[i]);
			int res = -negamax(nextbb, depth + 1, -beta, -alpha);
			if (alpha < res) { alpha = res; if (depth == 0) { bestMove = MoveCommand::parse(from[i], to[i]); } }
			if (alpha >= beta) { return beta; }	//βcut
		}
		
		return alpha;
	}
};