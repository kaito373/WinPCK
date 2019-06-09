//////////////////////////////////////////////////////////////////////
// PckControlCenter.cpp: УГУЪЅвОцНкГАКАЅз№«ЛѕµДpckОДјюЦРµДКэѕЭЈ¬ІўПФКѕФЪListЦР
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
#include "PckClass.h"
#include "PckClassLog.h"
#include <thread>
	
//const DWORD	CPckControlCenter::m_dwNumberOfProcessors = thread::hardware_concurrency();

CPckControlCenter::CPckControlCenter():
	m_lpClassPck(NULL),
	m_lpPckRootNode(NULL),
	m_emunFileFormat(FMTPCK_UNKNOWN),
	cParams({ 0 })
{
	init();
	New();
}

CPckControlCenter::~CPckControlCenter()
{
	regMsgFeedback(NULL, DefaultFeedbackCallback);
	Close();
	uninit();
}

void CPckControlCenter::init()
{
	cParams.lpPckControlCenter = this;
	//cParams.code_page = 936;
	cParams.dwCompressLevel = getDefaultCompressLevel();
	cParams.dwMTThread = thread::hardware_concurrency();
	cParams.dwMTMaxMemory = getMaxMemoryAllowed();

	m_lpPckLog = new CPckClassLog();
}

void CPckControlCenter::uninit()
{
	lpszFilePathToAdd.clear();
	delete m_lpPckLog;
}

void CPckControlCenter::Reset(DWORD dwUIProgressUpper)
{
	memset(&cParams.cVarParams, 0, sizeof(PCK_VARIETY_PARAMS));
	cParams.cVarParams.dwUIProgressUpper = dwUIProgressUpper;
}

#pragma region 日志相关功能
//ИХЦѕ
void	CPckControlCenter::regShowFunc(ShowLogW _ShowLogW)
{
	CPckClassLog::PckClassLog_func_register(_ShowLogW);
}

const char CPckControlCenter::getLogLevelPrefix(int _loglevel)
{
	return CPckClassLog::GetLogLevelPrefix(_loglevel);
}

void CPckControlCenter::Print(const char chLevel, LPCSTR _fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}

void CPckControlCenter::Print(const char chLevel, LPCWSTR _fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}

void CPckControlCenter::Print(const char chLevel, LPCSTR _fmt, va_list ap)
{
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
}

void CPckControlCenter::Print(const char chLevel, LPCWSTR _fmt, va_list ap)
{
	CPckClassLog::PrintLog(chLevel, _fmt, ap);
}
#pragma endregion