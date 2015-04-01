#ifndef __XCPP_PROJECT__
#error
#elif !defined(OS_WIN)
X_EXTERN_C XE_CHAR_TYPE* XE_DEFINE_XESTRINGFUNC(XStrUpr) (XE_CHAR_TYPE* pStrIn)
{
	_ASSERT(pStrIn);
	XE_CHAR_TYPE* pIter = pStrIn;

	while (*pIter != X_EOSA)
	{
		if (*pIter >= XE_T('a') && *pIter <= XE_T('z'))
		{
			*pIter -= 0x20; //0x20 = ' ' = 'A'-'a'
		}
		*pIter++;
	}

	return pStrIn;
}

X_EXTERN_C XE_CHAR_TYPE* XE_DEFINE_XESTRINGFUNC(XStrLwr) (XE_CHAR_TYPE* pStrIn)
{
	_ASSERT(pStrIn);
	XE_CHAR_TYPE* p = pStrIn;

	while (*p != X_EOSA)
	{
		if (*p >= XE_T('A') && *p <= XE_T('Z'))
			*p = (*p) + 0x20;
		p++;
	}

	return pStrIn;
}


X_EXTERN_C XE_CHAR_TYPE* XE_DEFINE_XESTRINGFUNC(XStrCpyN) (XE_CHAR_TYPE* pDest, const XE_CHAR_TYPE* pSrc, int iMaxLength)
{
	_ASSERT(pDest != NULL && pSrc != NULL);
	XE_CHAR_TYPE* p = pDest;
	for (int i = 0; i<iMaxLength && *pSrc != XE_T('\0'); ++i)
	{
		*p = *pSrc++;
		p++;
	}
	pDest[iMaxLength - 1] = X_EOS;
	return pDest;
}

#endif

X_EXTERN_C XE_CHAR_TYPE* XE_DEFINE_XESTRINGFUNC(XStrReplace)(IN OUT XE_CHAR_TYPE* pTxt, XE_CHAR_TYPE cFrom, XE_CHAR_TYPE cTo)
{
	for (XE_CHAR_TYPE* p = pTxt; *p != XE_T('\0'); ++p)
	{
		if (*p == cFrom)
		{
			*p = cTo;
		}
	}
	return pTxt;
}

X_EXTERN_C int XE_DEFINE_XESTRINGFUNC(XStrSplit) (IN XE_CHAR_TYPE* pszString, OUT XE_CHAR_TYPE** pszSubs, IN int iSubNum, IN XE_CHAR_TYPE szDelimit)
{
	_ASSERT(pszString != NULL);
	_ASSERT(pszSubs != NULL);

	XE_CHAR_TYPE* p2 = NULL;
	XE_CHAR_TYPE* p1 = pszString;
	int iCount = 0;
	for (iCount = 0; iCount < iSubNum; iCount++)
	{
		//		p2 = _tcschr(p1, szDelimit);
		p2 = XE_STRCHAR(p1, szDelimit);	//#include <wchar.h>
		if (p2)
		{
			pszSubs[iCount] = (XE_CHAR_TYPE*)p1;
			*p2 = X_EOSW;
			p1 = p2 + 1;
		}
		else
		{
			pszSubs[iCount] = (XE_CHAR_TYPE*)p1;
			return iCount + 1;
		}
	}

	return iCount;
}

static XE_CHAR_TYPE* XE_DEFINE_XESTRINGFUNC(mystrchrs) (XE_CHAR_TYPE* pszString, const XE_CHAR_TYPE szDlimis[], int iDelimit)
{
	XE_CHAR_TYPE* pStart = pszString;
	for (; *pStart != XE_T('\0'); pStart++)
	{
		for (int i = 0; i < iDelimit; i++)
		{
			if (szDlimis[i] == *pStart)
			{
				return pStart;
			}
		}
	}
	return NULL;
}

// �з��ַ�������ȥ��ǰ��ո�
X_EXTERN_C int XE_DEFINE_XESTRINGFUNC(XStrSplitMd) (IN XE_CHAR_TYPE* pszString, OUT XE_CHAR_TYPE** pszSubs, IN int iSubNum, IN const XE_CHAR_TYPE szDelimits[], IN int iDelimit)
{
	_ASSERT(pszString != NULL);
	_ASSERT(pszSubs != NULL);

	XE_CHAR_TYPE* p2 = NULL;
	XE_CHAR_TYPE* p1 = pszString;
	int i = 0;
	for (; i<iSubNum && *p1 != XE_T('\0'); i++)
	{
		p2 = XE_DEFINE_XESTRINGFUNC(mystrchrs)(p1, szDelimits, iDelimit);

		if (p2)
		{
			pszSubs[i] = (XE_CHAR_TYPE*)p1;
			*p2 = 0x00;
			p1 = p2 + 1;
		}
		else
		{
			pszSubs[i] = (XE_CHAR_TYPE*)p1;
			return i + 1;
		}
	}

	return i;
}

