//////////////////////////////////////////////////////////////////////
// threadproc.cpp: WinPCK ½çÃæÏß³Ì²¿·Ö
// Ñ¹Ëõ¡¢½âÑ¹¡¢¸üÐÂµÈ´óÓÃÊ±¹ý³Ì£¬Ðè¶àÏß³Ìµ÷ÓÃ 
//
// ´Ë³ÌÐòÓÉ ÀîÇï·ã/stsm/liqf ±àÐ´
//
// ´Ë´úÂëÔ¤¼Æ½«»á¿ªÔ´£¬ÈÎºÎ»ùÓÚ´Ë´úÂëµÄÐÞ¸Ä·¢²¼Çë±£ÁôÔ­×÷ÕßÐÅÏ¢
// 
// 2012.4.10
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4311 )
//#pragma warning ( disable : 4018 )
#pragma warning ( disable : 4005 )

#include "tlib.h"
#include "resource.h"
#include "globals.h"
#include "winmain.h"
#include <process.h>
#include <tchar.h>
#include "OpenSaveDlg.h"
#include "StopWatch.h"

VOID GetPckFileNameBySource(LPTSTR dst, LPCTSTR src, BOOL isDirectory);

VOID TInstDlg::UpdatePckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;
	BOOL		bHasPckOpened = !pck_IsValidPck();
	TCHAR		szFilenameToSave[MAX_PATH];
	TCHAR		szPrintf[320];
	CStopWatch	timer;;

	*szFilenameToSave = 0;

	if (bHasPckOpened) {
		//´ËÊ±Ã»ÓÐ´ò¿ªÎÄ¼þ£¬ÕâÊ±µÄ²Ù×÷Ïàµ±ÓÚÐÂ½¨ÎÄµµ
		pThis->m_currentNodeOnShow = NULL;

		if (1 == pThis->m_lpszFilePath.size()) {
			GetPckFileNameBySource(szFilenameToSave, pThis->m_lpszFilePath[0].c_str(), FALSE);
		}

		//Ñ¡Ôñ±£´æµÄÎÄ¼þÃû
		int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, TEXT("pck"), pThis->BuildSaveDlgFilterString());
		if (0 > nSelectFilter) {
			pck_close();
			return;
		}

		//Éè¶¨Ä¿±êpckµÄ°æ±¾
		if(WINPCK_OK != pck_setVersion(nSelectFilter))
			return;

		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), _tcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	}
	else {
		_tcscpy_s(szFilenameToSave, pThis->m_Filename);
		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
	}

	//¿ªÊ¼¼ÆÊ±
	timer.start();

	pThis->EnbaleButtons(ID_MENU_ADD, FALSE);

	pThis->SetStatusBarText(4, szPrintf);

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	pck_StringArrayReset();
	for (int i = 0; i < pThis->m_lpszFilePath.size(); i++) {
		pck_StringArrayAppend(pThis->m_lpszFilePath[i].c_str());
	}

	if (WINPCK_OK == pck_UpdatePckFileSubmit(szFilenameToSave, pThis->m_currentNodeOnShow)) {

		//¼ÆÊ±½áÊø
		timer.stop();

		if (pck_isLastOptSuccess()) {

			//pThis->m_PckLog.PrintLogN(GetLoadStr(IDS_STRING_RENEWOK), timer.getElapsedTime());
			log_Print(LOG_IMAGE_NOTICE, GetLoadStr(IDS_STRING_RENEWOK), timer.getElapsedTime());
			//pThis->SetStatusBarText(4, szPrintf);
		}
		else {

			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}


		//ÕûºÏ±¨¸æ
		// ´òÓ¡±¨¸æ
		// pck°üÖÐÔ­ÓÐÎÄ¼þ %d ¸ö\r\n
		// ÐÂ¼ÓÈëÎÄ¼þ %d ¸ö£¬ÆäÖÐÖØ¸´ÎÄ¼þÃû %d ¸ö\r\n
		// Ê¹ÓÃÔ­Êý¾ÝµØÖ· %d ¸ö£¬ÐÂÊý¾ÝµØÖ· %d ¸ö\r\n
		// Í¨¹ý³ÉµÄÐÂpckÎÄ¼þ¹² %d ¸öÎÄ¼þ\r\n

		if (0 != pck_getUpdateResult_PrepareToAddFileCount()) {

			_stprintf_s(szPrintf,
				TEXT("Âñåãî ôàéëîâ: %d\r\n")
				TEXT("Äîáàâëåíî: %d"),
				pck_getUpdateResult_OldFileCount(),
				pck_getUpdateResult_PrepareToAddFileCount(),
				pck_getUpdateResult_ChangedFileCount(),
				pck_getUpdateResult_DuplicateFileCount(),
				pck_getUpdateResult_PrepareToAddFileCount() - pck_getUpdateResult_ChangedFileCount(),
				pck_getUpdateResult_FinalFileCount());

			pThis->MessageBox(szPrintf, TEXT("Äîáàâëåíèå ôàéëîâ"), MB_OK | MB_ICONINFORMATION);

			//pThis->m_PckLog.PrintLogI(szPrintf);
			log_Print(LOG_IMAGE_INFO, szPrintf);
		}


		if (bHasPckOpened) {

			pThis->OpenPckFile(szFilenameToSave, TRUE);
		}
		else {
			pThis->OpenPckFile(pThis->m_Filename, TRUE);
		}

	}
	else {

		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));

		if (bHasPckOpened) {
			pck_close();
		}
	}

	pThis->EnbaleButtons(ID_MENU_ADD, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	//»¹Ô­Drop×´Ì¬
	pThis->m_lpszFilePath.clear();
	DragAcceptFiles(pThis->hWnd, TRUE);

	return;

}

