#include "stdafx.h"
#include "XAtomicCounter.h"

/********************************************************************************************************************************************************************************

==============================================================================================================
||													Linux 原子操作											||
==============================================================================================================

所谓原子操作，就是该操作绝不会在执行完毕前被任何其他任务或事件打断，也就说，它的最小的执行单位，不可能有比它更小的执行单位，因此这里的原子实际是使用了物理学里的物质微粒的概念。
原子操作需要硬件的支持，因此是架构相关的，其API和原子类型的定义都定义在内核源码树的include / asm / atomic.h文件中，它们都使用汇编语言实现，因为C语言并不能实现这样的操作。
原子操作主要用于实现资源计数，很多引用计数(refcnt)就是通过原子操作实现的。原子类型定义如下：



typedef struct  { volatile int counter; }  atomic_t;
volatile修饰字段告诉gcc不要对该类型的数据做优化处理，对它的访问都是对内存的访问，而不是对寄存器的访问。


原子操作API包括：
1. atomic_read(atomic_t * v);
该函数对原子类型的变量进行原子读操作，它返回原子类型的变量v的值。

2. atomic_set(atomic_t * v, int i);
该函数设置原子类型的变量v的值为i。

3. void atomic_add(int i, atomic_t *v);
该函数给原子类型的变量v增加值i。

4. atomic_sub(int i, atomic_t *v);
该函数从原子类型的变量v中减去i。

5. int atomic_sub_and_test(int i, atomic_t *v);
该函数从原子类型的变量v中减去i，并判断结果是否为0，如果为0，返回真，否则返回假。

6. void atomic_inc(atomic_t *v);
该函数对原子类型变量v原子地增加1。

7. void atomic_dec(atomic_t *v);
该函数对原子类型的变量v原子地减1。

8. int atomic_dec_and_test(atomic_t *v);
该函数对原子类型的变量v原子地减1，并判断结果是否为0，如果为0，返回真，否则返回假。

9. int atomic_inc_and_test(atomic_t *v);
该函数对原子类型的变量v原子地增加1，并判断结果是否为0，如果为0，返回真，否则返回假。

10. int atomic_add_negative(int i, atomic_t *v);
该函数对原子类型的变量v原子地增加I，并判断结果是否为负数，如果是，返回真，否则返回假。

11. int atomic_add_return(int i, atomic_t *v);
该函数对原子类型的变量v原子地增加i，并且返回指向v的指针。

12. int atomic_sub_return(int i, atomic_t *v);
该函数从原子类型的变量v中减去i，并且返回指向v的指针。

13. int atomic_inc_return(atomic_t * v);
该函数对原子类型的变量v原子地增加1并且返回指向v的指针。

14. int atomic_dec_return(atomic_t * v);
该函数对原子类型的变量v原子地减1并且返回指向v的指针。

原子操作通常用于实现资源的引用计数，在TCP / IP协议栈的IP碎片处理中，就使用了引用计数，
碎片队列结构struct ipq描述了一个IP碎片，字段refcnt就是引用计数器，它的类型为atomic_t，
当创建IP碎片时（在函数ip_frag_create中），使用atomic_set函数把它设置为1，
当引用该IP碎片时，就使用函数atomic_inc把引用计数加1。
当不需要引用该IP碎片时，就使用函数ipq_put来释放该IP碎片，
ipq_put使用函数atomic_dec_and_test把引用计数减1并判断引用计数是否为0，
如果是就释放IP碎片。函数ipq_kill把IP碎片从ipq队列中删除，
并把该删除的IP碎片的引用计数减1（通过使用函数atomic_dec实现）。



==============================================================================================================
||										GCC>=4.1 支持的原子操作												||
==============================================================================================================

type __sync_fetch_and_add (type *ptr, type value);             //加  先取值，再做++
type __sync_fetch_and_sub (type *ptr, type value);             //减
type __sync_fetch_and_or (type *ptr, type value);                //或
type __sync_fetch_and_and (type *ptr, type value);             //与
type __sync_fetch_and_xor (type *ptr, type value);
type __sync_fetch_and_nand (type *ptr, type value);
type __sync_add_and_fetch (type *ptr, type value);
type __sync_sub_and_fetch (type *ptr, type value);
type __sync_or_and_fetch (type *ptr, type value);
type __sync_and_and_fetch (type *ptr, type value);
type __sync_xor_and_fetch (type *ptr, type value);
type __sync_nand_and_fetch (type *ptr, type value);

==============================================================================================================
||											C++1 支持CAS													||
==============================================================================================================
1）template< class T > bool atomic_compare_exchange_weak( std::atomic<T>* obj,T* expected, T desired );
2）template< class T > bool atomic_compare_exchange_weak( volatile std::atomic<T>* obj,T* expected, T desired );





==============================================================================================================
||											Windows下的原子操作												||
==============================================================================================================
自增1， 经过我反汇编验证，该函数是无锁操作！直接用编译器的LOCK EXG指令实现的！
LONG __cdecl InterlockedIncrement(LONG volatile* Addend);
LONG __cdecl InterlockedDecrement(LONG volatile* Addend);

long _InterlockedAnd( long volatile * value, long mask); *value和mask进行与运算，返回运算之前的结果


InterlockedOr
InterlockedXor

LONG InterlockedExchange( volatile*  pTarget, IN LONG  Value );以原子操作的方式交换俩个参数 pTarget , Value，并返回pTarget以前的值
LONG __cdecl InterlockedExchangeAdd(LONG volatile * Addend,  LONG Value); 对addend进行加法运算返回运算之前的值

LONG InterlockedCompareExchange(LPLONG Destination, LONG Exchange, LONG Comperand );
如果第三个参数与第一个参数指向的值相同，那么用第二个参数取代第一个参数指向的值。函数返回值为原始值。
这里用到了一个编译器指令，待研究
http://technet.microsoft.com/zh-cn/library/tzkfha43.aspx

**/




