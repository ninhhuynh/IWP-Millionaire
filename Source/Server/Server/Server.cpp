// Server.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

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
		int i;
		AfxSocketInit(NULL);
			
		server.Create(port);
		server.Listen(5);

		//Nhap so luong client
		printf("\n Nhap so luong Client: ");
		int num_client;
		scanf("%d",&num_client);
		printf("\n Dang lang nghe ket noi tu Client...\n");
		//Tao mang chua cac socket client
		vector<string> nameList;
		const int num = num_client;
		CSocket *sockClients = new CSocket[num_client];
		for (i = 0; i < num_client; i++){
			server.Accept(sockClients[i]);
			printf("Da tiep nhan client %d/%d\n",i+1,num_client);
			nameList.push_back("Player" + to_string(i));
		}
		/*Game game(nameList, sockClients);*/
		Game game(nameList, sockClients);
		// normal start and quit because we havent implement UI yet
		game.StartGame();
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
		server.Close();
	}
	getchar();
	return nRetCode;
}