VOID TInstDlg::RenamePckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;
	TCHAR		szPrintf[64];
	CStopWatch	timer;

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);

	//¿ªÊ¼¼ÆÊ±
	timer.start();

	pThis->EnbaleButtons(ID_MENU_RENAME, FALSE);

	pThis->SetStatusBarText(4, szPrintf);

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if (WINPCK_OK == pck_RenameSubmit()) {

		//¼ÆÊ±½áÊø
		timer.stop();
		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWOK), timer.getElapsedTime());

		pThis->SetStatusBarText(4, szPrintf);

		pThis->OpenPckFile(pThis->m_Filename, TRUE);

	}
	else {

		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		pck_close();
	}

	pThis->EnbaleButtons(ID_MENU_RENAME, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if (pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	return;
}

VOID TInstDlg::RebuildPckFile(VOID	*pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		bNeedCreatePck = !pck_IsValidPck();

	TCHAR		szFilenameToSave[MAX_PATH];
	TCHAR		szScriptFile[MAX_PATH];
	TCHAR		szPrintf[288];

	CStopWatch	timer;

	if (bNeedCreatePck) {
		if (!pThis->OpenPckFile()) {
			return;
		}
	}

	_tcscpy_s(szFilenameToSave, pThis->m_Filename);

	TCHAR		*lpszFileTitle = _tcsrchr(szFilenameToSave, TEXT('\\')) + 1;
	_tcscpy(lpszFileTitle, TEXT("Rebuild_"));
	_tcscat_s(szFilenameToSave, _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);

	//µ¯³öÑ¡Ïî¶Ô»°¿ò
	//µ÷ÓÃ¶Ô»°¿ò
	BOOL  bNeedRecompress;
	TRebuildOptDlg	dlg(szScriptFile, &bNeedRecompress, pThis);
	if (IDCANCEL == dlg.Exec())
		return;

	//Ñ¡Ôñ±£´æµÄÎÄ¼þÃû
	int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, TEXT("pck"), pThis->BuildSaveDlgFilterString(), pck_getVersion());
	if (0 > nSelectFilter) {
		return;
	}

	if (WINPCK_OK != pck_setVersion(nSelectFilter))
		return;

	//¿ªÊ¼¼ÆÊ±
	timer.start();

	pThis->EnbaleButtons(ID_MENU_REBUILD, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_REBUILDING), _tcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	pThis->SetStatusBarText(4, szPrintf);

	pThis->SetTimer(WM_TIMER_PROGRESS_100, 100, NULL);

	if (WINPCK_OK == pck_RebuildPckFileWithScript(szScriptFile, szFilenameToSave, bNeedRecompress)) {

		//¼ÆÊ±½áÊø
		timer.stop();

		if (pck_isLastOptSuccess()) {

			log_Print(LOG_IMAGE_NOTICE, GetLoadStr(IDS_STRING_REBUILDOK), timer.getElapsedTime());
		}
		else {
			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

	}
	else {

		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));

	}

	pThis->EnbaleButtons(ID_MENU_REBUILD, TRUE);

	if (bNeedCreatePck) {
		ListView_DeleteAllItems(pThis->GetDlgItem(IDC_LIST));
		pck_close();
	}

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	return;
}

