#pragma once
#pragma comment(lib, "wsock32.lib")
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

namespace tcp {
	void mySend(int dstSocket, std::string str = "")
	{
		if (str.length() == 0) {	//null文字なら「入力」を受け付ける
			std::cin >> str;
		}
		if (str.length() < 2 || str[str.length() - 2] != '\r' || str[str.length() - 1] != '\n') {	//\r\nが末尾になければ追加
			str += '\r';
			str += '\n';
		}
		int byte = send(dstSocket, str.c_str(), str.length(), 0);	//文字列を送信
		if (byte <= 0) {
			std::cout << "送信エラー" << std::endl;
		}
	}

	std::string myRecv(int dstSocket)
	{
		char buffer[10];
		std::string msg;
		
		do {
			int byte = recv(dstSocket, buffer, 1, 0);	//文字を受信
			
			if (byte == 0) break;
			if (byte < 0) { std::cout << "受信に失敗しました" << std::endl; return msg; }
			
			msg += buffer[0];
		} while (msg.length() < 2 || msg[msg.length() - 2] != '\r' || msg[msg.length() - 1] != '\n');
		
		std::cout << "受信 = " << msg << std::endl;
		
		return msg;
	}

	bool openPort(int &dstSocket, int port = -1, std::string dest = "")
	{
		// IP アドレス，ポート番号，ソケット，sockaddr_in 構造体
		char destination[32];
		struct sockaddr_in dstAddr;
		int PORT;
		
		// Windows の場合
		WSADATA data;
		WSAStartup(MAKEWORD(2,0), &data);

		// 相手先アドレスの入力と送る文字の入力
		if (port == -1) {
			printf("ポート番号は？：");
			scanf("%d", &PORT);
		} else {
			PORT = port;
		}
		
		if (dest.length() == 0) {
			printf("サーバーマシンのIPは？:");
			scanf("%s", destination);
		} else {
			for (int i = 0; i < dest.size(); i++) destination[i] = dest[i];
			destination[dest.size()] = '\0';
		}
		
		// sockaddr_in 構造体のセット
		memset(&dstAddr, 0, sizeof(dstAddr));
		dstAddr.sin_port = htons(PORT);
		dstAddr.sin_family = AF_INET;
		dstAddr.sin_addr.s_addr = inet_addr(destination);

		// ソケットの生成
		dstSocket = socket(AF_INET, SOCK_STREAM, 0);

		//接続
		if(connect(dstSocket, (struct sockaddr *) &dstAddr, sizeof(dstAddr))){
			printf("%s　に接続できませんでした\n",destination);
			return false;
		}
	  	printf("%s に接続しました\n", destination);

		return true;
	}

	void closePort(int &dstSocket)
	{
		// Windows でのソケットの終了
		closesocket(dstSocket);
		WSACleanup();
	}
}