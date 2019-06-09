//////////////////////////////////////////////////////////////////////
// CPckControlCenter.h: УГУЪЅвОцНкГАКАЅз№«ЛѕµДpckОДјюЦРµДКэѕЭЈ¬ІўПФКѕФЪListЦР
// Н·ОДјю,ЅзГжУлPCKАаµДКэѕЭЅ»»ҐЈ¬їШЦЖЦРРД
//
// ґЛіМРтУЙ АоЗп·г/stsm/liqf ±аРґЈ¬pckЅб№№ІОїјИфЛ®µДpckЅб№№.txtЈ¬Іў
// ІОїјБЛЖдТЧУпСФґъВлЦРІўУЪ¶БpckОДјюБР±нµДІї·Ц
//
// ґЛґъВлФ¤јЖЅ«»бїЄФґЈ¬ИОєО»щУЪґЛґъВлµДРЮёД·ўІјЗл±ЈБфФ­ЧчХЯРЕПў
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#pragma once

#include "pck_dependencies.h"

typedef struct _PCK_PATH_NODE * LPPCK_PATH_NODE;
typedef struct _PCK_RUNTIME_PARAMS * LPPCK_RUNTIME_PARAMS;

#if 0
#ifdef _WINDLL	//.dll
#define EXPORT_CLASS _declspec(dllexport) 
#elif defined(_DLL)	//.exe
#define EXPORT_CLASS _declspec(dllimport)
#else	//other
#define EXPORT_CLASS 
#endif
#else
#define EXPORT_CLASS 
#endif

class CPckClass;
class CPckClassLog;

class EXPORT_CLASS CPckControlCenter
{
	//єЇКэ
public:
	CPckControlCenter();
	virtual ~CPckControlCenter();

	void	New();

private:

	void	init();
	void	uninit();
	void	Reset(DWORD dwUIProgressUpper = 1);
#pragma region 打开关闭文件

	//BOOL	Open(LPCTSTR lpszFile, BOOL isOpenAfterRestore);
	void	Close();
public:

	BOOL	Open(LPCTSTR lpszFile);

#pragma endregion

#pragma region ЦШГьГыЅЪµг

	//ЦШГьГыТ»ёцЅЪµг
	BOOL	RenameEntry(LPENTRY lpFileEntry, LPCWSTR lpszReplaceString);
	//МбЅ»
	BOOL	RenameSubmit();

#pragma endregion

#pragma region 预览解压文件

	//Ф¤ААОДјю
	BOOL		GetSingleFileData(LPCENTRY lpFileEntry, char *buffer, size_t sizeOfBuffer = 0);

	//ЅвС№ОДјю
	BOOL		ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, LPCWSTR lpszDestDirectory);
	BOOL		ExtractAllFiles(LPCWSTR lpszDestDirectory);

#pragma endregion

#pragma region ЦШЅЁpckОДјю
	//ЦШЅЁpckОДјю
	BOOL	TestScript(LPCWSTR lpszScriptFile);
	BOOL	RebuildPckFile(LPCWSTR lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress);

#pragma endregion

#pragma region 游戏精简
	BOOL	StripPck(LPCWSTR lpszStripedPckFile, int flag);
#pragma endregion



#pragma region create or update pck file

	//¶аёцОДјюБР±нЅшРРС№ЛхК±К№УГµДvectorІЩЧч
	void	StringArrayReset();
	void	StringArrayAppend(LPCWSTR lpszFilePath);
	BOOL	UpdatePckFileSubmit(LPCWSTR szPckFile, LPCENTRY lpFileEntry);

#pragma endregion

#pragma region ЙѕіэЅЪµг
	//ЙѕіэТ»ёцЅЪµг
	BOOL	DeleteEntry(LPCENTRY lpFileEntry);
	//МбЅ»
	BOOL	DeleteEntrySubmit();

#pragma endregion


#pragma region 版本相关

	int		GetPckVersion();
	BOOL	SetPckVersion(int verID);
	//»сИЎµ±З°ЕдЦГГыіЖ
	LPCWSTR	GetCurrentVersionName();
	static DWORD	GetVersionCount();
	static LPCWSTR	GetVersionNameById(int verID);
	static int		AddVersionAlgorithmId(int AlgorithmId, int Version);

#pragma endregion

#pragma region ЅЪµгКфРФІЩЧч

	LPCENTRY GetRootNode();
	//»сИЎnodeВ·ѕ¶
	static BOOL			GetCurrentNodeString(LPWSTR szCurrentNodePathString, LPCENTRY lpFileEntry);
	LPCENTRY			GetFileEntryByPath(LPCWSTR _in_szCurrentNodePathString);
#pragma endregion

