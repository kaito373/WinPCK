#include "PckClassWriteOperator.h"
#include "PckClassFileDisk.h"
#include "PckClassRebuildFilter.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4244 )

CPckClassWriteOperator::CPckClassWriteOperator()
{}

CPckClassWriteOperator::~CPckClassWriteOperator()
{}

/********************************
*
*№«№ІєЇКэ
*
********************/

BOOL CPckClassWriteOperator::RebuildPckFile(LPCTSTR lpszScriptFile, LPCTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	CPckClassRebuildFilter cScriptFilter;

	if ((nullptr != lpszScriptFile) && (0 != *lpszScriptFile)) 
		cScriptFilter.ApplyScript(lpszScriptFile, &m_PckAllInfo.cRootNode);

	return bUseRecompress ? RecompressPckFile(szRebuildPckFile) : RebuildPckFile(szRebuildPckFile);
}

BOOL CPckClassWriteOperator::StripPck(LPCTSTR lpszStripedPckFile, int flag)
{
	//КЧПИ№эВЛ*\textures\*.dds
	CPckClassRebuildFilter cScriptFilter;

	if (PCK_STRIP_DDS & flag) {
		
		cScriptFilter.StripModelTexture(
			m_PckAllInfo.lpPckIndexTable, 
			m_PckAllInfo.dwFileCount,
			m_PckAllInfo.cRootNode.child,
			m_PckAllInfo.szFileTitle
		);
	}
	return RecompressPckFile(lpszStripedPckFile, flag);
}

BOOL CPckClassWriteOperator::RebuildPckFile(LPCTSTR szRebuildPckFile)
{

	m_PckLog.PrintLogI(TEXT_LOG_REBUILD);

	
	QWORD	dwAddress = PCK_DATA_START_AT;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwValidFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRebuildPckFile, m_PckAllInfo.qwPckSize);

	//№№ФмН·єНОІК±РиТЄµДІОКэ
	PCK_ALL_INFOS		pckAllInfo;
	//№№ФмН·єНОІК±РиТЄµДІОКэ
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	_tcscpy_s(pckAllInfo.szNewFilename, szRebuildPckFile);

	//ПЯіМ±кјЗ
	SetThreadFlag(TRUE);

	//ЙиЦГЅзГжЅш¶ИМхЧЬЦµ
	SetParams_ProgressUpper(dwValidFileCount);

	//ґтїЄФґОДјю 
	CMapViewFileMultiPckRead	cFileRead;
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename)) 
		return FALSE;

	//ґтїЄДї±кОДјю 
	//ТФПВКЗґґЅЁТ»ёцОДјюЈ¬УГАґ±ЈґжЦШЅЁєуµДОДјю
	CMapViewFileMultiPckWrite	cFileWrite(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	if(!cFileWrite.OpenPckAndMappingWrite(szRebuildPckFile, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

	vector<PCKINDEXTABLE_COMPRESS> cPckIndexTable(dwValidFileCount);

	//І»К№УГEnumЅшРР±йАъґ¦АнЈ¬ёДУГ_PCK_INDEX_TABLE

	LPPCKINDEXTABLE lpPckIndexTableSource = pckAllInfo.lpPckIndexTable;
	pckAllInfo.dwFileCountToAdd = 0;

	for(DWORD i = 0; i < dwFileCount; ++i) {

		if(CheckIfNeedForcedStopWorking()) {
			m_PckLog.PrintLogW(TEXT_USERCANCLE);
			break;
		}

		if(lpPckIndexTableSource->isInvalid) {
			++lpPckIndexTableSource;
			continue;
		}

		LPBYTE lpBufferToRead;

		DWORD dwNumberOfBytesToMap = lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;
		DWORD dwSrcAddress = lpPckIndexTableSource->cFileIndex.dwAddressOffset;	//±ЈґжФ­АґµДµШЦ·

		if (0 != dwNumberOfBytesToMap) {

			if (NULL == (lpBufferToRead = cFileRead.View(dwSrcAddress, dwNumberOfBytesToMap))) {
				m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
				return FALSE;
			}

			cFileWrite.Write2(dwAddress, lpBufferToRead, dwNumberOfBytesToMap);
			cFileRead.UnmapViewAll();

		}

		//РґИлґЛОДјюµДPckFileIndexОДјюС№ЛхРЕПўІўС№Лх
		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwAddress;	//ґЛОДјюµДС№ЛхКэѕЭЖрКјµШЦ·

		dwAddress += dwNumberOfBytesToMap;	//ПВТ»ёцОДјюµДС№ЛхКэѕЭЖрКјµШЦ·

		FillAndCompressIndexData(&cPckIndexTable[pckAllInfo.dwFileCountToAdd], &lpPckIndexTableSource->cFileIndex);

		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwSrcAddress;	//»№Ф­Ф­АґµДµШЦ·

		++lpPckIndexTableSource;
		++(pckAllInfo.dwFileCountToAdd);
		SetParams_ProgressInc();

	}

	pckAllInfo.dwFileCountOld = pckAllInfo.dwFileCount = 0;
	pckAllInfo.lpPckIndexTableToAdd = &cPckIndexTable;

	//№Ш±Х¶БОДјю
	//РґОДјюЛчТэ
	pckAllInfo.dwAddressOfFilenameIndex = dwAddress;

	WriteAllIndex(&cFileWrite, &pckAllInfo, dwAddress);

	AfterProcess(&cFileWrite, pckAllInfo, dwAddress);

	//ПЯіМ±кјЗ
	SetThreadFlag(FALSE);

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;
}


//ЦШС№ЛхОДјю
BOOL CPckClassWriteOperator::RecompressPckFile(LPCTSTR szRecompressPckFile, int iStripMode)
{

	m_PckLog.PrintLogI(TEXT_LOG_RECOMPRESS);

	QWORD	dwAddress = PCK_DATA_START_AT;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRecompressPckFile, m_PckAllInfo.qwPckSize);

	THREAD_PARAMS		cThreadParams;
	CMapViewFileMultiPckRead	cFileRead;
	int					threadnum = m_lpPckParams->dwMTThread;

#pragma region ЦШЦГС№ЛхІОКэ
	m_zlib.init_compressor(m_lpPckParams->dwCompressLevel);
#pragma endregion

	//№№ФмН·єНОІК±РиТЄµДІОКэ
	PCK_ALL_INFOS		pckAllInfo;
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	_tcscpy_s(pckAllInfo.szNewFilename, szRecompressPckFile);

	//ЙиЦГЅзГжЅш¶ИМхЧЬЦµ
	SetParams_ProgressUpper(dwNoDupFileCount);

	//ґтїЄФґОДјю 
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename))
		return FALSE;