VOID TInstDlg::StripPckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		bNeedCreatePck = !pck_IsValidPck();

	TCHAR		szFilenameToSave[MAX_PATH];
	TCHAR		szPrintf[288];

	CStopWatch	timer;

	if (bNeedCreatePck) {
		if (!pThis->OpenPckFile()) {
			return;
		}
	}

	_tcscpy_s(szFilenameToSave, pThis->m_Filename);

	TCHAR		*lpszFileTitle = _tcsrchr(szFilenameToSave, TEXT('\\')) + 1;
	_tcscpy(lpszFileTitle, TEXT("Striped_"));
	_tcscat_s(szFilenameToSave, _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);

	//µ¯³öÑ¡Ïî¶Ô»°¿ò
	//µ÷ÓÃ¶Ô»°¿ò
	int stripFlag;
	TStripDlg	dlg(&stripFlag, pThis);
	if (IDCANCEL == dlg.Exec())
		return;

	//Ñ¡Ôñ±£´æµÄÎÄ¼þÃû
	int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, TEXT("pck"), pThis->BuildSaveDlgFilterString(), pck_getVersion());
	if (0 > nSelectFilter) {
		return;
	}

	if (WINPCK_OK != pck_setVersion(nSelectFilter))
		return;

	//¿ªÊ¼¼ÆÊ±
	timer.start();

	pThis->EnbaleButtons(ID_MENU_REBUILD, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_REBUILDING), _tcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	pThis->SetStatusBarText(4, szPrintf);

	pThis->SetTimer(WM_TIMER_PROGRESS_100, 100, NULL);

	if (WINPCK_OK == pck_StripPck(szFilenameToSave, stripFlag)) {

		//¼ÆÊ±½áÊø
		timer.stop();

		if (pck_isLastOptSuccess()) {

			log_Print(LOG_IMAGE_NOTICE, GetLoadStr(IDS_STRING_REBUILDOK), timer.getElapsedTime());
		}
		else {
			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

	}
	else {

		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));

	}

	pThis->EnbaleButtons(ID_MENU_REBUILD, TRUE);

	if (bNeedCreatePck) {
		ListView_DeleteAllItems(pThis->GetDlgItem(IDC_LIST));
		pck_close();
	}

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}
	return;
}

VOID TInstDlg::CreateNewPckFile(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		isNotOpenedPck = !pck_IsValidPck();
	TCHAR		szFilenameToSave[MAX_PATH];

	TCHAR		szPrintf[64];

	CStopWatch	timer;

	//Ñ¡ÔñÄ¿Â¼
	if (!OpenFilesVistaUp(pThis->hWnd, pThis->m_CurrentPath))
		return;

	pThis->m_lpszFilePath.push_back(pThis->m_CurrentPath);

	GetPckFileNameBySource(szFilenameToSave, pThis->m_CurrentPath, TRUE);

	//Ñ¡Ôñ±£´æµÄÎÄ¼þÃû
	int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, TEXT("pck"), pThis->BuildSaveDlgFilterString());
	if (0 > nSelectFilter)
		return;

	//Éè¶¨Ä¿±êpckµÄ°æ±¾
	if (WINPCK_OK != pck_setVersion(nSelectFilter))
		return;

	//¿ªÊ¼¼ÆÊ±
	timer.start();

	pThis->EnbaleButtons(ID_MENU_NEW, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_COMPING), _tcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	pThis->SetStatusBarText(4, szPrintf);

	//pck_setThreadWorking(pThis->m_PckHandle);

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	pck_StringArrayReset();
	for (int i = 0; i < pThis->m_lpszFilePath.size(); i++) {
		pck_StringArrayAppend(pThis->m_lpszFilePath[i].c_str());
	}

	if (WINPCK_OK == pck_UpdatePckFileSubmit(szFilenameToSave, pThis->m_currentNodeOnShow)) {

		//¼ÆÊ±½áÊø
		timer.stop();

		if (pck_isLastOptSuccess()) {
			log_Print(LOG_IMAGE_NOTICE, GetLoadStr(IDS_STRING_REBUILDOK), timer.getElapsedTime());
		}
		else {
			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

	}
	else {

		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(ID_MENU_NEW, TRUE);

	if (isNotOpenedPck)
		pck_close();

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {

		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}
	return;
}

VOID TInstDlg::ToExtractAllFiles(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;
	TCHAR		szPrintf[64];

	CStopWatch	timer;

	//¿ªÊ¼¼ÆÊ±
	timer.start();

	pThis->EnbaleButtons(ID_MENU_UNPACK_ALL, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
	pThis->SetStatusBarText(4, szPrintf);

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if (WINPCK_OK == pck_ExtractAllFiles(pThis->m_CurrentPath)) {
		//¼ÆÊ±½áÊø
		timer.stop();
		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPOK), timer.getElapsedTime());

		pThis->SetStatusBarText(4, szPrintf);

	}
	else {
		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(ID_MENU_UNPACK_ALL, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	return;
}

VOID TInstDlg::ToExtractSelectedFiles(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	HWND	hList = pThis->GetDlgItem(IDC_LIST);

	CStopWatch	timer;

	const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, **lpFileEntryArrayPtr;

	UINT uiSelectCount = ListView_GetSelectedCount(hList);

	if (0 != uiSelectCount) {
		if (NULL != (lpFileEntryArray = (const PCK_UNIFIED_FILE_ENTRY **)malloc(sizeof(PCK_UNIFIED_FILE_ENTRY *) * uiSelectCount))) {
			TCHAR		szPrintf[64];

			//È¡lpNodeToShow
			int	nCurrentItemCount = ListView_GetItemCount(hList);

			LVITEM item;
			item.mask = LVIF_PARAM | LVIF_STATE;
			item.iSubItem = 0;
			item.stateMask = LVIS_SELECTED;		// get all state flags

			lpFileEntryArrayPtr = lpFileEntryArray;

			uiSelectCount = 0;

			//´Ó1¿ªÊ¼£¬Ìø¹ý..Ä¿Â¼
			for (item.iItem = 1; item.iItem < nCurrentItemCount; item.iItem++) {
				ListView_GetItem(hList, &item);

				if (item.state & LVIS_SELECTED) {
					*lpFileEntryArrayPtr = (LPPCK_UNIFIED_FILE_ENTRY)item.lParam;
					lpFileEntryArrayPtr++;

					uiSelectCount++;
				}
			}

			if (0 == uiSelectCount)return;

			//¿ªÊ¼¼ÆÊ±
			timer.start();

			pThis->EnbaleButtons(ID_MENU_UNPACK_SELECTED, FALSE);

			_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
			pThis->SetStatusBarText(4, szPrintf);

			pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

			lpFileEntryArrayPtr = lpFileEntryArray;

			if (WINPCK_OK == pck_ExtractFilesByEntrys(lpFileEntryArray, uiSelectCount, pThis->m_CurrentPath)) {
				//¼ÆÊ±½áÊø
				timer.stop();
				_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPOK), timer.getElapsedTime());

				pThis->SetStatusBarText(4, szPrintf);
			}
			else {
				pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
			}

			free(lpFileEntryArray);

			pThis->EnbaleButtons(ID_MENU_UNPACK_SELECTED, TRUE);

			pThis->KillTimer(WM_TIMER_PROGRESS_100);
			pThis->RefreshProgress();

			if ((!(pck_isLastOptSuccess())) && pThis->bGoingToExit) {
				pThis->bGoingToExit = FALSE;
				pThis->SendMessage(WM_CLOSE, 0, 0);
			}
		}
	}
	return;
}


