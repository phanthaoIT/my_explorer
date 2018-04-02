#include "Drive.h"
#include"Header.h"
#include "resource.h"
HIMAGELIST imageList;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG	msg;
	HWND hwnd;
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpszClassName = L"File Explore";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClass(&wc);
	hwnd = CreateWindow(L"File Explore", L"File Explore", WS_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_MAXIMIZE, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
					  g_hWnd = hWnd;
					  g_Drive = new DriveHelper();
					  g_Drive->getSystemDrives();

					  InitCommonControls();

					  GetClientRect(hWnd, &rcClient);
					  int parentWidth = (rcClient.right - rcClient.left);
					  int parentHeight = rcClient.bottom - rcClient.top;
					  int x = 0, y = 0;
					  int nWidth = parentWidth / 4, nHeight = parentHeight;

					  g_hTreeView = CreateWindow(WC_TREEVIEW, L"Tree View", WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, x, y, nWidth, nHeight, hWnd, (HMENU)IDT_TREEVIEW, g_hInstance, NULL);

					  loadMyComputerToTree(g_Drive, g_hTreeView);
					  SetFocus(g_hTreeView);

					  x = nWidth + 1;
					  nWidth = (rcClient.right - rcClient.left) * 3 / 4;

					  g_hListView = createListView(hWnd, IDL_LISTVIEW, g_hInstance, x, y, nWidth, nHeight);
					  loadMyComputerToListView(g_Drive, g_hListView);

	}
		break;
	case WM_NOTIFY:
	{
					  int nCurSelIndex;

					  NMHDR* notifyMess = (NMHDR*)lParam; 
					  LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)notifyMess; 
					  HTREEITEM currSelected;

					  switch (notifyMess->code)
					  {
					  case TVN_ITEMEXPANDING: 
						  currSelected = lpnmTree->itemNew.hItem;
						  loadExpandedChild(currSelected, g_hTreeView);
						  break;
					  case TVN_SELCHANGED:
						  currSelected = TreeView_GetSelection(g_hTreeView); 
						  TreeView_Expand(g_hTreeView, currSelected, TVE_EXPAND);

						  ListView_DeleteAllItems(g_hListView); 
						  loadListviewItemAt(getPath(currSelected, g_hTreeView), hWnd, g_hListView, g_Drive);
						  break;

					  case NM_CLICK:
						  break;

					  case NM_DBLCLK:
						  if (notifyMess->hwndFrom == g_hListView)
							  loadOrExecSelected(g_hListView);
						  break;
					  }

	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

HWND createListView( HWND parentWnd, long ID, HINSTANCE hParentInst, int x, int y, int nWidth, int nHeight)
{
	
	HWND m_hListView = CreateWindow( WC_LISTVIEW, L"List View",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | LVS_REPORT | LVS_ICON | LVS_EDITLABELS | LVS_SHOWSELALWAYS|WS_BORDER,
		x, y, nWidth, nHeight, parentWnd, (HMENU)ID, hParentInst, NULL);

	LVCOLUMN lvCol;

	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 150;
	lvCol.pszText = L"Name";
	ListView_InsertColumn(m_hListView, 0, &lvCol);

	lvCol.pszText = L"Type";
	lvCol.cx = 125;
	ListView_InsertColumn(m_hListView, 1, &lvCol);

	lvCol.cx = 125;
	lvCol.pszText = L"Total Size";
	ListView_InsertColumn(m_hListView, 2, &lvCol);

	lvCol.pszText = L"Free Space";
	lvCol.cx = 125;
	ListView_InsertColumn(m_hListView, 3, &lvCol);

	return m_hListView;
}

void loadMyComputerToTree(DriveHelper *drive, HWND m_hTreeView)
{
	TV_INSERTSTRUCT tvInsert;

	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = TVI_LAST; 
	tvInsert.item.iImage =0;
	tvInsert.item.iSelectedImage = 0;
	tvInsert.item.pszText = L"This PC";
	tvInsert.item.lParam = (LPARAM)L"This PC";
	HTREEITEM hMyComputer = TreeView_InsertItem(m_hTreeView, &tvInsert);

	for (int i = 0; i < g_Drive->getCount(); ++i)
	{
		tvInsert.hParent = hMyComputer; 
		tvInsert.item.iImage = 0;
		tvInsert.item.iSelectedImage = 0;
		tvInsert.item.pszText = g_Drive->getDisplayName(i);
		tvInsert.item.lParam = (LPARAM)g_Drive->getDriveLetter(i);
		HTREEITEM hDrive = TreeView_InsertItem(m_hTreeView, &tvInsert);

		loadTreeviewItemAt(hDrive, getPath(hDrive, m_hTreeView), m_hTreeView);
	}

	TreeView_Expand(m_hTreeView, hMyComputer, TVE_EXPAND);
	TreeView_SelectItem(m_hTreeView, hMyComputer);
}


