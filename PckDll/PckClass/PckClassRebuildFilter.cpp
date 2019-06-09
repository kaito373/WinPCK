//////////////////////////////////////////////////////////////////////
// PckClassRebuildFilter.cpp: УГУЪЅвОц№эВЛЅЕ±ѕ
//
// ґЛіМРтУЙ АоЗп·г/stsm/liqf ±аРґЈ¬Ії·ЦґъВлёДРґЧФRapidCRC
//
// ґЛґъВлїЄФґЈ¬ИОєО»щУЪґЛґъВлµДРЮёД·ўІјЗл±ЈБфФ­ЧчХЯРЕПў
// 
// 2018.5.15
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "MapViewFileMultiPck.h"
#include "PckClassRebuildFilter.h"
#include "CharsCodeConv.h"
#include "TextLineSpliter.h"

#define MAX_SCRIPT_SIZE	(10*1024*1024)

CPckClassRebuildFilter::CPckClassRebuildFilter()
{
}

CPckClassRebuildFilter::~CPckClassRebuildFilter()
{
	ResetRebuildFilterInIndexList();
}

#pragma region 读取文件并转换为Unicode

BOOL CPckClassRebuildFilter::OpenScriptFileAndConvBufToUcs2(LPCTSTR lpszScriptFile)
{
	char   * lpBufferToRead;
	CMapViewFileRead	cFileRead;
	
	CTextLineSpliter	cText2Line;

	//¶БИЎОДјюЛщУРЧЦ·ы
	if (nullptr == (lpBufferToRead = (char*)cFileRead.OpenMappingViewAllRead(lpszScriptFile))) 
		return FALSE;

	CTextConv2UCS2 cText2Ucs;
	const wchar_t* lpszUnicodeString = cText2Ucs.GetUnicodeString(lpBufferToRead, cFileRead.GetFileSize());

	cText2Line.SplitText(lpszUnicodeString, wcslen(lpszUnicodeString), m_ScriptLines, LINE_TRIM_LEFT | LINE_TRIM_RIGHT | LINE_EMPTY_DELETE);

	return TRUE;
}
#pragma endregion


BOOL CPckClassRebuildFilter::ParseOneLine(FILEOP * pFileOp, LPCWSTR lpszLine)
{
	wchar_t szOperator[16] = { 0 };
	//КЧПИјмІй16ёцЧЦ·ыДЪУРГ»УРїХёс»тtab
	const wchar_t *lpszCell = lpszLine, *lpszSearch = lpszLine;
	size_t count = 0;

	wchar_t *lpszOperator = szOperator;
	BOOL isValid = FALSE;

	while((16 > (++count)) &&
		(*lpszSearch)) {

		if((' ' == *lpszSearch) || ('\t' == *lpszSearch)) {
			isValid = TRUE;
			break;
		}
		*lpszOperator++ = *lpszSearch++;
	}

	if(!isValid)
		return FALSE;

	//ЅвОцszOperator
	const wchar_t *lpszOpPos = wcsstr(szOperators, szOperator);
	if(NULL == lpszOpPos)
		return FALSE;

	pFileOp->op = SCRIPTOP((lpszOpPos - szOperators) / 8);

	//№эВЛµфКЈПВµДїХёсєНtab
	while((' ' == *lpszSearch) || ('\t' == *lpszSearch))
		lpszSearch++;

	isValid = FALSE;
	count = 0;

	if((MAX_PATH <= wcslen(lpszSearch)) || (0 == *lpszSearch))
		return FALSE;

	wcscpy(pFileOp->szFilename, lpszSearch);

	////јмІйОДјюГыКЗ·сХэИ·
	//if(OP_CheckFile == pFileOp->op) {

	//	if(0 == _tcsicmp(lpszFileName, pFileOp->szFilename))

	//		return TRUE;
	//	else
	//		return FALSE;
	//}

	return TRUE;
}

//·ЦЅвЅЕ±ѕЦРµДДїВј
void CPckClassRebuildFilter::SepratePaths(FILEOP * pFileOp)
{
	wcscpy(pFileOp->szFilenameBuffer, pFileOp->szFilename);

	pFileOp->lpszSepratedPaths[0] = pFileOp->szFilenameBuffer;

	wchar_t *lpszSearch = pFileOp->szFilenameBuffer;
	int nPathDepthCount = 1;


	while(*lpszSearch) {

		wchar_t *test = wcschr(lpszSearch, L'\\');
		//ХвАпГ»УРїјВЗґжФЪ\\Л«Р±ёЬµДЗйїц
		if((L'\\' == *lpszSearch) || (L'/' == *lpszSearch)) {
			*lpszSearch = 0;
			++lpszSearch;
			pFileOp->lpszSepratedPaths[nPathDepthCount] = lpszSearch;
			++nPathDepthCount;
		}
		++lpszSearch;
	}

}

