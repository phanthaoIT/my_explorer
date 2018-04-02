#include "Drive.h"

CDriveSize::CDriveSize()
{
}

CDriveSize::~CDriveSize()
{
}

CDriveSize::CDriveSize(__int64 totalSize, __int64 freeSpace)
{
	mTotalSize = totalSize;
	mFreeSpace = freeSpace;
}

LPWSTR CDriveSize::convertByteToStringSize(__int64 nSize)
{
	int nType = 0; 

	while (nSize >= 1048576)
	{
		nSize /= 1024;
		++nType;
	}

	__int64 nRight;

	if (nSize >= 1024)
	{
		
		nRight = nSize % 1024;

		while (nRight > 99)
			nRight /= 10;

		nSize /= 1024;
		++nType;
	}
	else
		nRight = 0;

	TCHAR *buffer = new TCHAR[11];
	_itow_s(nSize, buffer, 11, RADIX);

	if (nRight != 0 && nType > KB)
	{
		StrCat(buffer, _T("."));
		TCHAR *right = new TCHAR[3];
		_itow_s(nRight, right, 3, RADIX);
		StrCat(buffer, right);
	}

	switch (nType)
	{
	case 0:
		StrCat(buffer, _T(" bytes"));
		break;
	case KB:
		StrCat(buffer, _T(" KB"));
		break;
	case MB:
		StrCat(buffer, _T(" MB"));
		break;
	case GB:
		StrCat(buffer, _T(" GB"));
		break;
	case TB:
		StrCat(buffer, _T(" TB"));
		break;
	}

	return buffer;
}

LPWSTR CDriveSize::getTotalSize()
{
	return convertByteToStringSize(mTotalSize);
}

LPWSTR CDriveSize::getFreeSpace()
{
	return convertByteToStringSize(mFreeSpace);
}


DriveHelper::DriveHelper()
{
	mDriveLetter = NULL;
	mVolumeLabel = NULL;
	mDriveType = NULL;
	mNumberOfDrive = 0;
}

DriveHelper::~DriveHelper()
{
	for (int i = 0; i < mNumberOfDrive; ++i)
	{
		delete[] mDriveLetter[i];
		delete[] mVolumeLabel[i];
		delete[] mDriveType[i];
	}

	delete[] mDriveLetter;
	delete[] mVolumeLabel;
	delete[] mDriveType;
	mNumberOfDrive = 0;
}

TCHAR* DriveHelper::getDriveType(int position)
{
	return mDriveType[position];
}

TCHAR* DriveHelper::getDriveLetter(const int &i)
{
	return mDriveLetter[i];
}


TCHAR* DriveHelper::getDisplayName(const int &i)
{
	return mVolumeLabel[i];
}

int DriveHelper::getCount()
{
	return mNumberOfDrive;
}


void DriveHelper::getSystemDrives()
{
	TCHAR buffer[MAX_BUFFER_LEN];
	TCHAR* volumeTempName = new TCHAR[VOLUME_NAME_SIZE];

	GetLogicalDriveStrings(MAX_BUFFER_LEN, buffer);
	mNumberOfDrive = 0;
	for (int i = 0; (buffer[i] != '\0') || (buffer[i - 1] != '\0'); i++) 
	{
		if (buffer[i] == '\0')
		{
			mNumberOfDrive++;
		}
	}

	mDriveLetter = new TCHAR*[mNumberOfDrive];
	mVolumeLabel = new TCHAR*[mNumberOfDrive];
	mDriveType = new TCHAR*[mNumberOfDrive];
	mDriveSize = new CDriveSize*[mNumberOfDrive];

	for (int i = 0; i < mNumberOfDrive; ++i)
	{
		mDriveLetter[i] = new TCHAR[MAX_DRIVE_LETTER];
		mVolumeLabel[i] = new TCHAR[MAX_DISPLAY_NAME];
		mDriveType[i] = new TCHAR[MAX_TYPE_LEN];
	}

	int j, k;
	int index = 0;

	for (j = 0; j < mNumberOfDrive; ++j)
	{
		k = 0;
		while (buffer[index] != 0)
		{
			mDriveLetter[j][k++] = buffer[index++];
		}
		mDriveLetter[j][k] = '\0'; 
		++index;
	}


	int nType;
	for (int i = 0; i < mNumberOfDrive; ++i)
	{
		nType = GetDriveType(mDriveLetter[i]);

		switch (nType)
		{
		case DRIVE_FIXED:
			StrCpy(mDriveType[i], FIXED_DRIVE);
			break;
		case DRIVE_REMOVABLE:
			StrCpy(mDriveType[i], REMOVABLE_DRIVE);
			break;
		case DRIVE_REMOTE:
			StrCpy(mDriveType[i], REMOTE_DRIVE);
			break;
		case DRIVE_CDROM:
			StrCpy(mDriveType[i], CD_ROM);
			break;
		default:
			break;
		}

		mDriveSize[i] = getDriveSize(i);

		StrCpy(buffer, _T(""));

		if ((nType == DRIVE_FIXED) || ((nType == DRIVE_REMOVABLE)) || (nType == DRIVE_REMOTE))
		{
			GetVolumeInformation(mDriveLetter[i], buffer, MAX_BUFFER_LEN, VOLUME_SERIAL_NUMBER, MAX_COMPONENT_LEN, FILE_SYSTEM_FLAG, FILE_SYSTEM_NAME_BUFF, FILE_SYSTEM_NAME_SIZE);
			StrCpy(volumeTempName, buffer);
		}
		else
		if (nType == DRIVE_CDROM)
		{
			GetVolumeInformation(mDriveLetter[i], buffer, MAX_BUFFER_LEN, VOLUME_SERIAL_NUMBER, MAX_COMPONENT_LEN, FILE_SYSTEM_FLAG, FILE_SYSTEM_NAME_BUFF, FILE_SYSTEM_NAME_SIZE);

			if (wcslen(buffer) == 0)
			{
				StrCpy(volumeTempName, _T("CD-ROM"));
			}
			else
				StrCpy(volumeTempName, buffer);
		}
		else if (((i == 0) || (i == 1)) && (nType == DRIVE_REMOVABLE)) 
		{
			StrCpy(volumeTempName, _T("loppy disk"));
		}

		if (wcslen(volumeTempName) == 0)
		{
			StrCpy(mVolumeLabel[i], _T("Local Disk"));
		}
		else
		{
			StrCpy(mVolumeLabel[i], volumeTempName);
		}

		StrCat(mVolumeLabel[i], _T(" ("));
		StrNCat(mVolumeLabel[i], mDriveLetter[i], 3);
		StrCat(mVolumeLabel[i], _T(")"));
	}
}


CDriveSize* DriveHelper::getDriveSize(int i)
{
	__int64 totalSize;
	__int64 freeSpace;

	SHGetDiskFreeSpaceEx(getDriveLetter(i), NULL, (PULARGE_INTEGER)&totalSize, (PULARGE_INTEGER)&freeSpace);

	CDriveSize* driveSize = new CDriveSize(totalSize, freeSpace);

	return driveSize;
}

LPWSTR DriveHelper::getFreeSpace(int i)
{
	return mDriveSize[i]->getFreeSpace();
}

LPWSTR DriveHelper::getTotalSize(int i)
{
	return mDriveSize[i]->getTotalSize();
}