void loadMyComputerToListView(DriveHelper *drive, HWND m_hListView)
{
	initListviewColumn(m_hListView, LVCOL_DRIVE_TYPE);
	LV_ITEM lv;

	for (int i = 0; i < drive->getCount(); ++i)
	{
		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;

		lv.iItem = i;
		lv.iSubItem = 0;
		lv.pszText = drive->getDisplayName(i);
		lv.lParam = (LPARAM)drive->getDriveLetter(i);
		lv.iImage = 0;
		ListView_InsertItem(m_hListView, &lv);

		lv.mask = LVIF_TEXT;

		lv.iSubItem = 1;
		lv.pszText = drive->getDriveType(i);
		ListView_SetItem(m_hListView, &lv);

		lv.iSubItem = 2;
		if (wcscmp(drive->getDriveType(i), CD_ROM) != 0)
			lv.pszText = drive->getTotalSize(i);
		else
			lv.pszText = NULL;
		ListView_SetItem(m_hListView, &lv);

		lv.iSubItem = 3;
		if (wcscmp(drive->getDriveType(i), CD_ROM) != 0)
			lv.pszText = drive->getFreeSpace(i);
		else
			lv.pszText = NULL;

		ListView_SetItem(m_hListView, &lv);
	}
}

LPCWSTR getPath(HTREEITEM hItem, HWND m_hTreeView)
{
	TVITEMEX tv; 
	tv.mask = TVIF_PARAM;
	tv.hItem = hItem;
	TreeView_GetItem(m_hTreeView, &tv); 
	return (LPCWSTR)tv.lParam;
}

LPCWSTR getPath(HWND m_hListView, int iItem)
{
	LVITEM lv;
	lv.mask = LVIF_PARAM;
	lv.iItem = iItem;
	lv.iSubItem = 0;
	ListView_GetItem(m_hListView, &lv);
	return (LPCWSTR)lv.lParam;
}

void loadExpandedChild(HTREEITEM hCurrSelected, HWND m_hTreeView)
{
	
	HTREEITEM myComputer = TreeView_GetRoot(m_hTreeView); 
	if (hCurrSelected == myComputer) 
		return;

	HTREEITEM hCurrSelectedChild = TreeView_GetChild(m_hTreeView, hCurrSelected); 

	if (hCurrSelectedChild != NULL)
	{
		do
		{
			
			if (TreeView_GetChild(m_hTreeView, hCurrSelectedChild) == NULL)
			{
				loadTreeviewItemAt(hCurrSelectedChild, getPath(hCurrSelectedChild, m_hTreeView), m_hTreeView);
			}
		} while (hCurrSelectedChild = TreeView_GetNextSibling(m_hTreeView, hCurrSelectedChild));

	}
	else
	{
		loadTreeviewItemAt(hCurrSelected, getPath(hCurrSelected, m_hTreeView), m_hTreeView);
	}
}