VOID TInstDlg::DeleteFileFromPckFile(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	HWND	hList = pThis->GetDlgItem(IDC_LIST);

	CStopWatch	timer;

	UINT uiSelectCount = ListView_GetSelectedCount(hList);

	if (0 != uiSelectCount) {

		TCHAR		szPrintf[64];

		int	nCurrentItemCount = ListView_GetItemCount(hList);

		LVITEM item;
		item.mask = LVIF_PARAM | LVIF_STATE;
		item.iSubItem = 0;
		item.stateMask = LVIS_SELECTED;		// get all state flags

		uiSelectCount = 0;

		for (item.iItem = 1; item.iItem < nCurrentItemCount; item.iItem++) {
			ListView_GetItem(hList, &item);

			if (item.state & LVIS_SELECTED) {
				//WINPCK_OK
				pck_DeleteEntry((LPPCK_UNIFIED_FILE_ENTRY)item.lParam);
				uiSelectCount++;
			}
		}

		if (0 == uiSelectCount)return;

		//¿ªÊ¼¼ÆÊ±
		timer.start();

		pThis->EnbaleButtons(ID_MENU_DELETE, FALSE);

		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
		pThis->SetStatusBarText(4, szPrintf);

		pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);


		if (WINPCK_OK == pck_RenameSubmit()) {
			//¼ÆÊ±½áÊø
			timer.stop();
			_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWOK), timer.getElapsedTime());

			pThis->SetStatusBarText(4, szPrintf);

			pThis->OpenPckFile(pThis->m_Filename, TRUE);

		}
		else {
			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

		pThis->EnbaleButtons(ID_MENU_DELETE, TRUE);

		pThis->KillTimer(WM_TIMER_PROGRESS_100);
		pThis->RefreshProgress();

		if (pThis->bGoingToExit) {
			pThis->bGoingToExit = FALSE;
			pThis->SendMessage(WM_CLOSE, 0, 0);
		}

	}
	return;
}

//´ÓÍÏÈëµÄÔ´ÎÄ¼þÃûÍÆ³öÔ¤±£´æµÄpckÎÄ¼þÃû
VOID GetPckFileNameBySource(LPTSTR dst, LPCTSTR src, BOOL isDirectory)
{
	int szPathToCompressLen;
	_tcscpy(dst, src);

	if (isDirectory) {
		if ((szPathToCompressLen = lstrlen(dst)) > 13 && 0 == lstrcmp(dst + szPathToCompressLen - 10, TEXT(".pck.files"))) {
			*(dst + szPathToCompressLen - 10) = 0;
		}
	}

	_tcscat(dst, TEXT(".pck"));
}