#pragma once

class CIrcBot
{
public:
	CIrcBot(void);
	~CIrcBot(void);
	void Startup(char* cServerAddr, short sServerPort, HWND hWnd);
	class XSocket*m_pIrcSocket;
	void Shutdown(void);
	void SendTestMessage(void);
};