LPPCK_PATH_NODE CPckClassRebuildFilter::LocationFileIndex(LPWSTR *lpszPaths, LPPCK_PATH_NODE lpNode)
{
	LPCWSTR lpszSearchDir = *lpszPaths;

	if((NULL == lpszSearchDir) || (NULL == lpNode))
		return NULL;

	//Ц±ЅУМш№э..ДїВј
	LPPCK_PATH_NODE lpNodeSearch = lpNode->next;

	while(1) {

		if(NULL == lpNodeSearch)
			return NULL;

		if(0 == wcsicmp(lpszSearchDir, lpNodeSearch->szName)) {

			//КЗ·сТСѕ­ЖҐЕдНк
			if(NULL == *(lpszPaths + 1)) {

				return lpNodeSearch;
			} else {
				return LocationFileIndex(lpszPaths + 1, lpNodeSearch->child);
			}
		}
		lpNodeSearch = lpNodeSearch->next;
	}

	return NULL;
}

void CPckClassRebuildFilter::MarkFilterFlagToFileIndex(LPPCKINDEXTABLE	lpPckIndexTable, SCRIPTOP op)
{
	switch(op) {

	case OP_Protect:
		lpPckIndexTable->isProtected = TRUE;
		break;

	case OP_Delete:
	case OP_Rmdir:
		if((!lpPckIndexTable->isInvalid) && (!lpPckIndexTable->isProtected)) {

			lpPckIndexTable->isToDeDelete = TRUE;
			lpPckIndexTable->isInvalid = TRUE;
		}
		break;
	}
	m_EditedNode.push_back(lpPckIndexTable);
}

//Ѕ«Т»РРЅЕ±ѕДЪИЭУ¦УГµЅІйХТµЅµДОДјюБР±нЦР
void CPckClassRebuildFilter::MarkFilterFlagToNode(LPPCK_PATH_NODE lpNode, SCRIPTOP op)
{
	lpNode = lpNode->child->next;

	while(NULL != lpNode) {
		//if(NULL == lpNode->child) {
		if (PCK_ENTRY_TYPE_FOLDER & lpNode->entryType) {
			MarkFilterFlagToNode(lpNode, op);
		} else {
			MarkFilterFlagToFileIndex(lpNode->lpPckIndexTable, op);
		}

		lpNode = lpNode->next;
	}

}


#pragma region ApplyScript2IndexList,将脚本内容应用到文件列表中

//Ѕ«ЅЕ±ѕДЪИЭУ¦УГµЅОДјюБР±нЦР
BOOL CPckClassRebuildFilter::ApplyScript2IndexList(LPPCK_PATH_NODE lpRootNode)
{
	//ЅвОц№эіМКЗ·с·ўЙъБЛґнОу
	BOOL bHasErrorHappend = FALSE;
	m_EditedNode.clear();

	for(size_t i = 0;i< m_FirstFileOp.size();i++){

		FILEOP * pFileOp = &m_FirstFileOp[i];

		if(OP_CheckFile != pFileOp->op) {

			//·ЦЅвЅЕ±ѕЦРµДДїВј
			SepratePaths(pFileOp);

			//¶ЁО»ОДјюЛчТэ
			LPPCK_PATH_NODE lpFoundNode = LocationFileIndex(pFileOp->lpszSepratedPaths, lpRootNode->child);

			if(NULL == lpFoundNode) {

				m_PckLog.PrintLogW(TEXT("ТСЅвОцЅЕ±ѕК§°ЬФЪ: %s, Мш№э..."), pFileOp->szFilename);
				bHasErrorHappend = TRUE;

			} else {
				if(PCK_ENTRY_TYPE_FOLDER & lpFoundNode->entryType){
				//if(NULL != lpFoundNode->child) {

					MarkFilterFlagToNode(lpFoundNode, pFileOp->op);
				} else {
					MarkFilterFlagToFileIndex(lpFoundNode->lpPckIndexTable, pFileOp->op);
				}
			}

		}
	}

	return (!bHasErrorHappend);
}

#pragma endregion

BOOL CPckClassRebuildFilter::ParseScript(LPCTSTR lpszScriptFile)
{

	if (!OpenScriptFileAndConvBufToUcs2(lpszScriptFile)) {
		m_PckLog.PrintLogI("¶БИЎЅЕ±ѕК§°Ь");
		return FALSE; 
	}

	m_FirstFileOp.push_back(FILEOP{ 0 });
	FILEOP * pFileOp = &m_FirstFileOp.back();

	for (int i = 0; i < m_ScriptLines.size(); i++) {

		//№эВЛЧўКНРР
		if (L';' != m_ScriptLines[i].at(0)) {
			//Т»РРЅЕ±ѕ·ЦОЄБЅІї·ЦЈ¬ІЩЧчєНОДјюГы
			if (ParseOneLine(pFileOp, m_ScriptLines[i].c_str())) {

				m_FirstFileOp.push_back(FILEOP{ 0 });
				pFileOp = &m_FirstFileOp.back();

			}
			else {

				m_PckLog.PrintLogW("ЅЕ±ѕЅвОцК§°ЬФЪРР%d: %ls, Мш№э...", i, m_ScriptLines[i].c_str());

				return FALSE;
			}
		}
	}

	m_PckLog.PrintLogI("ЅвОцЅЕ±ѕіЙ№¦");
	return TRUE;
}