// ��ȥǰ���ͽ�β�Ŀո�ͻ��з�
X_EXTERN_C XE_CHAR_TYPE* XE_DEFINE_XESTRINGFUNC(XStrTrim) (XE_CHAR_TYPE* pIn)
{
	XE_CHAR_TYPE* p = pIn;
	// ��ȥǰ���ո�ͻ��з�
	while (*p == XE_T(' ') || *p == XE_T('\t') || *p == XE_T('\n') || *p == XE_T('\r'))
		p++;
	XE_CHAR_TYPE* ret = p;

	// ��ȡ��β�ո�ͻ��з�
	int len = (int)XE_STRLEN(p);
	if (len>0)
	{
		p += len - 1;
		while (*p == XE_T(' ') || *p == XE_T('\t') || *p == XE_T('\n') || *p == XE_T('\r'))
			p--;
		p[1] = XE_T('\0');
	}
	return ret;
}

// �з��ַ�������ȥǰ���ͽ�β�Ŀո�
X_EXTERN_C int XE_DEFINE_XESTRINGFUNC(XStrSplitEx) (IN XE_CHAR_TYPE* pszString, OUT XE_CHAR_TYPE** pszSubs, IN int iSubNum, IN XE_CHAR_TYPE szDelimit)
{
	int num = XE_DEFINE_XESTRINGFUNC(XStrSplit)(pszString, pszSubs, iSubNum, szDelimit);
	for (int i = 0; i<num; i++)
	{
		// ��ȥǰ���ո�ͻ��з�, ��ȡ��β�ո�ͻ��з�
		pszSubs[i] = XE_DEFINE_XESTRINGFUNC(XStrTrim)(pszSubs[i]);
	}
	return num;
}

X_EXTERN_C int XE_DEFINE_XESTRINGFUNC(XStrSplitMdEx) (IN XE_CHAR_TYPE* pszString, OUT XE_CHAR_TYPE** pszSubs, IN int iSubNum,
	IN const XE_CHAR_TYPE szDelimits[], IN int iDelimit)
{
	int num = XE_DEFINE_XESTRINGFUNC(XStrSplitMd)(pszString, pszSubs, iSubNum, szDelimits, iDelimit);
	for (int i = 0; i<num; i++)
	{
		// ��ȥǰ���ո�ͻ��з�, ��ȡ��β�ո�ͻ��з�
		pszSubs[i] = XE_DEFINE_XESTRINGFUNC(XStrTrim) (pszSubs[i]);
	}
	return num;
}

/////////////////////////////////////////////////////////////////////////////
// ��  �ܣ��ָ��ַ�����ȥ���ָ������ַ�����ǰ��ո񣬲������ַ�����ָ�����
//         pszSubs���顣
// ��  ����pszStrings=��Ҫ���ָ���ַ���
//         pszSubs=�ַ�ָ�����飬����ָ��ָ������ַ�����ָ��
//         nSubNums=����pszSubs�Ĵ�С
// ����ֵ�����طָ������ַ����ĸ���
// ��  ע�����ַ���֮��ķָ��ַ����Ʊ��TAB��ո�SPACE��
//         ���ָ�ĸ���������nSubNums��
X_EXTERN_C int XE_DEFINE_XESTRINGFUNC(XStrSplitBySpace) (IN OUT XE_CHAR_TYPE* pszStrings, OUT XE_CHAR_TYPE* pszSubs[], IN int nSubNum)
{
	_ASSERT(pszStrings != NULL);
	_ASSERT(pszSubs != NULL);

	XE_CHAR_TYPE* p1;
	XE_CHAR_TYPE* p2;

	// replace TAB / SPACE to NULL
	for (p2 = pszStrings; *p2 != 0x00; p2++)
	{
		if (*p2 == '\t' || *p2 == ' ')
			*p2 = 0x00;
	}

	int i = 0;
	for (p1 = pszStrings; p1<p2; p1++)
	{
		if (*p1 == 0x00)
			continue;

		if (i >= nSubNum)
			break;

		pszSubs[i++] = p1++;

		while (*p1 != 0x00)
			p1++;
	}
	return i;
}

X_EXTERN_C XE_CHAR_TYPE* XE_DEFINE_XESTRINGFUNC(XFormatFloat) (double value, int iPrecision, OUT XE_CHAR_TYPE* pBuffer, int iBufSize)
{
	_ASSERT(iBufSize >= 24);

	XE_SPRINTF(pBuffer, iBufSize - sizeof(XE_CHAR_TYPE), XE_T("%.*f"), iPrecision, value);
	pBuffer[iBufSize - 1] = 0x00;
	if (iPrecision <= 0)
		return pBuffer;

	int len = (int)XE_STRLEN(pBuffer);
	XE_CHAR_TYPE* p = pBuffer + len - 1;
	for (; p>pBuffer; p--)
	{
		if (*p == X_EOS)
			*p = 0x00;
		else
			break;
	}
	if (p>pBuffer && *p == '.')
		*p = 0x00;
	return pBuffer;
}
