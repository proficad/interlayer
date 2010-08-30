# Microsoft Developer Studio Project File - Name="DispLogCurve" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DispLogCurve - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DispLogCurve.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DispLogCurve.mak" CFG="DispLogCurve - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DispLogCurve - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DispLogCurve - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DispLogCurve", FIBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DispLogCurve - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"../BinRelease/DispLogCurve.dll"

!ELSEIF  "$(CFG)" == "DispLogCurve - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../BinDebug/DispLogCurve.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DispLogCurve - Win32 Release"
# Name "DispLogCurve - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DispLogCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\DispLogCurve.def
# End Source File
# Begin Source File

SOURCE=.\DispLogCurve.rc
# End Source File
# Begin Source File

SOURCE=.\DispLogCurveDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\DispLogCurveFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\DispLogCurveView.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAddCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAddLane.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCalcWXCS.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCurveSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDepthSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLaneSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetWidth.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTemplEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTemplSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\EmptyDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\LogCurveDatStruct.cpp
# End Source File
# Begin Source File

SOURCE=.\LogTemplPage.cpp
# End Source File
# Begin Source File

SOURCE=.\LogTemplSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\SetWidthEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\SetWidthView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WatchBar.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DispLogCurve.h
# End Source File
# Begin Source File

SOURCE=.\DispLogCurveDoc.h
# End Source File
# Begin Source File

SOURCE=.\DispLogCurveFrame.h
# End Source File
# Begin Source File

SOURCE=.\DispLogCurveView.h
# End Source File
# Begin Source File

SOURCE=.\DlgAddCurve.h
# End Source File
# Begin Source File

SOURCE=.\DlgAddLane.h
# End Source File
# Begin Source File

SOURCE=.\DlgCalcWXCS.h
# End Source File
# Begin Source File

SOURCE=.\DlgCurveSetup.h
# End Source File
# Begin Source File

SOURCE=.\DlgDepthSetup.h
# End Source File
# Begin Source File

SOURCE=.\DlgLaneSetup.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetWidth.h
# End Source File
# Begin Source File

SOURCE=.\DlgTemplEdit.h
# End Source File
# Begin Source File

SOURCE=.\DlgTemplSelect.h
# End Source File
# Begin Source File

SOURCE=.\EmptyDoc.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=.\LogCurveDatStruct.h
# End Source File
# Begin Source File

SOURCE=.\LogTemplPage.h
# End Source File
# Begin Source File

SOURCE=.\LogTemplSheet.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SetWidthEdit.h
# End Source File
# Begin Source File

SOURCE=.\SetWidthView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WatchBar.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bs0.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bs1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bs2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bs3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bs4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bs5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DispLogCurve.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00008.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00009.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00010.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00011.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00012.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00013.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00014.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00015.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00016.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00017.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00018.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\res\log.ico
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\xch.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
