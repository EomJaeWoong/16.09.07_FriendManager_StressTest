#include <WinSock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <Ws2tcpip.h>
#include <list>

#include "Protocol.h"
#include "StreamQueue.h"
#include "NPacket.h"
#include "StressTest.h"


#pragma comment(lib, "Ws2_32.lib")

stClient *pClient[MAX_CLIENT];
list<FD_SET *>		Readlist;
list<FD_SET *>		Writelist;

void main()
{
	if (!InitNetwork())		return;
	if (!Connection())		return;

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

/*
일단 패킷 보내고
받은거 검사후 맞으면 또 보냄
아니면 끊어야됨
*/
void SendPacket()
{
	int retval;
	int iSockCount;

	FD_SET *ReadSet;
	FD_SET *WriteSet;

	while (1){
		iSockCount = 0;

		ReadSet = new FD_SET;
		WriteSet = new FD_SET;

		Readlist.push_back(ReadSet);
		Writelist.push_back(WriteSet);

		FD_ZERO(ReadSet);
		FD_ZERO(WriteSet);

		iSockCount++;

		for (int iCnt = 0; iCnt < MAX_CLIENT; iCnt++)
		{
			if (iSockCount > 64)
			{
				ReadSet = new FD_SET;
				WriteSet = new FD_SET;

				Readlist.push_back(ReadSet);
				Writelist.push_back(WriteSet);

				FD_ZERO(ReadSet);
				FD_ZERO(WriteSet);

				iSockCount = 0;
			}

			if (pClient[iCnt]->SendQ.GetUseSize() > 0)
				FD_SET(pClient[iCnt]->sock, WriteSet);

			FD_SET(pClient[iCnt]->sock, ReadSet);

			iSockCount++;
		}

		TIMEVAL Time;
		Time.tv_sec = 0;
		Time.tv_usec = 0;

		//-----------------------------------------------------------------------------------
		// Select
		//-----------------------------------------------------------------------------------
		list<FD_SET *>::iterator ReadIter = Readlist.begin();
		list<FD_SET *>::iterator WriteIter = Writelist.begin();

		for (int iCnt = 0; iCnt < Readlist.size() || iCnt < Writelist.size(); iCnt++){
			retval = select(0, (*ReadIter), (*WriteIter), NULL, &Time);

			if (retval == 0)		break;

			else if (retval < 0)
			{
				DWORD dwError = GetLastError();
				wprintf(L"Select() Error : %d\n", dwError);
				exit(1);
			}

			else
			{
				//-------------------------------------------------------------------------------
				// Socket 처리
				//-------------------------------------------------------------------------------
				SocketProc((*ReadIter), (*WriteIter));
			}
		}

		for (ReadIter = Readlist.begin(); ReadIter != Readlist.end(); ++ReadIter)
			delete (*ReadIter);

		for (WriteIter = Writelist.begin(); WriteIter != Writelist.end(); ++WriteIter)
			delete (*WriteIter);

		Readlist.clear();
		Writelist.clear();
	}
}

void SocketProc(FD_SET *ReadIter, FD_SET *WriteIter)
{

}

BOOL packetProc_ResStressEcho(stClient *pClient, CNPacket *cPacket)
{

}

BOOL sendProc_ReqStressEcho(stClient *pClient, WORD wSize, WCHAR *wText)
{

}

void makePacket_ReqStressEcho(st_PACKET_HEADER *header, CNPacket *cPacket,
	WORD wSize, WCHAR *wText)
{

}

void makeEchoString()
{

}