#pragma region pckОДјюКфРФ

	BOOL				IsValidPck();

	//»сИЎОДјюКэ
	DWORD				GetPckFileCount();
	//КµјКОДјюґуРЎ
	QWORD				GetPckSize();
	QWORD				GetPckDataAreaSize();
	QWORD				GetPckRedundancyDataSize();

	static QWORD		GetFileSizeInEntry(LPCENTRY lpFileEntry);
	static QWORD		GetCompressedSizeInEntry(LPCENTRY lpFileEntry);
	static DWORD		GetFoldersCountInEntry(LPCENTRY lpFileEntry);
	static DWORD		GetFilesCountInEntry(LPCENTRY lpFileEntry);

	static size_t		GetFilelenBytesOfEntry(LPCENTRY lpFileEntry);
	static size_t		GetFilelenLeftBytesOfEntry(LPCENTRY lpFileEntry);

	static QWORD		GetFileOffset(LPCENTRY lpFileEntry);

	//ЙиЦГёЅјУРЕПў
	const char*			GetAdditionalInfo();
	static DWORD		GetAdditionalInfoMaxSize();
	BOOL				SetAdditionalInfo(LPCSTR lpszAdditionalInfo);

	//КЗ·сКЗЦ§іЦёьРВµДОДјю
	BOOL				isSupportAddFileToPck();

#pragma endregion

	//ґтїЄЎў№Ш±ХЎўёґФ­µИКВјюЧўІб
	static void		regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack);
	static int		DefaultFeedbackCallback(void* pTag, int eventId, WPARAM wParam, LPARAM lParam);

#pragma region 查询及目录浏览
private:
	static void		DefaultShowFilelistCallback(void* _in_param, int sn, LPCWSTR lpszFilename, int entry_type, unsigned __int64 qwFileSize, unsigned __int64 qwFileSizeCompressed, void* fileEntry);

public:
	DWORD			SearchByName(LPCWSTR lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);
	static DWORD	ListByNode(LPCENTRY lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);

#pragma endregion

#pragma region ПЯіМїШЦЖ
	//ПЯіМФЛРРІОКэ
	BOOL			isThreadWorking();
	void			ForceBreakThreadWorking();
#pragma endregion

	//error no
	int				GetLastErrorMsg();
	BOOL			isLastOptSuccess();

#pragma region ДЪґжХјУГ
	//ДЪґжХјУГ
	DWORD			getMTMemoryUsed();
	//ДЪґжЦµ
	void			setMTMaxMemory(DWORD dwMTMaxMemory);
	DWORD			getMTMaxMemory();

	//ЧоґуДЪґж
	static DWORD	getMaxMemoryAllowed();

#pragma endregion

#pragma region ПЯіМКэ
	//ПЯіМКэ
	DWORD	getMaxThread();
	void	setMaxThread(DWORD dwThread);
	//ПЯіМД¬ИПІОКэ
	static DWORD	getMaxThreadUpperLimit();
#pragma endregion

#pragma region 压缩等级

	//С№ЛхµИј¶
	DWORD	getCompressLevel();
	void	setCompressLevel(DWORD dwCompressLevel = Z_DEFAULT_COMPRESS_LEVEL);

	//С№ЛхµИј¶Д¬ИПІОКэ
	static DWORD	getMaxCompressLevel();
	static DWORD	getDefaultCompressLevel();
#pragma endregion

#pragma region 进度相关

	DWORD	getUIProgress();
	void	setUIProgress(DWORD dwUIProgress);
	DWORD	getUIProgressUpper();

#pragma endregion


	//ИХЦѕ
	static void	regShowFunc(ShowLogW _ShowLogW);
	static const char	getLogLevelPrefix(int _loglevel);
	static void	Print(const char chLevel, LPCSTR _fmt, ...);
	static void	Print(const char chLevel, LPCWSTR _fmt, ...);
	static void	Print(const char chLevel, LPCSTR _fmt, va_list ap);
	static void	Print(const char chLevel, LPCWSTR _fmt, va_list ap);
#pragma endregion



#pragma region ґтУЎМнјУ/РВФцОДјюЅб№ы

	DWORD	GetUpdateResult_OldFileCount();
	DWORD	GetUpdateResult_PrepareToAddFileCount();
	DWORD	GetUpdateResult_ChangedFileCount();
	DWORD	GetUpdateResult_DuplicateFileCount();
	DWORD	GetUpdateResult_FinalFileCount();

#pragma endregion

private:

	FMTPCK	GetPckTypeFromFilename(LPCTSTR lpszFile);

	LPPCK_PATH_NODE				m_lpPckRootNode;
	vector<wstring>				lpszFilePathToAdd;	//¶ФМнјУ¶аёцОДјюК±Мб№©КэѕЭ

	wstring						szUpdateResultString;

	PCK_RUNTIME_PARAMS			cParams;
	CPckClass					*m_lpClassPck;
	CPckClassLog				*m_lpPckLog;

	//ёсКЅ
	FMTPCK						m_emunFileFormat;

	static FeedbackCallback		pFeedbackCallBack;
	static void*				pTag;

};