#pragma region 创建目标文件
	CMapViewFileMultiPckWrite cFileWriter(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	//OPEN_ALWAYSЈ¬РВЅЁРВµДpck(CREATE_ALWAYS)»тёьРВґжФЪµДpck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(pckAllInfo.szNewFilename, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

#pragma endregion

	cThreadParams.pThis = (CPckClassThreadWorker*)this;
	cThreadParams.cDataFetchMethod.lpFileReadPCK = &cFileRead;
	cThreadParams.cDataFetchMethod.iStripFlag = iStripMode;
	cThreadParams.cDataFetchMethod.dwProcessIndex = 0;
	cThreadParams.cDataFetchMethod.dwTotalIndexCount = pckAllInfo.dwFileCount;
	cThreadParams.cDataFetchMethod.lpPckIndexTablePtrSrc = pckAllInfo.lpPckIndexTable;
	cThreadParams.GetUncompressedData = &GetUncompressedDataFromPCK;

	if(!initMultiThreadVars(&cFileWriter)) {
		assert(FALSE);
		return FALSE;
	}

	if(!initCompressedDataQueue(dwFileCount, PCK_DATA_START_AT)) {
		assert(FALSE);
		return FALSE;
	}

	mt_dwFileCountOfWriteTarget = dwNoDupFileCount;

	ExecuteMainThreadGroup(pckAllInfo, threadnum, &cThreadParams);

	//РґОДјюЛчТэ

	pckAllInfo.lpPckIndexTable = NULL;
	pckAllInfo.dwFileCountOld = pckAllInfo.dwFileCount = 0;

	//ИЎПыєуЈ¬ОДјюКэБї
	dwAddress = pckAllInfo.dwAddressOfFilenameIndex;

	WriteAllIndex(&cFileWriter, &pckAllInfo, dwAddress);
	AfterProcess(&cFileWriter, pckAllInfo, dwAddress);

	uninitCompressedDataQueue();

	m_PckLog.PrintLogN(TEXT_LOG_WORKING_DONE);

	return TRUE;
}