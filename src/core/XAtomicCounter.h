/*! XAtomic.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform Atomic Value class tools
related files		:  XThread.cpp
create date			:  2014-10-13
author				:  CHENQ
version				:  0.0.1
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------
TODO:

history :
2014/10/13			CHENQ	created
2014/10/15			CHENQ	finish min operation of CXAtomicInt in windows
*******************************************************************************/


#ifndef __XATOMIC_H__
#define __XATOMIC_H__

#ifdef OS_LINUX
typedef atomic_t XAtomic_t;
#elif defined(OS_WIN)
typedef volatile long XAtomic_t;
#endif

class CXAtomicInt
{
public:
	CXAtomicInt(int value=0);
	
	int operator+(int change)
	{
		return m_value + change;
	}

	int operator-(int change)
	{
		return m_value - change;
	}

	inline int operator&(int value)
	{
		return m_value&value;
	}
	inline int operator|(int value)
	{
		return m_value|value;
	}

	inline int operator~()
	{
		return ~m_value;
	}

	inline int operator^(int value)
	{
		return m_value^value;
	}

	CXAtomicInt& operator=(int value)
	{
		m_value = value;
		return *this;
	}

	//比较运算符
	bool operator==(int value) const
	{
		return m_value == value;
	}

	inline bool operator!=(int value) const
	{
		return m_value != value;
	}

	inline bool operator!() const
	{
		return m_value == 0;
	}

	/*! 类型转换，将该类转换为int型量。 */
	operator int() const
	{
		return m_value;
	}
	//++i
	int operator++();	//win OK
	int operator--();	//win OK

#if 0
	//i++
	int operator++(int);//TODO:!!
	int operator--(int);//TODO:!!
#endif

	int operator+=(int change);//win OK
	int operator-=(int change);//win OK

#if 0
	int operator|=(int value);//TODO:!!
	int operator&=(int value);//TODO:!!
	int operator^=(int value);//TODO:!!
#endif

private:
	XAtomic_t m_value;
};


template <typename T>
class CXAtomicPointer
{
	//TODO::
private:
	volatile T*  m_pValue;
};


#endif /*__XATOMIC_H__*/