
// LogMakerDlg.cpp: 구현 파일
//
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "framework.h"
#include "LogMaker.h"
#include "LogMakerDlg.h"
#include "afxdialogex.h"
#include "timeapi.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "postgresql_sink.h"


#include <string>
#include <cstring>
#include <cmath>
#include <pqxx/pqxx>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned int g_nSendCnt = 0;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLogMakerDlg 대화 상자

CLogMakerDlg::CLogMakerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGMAKER_DIALOG, pParent)
	, strLevel1(_T(""))
	, strLevel2(_T(""))
	, strLevel3(_T(""))
	, strLevel4(_T(""))
	, strLevel5(_T(""))
	, strLevel6(_T(""))
	, strLevel7(_T(""))
	, strLevel8(_T(""))
	, strLevel9(_T(""))
	, strLevel10(_T(""))
	, strDevName(_T(""))
	, strMsg1(_T(""))
	, strMsg2(_T(""))
	, strMsg3(_T(""))
	, strMsg4(_T(""))
	, strMsg5(_T(""))
	, strMsg6(_T(""))
	, strMsg7(_T(""))
	, strMsg8(_T(""))
	, strMsg9(_T(""))
	, strMsg10(_T(""))
	, nCnt1(10)
	, nCnt2(100)
	, nCnt3(500)
	, nCnt4(1000)
	, nCnt5(5000)
	, nCnt6(10000)
	, nCnt7(50000)
	, nCnt8(100000)
	, nCnt9(30)
	, nCnt10(5)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pSpdLog = NULL;

	m_pThreadSend = NULL;
	m_pThreadSend2 = NULL;
	m_bSendRep = false;

	m_nStartTime = 0;

}

void CLogMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_COMBO_Level1, strLevel1);
	DDX_CBString(pDX, IDC_COMBO_Level2, strLevel2);
	DDX_CBString(pDX, IDC_COMBO_Level3, strLevel3);
	DDX_CBString(pDX, IDC_COMBO_Level4, strLevel4);
	DDX_CBString(pDX, IDC_COMBO_Level5, strLevel5);
	DDX_CBString(pDX, IDC_COMBO_Level6, strLevel6);
	DDX_CBString(pDX, IDC_COMBO_Level7, strLevel7);
	DDX_CBString(pDX, IDC_COMBO_Level8, strLevel8);
	DDX_CBString(pDX, IDC_COMBO_Level9, strLevel9);
	DDX_CBString(pDX, IDC_COMBO_Level10, strLevel10);
	DDX_Text(pDX, IDC_EDIT_DevName, strDevName);
	DDX_Text(pDX, IDC_EDIT_Msg1, strMsg1);
	DDX_Text(pDX, IDC_EDIT_Msg2, strMsg2);
	DDX_Text(pDX, IDC_EDIT_Msg3, strMsg3);
	DDX_Text(pDX, IDC_EDIT_Msg4, strMsg4);
	DDX_Text(pDX, IDC_EDIT_Msg5, strMsg5);
	DDX_Text(pDX, IDC_EDIT_Msg6, strMsg6);
	DDX_Text(pDX, IDC_EDIT_Msg7, strMsg7);
	DDX_Text(pDX, IDC_EDIT_Msg8, strMsg8);
	DDX_Text(pDX, IDC_EDIT_Msg9, strMsg9);
	DDX_Text(pDX, IDC_EDIT_Msg10, strMsg10);
	DDX_Text(pDX, IDC_EDIT_Cnt1, nCnt1);	DDV_MinMaxInt(pDX, nCnt1, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_Cnt2, nCnt2);	DDV_MinMaxInt(pDX, nCnt2, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_Cnt3, nCnt3);	DDV_MinMaxInt(pDX, nCnt3, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_Cnt4, nCnt4);	DDV_MinMaxInt(pDX, nCnt4, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_Cnt5, nCnt5);	DDV_MinMaxInt(pDX, nCnt5, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_Cnt6, nCnt6);	DDV_MinMaxInt(pDX, nCnt6, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_Cnt7, nCnt7);	DDV_MinMaxInt(pDX, nCnt7, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_Cnt8, nCnt8);	DDV_MinMaxInt(pDX, nCnt8, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_Cnt9, nCnt9);	DDV_MinMaxInt(pDX, nCnt9, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_Cnt10, nCnt10);	DDV_MinMaxInt(pDX, nCnt10, 0, 999999);
}

