#pragma once
#include <string>

namespace Game_
{
	using namespace std;
	
	char board[6][6];			//board[y][x] = {R:自分の赤, B:自分の青, u:相手の駒, '.':空マス, 自分はy=5の側にいる
	char komaName[6][6];		//komaName[y][x] = {受信時に, (y, x)にある駒の名前}
	int rNum, uNum;				//盤面にある敵の赤コマの個数, 敵のコマの個数
	
	const int WON = 1;
	const int LST = 2;
	const int DRW = 3;
	
	//sの先頭がt ⇔ true
	bool startWith(string &s, string t) {
		for (int i = 0; i < t.length(); i++) {
			if (i >= s.length() || s[i] != t[i]) return false;
		}
		return true;
	}

	//ゲームの終了判定. dispFlag = trueにすると, 結果を表示できる。
	int isEnd(string s, bool dispFlag = true) {
		if (startWith(s, "WON")) {
			if (dispFlag) cout << "won" << endl;
			return WON;
		}
		if (startWith(s, "LST")) {
			if (dispFlag) cout << "lost" << endl;
			return LST;
		}
		if (startWith(s, "DRW")) {
			if (dispFlag) cout << "draw" << endl;
			return DRW;
		}
		return 0;
	}

	//終了の原因
	string getEndInfo(string recv_msg) {
		if (startWith(recv_msg, "DRW")) return "draw";
		
		int i, Rnum = 0, Bnum = 0, rnum = 0, bnum = 0;
		const int baius = 4;
		
		for (i = 0; i < 16; i++) {
			int x = recv_msg[baius + 3 * i] - '0';
			int y = recv_msg[baius + 3 * i + 1] - '0';
			char type = recv_msg[baius + 3 * i + 2];
			
			if (0 <= x && x < 6 && 0 <= y && y < 6) {
				if (type == 'R') Rnum++;
				if (type == 'B') Bnum++;
				if (type == 'r') rnum++;
				if (type == 'b') bnum++;
			}
		}
		
		if (startWith(recv_msg, "WON")) {
			if (Rnum == 0) { return "won taken R"; }
			if (bnum == 0) { return "won taked b"; }
			return "won escaped B";
		}
		
		if (rnum == 0) { return "lost taked r"; }
		if (Bnum == 0) { return "lost taken B"; }
		return "lost escaped b";
	}
	
	//ボードの受信
	void recvBoard(string msg) {
		int i, j;
		
		for (i = 0; i < 6; i++) {
			for (j = 0; j < 6; j++) {
				board[i][j] = '.';
				komaName[i][j] = '.';
			}
		}
		
		const int baius = 4;
		rNum = 4;	//敵の赤い駒の個数
		uNum = 0;
		
		for (i = 0; i < 16; i++) {
			int x = msg[baius + 3 * i] - '0';
			int y = msg[baius + 3 * i + 1] - '0';
			char type = msg[baius + 3 * i + 2];
			
			if (0 <= x && x < 6 && 0 <= y && y < 6) {
				if (type == 'R' || type == 'B' || type == 'u') {
					board[y][x] = type;
				}
				if (i < 8) {
					komaName[y][x] = (char)(i + 'A');
				}
				else {
					komaName[y][x] = (char)(i - 8 + 'a');
				}
			}
			else {
				if (type == 'r' && i >= 8) rNum--;
			}
			if (type == 'u') uNum++;
		}
	}
	
	//コマンドの変換
	string move(int y, int x, int dir) {
		string moveStr = "NESW";
		string ret;
		
		ret += "MOV:";
		ret += komaName[y][x];
		ret += ",";
		ret += moveStr[dir];
		return ret;
	}
}