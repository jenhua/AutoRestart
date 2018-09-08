// AutoRstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoRst.h"
#include "AutoRstDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TimerID 1234

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAutoRstDlg dialog




CAutoRstDlg::CAutoRstDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoRstDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoRstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COUNTDOWN, m_COMBO_COUNT);
	DDX_Control(pDX, IDC_COMBO_ACTION, m_COMBO_ACTION);
}

BEGIN_MESSAGE_MAP(CAutoRstDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SET, &CAutoRstDlg::OnBnClickedButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CAutoRstDlg::OnBnClickedButtonExit)
	ON_CBN_SELCHANGE(IDC_COMBO_COUNTDOWN, &CAutoRstDlg::OnCbnSelchangeComboCountdown)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION, &CAutoRstDlg::OnCbnSelchangeComboAction)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_START, &CAutoRstDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_CLEARLOG, &CAutoRstDlg::OnBnClickedButtonClearlog)
END_MESSAGE_MAP()


// CAutoRstDlg message handlers

BOOL CAutoRstDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here	
	// Retrieve current directory path
	TCHAR buf[MAX_PATH], buf2[MAX_PATH];
	int filelen = 0, pathlen = 0;
	memset(buf, 0, sizeof(buf));
	memset(buf2, 0, sizeof(buf2));
    GetModuleFileName( NULL , buf , sizeof(buf));
	m_sRoot = buf;
	filelen = (int)wcslen(buf);

	// Get Working Directory
	//
	for (int k = filelen-1; k > 0; k--)
	{
		if (buf[k] == L'\\')
		{
			pathlen = k+1;
			memcpy(buf2, buf, pathlen*2);
			break;
		}
	}
	// Save working directory in m_sRoot
	//
	m_sRoot = buf2;

	m_COMBO_COUNT.AddString(L" 10 sec");
	m_COMBO_COUNT.AddString(L" 30 sec");
	m_COMBO_COUNT.AddString(L" 60 sec");
	m_COMBO_COUNT.AddString(L" 90 sec");
	m_COMBO_COUNT.AddString(L"120 sec");
	m_COMBO_COUNT.AddString(L"180 sec");
	m_COMBO_COUNT.SetCurSel(1);		// set default value to 30 seconds

	m_COMBO_ACTION.AddString(L"Reboot");
	m_COMBO_ACTION.AddString(L"Shutdown");
	m_COMBO_ACTION.AddString(L"Log off");
	m_COMBO_ACTION.SetCurSel(0);

	// Default values
	m_CountDownValue = 30;
	m_ActionItem = REBOOT;
	m_TestCycleValue = 0;

	if (TRUE == LoadSetupFile())	// Load autorst.ini file for count down value and action item value
	{
		int idx = 0;
		if (m_CountDownValue == 10)
			idx = 0;
		else if (m_CountDownValue == 60)
			idx = 2;
		else if (m_CountDownValue == 90)
			idx = 3;
		else if (m_CountDownValue == 120)
			idx = 4;
		else if (m_CountDownValue == 180)
			idx = 5;
		else
			idx = 1;		// 30 seconds
		m_COMBO_COUNT.SetCurSel(idx);

		CString str;
		if (m_ActionItem == SHUTDOWN)
		{
			str.Format(L"->  Shutdown");
			GetDlgItem(IDC_STATIC_ACTION)->SetWindowTextW(str);
		}
		else if (m_ActionItem == LOGOFF)
		{
			str.Format(L"->  Log Off");
			GetDlgItem(IDC_STATIC_ACTION)->SetWindowTextW(str);
		}
		else		// m_ActionItem = REBOOT
		{
			str.Format(L"->  Reboot");
			GetDlgItem(IDC_STATIC_ACTION)->SetWindowTextW(str);
		}
		m_COMBO_ACTION.SetCurSel(m_ActionItem);

		if (m_StartTesting == 1)	// continue testing
		{
			SetTimer(TimerID, 1000, NULL);		// Setup for count down
			GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(L"STOP");	
			UpdateTestCountValue();
			str.Format(L"%d", m_CountDownValue);		
			GetDlgItem(IDC_EDIT_CNT)->SetWindowTextW(str);
		}
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAutoRstDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutoRstDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutoRstDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoRstDlg::OnTimer(UINT nIDEvent) 
{
	CString str;
	if (nIDEvent == TimerID)
	{
		// Count down 
		if (m_CountDownValue > 0)
			m_CountDownValue--;

		str.Format(L"%d", m_CountDownValue);		
		GetDlgItem(IDC_EDIT_CNT)->SetWindowTextW(str);
		
		// Count down to 0 second
		if (m_CountDownValue == 0)
		{
			KillTimer(TimerID);

			// Record test result
			OutputTestResult();

			HANDLE hToken; 
			TOKEN_PRIVILEGES tkp; 
			if (m_ActionItem == SHUTDOWN)
			{					
				str.Format(L"Shutdown");		
				GetDlgItem(IDC_EDIT_CNT)->SetWindowTextW(str);
				if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
				{
					LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
					tkp.PrivilegeCount = 1; 
		            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
					if(AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0))
					{						
						ExitWindowsEx(EWX_POWEROFF|EWX_FORCE,0);		// Shutdown system
					}
				}
			}
			else if (m_ActionItem == LOGOFF)
			{
				str.Format(L"Log off");		
				GetDlgItem(IDC_EDIT_CNT)->SetWindowTextW(str);
				if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
				{
					LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
					tkp.PrivilegeCount = 1; 
		            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
					if(AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0))
					{						
						ExitWindowsEx(EWX_LOGOFF|EWX_FORCE,0);			// Log off
					}
				}
			}
			else		// (m_ActionItem == REBOOT)
			{
				str.Format(L"Reboot");		
				GetDlgItem(IDC_EDIT_CNT)->SetWindowTextW(str);
				if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
				{
					LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
					tkp.PrivilegeCount = 1; 
		            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
					if(AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0))
					{						
						ExitWindowsEx(EWX_REBOOT|EWX_FORCE,0);			// Reboot system
					}
				}
			}	
		}		
	}
	CDialog::OnTimer(nIDEvent);
}
void CAutoRstDlg::OnBnClickedButtonSet()
{
	// TODO: Add your control notification handler code here
	m_StartTesting = 0;
	GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(L"START");
	KillTimer(TimerID);
	SaveConfigToFile();				// save configuration
}

