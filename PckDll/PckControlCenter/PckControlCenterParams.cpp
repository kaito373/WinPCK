//////////////////////////////////////////////////////////////////////
// PckControlCenterParams.cpp: УГУЪЅвОцНкГАКАЅз№«ЛѕµДpckОДјюЦРµДКэѕЭЈ¬ІўПФКѕФЪListЦР
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
#include <thread>

#pragma region ПЯіМїШЦЖ

//ПЯіМФЛРРІОКэ
BOOL CPckControlCenter::isThreadWorking()
{
	return cParams.cVarParams.bThreadRunning;
}

void CPckControlCenter::ForceBreakThreadWorking()
{
	cParams.cVarParams.bForcedStopWorking = TRUE;
}

#pragma endregion


#pragma region ПЯіМИООсРЕПў
//
int CPckControlCenter::GetLastErrorMsg()
{
	return cParams.cVarParams.errMessageNo;
}

BOOL CPckControlCenter::isLastOptSuccess()
{
	return (PCK_OK == GetLastErrorMsg());
}

#pragma endregion

#pragma region ДЪґжХјУГ
//ДЪґжХјУГ
DWORD CPckControlCenter::getMTMemoryUsed()
{
	return cParams.cVarParams.dwMTMemoryUsed;
}

//ДЪґжЦµ
void CPckControlCenter::setMTMaxMemory(DWORD dwMTMaxMemory)
{
	if ((0 < dwMTMaxMemory) && (getMaxMemoryAllowed() >= dwMTMaxMemory)) {
		cParams.dwMTMaxMemory = dwMTMaxMemory;
	}
	else {
		cParams.dwMTMaxMemory = getMaxMemoryAllowed();
	}
}

DWORD CPckControlCenter::getMTMaxMemory()
{
	return cParams.dwMTMaxMemory;
}

//ЧоґуДЪґжgetDefaultMaxMemoryAllowed
DWORD CPckControlCenter::getMaxMemoryAllowed()
{
	return MT_MAX_MEMORY;
}

#pragma endregion

#pragma region ПЯіМКэ


DWORD CPckControlCenter::getMaxThread()
{
	return cParams.dwMTThread;
}

void CPckControlCenter::setMaxThread(DWORD dwThread)
{
	cParams.dwMTThread = dwThread;
}

//ПЯіМД¬ИПІОКэ
DWORD CPckControlCenter::getMaxThreadUpperLimit()
{
	return (thread::hardware_concurrency() + ((thread::hardware_concurrency() + (thread::hardware_concurrency() & 1)) >> 1));
}

#pragma endregion

#pragma region 压缩等级

//С№ЛхµИј¶
DWORD CPckControlCenter::getCompressLevel()
{
	return cParams.dwCompressLevel;
}

void CPckControlCenter::setCompressLevel(DWORD dwCompressLevel)
{
	cParams.dwCompressLevel = dwCompressLevel;
}

//С№ЛхµИј¶Д¬ИПІОКэ
//ІОКэД¬ИПЦµ
DWORD CPckControlCenter::getDefaultCompressLevel()
{
	return Z_DEFAULT_COMPRESS_LEVEL;
}

DWORD CPckControlCenter::getMaxCompressLevel()
{
	return MAX_COMPRESS_LEVEL;
}

#pragma endregion


#pragma region 进度相关

DWORD CPckControlCenter::getUIProgress()
{
	return cParams.cVarParams.dwUIProgress;
}

void CPckControlCenter::setUIProgress(DWORD dwUIProgress)
{
	cParams.cVarParams.dwUIProgress = dwUIProgress;
}

DWORD CPckControlCenter::getUIProgressUpper()
{
	return cParams.cVarParams.dwUIProgressUpper;
}

//void CPckControlCenter::setUIProgressUpper(DWORD dwUIProgressUpper)
//{
//	cParams.cVarParams.dwUIProgressUpper = dwUIProgressUpper;
//}

#pragma endregion

#pragma region ґтУЎМнјУ/РВФцОДјюЅб№ы

DWORD CPckControlCenter::GetUpdateResult_OldFileCount()
{
	return cParams.cVarParams.dwOldFileCount;
}

DWORD CPckControlCenter::GetUpdateResult_PrepareToAddFileCount()
{
	return cParams.cVarParams.dwPrepareToAddFileCount;
}

DWORD CPckControlCenter::GetUpdateResult_ChangedFileCount()
{
	return cParams.cVarParams.dwChangedFileCount;
}

DWORD CPckControlCenter::GetUpdateResult_DuplicateFileCount()
{
	return cParams.cVarParams.dwDuplicateFileCount;
}

DWORD CPckControlCenter::GetUpdateResult_FinalFileCount()
{
	return cParams.cVarParams.dwFinalFileCount;
}

#pragma endregion
