
//////////////////////////////////////////////////////////////////////
// PckControlCenterOperation.cpp: УГУЪЅвОцНкГАКАЅз№«ЛѕµДpckОДјюЦРµДКэѕЭЈ¬ІўПФКѕФЪListЦР
// Н·ОДјю,ЅзГжУлPCKАаµДКэѕЭЅ»»ҐЈ¬їШЦЖЦРРД
//
// ґЛіМРтУЙ АоЗп·г/stsm/liqf ±аРґЈ¬pckЅб№№ІОїјИфЛ®µДpckЅб№№.txtЈ¬Іў
// ІОїјБЛЖдТЧУпСФґъВлЦРІўУЪ¶БpckОДјюБР±нµДІї·Ц
//
// ґЛґъВлФ¤јЖЅ«»бїЄФґЈ¬ИОєО»щУЪґЛґъВлµДРЮёД·ўІјЗл±ЈБфФ­ЧчХЯРЕПў
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include "ZupClass.h"
#include "PckClassZlib.h"
#include "PckClassLog.h"
#include "PckClassRebuildFilter.h"


void CPckControlCenter::New()
{
	Close();
	m_lpClassPck = new CPckClass(&cParams);
}

#pragma region 打开关闭文件

FMTPCK	CPckControlCenter::GetPckTypeFromFilename(LPCTSTR lpszFile)
{
	size_t nFileLength = _tcsnlen(lpszFile, MAX_PATH);

	if(0 == _tcsicmp(lpszFile + nFileLength - 4, TEXT(".pck"))) {
		return FMTPCK_PCK;
	} else if(0 == _tcsicmp(lpszFile + nFileLength - 4, TEXT(".zup"))) {
		return FMTPCK_ZUP;
	}
	return FMTPCK_UNKNOWN;
}

BOOL CPckControlCenter::Open(LPCTSTR lpszFile)
{
	//ЕР¶ПОДјюёсКЅ
	FMTPCK emunFileFormat = GetPckTypeFromFilename(lpszFile);

	while(1) {

		Close();
		switch(emunFileFormat) {
		case FMTPCK_ZUP:
			m_lpClassPck = new CZupClass(&cParams);
			break;
		case FMTPCK_PCK:
		default:
			m_lpClassPck = new CPckClass(&cParams);
			break;
		}

		m_lpPckLog->PrintLogI(TEXT(TEXT_LOG_OPENFILE), lpszFile);

		if(m_lpClassPck->Init(lpszFile)) {

			m_emunFileFormat = emunFileFormat;

			m_lpPckRootNode = m_lpClassPck->GetPckPathNode();

			//ґтїЄіЙ№¦Ј¬ЛўРВ±кМв
			pFeedbackCallBack(pTag, PCK_FILE_OPEN_SUCESS, NULL, (LPARAM)(m_lpClassPck->GetPckVersion()->name));
			return TRUE;

		} else {

			return FALSE;
		}
	}
}

void CPckControlCenter::Close()
{
	if(NULL != m_lpClassPck) {

		if(IsValidPck())
			m_lpPckLog->PrintLogI(TEXT_LOG_CLOSEFILE);

		delete m_lpClassPck;
		m_lpClassPck = NULL;

		m_lpPckRootNode = NULL;
	}

	m_emunFileFormat = FMTPCK_UNKNOWN;

	//ЗеїХІОКэ
	Reset();

	//№Ш±ХОДјюЈ¬ЛўРВ±кМв
	pFeedbackCallBack(pTag, PCK_FILE_CLOSE, NULL, NULL);
}

#pragma endregion

#pragma region ЦШГьГыЅЪµг

BOOL CPckControlCenter::RenameEntry(LPENTRY lpFileEntry, LPCWSTR lpszReplaceString)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	int entryType = lpFileEntry->entryType;
	if (PCK_ENTRY_TYPE_NODE == entryType) {

		return m_lpClassPck->RenameIndex((LPPCK_PATH_NODE)lpFileEntry, lpszReplaceString);
	}
	else if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entryType)) {

		return m_lpClassPck->RenameNode((LPPCK_PATH_NODE)lpFileEntry, lpszReplaceString);
	}
	else if (PCK_ENTRY_TYPE_INDEX == entryType)
	{
		return m_lpClassPck->RenameIndex((LPPCKINDEXTABLE)lpFileEntry, lpszReplaceString);
	}
	else {
		return FALSE;
	}
}
BOOL CPckControlCenter::RenameSubmit()
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->RenameFilename();
}