void loadTreeviewItemAt(HTREEITEM &hParent, LPCWSTR path, HWND m_hTreeView)
{
	TCHAR buffer[MAX_PATH_LEN];
	StrCpy(buffer, path); 
	StrCat(buffer, L"\\*"); 

	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = TVI_SORT;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvInsert.item.iImage = 0;
	tvInsert.item.iSelectedImage = 0;

	WIN32_FIND_DATA ffd; 
	HANDLE hFind = FindFirstFileW(buffer, &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	TCHAR* folderPath;

	do
	{
		DWORD fileAttribute = ffd.dwFileAttributes;
		if ((fileAttribute & FILE_ATTRIBUTE_DIRECTORY) 
			&& (fileAttribute != FILE_ATTRIBUTE_HIDDEN) 
			&& (_tcscmp(ffd.cFileName, L".") != 0) && (_tcscmp(ffd.cFileName, L"..") != 0)) 
		{
			tvInsert.item.pszText = ffd.cFileName;
			folderPath = new TCHAR[wcslen(path) + wcslen(ffd.cFileName) + 2];

			StrCpy(folderPath, path);
			if (wcslen(path) != 3)
				StrCat(folderPath, L"\\");
			StrCat(folderPath, ffd.cFileName);

			tvInsert.item.lParam = (LPARAM)folderPath;

			HTREEITEM hItem = TreeView_InsertItem(m_hTreeView, &tvInsert);
		}
	} while (FindNextFileW(hFind, &ffd));

}

void loadListviewItemAt(LPCWSTR path, HWND m_hParent, HWND m_hListView, DriveHelper *drive)
{
	if (path == NULL)
		return;

	LV_ITEM lv;

	if (_tcscmp(path,L"This PC") == 0)
	{
	
		initListviewColumn(m_hListView, LVCOL_DRIVE_TYPE);

		for (int i = 0; i < drive->getCount(); ++i)
		{
			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = i;
			lv.iSubItem = 0;
			lv.pszText = drive->getDisplayName(i);
			lv.lParam = (LPARAM)drive->getDriveLetter(i);
			lv.iImage = 0;
			ListView_InsertItem(m_hListView, &lv);

			lv.mask = LVIF_TEXT;

			lv.iSubItem = 1;
			lv.pszText = drive->getDriveType(i);
			ListView_SetItem(m_hListView, &lv); 
			lv.iSubItem = 2;
			if (wcscmp(drive->getDriveType(i), CD_ROM) != 0)
				lv.pszText = drive->getTotalSize(i);
			else
				lv.pszText = NULL;

			ListView_SetItem(m_hListView, &lv);

			lv.iSubItem = 3;
			if (wcscmp(drive->getDriveType(i), CD_ROM) != 0)
				lv.pszText = drive->getFreeSpace(i);
			else
				lv.pszText = NULL;

			ListView_SetItem(m_hListView, &lv);
		}
	}
	else
		loadDirItemToLisview(g_hWnd, m_hListView, path);
}


void loadOrExecSelected(HWND m_hListView)
{
	LPCWSTR filePath = getPath(m_hListView, ListView_GetSelectionMark(m_hListView));

	WIN32_FIND_DATA fd;

	if (GetFileAttributesEx(filePath, GetFileExInfoStandard, &fd) != 0)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			ListView_DeleteAllItems(m_hListView);
			loadDirItemToLisview(g_hWnd, m_hListView, filePath);
		}
		else
		{
			ShellExecute(NULL, L"open", filePath, NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

WCHAR* LayKieuLoai(WIN32_FIND_DATA fd)
{
	if (fd.dwFileAttributes& FILE_ATTRIBUTE_DIRECTORY)
		return  L"File Folder";
	int vt = wcsrchr(fd.cFileName, L'.') - fd.cFileName;
	int len = wcslen(fd.cFileName);
	if (vt < 0 || vt >= len) //Nếu không tìm thấy
		return L"";
	WCHAR *duoi = new WCHAR[len - vt + 1];
	for (int i = 0; i < len - vt; i++)
		duoi[i] = fd.cFileName[vt + i];
	duoi[len - vt] = 0;
	if (!wcsicmp(duoi, L".htm") || !wcsicmp(duoi, L".html"))
		return L"Web page";
	WCHAR pszOut[256];
	HKEY hKey;
	DWORD dwType = REG_SZ;
	DWORD dwSize = 256;
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, duoi, 0, KEY_READ, &hKey))
	{
		RegCloseKey(hKey);
		return L"";
	}
	if (RegQueryValueEx(hKey, NULL, NULL, &dwType, (PBYTE)pszOut, &dwSize))
	{
		RegCloseKey(hKey);
		return L"";
	}
	RegCloseKey(hKey);
	WCHAR *pszPath = new WCHAR[1000];
	dwSize = 1000;
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, pszOut, 0, KEY_READ, &hKey))
	{
		RegCloseKey(hKey);
		return L"";
	}

	if (RegQueryValueEx(hKey, NULL, NULL, &dwType, (PBYTE)pszPath, &dwSize))
	{
		RegCloseKey(hKey);
		return L"";
	}
	RegCloseKey(hKey);
	return pszPath;
}
void loadDirItemToLisview(HWND m_hParent, HWND m_hListView, LPCWSTR path)
{
	initListviewColumn(m_hListView, LVCOL_FOLDER_TYPE);
	TCHAR buffer[10240];

	StrCpy(buffer, path);

	if (wcslen(path) == 3)
		StrCat(buffer, L"*");
	else
		StrCat(buffer, L"\\*");

	WIN32_FIND_DATA fd; 
	HANDLE hFind = INVALID_HANDLE_VALUE;
	LV_ITEM lv;
	TCHAR* temporaryPath;
	int itemIndex = 0;

	hFind = FindFirstFileW(buffer, &fd);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN) &&
			(_tcscmp(fd.cFileName,L".") != 0) && (_tcscmp(fd.cFileName, L"..") != 0)) 
		{
			temporaryPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(temporaryPath, path);

			if (wcslen(path) != 3)
				StrCat(temporaryPath, L"\\");

			StrCat(temporaryPath, fd.cFileName);

			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = itemIndex;
			lv.iSubItem = 0;
			lv.pszText = fd.cFileName;
			lv.lParam = (LPARAM)temporaryPath;
			lv.iImage = lv.lParam;
			ListView_InsertItem(m_hListView, &lv);

			ListView_SetItemText(m_hListView, itemIndex, 1, convertTimeStampToString(fd.ftLastWriteTime)); 

			ListView_SetItemText(m_hListView, itemIndex, 2, LayKieuLoai(fd));

			itemIndex++;
		}


	} while (FindNextFileW(hFind, &fd));


	hFind = FindFirstFileW(buffer, &fd);

	if (hFind == INVALID_HANDLE_VALUE)
		return;
	do
	{
		if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN))
		{
			temporaryPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(temporaryPath, path);

			if (wcslen(path) != 3)
				StrCat(temporaryPath, L"\\");

			StrCat(temporaryPath, fd.cFileName);

			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = itemIndex;
			lv.iSubItem = 0;
			lv.pszText = fd.cFileName;
			lv.lParam = (LPARAM)temporaryPath;

			ListView_InsertItem(m_hListView, &lv);

			ListView_SetItemText(m_hListView, itemIndex, 1, convertTimeStampToString(fd.ftLastWriteTime));

			DWORD fileSizeLow = fd.nFileSizeLow; 
			ListView_SetItemText(m_hListView, itemIndex, 3, CDriveSize::convertByteToStringSize(fileSizeLow));
			ListView_SetItemText(m_hListView,itemIndex, 2, LayKieuLoai(fd));

			itemIndex++;
		}
	} while (FindNextFileW(hFind, &fd));

}