//ЗеіэµфЦШЅЁ°ьК±ЛщРиТЄ¶БИЎµД№эВЛРЕПў
void CPckClassRebuildFilter::ResetRebuildFilterInIndexList()
{
	for(DWORD i = 0;i < m_EditedNode.size();++i) {

		LPPCKINDEXTABLE lpPckIndexTable = m_EditedNode[i];

		if(lpPckIndexTable->isToDeDelete) {
			lpPckIndexTable->isInvalid = FALSE;
		}
		lpPckIndexTable->isProtected = lpPckIndexTable->isToDeDelete = FALSE;
	}
}

//У¦УГЅЕ±ѕДЪИЭ
BOOL CPckClassRebuildFilter::Apply(LPPCK_PATH_NODE lpRootNode)
{
	BOOL rtn = FALSE;

	//Ѕ«КэѕЭУ¦УГУЪtreeЦР
	rtn = ApplyScript2IndexList(lpRootNode);

	if (!rtn) {
		ResetRebuildFilterInIndexList();
		m_PckLog.PrintLogI("У¦УГЅЕ±ѕК§°Ь");
	}
	else {
		m_PckLog.PrintLogI("У¦УГЅЕ±ѕіЙ№¦");
	}

	return rtn;
}

BOOL CPckClassRebuildFilter::ApplyScript(LPCTSTR lpszScriptFile, LPPCK_PATH_NODE lpRootNode)
{
	if (!ParseScript(lpszScriptFile))
		return FALSE;

	return Apply(lpRootNode);
}

BOOL CPckClassRebuildFilter::TestScript(LPCTSTR lpszScriptFile)
{
	return ParseScript(lpszScriptFile);
}


BOOL CPckClassRebuildFilter::ModelTextureCheck(LPCWSTR lpszFilename)
{
	//В·ѕ¶№жФтЈ¬*\textures\*.dds

	LPCWSTR constTexturePath = L"\\textures\\";
	LPCWSTR constDdsExt = L".dds";
	LPCWSTR constTgaExt = L".tga";

	LPCWSTR lpszTexturePath = wcsstr(lpszFilename, constTexturePath);

	if (nullptr == lpszTexturePath)
		return FALSE;

	lpszTexturePath += wcslen(constTexturePath);

	LPCWSTR subdir = wcschr(lpszTexturePath, L'\\');
	if (nullptr != subdir) {
		if (nullptr != wcschr(subdir+1, L'\\'))
			return FALSE;
	}

	lpszTexturePath += wcslen(lpszTexturePath) - wcslen(constDdsExt);
	if (0 == wcsicmp(lpszTexturePath, constDdsExt))
		return TRUE;

	if (0 == wcsicmp(lpszTexturePath, constTgaExt))
		return TRUE;

	return FALSE;
}

void CPckClassRebuildFilter::StripModelTexture(LPPCKINDEXTABLE lpPckIndexHead, DWORD dwFileCount, LPPCK_PATH_NODE lpRootNode, LPCWSTR lpszPckFilename)
{

	int nDetectOffset = 0;
#if 0
	//јмІвpckДїВјЦРµДОДјюјРКЗ·сТФpckОДјюГыїЄН·
	//Изgfx.pckЦРёщДїВјОЄgfxЗТЦ»УРТ»ёц
	LPPCK_PATH_NODE lpRootNodeFirstDir = lpRootNode->next;
	int nRootDirCount = 0;
	vector<wstring> sRootDirs;

	while (NULL != lpRootNodeFirstDir) {

		if (PCK_ENTRY_TYPE_FOLDER & lpRootNodeFirstDir->entryType) {

			sRootDirs.push_back(lpRootNodeFirstDir->szName);
			++nRootDirCount;
		}
		lpRootNodeFirstDir = lpRootNodeFirstDir->next;
	}

	//Ц»УР3ёцІ»µЅµДёщДїВјЈ¬ИЎёщДїВјГы,¶Ф±ИОДјюГы
	if (3 > nRootDirCount) {
		wchar_t szFileTitle[MAX_PATH];
		wchar_t* lpszExt = nullptr;

		wcscpy_s(szFileTitle, lpszPckFilename);

		if (nullptr != (lpszExt = wcsrchr(szFileTitle, L'.'))) {

			*lpszExt = 0;
		}

		//¶Ф±И
		for (int i = 0; i < sRootDirs.size(); i++) {

			if (nullptr != wcsstr(szFileTitle, sRootDirs[i].c_str())) {

				nDetectOffset = sRootDirs[i].length() + 1;
				break;
			}
		}
	}
#endif

	LPPCKINDEXTABLE lpPckIndexTable = lpPckIndexHead;

	for (DWORD i = 0; i < dwFileCount; i++) {

		if (ModelTextureCheck(lpPckIndexTable->cFileIndex.szwFilename + nDetectOffset)) {
			if (!lpPckIndexTable->isInvalid) {

				lpPckIndexTable->isToDeDelete = TRUE;
				lpPckIndexTable->isInvalid = TRUE;

				m_EditedNode.push_back(lpPckIndexTable);
			}
		}
		lpPckIndexTable++;
	}
}