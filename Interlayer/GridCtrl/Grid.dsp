# Microsoft Developer Studio Project File - Name="Grid" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Grid - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Grid.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Grid.mak" CFG="Grid - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Grid - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Grid - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Grid", UTAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Grid - Win32 Release"

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
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /def:".\Grid.def" /out:"..\BinRelease\Grid.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Grid - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_VIS_NOLIB" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\Grid.def" /out:"..\BinDebug\Grid.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Grid - Win32 Release"
# Name "Grid - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\GridCtrl_src\amsEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\CheckComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\DummyView.cpp
# End Source File
# Begin Source File

SOURCE=.\Grid.cpp
# End Source File
# Begin Source File

SOURCE=.\Grid.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Grid.rc
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\GridCell.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\GridCellBase.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellCheckCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\gridcellcurrency.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellDateTime.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellDouble.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCellEditFile.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellMask.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellNumeric.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\GridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\GridDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridURLCell.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\MyPreviewView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\TitleTip.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\GridCtrl_src\amsEdit.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\CellRange.h
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\CheckComboBox.h
# End Source File
# Begin Source File

SOURCE=.\DummyView.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\GridCell.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\GridCellBase.h
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellCheck.h
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellCheckCombo.h
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellCombo.h
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\gridcellcurrency.h
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellDateTime.h
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellDouble.h
# End Source File
# Begin Source File

SOURCE=.\GridCellEditFile.h
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellMask.h
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridCellNumeric.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\GridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\GridDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\GridLib.h
# End Source File
# Begin Source File

SOURCE=.\NewCellTypes\GridURLCell.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\InPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\MyPreviewView.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_src\TitleTip.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Grid.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section Grid : {5B23564B-FCFB-4498-AA4D-81134C3BDBAD}
# 	2:21:CLASS: CMyPreviewView:CMyPreviewView
# 	2:17:MyPreviewView.cpp:MyPreviewView.cpp
# 	2:15:MyPreviewView.h:MyPreviewView.h
# 	2:19:Application Include:Grid.h
# End Section
# Section Grid : {52DDADFA-4A9E-4077-95FB-FE6489B84000}
# 	2:13:DummyView.cpp:DummyView.cpp
# 	2:17:CLASS: CDummyView:CDummyView
# 	2:11:DummyView.h:DummyView.h
# 	2:19:Application Include:Grid.h
# End Section
