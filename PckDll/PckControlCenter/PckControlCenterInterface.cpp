
//////////////////////////////////////////////////////////////////////
// PckControlCenterInterface.cpp: УГУЪЅвОцНкГАКАЅз№«ЛѕµДpckОДјюЦРµДКэѕЭЈ¬ІўПФКѕФЪListЦР
// Н·ОДјю,ЅзГжУлPCKАаµДКэѕЭЅ»»ҐЈ¬їШЦЖЦРРДЈ¬ИХЦѕ№¦ДЬ
//
// ґЛіМРтУЙ АоЗп·г/stsm/liqf ±аРґ
//
// ґЛґъВлФ¤јЖЅ«»бїЄФґЈ¬ИОєО»щУЪґЛґъВлµДРЮёД·ўІјЗл±ЈБфФ­ЧчХЯРЕПў
// 
// 2015.5.19
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include "PckClass.h"

#pragma region 文件打开关闭信息界面交互

FeedbackCallback CPckControlCenter::pFeedbackCallBack = DefaultFeedbackCallback;
void* CPckControlCenter::pTag = NULL;

void CPckControlCenter::regMsgFeedback(void* _pTag, FeedbackCallback _FeedbackCallBack)
{
	pTag = _pTag;
	if(NULL != _FeedbackCallBack)
		pFeedbackCallBack = _FeedbackCallBack;
}

int CPckControlCenter::DefaultFeedbackCallback(void* pTag, int eventId, WPARAM wParam, LPARAM lParam)
{
	wchar_t szTitle[MAX_PATH] = { 0 };

	switch (eventId)
	{
	case PCK_FILE_OPEN_SUCESS:

		GetConsoleTitleW(szTitle, MAX_PATH);
		wcscat_s(szTitle, L" - ");
		wcscat_s(szTitle, (const wchar_t*)lParam);
		SetConsoleTitleW(szTitle);

		break;

	case PCK_FILE_CLOSE:
		SetConsoleTitleA("WinPCK");

		break;

	default:
		break;
	}

	return 0;
}

#pragma endregion

#pragma region 查询及目录浏览

void CPckControlCenter::DefaultShowFilelistCallback(void* _in_param, int sn, LPCWSTR lpszFilename, int entry_type, unsigned __int64 qwFileSize, unsigned __int64 qwFileSizeCompressed, void* fileEntry)
{
	auto fix_print_str = [](int nTabs, LPCWSTR str) {
		
		wprintf(str);

		int nSubTabs = wcslen(str) / 8;

		if (nTabs < nSubTabs)
			nTabs = 0;
		else
			nTabs -= nSubTabs;
		for (int i = 0; i < nTabs; i++) {
			printf("\t");
		}
	};

	auto fix_print_qword = [](int nTabs, unsigned __int64 num) {

		wchar_t szQword2Str[32];
		swprintf_s(szQword2Str, L"%llu", num);

		int len = wcslen(szQword2Str);
		int nSubTabs = (len-1) / 8;
		int nModnum = 8 - len % 8;

		if (8 != nModnum) {
			memmove_s(szQword2Str + nModnum, sizeof(szQword2Str), szQword2Str, sizeof(wchar_t) * (len + 1));
			for (int i = 0; i < nModnum; i++) {
				szQword2Str[i] = ' ';
			}
		}

		if (nTabs < nSubTabs)
			nTabs = 0;
		else
			nTabs -= nSubTabs;
		for (int i = 0; i < nTabs; i++) {
			printf("\t");
		}

		wprintf(szQword2Str);
	};

	fix_print_str(4, lpszFilename);

	printf("%s\t", (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type)) ? "Folder" : "File");

	fix_print_qword(1, qwFileSize);
	fix_print_qword(1, qwFileSizeCompressed);

	printf("\r\n");
}

