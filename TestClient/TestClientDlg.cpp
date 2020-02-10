
// TestClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "TestClient.h"
#include "TestClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestClientDlg 대화 상자



CTestClientDlg::CTestClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_event_list);
}

BEGIN_MESSAGE_MAP(CTestClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND_BTN, &CTestClientDlg::OnBnClickedSendBtn)
	ON_BN_CLICKED(IDOK, &CTestClientDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CTestClientDlg::AddEventString(CString param_string)
/* 프로그램에 있는 리스트 박스에 어떤 문자열을 출력하고 싶을 때
   호출하는 함수 AddEventString */
{
	/* InsertString의 반환값은 현재 이 문자열이 리스트 박스에
	   추가된 위치가 반환되어 index에 저장된다. */ 
	int index = m_event_list.InsertString(-1, param_string);
	m_event_list.SetCurSel(index);
}

// CTestClientDlg 메시지 처리기

BOOL CTestClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// AF_INET : TCP와 관련된 일반적인 주소체계
	// SOCK_STREAM : TCP를 의미함. 
	mh_socket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in srv_addr;

	srv_addr.sin_family = AF_INET;
	/* 127.0.0.1 은 해당 pc에서만 접속이 가능하고
	   다른 pc에서는 접속이 불가하다. */
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	srv_addr.sin_port = htons(20001);
	// 해당 서버의 고유 번호가 20001번에 접속함을 의미함.


	WSAAsyncSelect(mh_socket, m_hWnd, 27001, FD_CONNECT);
	connect(mh_socket, (sockaddr *)&srv_addr, sizeof(srv_addr));
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTestClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestClientDlg::ConnectProcess(LPARAM lParam)
{
		// 값이 0일 경우 에러가 없음을 의미함.
		// 서버 접속에 성공함을 의미
		if (WSAGETSELECTERROR(lParam) == 0)
		{
			WSAAsyncSelect(mh_socket, m_hWnd, 27002, FD_READ | FD_CLOSE);
			AddEventString(L"서버에 접속하였습니다!");
		}
		else // 서버 접속에 실패할 경우
		{
			DestroySocket();
			AddEventString(L"서버에 접속할 수 없습니다!");
		}

}

void CTestClientDlg::DestroySocket()
{
	/* 비정상 종료를 하는 소켓의 경우 LINGER 옵션을 하나
				   걸어서 타임아웃(대기시간)을 0초로 하여 즉각적으로 끊는다.*/
	LINGER temp_linger = { TRUE, 0 };
	setsockopt(mh_socket, SOL_SOCKET, SO_LINGER, (char*)&temp_linger, sizeof(temp_linger));
	// 소켓을 닫는다.
	closesocket(mh_socket);
	/* 소켓을 닫는다고 해서 mh_socket에 초기화된 값이 들어가지 않으므로
	   소켓이 확실히 닫아졌다는 것을 인지시키기 위해서
	   INVALID_SOCKET이라는 값을 넣어준다. */
	mh_socket = INVALID_SOCKET;
}

void CTestClientDlg::ReceiveData(char * param_p_buffer, int param_size)
{
	int current_size, total_size = 0, retry_count = 0;

	while (total_size < param_size)
	{
		current_size = recv(mh_socket, param_p_buffer + total_size, param_size - total_size, 0);

		if (current_size == SOCKET_ERROR)
		{
			retry_count++;
			Sleep(50); // 0.05초
			if (retry_count > 5) break;
		}
		else
		{
			retry_count = 0;
			total_size = total_size + current_size;
		}
	}
}

void CTestClientDlg::ReadFrameData()
{
	char key, message_id;
	recv(mh_socket, &key, 1, 0);
	if (key == 27)
	{
		unsigned short int body_size;
		recv(mh_socket, (char *)&body_size, 2, 0);
		recv(mh_socket, &message_id, 1, 0);

		if (body_size > 0)
		{
			char * p_body_data = new char[body_size];

			ReceiveData(p_body_data, body_size);

			if (message_id == 1)
			{
				AddEventString((wchar_t *)p_body_data);
			}

			delete[] p_body_data;
		}
	}
	else
	{
		DestroySocket();
		AddEventString(L"잘못된 프로토콜입니다.");
	}
}


LRESULT CTestClientDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == 27001) // FD_CONNECT
	{
		ConnectProcess(lParam);
	}
	else if (message == 27002) // FD_READ or FD_CLOSE
	{
		if (WSAGETSELECTEVENT(lParam) == FD_READ)
		{
			WSAAsyncSelect(mh_socket, m_hWnd, 27002, FD_CLOSE);
			
			ReadFrameData();

			if (mh_socket != INVALID_SOCKET)
				WSAAsyncSelect(mh_socket, m_hWnd, 27002, FD_CLOSE | FD_READ);
		}
		else // FD_CLOSE
		{
			DestroySocket();
			AddEventString(L"서버에서 연결을 해제하였습니다!");
		}
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}

void CTestClientDlg::SendFrameData(SOCKET param_h_socket, unsigned char param_id, const void* param_p_data, int param_size)
/* param_h_socket 소켓으로부터 const void * param_p_data가 가리키는 변수에 저장된 데이터를 
   param_size 크기만큼 보내는 함수 SendFrameData의 정의 */
{
	char * p_send_data = new char[param_size + 4];

	*p_send_data = 27; // 첫 번째 바이트에 27이 저장됨.

	// 2 바이트 크기만큼 넣어야 하므로 (unsigned short int *) 로 형변환 해야함.
	*(unsigned short int *)(p_send_data + 1) = param_size;

	*(p_send_data + 3) = param_id;

	memcpy(p_send_data + 4, param_p_data, param_size);

	send(param_h_socket, p_send_data, param_size + 4, 0);

	delete[] p_send_data;
}

void CTestClientDlg::OnBnClickedSendBtn()
{
	/* 서버에 정상적으로 접속된 경우 */
	if (mh_socket != INVALID_SOCKET)
	{
		CString str;

		// IDC_EDIT1에 입력한 문자열을 str 객체로 가져옴.
		GetDlgItemText(IDC_EDIT1, str);

		SendFrameData(mh_socket, 1, (const wchar_t*)str, (str.GetLength() + 1) * 2);
	}
	else // 서버에 접속되어 있지 않은 경우
	{
		AddEventString(L"서버에 접속된 상태가 아닙니다.");
	}

}


void CTestClientDlg::OnBnClickedOk()
{
	/* 엔터를 치면 Send 버튼을 누른것처럼 채팅 내용이 서버로 전송된다.*/
	OnBnClickedSendBtn();

	/* 내가 원하는 다이얼로그에 있는 컨트롤로 이동하기 위해서
	   GotoDlgCtrl 함수를 호출함. 
	   해당 컨트롤의 윈도우 포인터를 인자값으로 넘겨줘야함. 
	   GetDlgItem 함수를 호출하면 윈도우 포인터 값을 알 수 있다. */
	GotoDlgCtrl(GetDlgItem(IDC_EDIT1));

	/* Onok이라는 함수가 호출되면 대화상자가 닫힌다. 
	   그래서 OnOK이라는 함수를 호출하지 못하도록
	   주석 처리를 하면 확인 버튼을 눌러도 대화상자가 닫히지 않는다. */
	// CDialogEx::OnOK();
}
