// Client.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"
#include <string>
#include <string.h>
#include <math.h>
#include<vector>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;
using namespace std;

int clientstate = 0;
int answer = 0;
CSocket client;
string Recieve() {
	uint32_t length, nlength;
	int length_bytes = 0;
	while (length_bytes < 4) {
		int read = client.Receive((char*)&nlength + length_bytes, 4 - length_bytes, 0);
		if (read == -1) {
			cout << "err" << endl;
			return "";
		}
		length_bytes += read;
	}
	length = ntohl(nlength);
	string data;
	char buf;
	while (data.length() < length) {
		client.Receive(&buf, 1, 0);
		data.push_back(buf);
	}
	return data;
}
void checkNameValid(string& client_name);
bool isLetterString(string input);
int StringToQuestion(string str);
bool Answer();
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
		
		// TODO: code your application's behavior here.s
		char sAdd[1000];
		unsigned int port = 1234; //Cung port voi server
		AfxSocketInit(NULL);

		//1. Tao socket
		client.Create();

		// Nhap dic chi IP cua server
		cout << "Nhap dia chi IP cua server: ";
		strcpy_s(sAdd, "127.0.0.1");
		if (client.Connect(CA2W(sAdd), port))
		{
			cout << "Da ket noi toi server" << endl;
			//Nhan tu server, cho biet day la client thu may
			int id;
			int num_client; // so nguoi choi
			string client_name; // ten cua client
			client.Receive((char*)&id, sizeof(id), 0);
			cout << "Day la client thu " << id + 1 << endl;
			//Kiem tra ten tu server
			while (true) {
				cout << "Nhap ten client: ";
				checkNameValid(client_name);
				//Convert string name to char
				char char_client_name[100];
				strcpy_s(char_client_name, client_name.c_str());
				client.Send(char_client_name, sizeof(char_client_name), 0);
				//Nhan msgNotif tu server de check ten ton tai
				int msgNotif;
				client.Receive((char*)&msgNotif, sizeof(msgNotif), 0);
				if (msgNotif == 1) {
					//Ten chua ton tai => hop le
					cout << "Ten dang nhap hop le. Dang ky thanh cong" << endl;
					break;
				}
				else {
					//Ten da ton tai => nhap lai
					cout << "Ten dang nhap da ton tai, vui long nhap lai " << endl;
					continue;
				}
			}
			//Receive play order and num_of_ques
			int play_order;
			client.Receive((char*)&play_order, sizeof(play_order), 0);
			cout << "Thu tu choi cua ban la " << play_order << endl;
			int num_of_ques;
			client.Receive((char*)&num_of_ques, sizeof(num_of_ques), 0);
			cout << "So cau hoi la " << num_of_ques << endl;
			client.Receive((char*)&num_client, sizeof(num_client), 0);
			cout << "Tong so nguoi choi la: " << num_client << endl;
			//------------------------------
			//Receive questions from server

			//Game Start
			cout << Recieve() << endl;
			while (1) {
				string tmp = Recieve();
				if (tmp.at(0) == '*') {
					tmp.erase(tmp.begin());
					StringToQuestion(tmp);
				}
				else cout << tmp << endl;
				if (tmp == "-1") {
					cout << "the host has quit" << endl;
					return 1;
				}
				if (tmp == "-2") {
					cout << "the host has signal to start another round";
					//game start 
					cout << Recieve() << endl;
				}
			}
			client.Close();
		}
		else
			cout << "Fail to connect to server" << endl;
		
	}
	getchar();
	return nRetCode;
}
int StringToQuestion(string str) {
	string delimiter = "`";
	size_t pos = 0;
	string token;// hold question and answer
	string prefix = "Q: ";
	int n = 1;
	int returnval = 0;

	while ((pos = str.find(delimiter)) != std::string::npos) {
		cout <<prefix << str.substr(0, pos) << endl;//cout question and answer here
		str.erase(0, pos + delimiter.length());
		prefix = to_string(n++)+": ";
	}
	cout << prefix << str << endl;


	cout << "please choose one!(5 for help, skip turn can be used only once): ";// change the global var(answer) to answer, 1-5
	while (1) {
		if (Answer()) {
			client.Send((char*)answer, 1,0);
			answer = 0;
			returnval = 1;
			break;
		}
		string s = Recieve();
		if (s == ".") {
			cout << "you have ran out of time\n";
			returnval = 0;
			break;
		}
		else cout << s << endl;
	}
	clientstate = 0;
	return returnval;
}
bool Answer() {
	if(answer>0)
		return true;
	return false;
}
bool isLetterString(string input) {
	for (int i = 0; i < input.size(); i++) {
		int lowercaseChar = tolower(input[i]);
		if ((lowercaseChar < 48) || (lowercaseChar > 57 && lowercaseChar < 95) || (lowercaseChar > 95
			&& lowercaseChar < 97) || (lowercaseChar > 122)) {
			return false;
		}
	}
	return true;
}
void checkNameValid(string& client_name) {
	bool check_name = false;
	do {
		getline(cin, client_name);
		if (client_name.length() < 1 || client_name.length() > 10) {
			cout << "Length must be larger than 1 and smaller than 10 " << endl;
			cout << "Please re-enter name" << endl;
		}
		else if (!isLetterString(client_name)) {
			cout << "Name must be composed of a-z, A-Z, 0-9 and _" << endl;
			cout << "Please re-enter name" << endl;
		}
		else check_name = true;
	} while (check_name == false);
}