DWORD CPckControlCenter::SearchByName(LPCWSTR lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK _showListCallback)
{
	if (NULL == m_lpClassPck)
		return 0;

	SHOW_LIST_CALLBACK _showList = _showListCallback;

	if (NULL == _showListCallback) {
		_showList = DefaultShowFilelistCallback;
	}

	DWORD	dwFileCount = GetPckFileCount(), dwFoundCount = 0;
	const PCKINDEXTABLE	* lpPckIndexTable = m_lpClassPck->GetPckIndexTable();

	//ґтУЎ¶ҐІг
	_showList(_in_param,
		dwFoundCount++,
		L"<--",
		PCK_ENTRY_TYPE_DOTDOT,
		0,
		0,
		(void*)GetRootNode());

	for (DWORD i = 0; i < dwFileCount; i++) {
		//while(PCK_ENTRY_TYPE_TAIL_INDEX != lpPckIndexTable->entryType){
		if (NULL != wcsstr(lpPckIndexTable->cFileIndex.szwFilename, lpszSearchString)) {

			_showList(_in_param,
				dwFoundCount,
				lpPckIndexTable->cFileIndex.szwFilename,
				lpPckIndexTable->entryType,
				lpPckIndexTable->cFileIndex.dwFileClearTextSize,
				lpPckIndexTable->cFileIndex.dwFileCipherTextSize,
				(void*)lpPckIndexTable);

			dwFoundCount++;
		}
		lpPckIndexTable++;
	}
	return dwFoundCount;
}

DWORD CPckControlCenter::ListByNode(LPCENTRY lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback)
{
	if (NULL == lpFileEntry)
		return 0;

	SHOW_LIST_CALLBACK _showList = _showListCallback;

	if (NULL == _showListCallback) {
		_showList = DefaultShowFilelistCallback;
	}

	//Из№ыКЗ..ОДјюјРЈ¬ѕНПФКѕЙПТ»ІгЈ¬І»КЗѕНПФКѕПВТ»Іг
	const PCK_PATH_NODE* lpNodeToShow = (LPPCK_PATH_NODE)lpFileEntry;

	int entry_type = lpFileEntry->entryType;
	//КЧПИТЄКЗОДјюјР
	if (PCK_ENTRY_TYPE_FOLDER != (PCK_ENTRY_TYPE_FOLDER & entry_type)) {
#if _DEBUG
		printf("%s:is not a folder\n", __FUNCTION__);
#endif
		return 0;
	}

	//ЅшИл·З..ОДјюјР
	if (PCK_ENTRY_TYPE_DOTDOT != (PCK_ENTRY_TYPE_DOTDOT & entry_type)){

		lpNodeToShow = ((LPPCK_PATH_NODE)lpFileEntry)->child;

	}//КЈПВµДКЗ..ОДјюјР
	else {
		lpNodeToShow = ((LPPCK_PATH_NODE)lpFileEntry)->parentfirst;
	}

	//lpNodeToShowКЗ·сОЄNULL
	if (NULL == lpNodeToShow) {
#if _DEBUG
		printf("%s:lpNodeToShow is NULL\n", __FUNCTION__);
#endif
		return 0;
	}

	DWORD dwSerialNumber = 0;

	const PCK_PATH_NODE* lpNodeToShowPtr = lpNodeToShow;

	while (NULL != lpNodeToShowPtr && 0 != *lpNodeToShowPtr->szName) {
		//ХвАпПИПФКѕОДјюјР
		int entryType = lpNodeToShowPtr->entryType;
		if ((PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entryType))) {

			if (NULL != lpNodeToShowPtr->child) {
				_showList(_in_param,
					dwSerialNumber,
					lpNodeToShowPtr->szName,
					entryType,
					lpNodeToShowPtr->child->qdwDirClearTextSize,
					lpNodeToShowPtr->child->qdwDirCipherTextSize,
					(void*)lpNodeToShowPtr);
			}
			else {
				_showList(_in_param,
					dwSerialNumber,
					lpNodeToShowPtr->szName,
					entryType,
					0,
					0,
					(void*)lpNodeToShowPtr);
			}

			dwSerialNumber++;
		}
		lpNodeToShowPtr = lpNodeToShowPtr->next;
	}

	lpNodeToShowPtr = lpNodeToShow;
	while (NULL != lpNodeToShowPtr && 0 != *lpNodeToShowPtr->szName) {
		if ((PCK_ENTRY_TYPE_FOLDER != (PCK_ENTRY_TYPE_FOLDER & lpNodeToShowPtr->entryType))) {
			const PCKINDEXTABLE* lpPckIndexTable = lpNodeToShowPtr->lpPckIndexTable;

			_showList(_in_param,
				dwSerialNumber,
				lpNodeToShowPtr->szName,
				lpPckIndexTable->entryType,
				lpPckIndexTable->cFileIndex.dwFileClearTextSize,
				lpPckIndexTable->cFileIndex.dwFileCipherTextSize,
				(void*)lpNodeToShowPtr);

			dwSerialNumber++;
		}

		lpNodeToShowPtr = lpNodeToShowPtr->next;

	}
	return dwSerialNumber;
}

#pragma endregion
