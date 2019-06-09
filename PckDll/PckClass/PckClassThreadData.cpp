#include "PckClassThread.h"
#include "PckModelStrip.h"

//ФЪ¶аПЯіМФЛЛгЦР»сИЎОґС№ЛхєГµДФґКэѕЭ
FETCHDATA_RET CPckClassThreadWorker::GetUncompressedDataFromFile(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex)
{

	while(1) {

		AcquireSRWLockExclusive(&g_mt_LockCompressedflag);
		if(lpDataFetchMethod->ciFilesList == lpDataFetchMethod->ciFilesListEnd) {
			ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);
			return FD_END;
		}
		vector<FILES_TO_COMPRESS>::const_pointer lpOneFile = &lpDataFetchMethod->ciFilesList[0];
		lpDataFetchMethod->ciFilesList++;
		ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);

#ifdef _DEBUG
		logOutput(__FUNCTION__, "lpfirstFile_id=%d\r\n", lpOneFile->id);
#endif
		LPBYTE lpCompressedBuffer = (BYTE*)MALLOCED_EMPTY_DATA;
		pckFileIndex.dwMallocSize = pThis->m_zlib.GetCompressBoundSizeByFileSize(pckFileIndex.cFileIndex.dwFileClearTextSize, pckFileIndex.cFileIndex.dwFileCipherTextSize, lpOneFile->dwFileSize);

		//№№ЅЁОДјюГы
		memcpy(mystrcpy(pckFileIndex.cFileIndex.szwFilename, lpDataFetchMethod->szCurrentNodeString), lpOneFile->szwFilename + lpOneFile->nFileTitleLen, lpOneFile->nBytesToCopy - lpDataFetchMethod->nCurrentNodeStringLen);
		//UnicodeОДјюГыЧЄ»»ОЄCP936µДANSI
		CPckClassCodepage::PckFilenameCode2Ansi(pckFileIndex.cFileIndex.szwFilename, pckFileIndex.cFileIndex.szFilename, sizeof(pckFileIndex.cFileIndex.szwFilename));

		//Из№ыОДјюґуРЎОЄ0Ј¬ФтМш№эґтїЄОДјюІЅЦи
		if(0 != pckFileIndex.cFileIndex.dwFileClearTextSize) {
			CMapViewFileRead		cFileRead;
			LPBYTE					lpBufferToRead;
			//ОДјюІ»ОЄ0К±µДґ¦Ан
			//ґтїЄТЄЅшРРС№ЛхµДОДјю
			if(NULL == (lpBufferToRead = cFileRead.OpenMappingViewAllRead(lpOneFile->szwFilename))) {
				pThis->SetErrMsgFlag(PCK_ERR_OPENMAPVIEWR);
				return FD_ERR;
			}

			//ЕР¶ПК№УГµДДЪґжКЗ·сі¬№эЧоґуЦµ
			FETCHDATA_RET rtn;
			if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpCompressedBuffer, pckFileIndex.dwMallocSize))) {
				return rtn;
			}

			if(PCK_BEGINCOMPRESS_SIZE < pckFileIndex.cFileIndex.dwFileClearTextSize) {
				pThis->m_zlib.compress(lpCompressedBuffer, &pckFileIndex.cFileIndex.dwFileCipherTextSize,
					lpBufferToRead, pckFileIndex.cFileIndex.dwFileClearTextSize);
			} else {
				memcpy(lpCompressedBuffer, lpBufferToRead, pckFileIndex.cFileIndex.dwFileClearTextSize);
			}

		}

		pckFileIndex.compressed_file_data = lpCompressedBuffer;
		return FD_OK;

	}
	return FD_END;
}

FETCHDATA_RET CPckClassThreadWorker::GetUncompressedDataFromPCK(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex)
{

	while(1) {

		AcquireSRWLockExclusive(&g_mt_LockCompressedflag);
		if(lpDataFetchMethod->dwProcessIndex >= lpDataFetchMethod->dwTotalIndexCount) {
			ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);
			return FD_END;
		}

		DATA_FETCH_METHOD cDataFetchMethod;
		memcpy(&cDataFetchMethod, lpDataFetchMethod, sizeof(DATA_FETCH_METHOD));
		++(lpDataFetchMethod->lpPckIndexTablePtrSrc);
		++(lpDataFetchMethod->dwProcessIndex);
		ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);

#ifdef _DEBUG
		logOutput(__FUNCTION__, "dwProcessIndex=%d\r\n", cDataFetchMethod.dwProcessIndex);
