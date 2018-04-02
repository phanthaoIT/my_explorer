#pragma once
#include <windows.h>
#include <tchar.h>
#include <shellapi.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#define KB 1
#define MB 2
#define GB 3
#define TB 4
#define RADIX 10


#define MAX_BUFFER_LEN 105 


#define VOLUME_NAME_SIZE		MAX_BUFFER_LEN
#define VOLUME_SERIAL_NUMBER	NULL
#define MAX_COMPONENT_LEN		NULL
#define FILE_SYSTEM_FLAG		NULL
#define FILE_SYSTEM_NAME_BUFF	NULL 
#define FILE_SYSTEM_NAME_SIZE	0  


#define MAX_TYPE_LEN		20
#define MAX_DRIVE_LETTER	4
#define MAX_VOLUME_NAME		30
#define MAX_DISPLAY_NAME	30


#define FIXED_DRIVE		_T("Local Disk")
#define REMOVABLE_DRIVE _T("Removable Drive")
#define CD_ROM			_T("CD-ROM")
#define REMOTE_DRIVE	_T("Network Drive")

class CDriveSize
{
private:
	__int64 mTotalSize;
	__int64 mFreeSpace;


public:
	CDriveSize();
	CDriveSize(__int64 totalSize, __int64 freeSpace);
	~CDriveSize();

	static LPWSTR convertByteToStringSize(__int64 nSize);

	LPWSTR getTotalSize();
	LPWSTR getFreeSpace();

};



class DriveHelper
{
private:
	TCHAR** mDriveLetter;
	TCHAR** mVolumeLabel;
	TCHAR** mDriveType;
	int mNumberOfDrive;
	CDriveSize** mDriveSize;

	CDriveSize* getDriveSize(int i);

public:
	DriveHelper();
	~DriveHelper();

	TCHAR* getDriveLetter(const int &i);
	TCHAR* getDisplayName(const int &i);
	int getCount();


	void getSystemDrives();

	TCHAR* getDriveType(int position);
	LPWSTR getTotalSize(int i);
	LPWSTR getFreeSpace(int i);
};