BEGIN_MESSAGE_MAP(CLogMakerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Send1, &CLogMakerDlg::OnBnClickedButtonSend1)
	ON_BN_CLICKED(IDOK, &CLogMakerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_Send2, &CLogMakerDlg::OnBnClickedButtonSend2)
	ON_BN_CLICKED(IDC_BUTTON_Send3, &CLogMakerDlg::OnBnClickedButtonSend3)
	ON_BN_CLICKED(IDC_BUTTON_Send4, &CLogMakerDlg::OnBnClickedButtonSend4)
	ON_BN_CLICKED(IDC_BUTTON_Send5, &CLogMakerDlg::OnBnClickedButtonSend5)
	ON_BN_CLICKED(IDC_BUTTON_Send6, &CLogMakerDlg::OnBnClickedButtonSend6)
	ON_BN_CLICKED(IDC_BUTTON_Send7, &CLogMakerDlg::OnBnClickedButtonSend7)
	ON_BN_CLICKED(IDC_BUTTON_Send8, &CLogMakerDlg::OnBnClickedButtonSend8)
	ON_BN_CLICKED(IDC_BUTTON_Send9, &CLogMakerDlg::OnBnClickedButtonSend9)
	ON_BN_CLICKED(IDC_BUTTON_Send10, &CLogMakerDlg::OnBnClickedButtonSend10)
	ON_BN_CLICKED(IDC_CHECK_SendRep, &CLogMakerDlg::OnBnClickedCheckSendrep)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLogMakerDlg 메시지 처리기

