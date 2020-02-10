
// TestClient.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "TestClient.h"
#include "TestClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestClientApp

BEGIN_MESSAGE_MAP(CTestClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestClientApp 생성

CTestClientApp::CTestClientApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CTestClientApp 개체입니다.

CTestClientApp theApp;


// CTestClientApp 초기화

BOOL CTestClientApp::InitInstance()
{
	CWinApp::InitInstance();
	
	WSADATA temp;
	WSAStartup(0x0202, &temp);
	
	CTestClientDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	// 더이상 소켓 시스템을 사용하지 않을 때 호출함.
	WSACleanup();

	return FALSE;
}