CXAtomicInt::CXAtomicInt(int value) : 	m_value(value)
{
}

int CXAtomicInt::operator++(void)
{
#ifdef OS_WIN
	return InterlockedIncrement(&m_value);
#else

#endif
}

/*! 自减量运算。 */
int CXAtomicInt::operator--(void)
{
#ifdef OS_LINUX
	return atomic_dec_return(&m_value);
#elif defined(OS_WIN)
	return InterlockedDecrement(&m_value);
#endif
}

/*! 增量运算。 */
int CXAtomicInt::operator+=(int change)
{
#ifdef OS_LINUX
#elif defined(OS_WIN)
 #if _MSC_VER>=1800 
	return InterlockedAdd(&m_value, change);
 #else
	return InterlockedExchangeAdd(&m_value, change);
 #endif
#else
#error "TODO:::"
#endif
}

/*! 减量运算。 */
int CXAtomicInt::operator-=(int change)
{
#ifdef OS_WIN
#if _MSC_VER>=1800 
	return InterlockedAdd(&m_value, -change);
#else
	return InterlockedExchangeAdd(&m_value, -change);
#endif
#else
#error "TODO:::"
#endif
}

#if 0
int CXAtomicInt::operator|=(int value)
{//TODO:::BUGS.. 这个不是原子操作
	return 0;
}

int CXAtomicInt::operator&=(int value)
{//TODO:::BUGS.. 这个不是原子操作
	return 0;
}

int CXAtomicInt::operator^=(int value)
{//TODO:::BUGS.. 这个不是原子操作
#ifdef OS_WIN
	InterlockedOr(&m_value, value);
#endif
	return m_value;
}

int CXAtomicInt::operator++(int)
{	//TODO:::BUGS.. 这个不是原子操作
	int value = m_value;
#ifdef OS_WIN
	InterlockedIncrement(&m_value);
#endif
	return value;
}

int CXAtomicInt::operator--(int)
{//TODO:::BUGS.. 这个不是原子操作
	int value = m_value;
#ifdef OS_WIN
	InterlockedDecrement(&m_value);
#endif
	return value;
}
#endif