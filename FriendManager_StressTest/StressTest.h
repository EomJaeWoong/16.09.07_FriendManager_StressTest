#ifndef __STRESSTEST__H__
#define __STRESSTEST__H__

using namespace std;

struct stClient
{
	SOCKET sock;

	CAyaStreamSQ RecvQ;
	CAyaStreamSQ SendQ;

	int iReqPacket;			//���´���
	int iResPacket;			//�޾ƾ��Ұ�

	DWORD dwReqTick;		//��û�ð�
	int iPacketcount;		//?

	char *pEchoString;		//���� ���ڿ�
};

#define MAX_CLIENT 500
#define ECHO_SIZE 1024

BOOL InitNetwork();
BOOL Connection();

void SendPacket();
void CheckStress();
void SocketProc(FD_SET *ReadIter, FD_SET *WriteIter);

BOOL packetProc_ResStressEcho(stClient *pClient, CNPacket *cPacket);
BOOL sendProc_ReqStressEcho(stClient *pClient, WORD wSize, WCHAR *wText);
void makePacket_ReqStressEcho(st_PACKET_HEADER *header, CNPacket *cPacket,
	WORD wSize, WCHAR *wText);

#endif