/*! HowToBuild
********************************************************************************
Description: This document descriped how to build xcpp
Author: chennqqi@qq.com  

history :
2014/10/13			CHENQ	created
*******************************************************************************/

1. How many platforms xcpp project support?
	
		Windows&Linux
	
2. How to build On Windows?

		Environment : VS2010/VS2013
		open xcpp.sln project file
		click build

		or uring devenv command line
		devenv /build xcpp.sln
		

3. How to Build On Linux?
	
		We build Makefile by Qt Project file *.pro
		Environment : qmake (version >= 4.0)
 		gcc(version >4.2)

		[xx@xxx$]cd xcpp/build
		[xx@xxx$]mkdir centos
		[xx@xxx$]cd centos
		[xx@xxx$]qmake -r ../../xcpp.pro
		[xx@xxx$]make all

	

4. How to gerneral project file.
		
		xcpp project using Qt's project *.pro as code manager.
		So we use qmake to create VS project or linux makefile.	

5. FAQ
		
		5.1 qmake version problem @ linux
			Most ditrbutions of linux using qmake3 as default qmake.
			You must change default soft link form qmake3 to qmake4(qmakeqt4)

		5.2 no debug excute program @ linux
			The default 'make' only create release application.
			'make all' will create both debug and release application.
			