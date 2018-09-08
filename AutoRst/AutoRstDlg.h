// AutoRstDlg.h : header file
//

#pragma once
#include "afxwin.h"

#define REBOOT		0
#define	SHUTDOWN	1
#define LOGOFF		2

// CAutoRstDlg dialog
class CAutoRstDlg : public CDialog
{
// Construction
public:
	CAutoRstDlg(CWnd* pParent = NULL);	// standard constructor

	CString m_sRoot;
	BOOL LoadSetupFile();
	void SaveConfigToFile();
	void ItemConfig(char *chBuf);
	void OutputTestResult();
	int m_ActionItem;
	int m_CountDownValue;
	int m_StartTesting;	
	bool UpdateTestCountValue();
	int m_TestCycleValue;

// Dialog Data
	enum { IDD = IDD_AUTORST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSet();
	afx_msg void OnBnClickedButtonExit();
	CComboBox m_COMBO_COUNT;
	CComboBox m_COMBO_ACTION;
	afx_msg void OnCbnSelchangeComboCountdown();
	afx_msg void OnCbnSelchangeComboAction();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonClearlog();
};
