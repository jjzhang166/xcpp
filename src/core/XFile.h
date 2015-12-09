#ifndef __XFILE_H__
#define __XFILE_H__

#include "XSTLMacro.h"
#include "XEString.h"

#ifdef OS_WIN
#define DEFAULT_XFILEOPEN_MODE _SH_DENYNO
#else
#define DEFAULT_XFILEOPEN_MODE 0
#endif

class CXFilePathW
{
public:
#if (XE_CHAR_MODE & XE_CHAR_UNICODE)
	static TCHAR* Unix2Dos(TCHAR* pszPath);
	static TCHAR* Dos2Unix(TCHAR* pszPath);
	static TCHAR* GetFileDir(TCHAR* pszPath);
	static XSTLString Unix2Dos(const XSTLString& unixPath);
	static XSTLString Dos2Unix(const XSTLString& dosPath);
	static BOOL IsDirExist(const TCHAR* pszDir);
	static BOOL IsPathDir(const TCHAR* pszPath);
	static BOOL IsPathRelative(const TCHAR* pszPath);
	static BOOL IsPathFile(const TCHAR* pszPath);
	static BOOL RemoveFile(const TCHAR* pszPath);
	static BOOL CopyFile(const TCHAR* pszSrcFile, const TCHAR* pszDstDir);
	static BOOL FullPath(const TCHAR* pszRelPath, TCHAR* pszAbsPath, size_t bufLen);
	/* @brief 测试一个文件活着目录是否存在
	@param pszPathFile[in] 要测试的文件名
	@return 存在返回TRUE,否则返回FALSE
	*/
	static BOOL IsFilePathExist(const TCHAR* pszPathFile);
	static TCHAR* XE_DEFINE_XESTRINGFUNCW(NormalizeFileName)(TCHAR* pszFileName);
#endif
#if (XE_CHAR_MODE & XE_CHAR_ANSI)
	static char* XE_DEFINE_XESTRINGFUNCA(Unix2Dos)(char* pszPath);
	static char* XE_DEFINE_XESTRINGFUNCA(Dos2Unix)(char* pszPath);
	static char* XE_DEFINE_XESTRINGFUNCA(GetFileDir)(char* pszPath);
	static XSTLStringA XE_DEFINE_XESTRINGFUNCA(Unix2Dos)(const XSTLString& unixPath);
	static XSTLStringA XE_DEFINE_XESTRINGFUNCA(Dos2Unix)(const XSTLString& dosPath);
	static BOOL XE_DEFINE_XESTRINGFUNCA(IsDirExist)(const char* pszDir);
	static BOOL XE_DEFINE_XESTRINGFUNCA(IsPathDir)(const char* pszPath);
	static BOOL XE_DEFINE_XESTRINGFUNCA(IsPathRelative)(const char* pszPath);
	static BOOL XE_DEFINE_XESTRINGFUNCA(IsPathFile)(const char* pszPath);
	static BOOL XE_DEFINE_XESTRINGFUNCA(RemoveFile)(const char* pszPath);
	static BOOL XE_DEFINE_XESTRINGFUNCA(CopyFile)(const char* pszSrcFile, const char* pszDstDir);
	static BOOL XE_DEFINE_XESTRINGFUNCA(FullPath)(const char* pszRelPath, char* pszAbsPath, size_t bufLen);
	/* @brief 测试一个文件活着目录是否存在
	@param pszPathFile[in] 要测试的文件名
	@return 存在返回TRUE,否则返回FALSE
	*/
	static BOOL XE_DEFINE_XESTRINGFUNCA(IsFilePathExist)(const char* pszPathFile);
	static char* XE_DEFINE_XESTRINGFUNCA(NormalizeFileName)(char* pszFileName);
#endif
};

class CXFile
{
public:
	CXFile();

	BOOL Attach(FILE* fp);
	void Detach();

	//TODO: GetLine();
	BOOL OpenWithCreateDir(const TCHAR* pszFileName, const TCHAR* pszFlags);
	BOOL Open(const TCHAR* pszFileName, const TCHAR* pszFlags, int mode=DEFAULT_XFILEOPEN_MODE);

	//Just For Read
	size_t GetSize() const;
	size_t GetOffset() const;

	//Seek
	BOOL Seek(ssize_t offset, int whence);
	void Rewind();

	ssize_t OutString(const char* pFmt, ...);
	void Flush();
	void Close();

	ssize_t Write(const void* pBuffer, size_t bufLen);
	ssize_t Read(void* pBuffere, size_t bufLen);

	virtual ~CXFile();
	inline BOOL IsValid() const { return m_fp != NULL; }

private:
	FILE* m_fp;
};

class CXFileFd
{
public:
	CXFileFd();

	BOOL Attach(FILE* fp);
	BOOL Attach(int fd);
	void Detach();

	BOOL OpenWithCreateDir(const TCHAR* pszFileName, int flags, int mode);
	BOOL Open(const TCHAR* pszFileName, int flags, int mode=0);

	ssize_t OutString(const char* pFmt, ...);
	
	void Flush();
	void Close();

	ssize_t Write(const void* pBuffer, size_t bufLen);
	ssize_t Read(void* pBuffere, size_t bufLen);

	virtual ~CXFileFd();
	inline BOOL IsValid() const { return m_fd != -1; }

private:
	int m_fd;
};

#define X_FILEPATH_DOS2UNIX CXFilePath::Dos2Unix
#define X_FILEPATH_UNIX2DOS CXFilePath::Unix2Dos
#define X_FILEPATH_GETDIR	CXFilePath::GetFileDir
#define X_FILEPATH_ISDIREXIST CXFilePath::IsDirExist
#define X_FILE_NORMALIZE	CXFilePath::NormalizeFileName

#endif /*__XFILE_H__*/