void CLogMakerDlg::initMsgs()
{
	UpdateData();
	strDevName.Format(_T("dev-%d"), std::rand());

	//strMsg1.Format(_T("%s-%s"), (LPCTSTR)strDevName, _T("InfoVehicleScanPose called (from InfoVehicleScanPose)"));
	//strMsg2.Format(_T("%s-%s"), (LPCTSTR)strDevName, _T("다음 차체 촬영지점에서의 현재 로봇 포즈 1:  1447.75 -1956.92  721.496 -177.722 -30.9631  -2.1694 (from InfoVehicleScanPose)"));
	//strMsg3.Format(_T("%s-%s"), (LPCTSTR)strDevName, _T("계산된 차체 보정값: 0.839, 1.174, -1.844, -0.058, -0.103, -0.019 (from UpdateInstallPosesToRobotAsync)"));
	//strMsg4.Format(_T("%s-%s"), (LPCTSTR)strDevName, _T("장착 포즈를 로봇에 업데이트 하였습니다. (from UpdateInstallPosesToRobotAsync)"));
	//strMsg5.Format(_T("%s-%s"), (LPCTSTR)strDevName, _T("Current ROI 1 glass_edge.back(): (140.000, 2.000) (from MeasureGap)"));
	//strMsg6.Format(_T("%s-%s"), (LPCTSTR)strDevName, _T("갭 체크 용 글라스 데이터 준비 소요시간 [ms]: 17 (from MeasureGap)"));
	//strMsg7.Format(_T("%s-%s"), (LPCTSTR)strDevName, _T("ROI 1 지점 갭 체크 측정 총 소요시간 [ms]: 55 ms (from MeasureGap)"));
	//strMsg8.Format(_T("%s-%s"), (LPCTSTR)strDevName, _T("다음 글라스 촬영지점에서의 레퍼런스 로봇 포즈:  2045.08  275.316   1140.5 -178.798 -11.5987  -0.7857        0 (from InfoGlassScanPose)"));
	//strMsg9.Format(_T("%s-%s"), (LPCTSTR)strDevName, _T("계산된 글라스 보정값: -0.585, -0.022, 0.000, 0.000, 0.000, 0.043 (from UpdateGlassPoseAsync)"));
	//strMsg10.Format(_T("%s-%s"), (LPCTSTR)strDevName, _T("!@ElapsedTimeForTempScanSaving [ms]: 900 (from SaveTempScanFilesGlassCheck)"));

	strMsg1.Format(_T("0-%s"), _T("InfoVehicleScanPose called (from InfoVehicleScanPose)"));
	strMsg2.Format(_T("1-%s"), _T("다음 차체 촬영지점에서의 현재 로봇 포즈 1:  1447.75 -1956.92  721.496 -177.722 -30.9631  -2.1694 (from InfoVehicleScanPose)"));
	strMsg3.Format(_T("2-%s"), _T("계산된 차체 보정값: 0.839, 1.174, -1.844, -0.058, -0.103, -0.019 (from UpdateInstallPosesToRobotAsync)"));
	strMsg4.Format(_T("3-%s"), _T("장착 포즈를 로봇에 업데이트 하였습니다. (from UpdateInstallPosesToRobotAsync)"));
	strMsg5.Format(_T("4-%s"), _T("Current ROI 1 glass_edge.back(): (140.000, 2.000) (from MeasureGap)"));
	strMsg6.Format(_T("5-%s"), _T("갭 체크 용 글라스 데이터 준비 소요시간 [ms]: 17 (from MeasureGap)"));
	strMsg7.Format(_T("6-%s"), _T("ROI 1 지점 갭 체크 측정 총 소요시간 [ms]: 55 ms (from MeasureGap)"));
	strMsg8.Format(_T("7-%s"), _T("다음 글라스 촬영지점에서의 레퍼런스 로봇 포즈:  2045.08  275.316   1140.5 -178.798 -11.5987  -0.7857        0 (from InfoGlassScanPose)"));
	strMsg9.Format(_T("8-%s"), _T("계산된 글라스 보정값: -0.585, -0.022, 0.000, 0.000, 0.000, 0.043 (from UpdateGlassPoseAsync)"));
	strMsg10.Format(_T("9-%s"), _T("!@ElapsedTimeForTempScanSaving [ms]: 900 (from SaveTempScanFilesGlassCheck)"));

	strLevel1 = "Error"; // Debug;Info;Warning;Error;Critical;Off
	strLevel2 = "Info";
	strLevel3 = "Warning";
	strLevel4 = "Error";
	strLevel5 = "Critical";
	strLevel6 = "Info";
	strLevel7 = "Critical";
	strLevel8 = "Error";
	strLevel9 = "Warning";
	strLevel10 = "Info";

	UpdateData(FALSE);
}

bool CLogMakerDlg::initSpdLog()
{
	auto console_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
	console_sink->set_level(spdlog::level::trace);
	console_sink->set_pattern("[%^%l%$] %v");

	time_t timer = time(NULL);
	struct tm now;
	localtime_s(&now, &timer);
	char logFile[128]; //[27];
	strftime(logFile, sizeof(logFile), "logs/%Y%m%d-%H%M%S.log", &now);

	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
	file_sink->set_level(spdlog::level::trace);

	auto postgresql_sink = std::make_shared<spdlog::sinks::postgresql_sink>("cleclecle");
	postgresql_sink->set_level(spdlog::level::trace);

	m_pSpdLog = new spdlog::logger("3sink", { console_sink, file_sink, postgresql_sink });
	m_pSpdLog->set_level(spdlog::level::trace);

//	m_pSpdLog->warn("this should appear in console, file, postgredb");

	return true;
}

BOOL CLogMakerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	initMsgs();
	m_pThreadSend = NULL;
	initSpdLog();
//	m_spdLog.initSpdLog("cleclecle");

	::InitializeCriticalSection(&cs);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CLogMakerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLogMakerDlg::OnPaint()
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
HCURSOR CLogMakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLogMakerDlg::closeAll()
{
	if (m_pThreadSend) {
		DWORD dwResult;
		GetExitCodeThread(m_pThreadSend->m_hThread, &dwResult);
		delete m_pThreadSend;
	}
	if (m_pSpdLog) {
		delete m_pSpdLog;
	}
	::DeleteCriticalSection(&cs);
}

