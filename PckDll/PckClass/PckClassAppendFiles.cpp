
#include "PckClassWriteOperator.h"

//РВЅЁЎўёьРВpck°ь
BOOL CPckClassWriteOperator::UpdatePckFile(LPCTSTR szPckFile, const vector<tstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert)
{
	DWORD		dwNewFileCount = 0;			//ОДјюКэБї, Ф­pckОДјюЦРµДОДјюКэ
	DWORD		dwDuplicateFileCount = 0;
	QWORD		qwTotalNewFileSize = 0;						//ОґС№ЛхК±ЛщУРОДјюґуРЎ

	int			level = m_lpPckParams->dwCompressLevel;
	int			threadnum = m_lpPckParams->dwMTThread;

	QWORD		dwAddressWhereToAppendData;
	THREAD_PARAMS		cThreadParams;

	//їЄКјІйХТОДјю
	const PCK_PATH_NODE*		lpNodeToInsertPtr;

#pragma region ЦШЦГС№ЛхІОКэ
	m_zlib.init_compressor(level);
#pragma endregion


#pragma region ЙиЦГІОКэ
	
	m_FilesToBeAdded.clear();
	m_PckAllInfo.lpFilesToBeAdded = &m_FilesToBeAdded;

	if(m_PckAllInfo.isPckFileLoaded) {

		lpNodeToInsertPtr = lpNodeToInsert;

		//ґУОДјюОІМнјУКэѕЭЈ¬ІЩЧчК§°ЬєуїЙ»ШНЛ
		dwAddressWhereToAppendData = m_PckAllInfo.qwPckSize;

		//ИЎµГµ±З°ЅЪµгµДПа¶ФВ·ѕ¶
		if(!GetCurrentNodeString(cThreadParams.cDataFetchMethod.szCurrentNodeString, lpNodeToInsert)) {
			assert(FALSE);
			return FALSE;
		}

		cThreadParams.cDataFetchMethod.nCurrentNodeStringLen = wcslen(cThreadParams.cDataFetchMethod.szCurrentNodeString);

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_ADD
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	} else {
		//РВЅЁОДјю
		//m_PckAllInfo.dwAddressOfFilenameIndex = PCK_DATA_START_AT;
		dwAddressWhereToAppendData = PCK_DATA_START_AT;

		lpNodeToInsertPtr = m_PckAllInfo.cRootNode.child;
		*cThreadParams.cDataFetchMethod.szCurrentNodeString = 0;
		cThreadParams.cDataFetchMethod.nCurrentNodeStringLen = 0;

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_NEW
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	}

	_tcscpy_s(m_PckAllInfo.szNewFilename, szPckFile);

#pragma endregion

#pragma region 遍历待添加的文件
	if(!EnumAllFilesByPathList(lpszFilePath, dwNewFileCount, qwTotalNewFileSize, m_PckAllInfo.lpFilesToBeAdded))
		return FALSE;

	if(0 == dwNewFileCount)return TRUE;

	m_PckAllInfo.dwFileCountToAdd = dwNewFileCount;
#pragma endregion
	//ІОКэЛµГчЈє
	// mt_dwFileCount	МнјУµДОДјюЧЬКэЈ¬јЖЦШёґ
	// dwFileCount		јЖЛг№эіМК№УГІОКэЈ¬ФЪПВГжµДјЖЛг№эіМЦРЅ«К№УГґЛІОКэ±нКѕМнјУµДОДјюЧЬКэЈ¬І»јЖЦШёґ

	//ОДјюКэРґИлґ°їЪАаЦР±ЈґжТФПФКѕЅш¶И
	SetParams_ProgressUpper(dwNewFileCount);

	//јЖЛгґуёЕРиТЄ¶аґуїХјдqwTotalFileSize
	mt_CompressTotalFileSize = GetPckFilesizeByCompressed(szPckFile, qwTotalNewFileSize, m_PckAllInfo.qwPckSize);

	//УлФ­АґДїВјЦРµДОДјю¶Ф±ИЈ¬КЗ·сУРЦШГы
	//ІЯВФЈєОЮМхјюёІёЗ°Й				Из№ыЦШГыЗТ¶јОЄОДјю»тОДјюјРЈ¬ФтёІёЗ
	//
	//µчУГFindFileNode·µ»Ш-1НЛіцЈ¬·µ»Ш0Ј¬±нКѕЦ±ЅУМнјУЈ¬·З0ѕНКЗУРЦШёґµД
	//РґЧЁУГµДwritethreadєНcompressthread,ТФµчУГ
	//ФЪPCKINDEXTABLE_COMPRESSАпМнјУaddЧЁУГКфРФЈ¬ТФЕР¶ПКЗ·сЖфУГґЛЅЪµгЈЁЦШГыК±Ј©0К№УГЈ¬1І»К№УГ
	//ЅбКш К±К№УГ2ёцС­»·РґИлС№ЛхЛчТэ 

	//dwFileCount±дБїФЪґЛґ¦ЦёµДКЗМнјУµДОДјюіэИҐЦШГыµДКэБї 
	if(m_PckAllInfo.isPckFileLoaded) {
		if(!FindDuplicateNodeFromFileList(lpNodeToInsertPtr, dwDuplicateFileCount))
			return FALSE;
	}

	//ИХЦѕ
	m_PckLog.PrintLogI(TEXT_UPDATE_FILE_INFO, m_PckAllInfo.dwFileCountToAdd, mt_CompressTotalFileSize);

#pragma region 创建目标文件
	CMapViewFileMultiPckWrite cFileWriter(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	//OPEN_ALWAYSЈ¬РВЅЁРВµДpck(CREATE_ALWAYS)»тёьРВґжФЪµДpck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(m_PckAllInfo.szNewFilename, OPEN_ALWAYS, mt_CompressTotalFileSize)) {
		return FALSE;
	}

#pragma endregion

	cThreadParams.pThis = (CPckClassThreadWorker*)this;
	cThreadParams.cDataFetchMethod.ciFilesList = m_PckAllInfo.lpFilesToBeAdded->cbegin();
	cThreadParams.cDataFetchMethod.ciFilesListEnd = m_PckAllInfo.lpFilesToBeAdded->cend();

	cThreadParams.GetUncompressedData = &GetUncompressedDataFromFile;

	//єЗєЗЈ¬ПВГжБЅёцєЇКэєГПсУАФ¶·µ»ШTRUE
	if(!initMultiThreadVars(&cFileWriter)) {
		return FALSE;
	}

	if(!initCompressedDataQueue(mt_dwFileCountOfWriteTarget = dwNewFileCount, dwAddressWhereToAppendData/*m_PckAllInfo.dwAddressOfFilenameIndex*/)) {
		return FALSE;
	}

	ExecuteMainThreadGroup(m_PckAllInfo, threadnum, &cThreadParams);
	dwAddressWhereToAppendData = m_PckAllInfo.dwAddressOfFilenameIndex;

	//РґОДјюЛчТэ
	m_PckAllInfo.dwFileCount = m_PckAllInfo.dwFileCountOld - dwDuplicateFileCount;

	WriteAllIndex(mt_lpFileWrite, &m_PckAllInfo, dwAddressWhereToAppendData);

	AfterProcess(mt_lpFileWrite, m_PckAllInfo, dwAddressWhereToAppendData);

	uninitCompressedDataQueue();

	//ФЪХвАпЦШРВґтїЄТ»ґОЈ¬»тХЯЦ±ЅУґтїЄЈ¬УЙЅзГжПЯіМНкіЙ
	m_lpPckParams->cVarParams.dwOldFileCount = m_PckAllInfo.dwFileCountOld;
	m_lpPckParams->cVarParams.dwPrepareToAddFileCount = dwNewFileCount;
	m_lpPckParams->cVarParams.dwChangedFileCount = m_PckAllInfo.dwFileCountToAdd;
	m_lpPckParams->cVarParams.dwDuplicateFileCount = dwDuplicateFileCount;
	//m_lpPckParams->cVarParams.dwUseNewDataAreaInDuplicateFileSize = m_PckAllInfo.dwFinalFileCount - m_PckAllInfo.dwFileCountOld;
	m_lpPckParams->cVarParams.dwFinalFileCount = m_PckAllInfo.dwFinalFileCount;

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;

}