#pragma endregion

#pragma region 预览解压文件

//Ф¤ААОДјю
BOOL CPckControlCenter::GetSingleFileData(LPCENTRY lpFileEntry, char *buffer, size_t sizeOfBuffer)
{
	if ((NULL == m_lpClassPck) || (NULL == lpFileEntry))
		return FALSE;

	const PCKINDEXTABLE* lpPckFileIndexTable = NULL;
	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		lpPckFileIndexTable = (LPPCKINDEXTABLE)lpFileEntry;
	else
		lpPckFileIndexTable = ((LPPCK_PATH_NODE)lpFileEntry)->lpPckIndexTable;

	return m_lpClassPck->GetSingleFileData(lpPckFileIndexTable, buffer, sizeOfBuffer);
}

//ЅвС№ОДјю
BOOL CPckControlCenter::ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, LPCWSTR lpszDestDirectory)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->ExtractFiles(lpFileEntryArray, nEntryCount, lpszDestDirectory);
}

BOOL CPckControlCenter::ExtractAllFiles(LPCWSTR lpszDestDirectory)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->ExtractAllFiles(lpszDestDirectory);
}
#pragma endregion

#pragma region ЦШЅЁpckОДјю
//ЦШЅЁpckОДјю
BOOL CPckControlCenter::TestScript(LPCWSTR lpszScriptFile)
{
	CPckClassRebuildFilter cScriptFilter;

	return cScriptFilter.TestScript(lpszScriptFile);
}

BOOL CPckControlCenter::RebuildPckFile(LPCWSTR lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->RebuildPckFile(lpszScriptFile, szRebuildPckFile, bUseRecompress);
}

#pragma endregion

#pragma region 游戏精简
BOOL CPckControlCenter::StripPck(LPCWSTR lpszStripedPckFile, int flag)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->StripPck(lpszStripedPckFile, flag);
	
}

#pragma endregion

#pragma region 多个文件列表进行压缩时使用的vector操作
void CPckControlCenter::StringArrayReset()
{
	lpszFilePathToAdd.clear();
}

void CPckControlCenter::StringArrayAppend(LPCWSTR lpszFilePath)
{
	lpszFilePathToAdd.push_back(lpszFilePath);
}

#pragma endregion

#pragma region РВЅЁ/ёьРВpckОДјю

BOOL CPckControlCenter::UpdatePckFileSubmit(LPCWSTR szPckFile, LPCENTRY lpFileEntry)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	BOOL rtn = FALSE;

	if (0 != lpszFilePathToAdd.size()) {
		rtn = m_lpClassPck->UpdatePckFile(szPckFile, lpszFilePathToAdd, (const PCK_PATH_NODE*)lpFileEntry);
		StringArrayReset();
	}
	return rtn;
}
#if 0
BOOL CPckControlCenter::AddFileToPckFile(LPCTSTR lpszFilePathSrc, LPCTSTR szPckFile, const wchar_t *lpszPathInPckToAdd)
{
	CPckControlCenter* new_handle = new CPckControlCenter();
	BOOL rtn = FALSE;

	if (new_handle->Open(szPckFile)) {
		if (new_handle->IsValidPck()) {

			LPCENTRY lpFileEntry = new_handle->GetFileEntryByPath(lpszPathInPckToAdd);

			new_handle->StringArrayReset();
			new_handle->StringArrayAppend(lpszFilePathSrc);

			rtn = new_handle->UpdatePckFileSubmit(szPckFile, lpFileEntry);
		}
	}

	delete new_handle;

	return rtn;
}

