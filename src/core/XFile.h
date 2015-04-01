#ifndef __XFILE_H__
#define __XFILE_H__

#include <string>

class CXFilePath
{
public:
	static TCHAR* Unix2Dos(TCHAR* pszPath);
	static TCHAR* Dos2Unix(TCHAR* pszPath);
	static TCHAR* GetFileDir(TCHAR* pszPath);
	static BOOL IsDirExist(const TCHAR* pszDir);
	static BOOL IsPathDir(const TCHAR* pszPath);
	static BOOL IsPathFile(const TCHAR* pszPath);
	/* @brief 测试一个文件活着目录是否存在
	@param pszPathFile[in] 要测试的文件名
	@return 存在返回TRUE,否则返回FALSE
	*/

	static BOOL IsFilePathExist(const TCHAR* pszPathFile);
	static TCHAR* NormalizeFileName(TCHAR* pszFileName);
};

class CXFile
{
public:
	CXFile();

	BOOL Attach(FILE* fp);
	void Detach();

	//TODO: GetLine();
	BOOL OpenWithCreateDir(const TCHAR* pszFileName, const TCHAR* pszFlags);
	BOOL Open(const TCHAR* pszFileName, const TCHAR* pszFlags, int mode=0);

	//Just For Read
	size_t GetSize() const;
	size_t GetOffset() const;

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