# Welcome to the XCPP wiki!
<img src="resources/1.png">

[XCPP开源项目](http://git.oschina.net/ccb/XCPP)

## 概述
	XCPP 是一个精简的、高效的跨平台C++开发库.
	
	X代表cross，跨平台特性;	CPP就是C++(cplusplus)
	
	XCPP的目标是用朴实的C++编程语言实现一个跨平台的C++开发库。

## 用处
	1. 编写跨平台程序，本API支持windows/Linux;
	2. 移植跨平台程序，将已有程序平台相关代码用本库的API替换，使之具有跨平台特性;


## 理念和原则：
	1. 简化API操作;
	2. 风格尽量接近于操作系统原语，去除平台各自独有的特性;
	3. 尽量使用NATIVE API以提高运行效率;
	4. 尽量不引用第三方库以降低耦合性;
	5. 使用简介实用的编程风格以提高可读性;
	
	*  目前的一些设计思想有些是学习[chromium](http://dev.chromium.org/)、[Qt](http://qt-project.org)等知名开源项目


## 功能特性：
	1. Support Windows and Linux
	2. Atomic operation
	3. UDP/TCP(client and server) Socket
	4. Process
	5. Thread
	6. Sempehpore binary、mutex、counter
	7. misc utilities
	8. hash string

## 其他
	当前版本在XCPP工程（V0.7.2） fork而来，相当于XCPP的Lite版本。
	版本号直接从0.7.2的基础上延续了，刚刚修改了部分代码。。linux可能暂时有少许编译不过.