void CAutoRstDlg::OnBnClickedButtonExit()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CAutoRstDlg::OnCbnSelchangeComboCountdown()
{
	// TODO: Add your control notification handler code here
	KillTimer(TimerID);
}

void CAutoRstDlg::OnCbnSelchangeComboAction()
{
	// TODO: Add your control notification handler code here
	KillTimer(TimerID);
}

void CAutoRstDlg::SaveConfigToFile()
{
	char buf[MAX_PATH];
	CFile destFile;
	CFileException e;
	CFileStatus rStatus;

	// Save items configuration to file
	if( !destFile.Open(m_sRoot+L"AutoRst.ini", CFile::modeCreate | CFile::modeWrite, &e ) )
	{
		return;
	}	

	destFile.SeekToBegin();

	// Write count down values
	if (0 == m_COMBO_COUNT.GetCurSel())
		m_CountDownValue = 10;
	else if (1 == m_COMBO_COUNT.GetCurSel())
		m_CountDownValue = 30;
	else if (2 == m_COMBO_COUNT.GetCurSel())
		m_CountDownValue = 60;
	else if (3 == m_COMBO_COUNT.GetCurSel())
		m_CountDownValue = 90;
	else if (4 == m_COMBO_COUNT.GetCurSel())
		m_CountDownValue = 120;
	else
		m_CountDownValue = 180;

	sprintf_s(buf, "${CNT=%d}\r\n", m_CountDownValue); 
	destFile.Write(buf, (int)strlen(buf));

	// Write action items
	if (0 == m_COMBO_ACTION.GetCurSel())
		m_ActionItem = REBOOT;
	else if (1 == m_COMBO_ACTION.GetCurSel())
		m_ActionItem = SHUTDOWN;
	else
		m_ActionItem = LOGOFF;

	sprintf_s(buf, "${ITEM=%d}\r\n", m_ActionItem); 
	destFile.Write(buf, (int)strlen(buf));

	sprintf_s(buf, "${ACT=%d}\r\n", m_StartTesting); 
	destFile.Write(buf, (int)strlen(buf));

	destFile.Close();
	
}