BOOL CPckControlCenter::CreatePckFile(LPCTSTR lpszFilePathSrc, LPCTSTR szPckFile, int _versionId)
{

	CPckControlCenter* new_handle = new CPckControlCenter();
	BOOL rtn = FALSE;

	new_handle->SetPckVersion(_versionId);
	new_handle->StringArrayReset();
	new_handle->StringArrayAppend(lpszFilePathSrc);

	rtn = new_handle->UpdatePckFileSubmit(szPckFile, NULL);

	delete new_handle;

	return rtn;

}
#endif
#pragma endregion

#pragma region ЙѕіэЅЪµг
//ЙѕіэТ»ёцЅЪµг
BOOL CPckControlCenter::DeleteEntry(LPCENTRY lpFileEntry)
{
	if ((NULL == m_lpClassPck) || (NULL == lpFileEntry))
		return FALSE;

	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		m_lpClassPck->DeleteNode((LPPCKINDEXTABLE)lpFileEntry);
	else if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & lpFileEntry->entryType))
		m_lpClassPck->DeleteNode((LPPCK_PATH_NODE)lpFileEntry);
	else
		m_lpClassPck->DeleteNode(((LPPCK_PATH_NODE)lpFileEntry)->lpPckIndexTable);
	return TRUE;
}

//МбЅ»
BOOL CPckControlCenter::DeleteEntrySubmit()
{
	return RenameSubmit();
}

#pragma endregion


#pragma region 版本相关

BOOL CPckControlCenter::SetPckVersion(int verID)
{
	return m_lpClassPck->SetSavePckVersion(verID);
}

int	CPckControlCenter::GetPckVersion()
{
	return m_lpClassPck->GetPckVersion()->id;
}

//»сИЎµ±З°ЕдЦГГыіЖ
LPCWSTR	CPckControlCenter::GetCurrentVersionName()
{
	if (NULL == m_lpClassPck)
		return NULL;

	return m_lpClassPck->GetPckVersion()->name;
}

DWORD	CPckControlCenter::GetVersionCount()
{
	return CPckClassVersionDetect::GetPckVersionCount();
}

LPCWSTR	CPckControlCenter::GetVersionNameById(int verID)
{
	return CPckClassVersionDetect::GetPckVersionNameById(verID);
}

int CPckControlCenter::AddVersionAlgorithmId(int AlgorithmId, int Version)
{
	return CPckClassVersionDetect::AddPckVersion(AlgorithmId, Version);
}

#pragma endregion

#pragma region 备份的还原数据
#if 0
void CPckControlCenter::CreateRestoreData()
{
	if(FMTPCK_PCK == m_emunFileFormat) {
		if(!m_lpClassPck->GetPckBasicInfo(&m_RestoreInfomation)) {
			m_lpPckLog->PrintLogE(TEXT_ERROR_GET_RESTORE_DATA);
		}
	}
}

BOOL CPckControlCenter::RestoreData(LPCTSTR lpszFile, FMTPCK emunFileFormat)
{
	BOOL rtn = FALSE;
	if(FMTPCK_PCK == emunFileFormat) {
		if(m_RestoreInfomation.isValid) {

			//if(IDYES == MessageBoxA(m_hWndMain, TEXT_ERROR_OPEN_AFTER_UPDATE, TEXT_ERROR, MB_YESNO | MB_ICONHAND)) {

			if(PCK_FEEDBACK_YES == pFeedbackCallBack(pTag, PCK_FILE_NEED_RESTORE, NULL, NULL)) {
				if(0 == _tcsicmp(m_RestoreInfomation.szFile, lpszFile)) {

					if(!m_lpClassPck->SetPckBasicInfo(&m_RestoreInfomation))
						m_lpPckLog->PrintLogE(TEXT_ERROR_RESTORING);
					else {
						m_lpPckLog->PrintLogI(TEXT_LOG_RESTOR_OK);
						rtn = TRUE;
					}
				}
			}
			DeleteRestoreData();
		}
	}
	return rtn;
}

void CPckControlCenter::DeleteRestoreData()
{
	if(m_RestoreInfomation.isValid) {
		if(NULL != m_RestoreInfomation.lpIndexTailBuffer) {

			free(m_RestoreInfomation.lpIndexTailBuffer);
			m_RestoreInfomation.lpIndexTailBuffer = NULL;
		}
		m_RestoreInfomation.isValid = FALSE;
	}
}
#endif
#pragma endregion