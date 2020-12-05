// Server.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"
#include "math.h"
#include <vector>
#include <string.h>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;
vector<string> GetQuestions();
void checkNumClient(int& num_client);
bool checkExistedName(char char_client_name[][100], char name[100], int num_client);

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		CSocket server, s;
		unsigned int port = 1234;
		AfxSocketInit(NULL);
			
		server.Create(port);
		server.Listen(5);
		//-------------------------------------------------
		//Read questions from database
		vector<string> questionList;
		ifstream file("question.txt");
		string fileContents;
		while (getline(file, fileContents)) {
			questionList.push_back(fileContents);
		}
		//shuffle the whole question list and get random number of questions
		random_shuffle(questionList.begin(), questionList.end());
		srand((unsigned)time(0));
		
		//-----------------------------------------------
		//Tao mang chua cac socket client
		vector<string> nameList;
		// initialize
		int num_client = 0;
		char char_client_name[11][100];
		// Kiem tra so luong client
		checkNumClient(num_client);
		int num_of_ques = rand() % questionList.size() + num_client*3;
		cout << "So cau hoi ngau nhien cho client la " << num_of_ques << endl;
		cout << "Dang lang nghe ket noi tu client..." << endl;
		//Tao mang chua cac socket client
		CSocket* sockClients = new CSocket[num_client];
		//Tiep nhan va bao cho client biet minh la client thu may
		for (int i = 0; i < num_client; i++) {
			server.Accept(sockClients[i]);
			cout << "Da tiep nhan client " << i + 1 << "/" << num_client << endl;
			sockClients[i].Send((char*)&i, sizeof(int), 0);
		}
		//Nhan ten client va kiem tra trung ten
		for (int i = 0; i < num_client; i++) {
			while (true) {
				char tempName[100];
				int msgNotif;
				sockClients[i].Receive(tempName, sizeof(tempName), 0);
				cout << "Ten da nhan tu client " << i + 1 << ": " << tempName << endl;
				if (!checkExistedName(char_client_name, tempName, num_client)) {
					cout << "Ten khong hop le " << endl;
					msgNotif = 0;
					sockClients[i].Send((char*)&msgNotif, sizeof(msgNotif), 0);
				}
				else {
					strcpy_s(char_client_name[i], tempName);
					msgNotif = 1;
					sockClients[i].Send((char*)&msgNotif, sizeof(msgNotif), 0);
					break;
				}
			}
		}
		//Xac nhan ten, gui thu tu choi va so cau hoi
		int play_order[11]; //mang thu tu choi
		for (int item = 0; item < num_client; item++) {
			play_order[item] = item + 1;
			cout << "Thu tu ne: " << play_order[item] << endl;
		}
		random_shuffle(play_order, play_order + num_client); //ngau nhien thu tu choi
		for (int i = 0; i < num_client; i++) {
			//check success
			cout << "Ten cua client thu " << i + 1 << ": " << char_client_name[i] << endl;
			nameList.push_back(char_client_name[i]);
			//Gui thu tu choi cho clients
			cout << "Thu tu choi cua client thu "<< i+1 << "la: "<< play_order[i] << endl;
			sockClients[i].Send((char*)&play_order[i], sizeof(play_order[i]), 0);
			//gui so cau hoi cho clients
			cout << "So cau hoi la: " << num_of_ques << endl;
			sockClients[i].Send((char*)&num_of_ques, sizeof(num_of_ques), 0);
			sockClients[i].Send((char*)&num_client, sizeof(num_client), 0);
		}

		//----------------------------------------------------
		//Set questions randomly to clients 
		// -> cu loop qua questionList roi gui so cau hoi bang voi num_of_ques la random nha 
		/*for (int i = 0; i < num_client; i++) {
			for (int j = 0; j < num_of_ques; j++) {
				char ques[1000];
				strcpy_s(ques, questionList[j].c_str());
				sockClients[i].Send(ques, sizeof(ques), 0);
			}
		}*/
		//------------------------------------------------------- toi day la phan code cu cua Ninh
		//nameList.push_back("Player" + to_string(i));
		
		/*Game game(nameList, sockClients);*/
		Game game(nameList, sockClients);
		
		// normal start and quit because we havent implement UI yet
		bool restart = game.StartGame(questionList);
		while (restart) {
			restart = game.StartGame(GetQuestions());
		}
		game.SendStringToAll("-1");  

		// template lobby UI
		//while (1) {
		//	if (OnClick(NewGame)) {
		//		game.SendStringToAll("-2");
		//		game.StartGame();
		//	}
		//	if (OnClick(Quit)) {
		//		game.SendStringToAll("-1");
		//		break;
		//	}
		//}
		
		//Game newGame(nameList,sockClients);
		for (int i = 0; i < num_client; i++) {
			sockClients[i].Close();
		}
		//phan nay Trung free memory
		cout << "Ket thuc lam viec" << endl;
		delete[] sockClients;
		server.Close();
	}
	getchar();
	return nRetCode;
}
void checkNumClient(int& num_client) {
	bool check_client = false;
	//Nhap so luong client
	do {
		cout << "Nhap so luong client: ";
		if (cin >> num_client) {
			if (num_client < 2 || num_client > 10) {
				check_client = false;
				cout << "Number of clients must be from 2 to 10. Please enter again" << endl;
			}
			else {
				cout << num_client << endl;
				check_client = true;
			}
		}
		else {
			cin.clear(); // clears the error flags
			// this line discards all the input waiting in the stream
			cin.ignore(0xff,'\n');
		}
	} while (check_client == false);
}
bool checkExistedName(char char_client_name[][100], char name[100], int num_client) {
	for (int i = 0; i < num_client; i++) {
		if (strcmp(name, char_client_name[i]) == 0) {
			return false;
		}
	}
	return true;
}

vector<string> GetQuestions() {
	vector<string> questionList;
	ifstream file("question.txt");
	string fileContents;
	while (getline(file, fileContents)) {
		questionList.push_back(fileContents);
	}
	//shuffle the whole question list and get random number of questions
	random_shuffle(questionList.begin(), questionList.end());
	srand((unsigned)time(0));
	int num_of_ques = (rand() % questionList.size());
	cout << "So cau hoi ngau nhien cho client la " << num_of_ques << endl;
	vector<string>::const_iterator first = questionList.begin();
	vector<string>::const_iterator last = questionList.begin() + num_of_ques;
	vector<string> newVec(first, last);
	return newVec;
}