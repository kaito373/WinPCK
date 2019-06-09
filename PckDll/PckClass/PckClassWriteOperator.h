#pragma once
#include "PckClassHeadTailWriter.h"
#include "PckClassIndexWriter.h"
#include "PckClassThread.h"


class CPckClassWriteOperator :
	public virtual CPckClassHeadTailWriter,
	public virtual CPckClassIndexWriter,
	public virtual CPckClassThreadWorker
{
public:
	CPckClassWriteOperator();
	~CPckClassWriteOperator();

	//ЦШЅЁpckОДјю
	virtual BOOL	RebuildPckFile(LPCTSTR lpszScriptFile, LPCTSTR szRebuildPckFile, BOOL bUseRecompress);

protected:
	BOOL	RebuildPckFile(LPCTSTR szRebuildPckFile);
	BOOL	RecompressPckFile(LPCTSTR szRecompressPckFile, int isStripMode = PCK_STRIP_NONE);

public:
#pragma region 游戏精简
	virtual BOOL	StripPck(LPCTSTR lpszStripedPckFile, int flag);
#pragma endregion

#pragma region PckClassAppendFiles.cpp
public:
	//РВЅЁЎўёьРВpckОДјю
	virtual BOOL	UpdatePckFile(LPCTSTR szPckFile, const vector<tstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert);
#pragma endregion
#pragma region PckClassRenamer.cpp

public:
	//ЦШГьГыОДјю
	virtual BOOL	RenameFilename();

#pragma endregion
};