void CLogMakerDlg::OnBnClickedOk()
{
	closeAll();

	CDialogEx::OnOK();
}

void CLogMakerDlg::OnBnClickedButtonSend1()
{
	UpdateData();
	sendDbMsgCnt(strLevel1, strMsg1, nCnt1);
}

void CLogMakerDlg::OnBnClickedButtonSend2()
{
	UpdateData();
	sendDbMsgCnt(strLevel2, strMsg2, nCnt2);
}


void CLogMakerDlg::OnBnClickedButtonSend3()
{
	UpdateData();
	sendDbMsgCnt(strLevel3, strMsg3, nCnt3);
}


void CLogMakerDlg::OnBnClickedButtonSend4()
{
	UpdateData();
	sendDbMsgCnt(strLevel4, strMsg4, nCnt4);
}


void CLogMakerDlg::OnBnClickedButtonSend5()
{
	UpdateData();
	sendDbMsgCnt(strLevel5, strMsg5, nCnt5);
}


void CLogMakerDlg::OnBnClickedButtonSend6()
{
	UpdateData();
	sendDbMsgCnt(strLevel6, strMsg6, nCnt6);

}


void CLogMakerDlg::OnBnClickedButtonSend7()
{
	UpdateData();
	sendDbMsgCnt(strLevel7, strMsg7, nCnt7);
}


void CLogMakerDlg::OnBnClickedButtonSend8()
{
	UpdateData();
	sendDbMsgCnt(strLevel8, strMsg8, nCnt8);
}

void CLogMakerDlg::OnBnClickedButtonSend9()
{
	UpdateData();
	sendDbMsgCnt(strLevel9, strMsg9, nCnt9);
}

void CLogMakerDlg::OnBnClickedButtonSend10()
{
	UpdateData();
	sendDbMsgCnt(strLevel10, strMsg10, nCnt10);
}

void CLogMakerDlg::printDebugString(const WCHAR* format, ...)
{
	WCHAR buf[1024];
	va_list vaList;
	va_start(vaList, format);
	_vsnwprintf_s(buf, sizeof(buf), format, vaList);
	va_end(vaList);
	OutputDebugStringW(buf);
}

void CLogMakerDlg::printDebugString(const char* format, ...)
{
	char buf[1024];
	va_list vaList;
	va_start(vaList, format);
	_vsnprintf_s(buf, sizeof(buf), format, vaList);
	va_end(vaList);
	OutputDebugStringA(buf);
}

CString CLogMakerDlg::makeMsgBuf(CString strMsg, UINT nPlusNo)
{
	CString strSend;
//	strSend.Format(_T("[%s] %s%d"), (LPCTSTR)strLvl, (LPCTSTR)strMsg, nPlusNo);
	strSend.Format(_T("%s%d"), (LPCTSTR)strMsg, nPlusNo);
	return strSend;
}

void CLogMakerDlg::toSpdLog(CString strLvl, CString strMsg)
{
	if (strLvl == "Debug")
		m_pSpdLog->debug(CT2CA(strMsg));
	else if (strLvl == "Info")
		m_pSpdLog->info(CT2CA(strMsg));
	else if (strLvl == "Warning")
		m_pSpdLog->warn(CT2CA(strMsg));
	else if (strLvl == "Error")
		m_pSpdLog->error(CT2CA(strMsg));
	else if (strLvl == "Critical")
		m_pSpdLog->critical(CT2CA(strMsg));
	else // if (strLvl == "Trace")
		m_pSpdLog->trace(CT2CA(strMsg));
//	else
//		m_pSpdLog->off(CT2CA(strMsg));
}