BOOL CAutoRstDlg::LoadSetupFile()
{
	CFileException ex;
	CFile m_File;

	if (!m_File.Open(m_sRoot+L"AutoRst.ini", CFile::modeRead, &ex))
	{
		return FALSE;
	}

	char buf[2048], chbuf[64];
	DWORD i, j, stringStart;
	DWORD readinglength = 0, totalfilesize = 0;
	CFileStatus rStatus;
	CString str;
	if (m_File.m_hFile == CFile::hFileNull)
		return FALSE;

	m_File.GetStatus(rStatus);
	memset(chbuf, 0, sizeof(chbuf));
	totalfilesize = (int)rStatus.m_size;
	if (totalfilesize > sizeof(buf))
	{
		AfxMessageBox(L"Warning ! AutoRst.ini size exceed 2KB, load defult setting...");
		m_File.Close();
		return FALSE;
	}
	memset(buf, 0, sizeof(buf));
	m_File.Read(buf, totalfilesize);		// Read all data to buf
	readinglength = totalfilesize;
	m_File.Close();
		
	i = 0;

	// Check all data we read
	while (i < readinglength)
	{
		memset(chbuf, 0, sizeof(chbuf));
		if ((buf[i] == '$') && (buf[i+1] == '{'))
		{
			stringStart = i + 2;	// record the start index number of this string
			// Count the length of this string									
			for (j = stringStart; j < readinglength; j++)
			{
				chbuf[j-stringStart] = buf[j];
				if (buf[j+1] == '}')
				{
					i = j + 1;
					break;
				}
			}

			// The test item name will save in chbuf buffer
			// Abstract item name and setup relative dialog resource ID
			ItemConfig(chbuf);
		}
		else
		{
				i++;
		}			
	}
	return TRUE;
}

void CAutoRstDlg::ItemConfig(char *chBuf)
{
	char chItemName[64], chItemSetting[64];
	int i = 0, j = 0, k = 0, l = 0, len = 0, idx = 0, cnt = 0;
	CString str;

	memset(chItemName, 0, sizeof(chItemName));
	memset(chItemSetting, 0, sizeof(chItemSetting));
	len = (int)strlen(chBuf); 

	// Abstract test item name to buffe chItemName
	for (i = 0; i < len; i++)
	{
		if(chBuf[i] == '=')
		{
			idx = i+1;
			break;
		}
		chItemName[i] = chBuf[i];		
	}

	// Abstract test setting to buffer chItemSetting
	for (i = idx; i < len; i++)
	{
		chItemSetting[i-idx] = chBuf[i];
	}

	if (0 == strcmp(chItemName, "CNT"))
	{
		m_CountDownValue = atoi(chItemSetting);
	}
	
	if (0 == strcmp(chItemName, "ITEM"))
	{
		m_ActionItem = atoi(chItemSetting);
	}

	if (0 == strcmp(chItemName, "ACT"))
	{
		m_StartTesting = atoi(chItemSetting);
	}

}

