#include "Drive.h"
#include <commctrl.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <shellapi.h>

#define IDC_EXPLORER 1
#define MAX_LOADSTRING 100
#define NUMBER_OF_INIT_ICON 8 
#define MAX_EXPAND_ICON 3
#define IDT_TREEVIEW 20
#define IDL_LISTVIEW 21

#define DEFAULT_ICON_INDEX 0

#define LVCOL_DRIVE_TYPE		0
#define LVCOL_FOLDER_TYPE		1

#define MAX_PATH_LEN 10240

HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
RECT rcClient;
HINSTANCE g_hInstance;
HWND	g_hWnd;
HWND	g_hTreeView;
HWND	g_hListView;
DriveHelper* g_Drive;

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


HWND createListView(HWND parentWnd, long ID, HINSTANCE hParentInst, int x, int y, int nWidth, int nHeight);
void loadMyComputerToTree(DriveHelper *drive, HWND m_hTreeView);
void loadMyComputerToListView(DriveHelper *drive, HWND m_hListView);
void loadExpandedChild(HTREEITEM hCurrSelected, HWND m_hTreeView);
LPCWSTR getPath(HTREEITEM hItem, HWND m_hTreeView);
void loadTreeviewItemAt(HTREEITEM &hParent, LPCWSTR path, HWND m_hTreeView);
void loadListviewItemAt(LPCWSTR path, HWND m_hParent, HWND m_hListView, DriveHelper *drive);
void loadOrExecSelected(HWND m_hListView);
void loadDirItemToLisview(HWND m_hParent, HWND m_hListView, LPCWSTR path);
void initListviewColumn(HWND m_hListView, int type);
LPWSTR convertTimeStampToString(const FILETIME &ftLastWrite);
int IconListView(WCHAR* dc);