bool CLogMakerDlg::sendDbMsg1(CString strLvl, CString strMsg)
{
	::EnterCriticalSection(&cs);
	CString str = makeMsgBuf(strMsg, g_nSendCnt);
	toSpdLog(strLvl, str);

	g_nSendCnt++;
	timeBeginPeriod(1);
	Sleep(1);
	timeEndPeriod(1);
	::LeaveCriticalSection(&cs);
	return true;
}

void CLogMakerDlg::sendDbMsgCnt(CString strLvl, CString strMsg, UINT nCnt)
{
	unsigned int tStart = clock();
	for (UINT i = 0; i < nCnt; i++) {
		sendDbMsg1(strLvl, strMsg);
	}
	CString strElapsed;
	strElapsed.Format(_T("elapsed : %d sec"), (clock() - tStart) / 1000);

	CString str;	str.Format(_T("%d"), g_nSendCnt);
	GetDlgItem(IDC_EDIT_SendCnt)->SetWindowTextW(str);
	MessageBox(strElapsed);
}

void CLogMakerDlg::sendDbAllMsg1()
{
	sendDbMsg1(strLevel1, strMsg1);
	sendDbMsg1(strLevel2, strMsg2);
	sendDbMsg1(strLevel3, strMsg3);
	sendDbMsg1(strLevel4, strMsg4);
	sendDbMsg1(strLevel5, strMsg5);
}

void CLogMakerDlg::sendDbAllMsg2()
{
	sendDbMsg1(strLevel6, strMsg6);
	sendDbMsg1(strLevel7, strMsg7);
	sendDbMsg1(strLevel8, strMsg8);
	sendDbMsg1(strLevel9, strMsg9);
	sendDbMsg1(strLevel10, strMsg10);
}

unsigned int WINAPI CLogMakerDlg::sendThread(void* arg)
{
	CLogMakerDlg* pDlg = (CLogMakerDlg*)arg;

	while (pDlg->m_bSendRep) {
		pDlg->sendDbAllMsg1();
		CString str;	str.Format(_T("%d"), g_nSendCnt);
		pDlg->GetDlgItem(IDC_EDIT_SendCnt)->SetWindowTextW(str);
	}

	return 0;
}

unsigned int WINAPI CLogMakerDlg::sendThread2(void* arg)
{
	CLogMakerDlg* pDlg = (CLogMakerDlg*)arg;

	while (pDlg->m_bSendRep) {
		pDlg->sendDbAllMsg2();
		CString str;	str.Format(_T("%d"), g_nSendCnt);
		pDlg->GetDlgItem(IDC_EDIT_SendCnt)->SetWindowTextW(str);
	}

	return 0;
}

void CLogMakerDlg::OnBnClickedCheckSendrep()
{
	UpdateData();
	
	if (IsDlgButtonChecked(IDC_CHECK_SendRep)) {
		GetDlgItem(IDC_CHECK_SendRep)->SetWindowTextW(_T("stop"));
		m_nStartTime = clock();
		m_bSendRep = true;
		if(m_pThreadSend == NULL) {
			m_pThreadSend = AfxBeginThread(sendThread, this);
		} else {
			m_pThreadSend->ResumeThread();
		}
		if (m_pThreadSend2 == NULL) {
			m_pThreadSend2 = AfxBeginThread(sendThread2, this);
		}
		else {
			m_pThreadSend2->ResumeThread();
		}
	} else {
		m_bSendRep = false;
		GetDlgItem(IDC_CHECK_SendRep)->SetWindowTextW(_T("send rep"));
		if (m_pThreadSend != NULL) {
			m_pThreadSend->SuspendThread();
		}
		if (m_pThreadSend2 != NULL) {
			m_pThreadSend2->SuspendThread();
		}
		CString strElapsed;
		strElapsed.Format(_T("elapsed : %d sec, %d 개"), (clock() - m_nStartTime)/1000, g_nSendCnt);
		CString str;	str.Format(_T("%d"), g_nSendCnt);
		GetDlgItem(IDC_EDIT_SendCnt)->SetWindowTextW(str);
		MessageBox(strElapsed);
	}
}

void CLogMakerDlg::OnClose()
{
	closeAll();

	CDialogEx::OnClose();
}

