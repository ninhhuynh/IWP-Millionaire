#include "stdafx.h"
bool Game::StartGame(vector<string> questions) {
	questionList.clear();
	answerList.clear();
	questionnum = 0;
	GetQuestion(questions);
	Flow();
	bool restart=0;
	cout << "do you want to restart(input 1 to restart)" << endl;
	cin >> restart;
	return restart;
}
void Game::SendStringToAll(string str) {
	for (u_int i = 0; i < Players.size(); i++) {
		SendStringToOne(str, sockets[i]);
		// check error if needed
	}
}
void Game::SendStringToOne(string str, CSocket &sock) {
	size_t tmp = str.length();
	uint32_t nlength = htonl(tmp);
	sock.Send(&nlength, 4, 0);
	sock.Send(str.data(), tmp, 0);
}
void Game::Flow() {
	SendStringToAll("The Game Has Started");
	while (questionnum  < questionList.size()) {
		cout << "Alive players num: " << GetAlivePlayersNum() << endl;
		//send question and answers to all sockets here

		//Turn based flow here
		// starting from first player, if player is dead, skip them, or wait for their answer
		for (u_int i = 0; i < Players.size(); i++) {
			if (Players[i].GetState() == 0) {
				continue;
			}
			if (GetAlivePlayersNum() == 1) {
				goto EndGame;
			}
			cout << "Turn: " << Players[i].name << endl;
			SendStringToAll("turn: " + Players[i].name);
			//send signal and question to i'th client to make them be able to answer
			SendStringToOne("*"+questionList[questionnum], sockets[i]);
			// listen then wait for answer in coundown
			if (!Countdown(countdown, i)) { //if no answer then deaD
				//rule 4.c.i, a.i
				Players[i].ChangeState(0);
				cout << Players[i].name << " has failed\n";
				SendStringToAll(Players[i].name + "has failed");
			}

		}
		questionnum++;
	}
	// rule 4.a.i
	EndGame:
	string winner;
	for (u_int i = 0; i < Players.size(); i++) {
		if (Players[i].GetState() != 0) {
			winner += Players[i].name;
			winner += " ";
		}
	}
	cout << winner << "Is our winner\n";
	SendStringToAll(winner + "Is our winner");
	cout << "The Game has ended, The host can now start another game by pressing start button\n";
}
void Game::GetQuestion(vector<string> questions) 
{
	string delimiter = "=";
	for (string s : questions) {
		int n = 0;
		size_t pos = 0;
		string token;
		while ((pos = s.find(delimiter)) != string::npos) {
			n++;
			token = s.substr(0, pos);
			questionList.push_back(token);
			cout << "appending: " << token << " to questionList" << endl;
			s.erase(0, pos + delimiter.length());
		}
		cout << "appending: " << s << " to answerList" << endl;
		answerList.push_back(s);
		if (n > 1) {
			cout << "err Getting Questions" << endl;
		}
	}
	/*srand((unsigned)time(0));
	int num_of_ques = (rand() % questionList.size()) + 1;
	cout << "So cau hoi ngau nhien cho client la " << num_of_ques << endl;
	questionList.push_back("ques1`one`two`three`four");
	answerList.push_back("1");
	questionList.push_back("ques2`one`two`three`four");
	answerList.push_back("3");
	questionList.push_back("ques3`one`two`three`four");
	answerList.push_back("4");*/
}
int Game::GetAlivePlayersNum()
{	
	int tmp=0;
	for (u_int i = 0; i < Players.size(); i++) {
		tmp += Players[i].GetState();
	}
	return tmp;
}
bool Game::Countdown(int counter,int socketindex) { //amount of seconds
	Sleep(1000);
	string s;
	while (counter >= 1)
	{
		if (Answered(socketindex,s)) {//if answered
			cout << "The player answered: " << s << ", checking database..." << endl;
			if (s == "5") {
				//5 is the code for HELP
				if (!(Players[socketindex].UseHelp())) {
					return false;
				}
				return true;
			}
			if (s != answerList[questionnum]) {
				return false;
			}
			return true;
		}
		cout << "\rTime remaining: " << counter << endl;
		string a = to_string(counter);
		SendStringToAll(a);
		Sleep(1000);
		counter--;
	}
	SendStringToAll(".");
	return false; // false mean the player failled to answer
}

bool Game::Answered(int socketindex, string &s) {
	u_long n = 0;
	sockets[socketindex].IOCtl(FIONREAD, &n);
	if (n < 1) {
		return false;
	}
	char buf[1];
	while ((n = sockets[socketindex].Receive(buf, sizeof(buf),0)) > 0) {
		s.append(buf);
	}
	if (n < 0) {
		cout << "some error";
	}
	return true;
}

Player::Player(string str) {
	name = str;
	state = 1;
}
int Player::GetState() {
	return state;
}
void Player::ChangeState(int n) {
	state = n;
}
bool Player::UseHelp() {
	if (help) {
		help = !help;
		return true;
	}
	return false;
}