void initListviewColumn(HWND m_hListView, int type)
{
	LVCOLUMN lvCol;
	if (type == LVCOL_DRIVE_TYPE)
	{
		lvCol.mask = LVCF_TEXT | LVCF_FMT;

		lvCol.fmt = LVCFMT_LEFT | LVCF_WIDTH;
		lvCol.cx = 100;
		lvCol.pszText = L"Type";
		ListView_SetColumn(m_hListView, 1, &lvCol);

		lvCol.fmt = LVCFMT_RIGHT | LVCF_WIDTH;
		lvCol.cx = 80;
		lvCol.pszText = L"Total Size";
		ListView_SetColumn(m_hListView, 2, &lvCol);

		lvCol.cx = 80;
		lvCol.pszText = L"Free Space";
		ListView_SetColumn(m_hListView, 3, &lvCol);
	}
	else if (type == LVCOL_FOLDER_TYPE)
	{
		lvCol.mask = LVCF_WIDTH;
		lvCol.cx = 180;
		ListView_SetColumn(m_hListView, 0, &lvCol);

		lvCol.mask = LVCF_TEXT | LVCF_FMT;
		lvCol.fmt = LVCFMT_RIGHT;
		lvCol.pszText = L"Date Modified";
		ListView_SetColumn(m_hListView, 1, &lvCol);


		lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		lvCol.fmt = LVCFMT_LEFT;
		lvCol.cx = 130;
		lvCol.pszText = L"Type";
		ListView_SetColumn(m_hListView, 2, &lvCol);

		lvCol.pszText = L"Size";
		ListView_SetColumn(m_hListView, 3, &lvCol);
	}
	
}

LPWSTR convertTimeStampToString(const FILETIME &ftLastWrite)
{
	TCHAR *buffer = new TCHAR[50];

	SYSTEMTIME st;

	char szLocalDate[255], szLocalTime[255];

	FileTimeToSystemTime(&ftLastWrite, &st);
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_AUTOLAYOUT, &st, NULL,
		(LPWSTR)szLocalDate, 255);
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, (LPWSTR)szLocalTime, 255);

	wsprintf(buffer, L"%s %s", szLocalDate, szLocalTime);

	return buffer;
}