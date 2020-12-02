#pragma once
#include "stdafx.h"
using namespace std;

class Player {
	int state=0; //0 is dead, 1 is waiting for turn, 2 is their turn
	bool help = true;
public:
	Player(string str);
	string name="";
	bool GetHelp() {
		return help;
	}
	void ChangeState(int newstate);
	int GetState();
	bool UseHelp();
};

class Game {
	vector<Player> Players;
	vector<string> questionList;
	vector<string> answerList;
	u_int questionnum = 0;
	int countdown = 10;//seconds
	u_int matchnum = 0;
public:
	CSocket* sockets;
	Game(vector<string>& nameList, CSocket* socks) {
		sockets = socks;

		for (u_int i = 0; i < nameList.size(); i++)
			Players.push_back(Player(nameList[i]));
	}
	bool StartGame(vector<string> questions);
	void Flow(); //Should be use when the game is started
	int GetAlivePlayersNum();
	bool Countdown(int n,int socketindex);
	void GetQuestion(vector<string> questions);
	bool Answered(int socketindex,string &s);
	void SendStringToAll(string str);
	void SendStringToOne(string str, CSocket& sock);
};


