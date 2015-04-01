#include "stdafx.h"

/*
http://0516.cootem.com
http://0598.059rc.com
http://198le.com
http://360shua.com
http://84zcb.com
http://admission.blcu.edu.cn
http://aiyowodemaya.com
http://alumni.blcu.edu.cn
http://arab.bfsu.edu.cn
http://bailian.duapp.com
http://bbs.miaogu.com
http://cdwnx.com
http://chumishi.com
http://cjpgj.com
http://cmbi.bjmu.edu.cn
http://dianmulu.com
http://dongdamen.net
http://dtdlylc.com
http://economy.caixun.com
http://epaichina.com
http://hyxb.net
http://khccts.com
http://maibaoxian.kblcw.com
http://mc25.com
http://mwxhf.com
http://sanms.com
http://sfes.cc
http://shdongao.net
http://smart029.com
http://srhui.com
http://szlnyy.net
http://taxrefund.com.cn
http://toto3.cc
http://votoro.com
http://woyaosf.com
http://www.020sjw.com
http://www.0663job.com
http://www.10jilu.com
http://www.173378.com
http://www.181tm.com
http://www.2500sc.com
http://www.30off.cn
http://www.360shua.com
http://www.54df.net
http://www.5800a.com
http://www.76.re
http://www.acne120.com
http://www.bestdong.com
http://www.booway.com.cn
http://www.ccpioo.com
http://www.cdwnx.com
http://www.ceec.net.cn
http://www.chinarhy.com
http://www.chumishi.com
http://www.cntexjob.com
http://www.cosco.com
http://www.cqtechang.com
http://www.gebufushi.cn
http://www.ggtv.cc
http://www.honkwin.com
http://www.hsmotor.com
http://www.ibaile.cc
http://www.instrnet.com
http://www.ip2525.com
http://www.jh3307.com
http://www.jieshafangsheng.com
http://www.mc25.com
http://www.meilijv.com
http://www.o6mr.com
http://www.omiwap.com
http://www.sou-biao.com
http://www.stackinsider.com
http://www.tqgcw.com
http://www.win7jia.com
http://www.woyaosf.com
http://www.ynssh.com
http://www.z8oo.com
http://www.zhaofupowang.com
http://www.zzcourt.gov.cn
http://www2.cdxph.com
http://xinlanglvyou.com
http://xinxxin.com
http://yes.omy88.com
http://yk.ay2fy.com
http://youth.bfsu.edu.cn
http://zssyb.com
*/

#ifdef OS_WIN

#ifdef COMPILE_DEMO_GOTOXY
extern "C" void gotoxy(HANDLE hOut, int x, int y)
{
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(hOut, pos); /* 设置光标位置 */
}

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>


#include<iostream>
using std::cout;
using std::endl;
#include<cmath>
using std::pow;

#define m 0.3
#define n 0.6
#define t 1.8
#define r 1.0   //some ratios


static int gotoxydemo()
{
	double myX, myY;
	double yourX, yourY;
	double width = 4.0; //width
	double heigh = 1.5; //heigh

	for (myY = heigh, yourY = heigh; myY >= -1.0, yourY >= -1.0; myY -= 0.1, yourY -= 0.1)// y position
	{
		for (myX = -3.0, yourX = -3.0; myX, yourX <= width; myX += 0.1, yourX += 0.1)// x position

		{
			if ((pow(n*pow(yourX, 2) + t*pow(yourY, 2) - 1.0, 3) - t*pow(yourX, 2)*pow(yourY, 3) >= 0)
				&& (pow(m*pow(myX, 2) + r*pow(myY, 2) - 1.0, 3) - r*pow(myX, 2)*pow(myY, 3) < 0))

				//the function (n*x^2+t*y^2-1)^3-t*x^2*y^3=0
				cout << "*";
			else
				cout << " ";
		}
		cout << endl;

	}
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	gotoxy(hOut, 22, 9);
	cout << "Dear Honey";
	gotoxy(hOut, 26, 11);
	cout << "I LOVE YOU";
	gotoxy(hOut, 35, 13);
	cout << "14-09-07";
	gotoxy(hOut, 0, 26);

	CloseHandle(hOut);
	return 0;
}

#endif /*COMPILE_DEMO_GOTOXY*/
#endif /* OS_WIN */