﻿
// LogMakerDlg.h: 헤더 파일
//
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "postgresql_sink.h"

#pragma once

#define sleep(x) Sleep((x)*1000)



// CLogMakerDlg 대화 상자
class CLogMakerDlg : public CDialogEx
{
// 생성입니다.
public:
	CLogMakerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGMAKER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSend1();
	CString strLevel1;
	CString strLevel2;
	CString strLevel3;
	CString strLevel4;
	CString strLevel5;
	CString strLevel6;
	CString strLevel7;
	CString strLevel8;
	CString strLevel9;
	CString strLevel10;
	CString strDevName;

	CString strMsg1;
	CString strMsg2;
	CString strMsg3;
	CString strMsg4;
	CString strMsg5;
	CString strMsg6;
	CString strMsg7;
	CString strMsg8;
	CString strMsg9;
	CString strMsg10;

	int nCnt1;
	int nCnt2;
	int nCnt3;
	int nCnt4;
	int nCnt5;
	int nCnt6;
	int nCnt7;
	int nCnt8;
	int nCnt9;
	int nCnt10;


private:
	unsigned int m_nStartTime;
	bool sendDbMsg1(CString strLvl, CString strMsg);
	void sendDbMsgCnt(CString strLvl, CString strMsg, UINT nCnt);

	void initMsgs();
	CWinThread* m_pThreadSend;
	void closeAll();

	CString makeMsgBuf(CString strMsg, UINT nPlusNo);

	spdlog::logger* m_pSpdLog;

	void toSpdLog(CString strLvl, CString strMsg);
	bool initSpdLog();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSend2();
	afx_msg void OnBnClickedButtonSend3();
	afx_msg void OnBnClickedButtonSend4();
	afx_msg void OnBnClickedButtonSend5();
	afx_msg void OnBnClickedButtonSend6();
	afx_msg void OnBnClickedButtonSend7();
	afx_msg void OnBnClickedButtonSend8();
	afx_msg void OnBnClickedButtonSend9();
	afx_msg void OnBnClickedButtonSend10();
	afx_msg void OnBnClickedCheckSendrep();

	static unsigned int WINAPI sendThread(void* arg);

	bool m_bSendRep;

	void sendDbAllMsg();
	static void printDebugString(const WCHAR* format, ...);
	static void printDebugString(const char* format, ...);
	
	afx_msg void OnClose();
};