#endif

		if(cDataFetchMethod.lpPckIndexTablePtrSrc->isInvalid)
			continue;

		LPBYTE				lpBufferToRead;
		//±ЈґжЦШС№ЛхКэѕЭµДЅвС№µДКэѕЭ
		LPBYTE				lpDecompressBuffer = NULL;
		//±ЈґжЦШС№ЛхКэѕЭµДФґКэѕЭ
		LPBYTE				lpSourceBuffer = NULL;

		LPPCKINDEXTABLE	lpPckIndexTablePtrSrc = cDataFetchMethod.lpPckIndexTablePtrSrc;
		DWORD dwNumberOfBytesToMap = lpPckIndexTablePtrSrc->cFileIndex.dwFileCipherTextSize;
		DWORD dwFileClearTextSize = lpPckIndexTablePtrSrc->cFileIndex.dwFileClearTextSize;

		memcpy(&pckFileIndex.cFileIndex, &lpPckIndexTablePtrSrc->cFileIndex, sizeof(PCKFILEINDEX));

		if(PCK_BEGINCOMPRESS_SIZE < dwFileClearTextSize) {
			pckFileIndex.cFileIndex.dwFileCipherTextSize = pThis->m_zlib.compressBound(dwFileClearTextSize);
		}
		else {
			pckFileIndex.cFileIndex.dwFileCipherTextSize = dwFileClearTextSize;
		}

		LPBYTE lpCompressedBuffer = (BYTE*)MALLOCED_EMPTY_DATA;
		pckFileIndex.dwMallocSize = pckFileIndex.cFileIndex.dwFileCipherTextSize;

		if(0 != dwFileClearTextSize) {

			//ЕР¶ПК№УГµДДЪґжКЗ·сі¬№эЧоґуЦµ
			FETCHDATA_RET rtn;
			if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpCompressedBuffer, pckFileIndex.dwMallocSize))) {
				return rtn;
			}

			//ОДјюКэѕЭРиТЄЦШС№Лх
			if(PCK_BEGINCOMPRESS_SIZE < dwFileClearTextSize) {
				//±ЈґжФґКэѕЭµДїХјд
				if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpSourceBuffer, dwNumberOfBytesToMap))) {
					return rtn;
				}

				if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpDecompressBuffer, dwFileClearTextSize))) {
					return rtn;
				}

				AcquireSRWLockExclusive(&g_mt_LockReadFileMap);
				if(NULL == (lpBufferToRead = cDataFetchMethod.lpFileReadPCK->View(lpPckIndexTablePtrSrc->cFileIndex.dwAddressOffset, dwNumberOfBytesToMap))) {
					ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);
					pThis->freeMaxAndSubtractMemory(lpSourceBuffer, dwNumberOfBytesToMap);
					pThis->freeMaxAndSubtractMemory(lpDecompressBuffer, dwFileClearTextSize);

					return FD_ERR;
					break;
				}

				memcpy(lpSourceBuffer, lpBufferToRead, dwNumberOfBytesToMap);
				cDataFetchMethod.lpFileReadPCK->UnmapViewAll();
				ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);

				if (pThis->m_zlib.check_zlib_header(lpSourceBuffer)) {

					pThis->m_zlib.decompress(lpDecompressBuffer, &dwFileClearTextSize, lpSourceBuffer, dwNumberOfBytesToMap);
					
					if (dwFileClearTextSize == lpPckIndexTablePtrSrc->cFileIndex.dwFileClearTextSize) {

						/*
						ФЪХвАпјУИлѕ«јтґъВл
						*/
						if (PCK_STRIP_NONE != cDataFetchMethod.iStripFlag) {
							CPckModelStrip cModelStrip;
							cModelStrip.StripContent(lpDecompressBuffer, &pckFileIndex.cFileIndex, cDataFetchMethod.iStripFlag);
						}

						pThis->m_zlib.compress(lpCompressedBuffer, &pckFileIndex.cFileIndex.dwFileCipherTextSize, lpDecompressBuffer, pckFileIndex.cFileIndex.dwFileClearTextSize);
					}
					else {
						memcpy(lpCompressedBuffer, lpSourceBuffer, dwNumberOfBytesToMap);
						pckFileIndex.cFileIndex.dwFileCipherTextSize = lpPckIndexTablePtrSrc->cFileIndex.dwFileCipherTextSize;
					}
				
				}
				else {
					memcpy(lpCompressedBuffer, lpSourceBuffer, dwNumberOfBytesToMap);
					pckFileIndex.cFileIndex.dwFileCipherTextSize = lpPckIndexTablePtrSrc->cFileIndex.dwFileCipherTextSize;
				}

				pThis->freeMaxAndSubtractMemory(lpSourceBuffer, dwNumberOfBytesToMap);
				pThis->freeMaxAndSubtractMemory(lpDecompressBuffer, dwFileClearTextSize);


			} else {
#pragma region 文件过小不需要压缩时
				AcquireSRWLockExclusive(&g_mt_LockReadFileMap);
				if(NULL == (lpBufferToRead = cDataFetchMethod.lpFileReadPCK->View(lpPckIndexTablePtrSrc->cFileIndex.dwAddressOffset, dwNumberOfBytesToMap))) {
					ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);
					return FD_ERR;
					break;
				}
				memcpy(lpCompressedBuffer, lpBufferToRead, dwNumberOfBytesToMap);
				cDataFetchMethod.lpFileReadPCK->UnmapViewAll();
				ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);
#pragma endregion
			}

		}

		pckFileIndex.compressed_file_data = lpCompressedBuffer;
		return FD_OK;

	}
	return FD_END;
}