BOOL CAutoRstDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
    {
		if (pMsg->wParam == VK_RETURN)
			return TRUE;				// Bypass "Enter" key down message
		if (pMsg->wParam == VK_ESCAPE)
			return TRUE;				// Bypass "Esc" key down message		
	}
	return CDialog::PreTranslateMessage(pMsg);
}
HBRUSH CAutoRstDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_CYCLE_NUMBER)	|| 
		(pWnd->GetDlgCtrlID() == IDC_EDIT_CNT))
	{						
		pDC-> SetTextColor(COLORREF(RGB(0,0,255)));	
		pDC -> SetBkMode( TRANSPARENT ); 
	}
	
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CAutoRstDlg::OutputTestResult()
{	
	CString str;

	// Write test result to log file
	//
	CFile destFile;
	CFileException e;
	char buf[MAX_PATH];
	SYSTEMTIME	st;

	// Check if TestResult.log exist. If not, create it.
	if( !destFile.Open(m_sRoot+L"AutoRst_Result.log", CFile::modeWrite, &e ) )
	{
		destFile.Open(m_sRoot+L"AutoRst_Result.log", CFile::modeCreate | CFile::modeWrite, &e );
		sprintf_s(buf, " Reboot record :\r\n");
		destFile.Write(buf, (int)strlen(buf));
		sprintf_s(buf, "-------------------------------\r\n");
		destFile.Write(buf, (int)strlen(buf));
	}	

	destFile.SeekToEnd();

	GetLocalTime(&st);			// Retrieve current time

	if (m_ActionItem == SHUTDOWN)
		sprintf_s(buf, "%04d/%02d/%02d %02d:%02d:%02d : Shutdown - %d\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, m_TestCycleValue);
	else if (m_ActionItem == LOGOFF)
		sprintf_s(buf, "%04d/%02d/%02d %02d:%02d:%02d : Log off - %d\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, m_TestCycleValue);
	else
		sprintf_s(buf, "%04d/%02d/%02d %02d:%02d:%02d : Reboot - %d\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, m_TestCycleValue);

	destFile.Write(buf, (int)strlen(buf));	
	destFile.Close();
}

void CAutoRstDlg::OnBnClickedButtonStart()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CString str;
	GetDlgItem(IDC_BUTTON_START)->GetWindowTextW(str);
	if (0 == str.Compare(L"START"))
	{
		SetTimer(TimerID, 1000, NULL);		// Setup for count down
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(L"STOP");	
		m_StartTesting = 1;		
		SaveConfigToFile();					// Save configure file
		UpdateTestCountValue();
		str.Format(L"%d", m_CountDownValue);		
		GetDlgItem(IDC_EDIT_CNT)->SetWindowTextW(str);
	}
	else
	{
		KillTimer(TimerID);
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(L"START");
		DeleteFile(m_sRoot+L"bootcnt.txt");
		m_StartTesting = 0;
		SaveConfigToFile();					// Save configure file
	}
}


void CAutoRstDlg::OnBnClickedButtonClearlog()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	DeleteFile(m_sRoot+L"bootcnt.txt");
	DeleteFile(m_sRoot+L"AutoRst_Result.log");
}

bool CAutoRstDlg::UpdateTestCountValue()
{
	CFile destFile;
	CFileException e;
	bool bFirstTest = false;
	char dat[MAX_PATH];
	int totalfilesize;
	CFileStatus rStatus;
	int value;
	CString str;

	if( !destFile.Open(m_sRoot+L"bootcnt.txt", CFile::modeRead, &e ) )
	{
		bFirstTest = true;
	}	
	destFile.Close();

	if (bFirstTest == true)
	{
		// First test, save 1 to BootCounter.txt
		if( !destFile.Open(m_sRoot+L"bootcnt.txt", CFile::modeCreate | CFile::modeWrite, &e ) )
		{
			AfxMessageBox(L"Fail to create \"bootcnt.txt\" !");
			return false;
		}	
		memset(dat, 0, sizeof(dat));
		sprintf_s(dat, "0");
		value = 0;		// Init value
		m_TestCycleValue = value;
		destFile.Write(dat, (int)strlen(dat));
		destFile.Close();
	}
	else
	{
		if( !destFile.Open(m_sRoot+L"bootcnt.txt", CFile::modeReadWrite, &e ) )
		{
			AfxMessageBox(L"Fail to open \"bootcnt.txt\" !");
			return false;
		}		
		destFile.GetStatus(rStatus);
		totalfilesize = (int)rStatus.m_size;		
		destFile.Read(dat, totalfilesize);
		value = atoi(dat);
		value = value + 1;
		m_TestCycleValue = value;
		destFile.SeekToBegin();
		sprintf_s(dat, "%d", value);
		destFile.Write(dat, (int)strlen(dat));
		destFile.Close();		
	}

	// Put text
	str.Format(L"%d", value);
	GetDlgItem(IDC_EDIT_CYCLE_NUMBER)->SetWindowTextW(str);

	return true;
}
