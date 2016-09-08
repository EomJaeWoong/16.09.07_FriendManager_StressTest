#include <WinSock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <Ws2tcpip.h>

#include "Protocol.h"
#include "StreamQueue.h"
#include "NPacket.h"
#include "StressTest.h"

#pragma comment(lib, "Ws2_32.lib")

stClient *pClient[MAX_CLIENT];

void main()
{
	if (!InitNetwork())
		return;

	if (!Connection())
		return;

	SendPacket();
}

BOOL InitNetwork()
{
	int retval;

	//-----------------------------------------------------------------------------------
	// Winsock 초기화
	//-----------------------------------------------------------------------------------
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		wprintf(L"Winsock() Error\n");
		return FALSE;
	}

	//-----------------------------------------------------------------------------------
	// socket()
	//----------------------------------------------------------------------------------
	for (int iCnt = 0; iCnt < MAX_CLIENT; iCnt++)
	{
		pClient[iCnt] = new stClient;
		pClient[iCnt]->sock = socket(AF_INET, SOCK_STREAM, 0);
		if (pClient[iCnt]->sock == INVALID_SOCKET)
		{
			wprintf(L"Socket() Error\n");
			return FALSE;
		}

		pClient[iCnt]->iReqPacket = 0;
		pClient[iCnt]->iResPacket = 0;
		pClient[iCnt]->iPacketcount = 0;
	}

	return TRUE;
}

BOOL Connection()
{
	int retval;

	SOCKADDR_IN sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(dfNETWORK_PORT);
	InetPton(AF_INET, L"127.0.0.1", &sockaddr.sin_addr);

	for (int iCnt = 0; iCnt < MAX_CLIENT; iCnt++)
	{
		retval = connect(pClient[iCnt]->sock, (SOCKADDR *)&sockaddr, sizeof(SOCKADDR_IN));
		if (retval == SOCKET_ERROR)
		{
			wprintf(L"connect() ERROR\n");
			return FALSE;
		}
	}

	return TRUE;
}

void SendPacket()
{
	while (1){
		FD_SET ReadSet, WriteSet;

		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);

		//-----------------------------------------------------------------------------------
		// Socket을 ReadSet, WriteSet에 등록
		//-----------------------------------------------------------------------------------
		for (aIter = g_lAccount.begin(); aIter != g_lAccount.end(); ++aIter)
		{
			if ((*aIter)->sock != INVALID_SOCKET){
				if ((*aIter)->SendQ.GetUseSize() > 0)
					FD_SET((*aIter)->sock, &WriteSet);

				FD_SET((*aIter)->sock, &ReadSet);
			}
		}

		TIMEVAL Time;
		Time.tv_sec = 0;
		Time.tv_usec = 0;

		//-----------------------------------------------------------------------------------
		// Select
		//-----------------------------------------------------------------------------------
		retval = select(0, &ReadSet, &WriteSet, NULL, &Time);

		if (retval == 0)		return;

		else if (retval < 0)
		{
			DWORD dwError = GetLastError();
			wprintf(L"Select() Error : %d\n", dwError);
			exit(1);
		}

		else
		{
			//-------------------------------------------------------------------------------
			// Accept 처리
			//-------------------------------------------------------------------------------
			if (FD_ISSET(listen_sock, &ReadSet))
				AcceptClient();
			//-------------------------------------------------------------------------------
			// Socket 처리
			//-------------------------------------------------------------------------------
			else
				SocketProc(ReadSet, WriteSet);
		}
	}
}