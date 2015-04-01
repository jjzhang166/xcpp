#include "stdafx.h"
#include "XAtomicCounter.h"

/********************************************************************************************************************************************************************************

==============================================================================================================
||													Linux ԭ�Ӳ���											||
==============================================================================================================

��νԭ�Ӳ��������Ǹò�����������ִ�����ǰ���κ�����������¼���ϣ�Ҳ��˵��������С��ִ�е�λ���������б�����С��ִ�е�λ����������ԭ��ʵ����ʹ��������ѧ�������΢���ĸ��
ԭ�Ӳ�����ҪӲ����֧�֣�����Ǽܹ���صģ���API��ԭ�����͵Ķ��嶼�������ں�Դ������include / asm / atomic.h�ļ��У����Ƕ�ʹ�û������ʵ�֣���ΪC���Բ�����ʵ�������Ĳ�����
ԭ�Ӳ�����Ҫ����ʵ����Դ�������ܶ����ü���(refcnt)����ͨ��ԭ�Ӳ���ʵ�ֵġ�ԭ�����Ͷ������£�



typedef struct  { volatile int counter; }  atomic_t;
volatile�����ֶθ���gcc��Ҫ�Ը����͵��������Ż����������ķ��ʶ��Ƕ��ڴ�ķ��ʣ������ǶԼĴ����ķ��ʡ�


ԭ�Ӳ���API������
1. atomic_read(atomic_t * v);
�ú�����ԭ�����͵ı�������ԭ�Ӷ�������������ԭ�����͵ı���v��ֵ��

2. atomic_set(atomic_t * v, int i);
�ú�������ԭ�����͵ı���v��ֵΪi��

3. void atomic_add(int i, atomic_t *v);
�ú�����ԭ�����͵ı���v����ֵi��

4. atomic_sub(int i, atomic_t *v);
�ú�����ԭ�����͵ı���v�м�ȥi��

5. int atomic_sub_and_test(int i, atomic_t *v);
�ú�����ԭ�����͵ı���v�м�ȥi�����жϽ���Ƿ�Ϊ0�����Ϊ0�������棬���򷵻ؼ١�

6. void atomic_inc(atomic_t *v);
�ú�����ԭ�����ͱ���vԭ�ӵ�����1��

7. void atomic_dec(atomic_t *v);
�ú�����ԭ�����͵ı���vԭ�ӵؼ�1��

8. int atomic_dec_and_test(atomic_t *v);
�ú�����ԭ�����͵ı���vԭ�ӵؼ�1�����жϽ���Ƿ�Ϊ0�����Ϊ0�������棬���򷵻ؼ١�

9. int atomic_inc_and_test(atomic_t *v);
�ú�����ԭ�����͵ı���vԭ�ӵ�����1�����жϽ���Ƿ�Ϊ0�����Ϊ0�������棬���򷵻ؼ١�

10. int atomic_add_negative(int i, atomic_t *v);
�ú�����ԭ�����͵ı���vԭ�ӵ�����I�����жϽ���Ƿ�Ϊ����������ǣ������棬���򷵻ؼ١�

11. int atomic_add_return(int i, atomic_t *v);
�ú�����ԭ�����͵ı���vԭ�ӵ�����i�����ҷ���ָ��v��ָ�롣

12. int atomic_sub_return(int i, atomic_t *v);
�ú�����ԭ�����͵ı���v�м�ȥi�����ҷ���ָ��v��ָ�롣

13. int atomic_inc_return(atomic_t * v);
�ú�����ԭ�����͵ı���vԭ�ӵ�����1���ҷ���ָ��v��ָ�롣

14. int atomic_dec_return(atomic_t * v);
�ú�����ԭ�����͵ı���vԭ�ӵؼ�1���ҷ���ָ��v��ָ�롣

ԭ�Ӳ���ͨ������ʵ����Դ�����ü�������TCP / IPЭ��ջ��IP��Ƭ�����У���ʹ�������ü�����
��Ƭ���нṹstruct ipq������һ��IP��Ƭ���ֶ�refcnt�������ü���������������Ϊatomic_t��
������IP��Ƭʱ���ں���ip_frag_create�У���ʹ��atomic_set������������Ϊ1��
�����ø�IP��Ƭʱ����ʹ�ú���atomic_inc�����ü�����1��
������Ҫ���ø�IP��Ƭʱ����ʹ�ú���ipq_put���ͷŸ�IP��Ƭ��
ipq_putʹ�ú���atomic_dec_and_test�����ü�����1���ж����ü����Ƿ�Ϊ0��
����Ǿ��ͷ�IP��Ƭ������ipq_kill��IP��Ƭ��ipq������ɾ����
���Ѹ�ɾ����IP��Ƭ�����ü�����1��ͨ��ʹ�ú���atomic_decʵ�֣���



==============================================================================================================
||										GCC>=4.1 ֧�ֵ�ԭ�Ӳ���												||
==============================================================================================================

type __sync_fetch_and_add (type *ptr, type value);             //��  ��ȡֵ������++
type __sync_fetch_and_sub (type *ptr, type value);             //��
type __sync_fetch_and_or (type *ptr, type value);                //��
type __sync_fetch_and_and (type *ptr, type value);             //��
type __sync_fetch_and_xor (type *ptr, type value);
type __sync_fetch_and_nand (type *ptr, type value);
type __sync_add_and_fetch (type *ptr, type value);
type __sync_sub_and_fetch (type *ptr, type value);
type __sync_or_and_fetch (type *ptr, type value);
type __sync_and_and_fetch (type *ptr, type value);
type __sync_xor_and_fetch (type *ptr, type value);
type __sync_nand_and_fetch (type *ptr, type value);

==============================================================================================================
||											C++1 ֧��CAS													||
==============================================================================================================
1��template< class T > bool atomic_compare_exchange_weak( std::atomic<T>* obj,T* expected, T desired );
2��template< class T > bool atomic_compare_exchange_weak( volatile std::atomic<T>* obj,T* expected, T desired );





==============================================================================================================
||											Windows�µ�ԭ�Ӳ���												||
==============================================================================================================
����1�� �����ҷ������֤���ú���������������ֱ���ñ�������LOCK EXGָ��ʵ�ֵģ�
LONG __cdecl InterlockedIncrement(LONG volatile* Addend);
LONG __cdecl InterlockedDecrement(LONG volatile* Addend);

long _InterlockedAnd( long volatile * value, long mask); *value��mask���������㣬��������֮ǰ�Ľ��


InterlockedOr
InterlockedXor

LONG InterlockedExchange( volatile*  pTarget, IN LONG  Value );��ԭ�Ӳ����ķ�ʽ������������ pTarget , Value��������pTarget��ǰ��ֵ
LONG __cdecl InterlockedExchangeAdd(LONG volatile * Addend,  LONG Value); ��addend���мӷ����㷵������֮ǰ��ֵ

LONG InterlockedCompareExchange(LPLONG Destination, LONG Exchange, LONG Comperand );
����������������һ������ָ���ֵ��ͬ����ô�õڶ�������ȡ����һ������ָ���ֵ����������ֵΪԭʼֵ��
�����õ���һ��������ָ����о�
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

/*! �Լ������㡣 */
int CXAtomicInt::operator--(void)
{
#ifdef OS_LINUX
	return atomic_dec_return(&m_value);
#elif defined(OS_WIN)
	return InterlockedDecrement(&m_value);
#endif
}

/*! �������㡣 */
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

/*! �������㡣 */
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
{//TODO:::BUGS.. �������ԭ�Ӳ���
	return 0;
}

int CXAtomicInt::operator&=(int value)
{//TODO:::BUGS.. �������ԭ�Ӳ���
	return 0;
}

int CXAtomicInt::operator^=(int value)
{//TODO:::BUGS.. �������ԭ�Ӳ���
#ifdef OS_WIN
	InterlockedOr(&m_value, value);
#endif
	return m_value;
}

int CXAtomicInt::operator++(int)
{	//TODO:::BUGS.. �������ԭ�Ӳ���
	int value = m_value;
#ifdef OS_WIN
	InterlockedIncrement(&m_value);
#endif
	return value;
}

int CXAtomicInt::operator--(int)
{//TODO:::BUGS.. �������ԭ�Ӳ���
	int value = m_value;
#ifdef OS_WIN
	InterlockedDecrement(&m_value);
#endif
	return value;
}
#endif