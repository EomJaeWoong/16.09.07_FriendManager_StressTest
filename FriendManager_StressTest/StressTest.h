#ifndef __STRESSTEST__H__
#define __STRESSTEST__H__

struct stClient
{
	SOCKET sock;

	CAyaStreamSQ RecvQ;
	CAyaStreamSQ SendQ;

	int iReqPacket;
	int iResPacket;

	DWORD dwReqTick;
	int iPacketcount;

	char *pEchoString;
};

#define MAX_CLIENT 5

BOOL InitNetwork();
BOOL Connection();

void SendPacket();

#endif