# Microsoft Developer Studio Project File - Name="Bonfire" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Bonfire - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Bonfire.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Bonfire.mak" CFG="Bonfire - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Bonfire - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Bonfire - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Gena/Bonfire", JSAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Bonfire - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "C:\Program Files\HTML Help Workshop\include" /I "C:\Program Files\MSXML 4.0\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 htmlhelp.lib msxml2.lib lib/mre222m.lib lib/urlmon.lib lib/shlwapi.lib /nologo /subsystem:windows /machine:I386 /libpath:"C:\Program Files\HTML Help Workshop\lib" /libpath:"C:\Program Files\MSXML 4.0\lib"
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "Bonfire - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "C:\Program Files\HTML Help Workshop\include" /I "C:\Program Files\MSXML 4.0\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 htmlhelp.lib msxml2.lib lib/mre222m.lib lib/urlmon.lib lib/shlwapi.lib /nologo /subsystem:windows /profile /debug /machine:I386 /libpath:"C:\Program Files\HTML Help Workshop\lib" /libpath:"C:\Program Files\MSXML 4.0\lib"

!ENDIF 

# Begin Target

# Name "Bonfire - Win32 Release"
# Name "Bonfire - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Bonfire.cpp
# End Source File
# Begin Source File

SOURCE=.\Bonfire.rc
# End Source File
# Begin Source File

SOURCE=.\FileWatch.cpp
# End Source File
# Begin Source File

SOURCE=.\FindInFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectBar.cpp
# End Source File
# Begin Source File

SOURCE=.\PropBar.cpp
# End Source File
# Begin Source File

SOURCE=.\RecentStringList.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\VIRecentFileList.cpp
# End Source File
# Begin Source File

SOURCE=.\XNode.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Bonfire.h
# End Source File
# Begin Source File

SOURCE=.\FileWatch.h
# End Source File
# Begin Source File

SOURCE=.\FindInFiles.h
# End Source File
# Begin Source File

SOURCE=.\Globals.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\OutputBar.h
# End Source File
# Begin Source File

SOURCE=.\ProjectBar.h
# End Source File
# Begin Source File

SOURCE=.\PropBar.h
# End Source File
# Begin Source File

SOURCE=.\RecentStringList.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SortedArray.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\VIRecentFileList.h
# End Source File
# Begin Source File

SOURCE=.\XNode.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Bonfire.rc2
# End Source File
# Begin Source File

SOURCE=.\res\BonNew.ico
# End Source File
# Begin Source File

SOURCE=.\res\BonType.ico
# End Source File
# Begin Source File

SOURCE=.\res\FullScreen.bmp
# End Source File
# Begin Source File

SOURCE=.\res\main.ico
# End Source File
# Begin Source File

SOURCE=.\res\MainFrame.ico
# End Source File
# Begin Source File

SOURCE=.\res\MarginCursor.cur
# End Source File
# Begin Source File

SOURCE=.\res\MarginIcons.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MenuIcons.bmp
# End Source File
# Begin Source File

SOURCE=.\res\OutputIcons.bmp
# End Source File
# Begin Source File

SOURCE=.\res\project.ico
# End Source File
# Begin Source File

SOURCE=.\res\ProjectBar256.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rt_manif.bin
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar256.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarFind.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarFind256.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarProject.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarProject256.bmp
# End Source File
# Begin Source File

SOURCE=.\res\XMLTree256.bmp
# End Source File
# End Group
# Begin Group "UI"

# PROP Default_Filter ""
# Begin Group "Downloaded Components"

# PROP Default_Filter ""
# Begin Group "SizingCBar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Downloaded Components\SizingCBar\scbarg.cpp"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\SizingCBar\scbarg.h"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\SizingCBar\SizeCBar.cpp"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\SizingCBar\SizeCBar.h"
# End Source File
# End Group
# Begin Group "CrystalEdit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CCrystalEditView.cpp"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CCrystalEditView.h"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CCrystalEditView.inl"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CCrystalTextBuffer.cpp"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CCrystalTextBuffer.h"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CCrystalTextBuffer.inl"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CCrystalTextView.cpp"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CCrystalTextView.h"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CCrystalTextView.inl"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CCrystalTextView2.cpp"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\cedefs.h"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CEditReplaceDlg.cpp"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CEditReplaceDlg.h"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CFindTextDlg.cpp"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\CFindTextDlg.h"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\editcmd.h"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\CrystalEdit\editreg.h"
# End Source File
# End Group
# Begin Group "ColorButton"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Downloaded Components\ColorButton\ColorButton.cpp"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\ColorButton\ColorButton.h"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\ColorButton\ColorPopup.cpp"
# End Source File
# Begin Source File

SOURCE=".\Downloaded Components\ColorButton\ColorPopup.h"
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\BCMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\BCMenu.h
# End Source File
# Begin Source File

SOURCE=.\MRUCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\MRUCombo.h
# End Source File
# Begin Source File

SOURCE=.\NZDockBar.cpp
# End Source File
# Begin Source File

SOURCE=.\NZDockBar.h
# End Source File
# Begin Source File

SOURCE=.\NZDockContext.cpp
# End Source File
# Begin Source File

SOURCE=.\NZDockContext.h
# End Source File
# Begin Source File

SOURCE=.\NZProjectTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\NZProjectTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\NZToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\NZToolBar.h
# End Source File
# Begin Source File

SOURCE=.\TabBar.cpp
# End Source File
# Begin Source File

SOURCE=.\TabBar.h
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AddFolderDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AddFolderDlg.h
# End Source File
# Begin Source File

SOURCE=.\FindInFilesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FindInFilesDlg.h
# End Source File
# Begin Source File

SOURCE=.\GoToDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GoToDlg.h
# End Source File
# Begin Source File

SOURCE=.\OpenFromURLDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenFromURLDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionsPages.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsPages.h
# End Source File
# End Group
# Begin Group "Docs/Views"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BonfireDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\BonfireDoc.h
# End Source File
# Begin Source File

SOURCE=.\BonfireView.cpp
# End Source File
# Begin Source File

SOURCE=.\BonfireView.h
# End Source File
# Begin Source File

SOURCE=.\BrowserView.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowserView.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\SourceView.cpp
# End Source File
# Begin Source File

SOURCE=.\SourceView.h
# End Source File
# Begin Source File

SOURCE=.\SyntaxParsers.cpp
# End Source File
# Begin Source File

SOURCE=.\SyntaxParsers.h
# End Source File
# Begin Source File

SOURCE=.\XMLTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\XMLTreeView.h
# End Source File
# End Group
# Begin Source File

SOURCE=".\bonfire features.txt"
# End Source File
# End Target
# End Project
