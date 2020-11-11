// Client.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"
#include <string>
#include<vector>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;
using namespace std;
int StringToQuestion(string str);
bool Answer();
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
		printf("\n Nhap dia chi IP cua server: ");
		//gets_s(sAdd);
		if (client.Connect(CA2W("127.0.0.1"), port))
		{
			printf("\n Client da ket noi toi server\n");
			//Nhan tu server, cho biet day la client thu may

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

		}
		else
			printf("Khong connect duoc toi server....");
		
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
	while ((pos = str.find(delimiter)) != std::string::npos) {
		cout <<prefix << str.substr(0, pos) << endl;//cout question and answer here
		str.erase(0, pos + delimiter.length());
		prefix = to_string(n++)+": ";
	}
	cout << "please choose one!(5 for help, skip turn can be used only once): ";// change the global var(answer) to answer, 1-5
	while (1) {
		if (Answer()) {
			client.Send((char*)answer, 1,0);
			answer = 0;
			break;
		}
		string s = Recieve();
		if (s == ".") {
			cout << "you have ran out of time\n";
			break;
		}
		else cout << s << endl;
	}
	clientstate = 0;
}
bool Answer() {
	if(answer>0)
		return true;
	return false;
}
