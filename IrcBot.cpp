#include "Game.h"
#include "ircbot.h"
#include "XSocket.h"

CIrcBot::CIrcBot(void)
: m_pIrcSocket(NULL)
{
}

CIrcBot::~CIrcBot(void)
{
}

void CIrcBot::Startup(char* cServerAddr, short sServerPort, HWND hWnd)
{
	m_pIrcSocket = new class XSocket(hWnd,DEF_XSOCKBLOCKLIMIT);
	if (m_pIrcSocket->bConnect("24.49.159.152",3000,WM_ONBOTSOCKETEVENT) == FALSE)
		MessageBox(hWnd,"IrcBot Failed To Start",0,0);
	m_pIrcSocket->bInitBufferSize(DEF_MSGBUFFERSIZE);
	//for(int i=0;i<10;i++)
	SendTestMessage();
}

void CIrcBot::Shutdown(void)
{
	if (m_pIrcSocket != NULL) delete m_pIrcSocket;
	m_pIrcSocket = NULL;
}

void CIrcBot::SendTestMessage(void)
{
	m_pIrcSocket->iSendMsgText("[KLKS]2Win",10,0);
}
