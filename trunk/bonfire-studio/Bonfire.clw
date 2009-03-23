; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAboutDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "bonfire.h"
LastPage=0

ClassCount=27
Class1=CAddFolderDlg
Class2=CBonfireApp
Class3=CAboutDlg
Class4=CBonfireDoc
Class5=CBrowserView
Class6=CChildFrame
Class7=CFileWatchDlg
Class8=CFindInFilesDlg
Class9=CGoToDlg
Class10=CMainFrame
Class11=CMRUComboBox
Class12=CNZProjectTreeCtrl
Class13=CNZToolBar
Class14=COpenFromURLDlg
Class15=COptionsDlg
Class16=CGeneralPageDlg
Class17=CEditorPageDlg
Class18=COutputPageDlg
Class19=CProjectMgrPageDlg
Class20=CXMLTreePageDlg
Class21=CViewsPageDlg
Class22=COutputBar
Class23=CProjectBar
Class24=CPropBar
Class25=CSourceView
Class26=CTabBar
Class27=CXMLTreeView

ResourceCount=28
Resource1=IDD_ABOUTBOX
Resource2=IDR_TOOLBAR_PROJECT
Resource3=IDR_TOOLBAR_FULLSCREEN
Resource4=IDR_TOOLBAR_FIND
Resource5=IDR_BONFIRETYPE
Resource6=IDR_POPUP_FILE
Resource7=IDR_POPUP_PROJECT
Resource8=IDR_POPUP_FOLDER
Resource9=IDR_POPUP_EDITOR
Resource10=IDR_POPUP_XMLELEMENT
Resource11=IDR_POPUP_XMLATTRIBUTE
Resource12=IDR_POPUP_XMLTEXT
Resource13=IDR_POPUP_XMLCOMMENT
Resource14=IDD_EDIT_REPLACE
Resource15=IDD_OPTIONS_XMLTREE_PAGE
Resource16=IDD_PROJECT_ADDFOLDER
Resource17=IDR_DEFAULT_ACCEL
Resource18=IDD_EDIT_FIND
Resource19=IDR_MAINFRAME
Resource20=IDD_FINDINFILES
Resource21=IDD_GOTO
Resource22=IDD_OPEN_FROM_URL
Resource23=IDD_OPTIONS_OUTPUT_PAGE
Resource24=IDD_OPTIONS_GENERAL_PAGE
Resource25=IDD_OPTIONS_EDITOR_PAGE
Resource26=IDD_OPTIONS_PROJECTMGR_PAGE
Resource27=IDD_OPTIONS
Resource28=IDD_OPTIONS_VIEWS_PAGE

[CLS:CAddFolderDlg]
Type=0
BaseClass=CDialog
HeaderFile=AddFolderDlg.h
ImplementationFile=AddFolderDlg.cpp

[CLS:CBonfireApp]
Type=0
BaseClass=CWinApp
HeaderFile=Bonfire.h
ImplementationFile=Bonfire.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Bonfire.cpp
ImplementationFile=Bonfire.cpp
LastObject=CAboutDlg

[CLS:CBonfireDoc]
Type=0
BaseClass=CDocument
HeaderFile=BonfireDoc.h
ImplementationFile=BonfireDoc.cpp

[CLS:CBrowserView]
Type=0
BaseClass=CHtmlView
HeaderFile=BrowserView.h
ImplementationFile=BrowserView.cpp

[CLS:CChildFrame]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp

[CLS:CFileWatchDlg]
Type=0
BaseClass=CDialog
HeaderFile=FileWatch.cpp
ImplementationFile=FileWatch.cpp

[CLS:CFindInFilesDlg]
Type=0
BaseClass=CDialog
HeaderFile=FindInFilesDlg.h
ImplementationFile=FindInFilesDlg.cpp

[CLS:CGoToDlg]
Type=0
BaseClass=CDialog
HeaderFile=GoToDlg.h
ImplementationFile=GoToDlg.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CMRUComboBox]
Type=0
BaseClass=CComboBox
HeaderFile=MRUCombo.h
ImplementationFile=MRUCombo.cpp

[CLS:CNZProjectTreeCtrl]
Type=0
BaseClass=CTreeCtrl
HeaderFile=nzprojecttreectrl.h
ImplementationFile=NZProjectTreeCtrl.cpp

[CLS:CNZToolBar]
Type=0
BaseClass=CToolBar
HeaderFile=NZToolBar.h
ImplementationFile=NZToolBar.cpp

[CLS:COpenFromURLDlg]
Type=0
BaseClass=CDialog
HeaderFile=OpenFromURLDlg.h
ImplementationFile=OpenFromURLDlg.cpp

[CLS:COptionsDlg]
Type=0
BaseClass=CDialog
HeaderFile=OptionsDlg.h
ImplementationFile=OptionsDlg.cpp

[CLS:CGeneralPageDlg]
Type=0
BaseClass=CDialog
HeaderFile=OptionsPages.h
ImplementationFile=OptionsPages.cpp

[CLS:CEditorPageDlg]
Type=0
BaseClass=CDialog
HeaderFile=OptionsPages.h
ImplementationFile=OptionsPages.cpp

[CLS:COutputPageDlg]
Type=0
BaseClass=CDialog
HeaderFile=OptionsPages.h
ImplementationFile=OptionsPages.cpp

[CLS:CProjectMgrPageDlg]
Type=0
BaseClass=CDialog
HeaderFile=OptionsPages.h
ImplementationFile=OptionsPages.cpp

[CLS:CXMLTreePageDlg]
Type=0
BaseClass=CDialog
HeaderFile=OptionsPages.h
ImplementationFile=OptionsPages.cpp

[CLS:CViewsPageDlg]
Type=0
BaseClass=CDialog
HeaderFile=OptionsPages.h
ImplementationFile=OptionsPages.cpp

[CLS:COutputBar]
Type=0
BaseClass=SIZEBAR_BASECLASS
HeaderFile=OutputBar.h
ImplementationFile=OutputBar.cpp

[CLS:CProjectBar]
Type=0
BaseClass=SIZEBAR_BASECLASS
HeaderFile=ProjectBar.h
ImplementationFile=ProjectBar.cpp

[CLS:CPropBar]
Type=0
BaseClass=SIZEBAR_BASECLASS
HeaderFile=PropBar.h
ImplementationFile=PropBar.cpp

[CLS:CSourceView]
Type=0
BaseClass=CCrystalEditView
HeaderFile=SourceView.h
ImplementationFile=SourceView.cpp

[CLS:CTabBar]
Type=0
BaseClass=CControlBar
HeaderFile=TabBar.h
ImplementationFile=TabBar.cpp

[CLS:CXMLTreeView]
Type=0
BaseClass=CTreeView
HeaderFile=XMLTreeView.h
ImplementationFile=XMLTreeView.cpp

[DLG:IDD_PROJECT_ADDFOLDER]
Type=1
Class=CAddFolderDlg
ControlCount=9
Control1=IDC_STATIC,static,1342308864
Control2=IDC_FOLDER_PATH,edit,1350631552
Control3=IDC_BROWSE,button,1342242816
Control4=IDC_STATIC,static,1342308864
Control5=IDC_ADD_FILES_EX,combobox,1344340226
Control6=IDC_STATIC,static,1342308352
Control7=IDC_OPT_ADD_SUBFOLDERS,button,1342242819
Control8=IDOK,button,1342242817
Control9=IDCANCEL,button,1342242816

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=7
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDOK,button,1342373889

[DLG:IDD_FILEWATCH]
Type=1
Class=CFileWatchDlg

[DLG:IDD_FINDINFILES]
Type=1
Class=CFindInFilesDlg
ControlCount=12
Control1=IDC_STATIC,static,1342308352
Control2=IDC_SEARCH_TEXT_EX,combobox,1344471106
Control3=IDC_STATIC,static,1342308864
Control4=IDC_SEARCH_FILETYPES_EX,combobox,1344471106
Control5=IDC_STATIC,static,1342308864
Control6=IDC_SEARCH_PATH_EX,combobox,1344471106
Control7=IDC_MATCH_WHOLE_WORD,button,1342373891
Control8=IDC_OPT_MATCH_CASE,button,1342242819
Control9=IDC_OPT_SEARCH_SUBFOLDERS,button,1342242819
Control10=IDOK,button,1342242817
Control11=IDCANCEL,button,1342242816
Control12=IDC_BROWSE,button,1342242816

[DLG:IDD_GOTO]
Type=1
Class=CGoToDlg
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_GOTO_VALUE,edit,1350631552
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816

[DLG:IDD_OPEN_FROM_URL]
Type=1
Class=COpenFromURLDlg
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_URL,edit,1350631552
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816

[DLG:IDD_OPTIONS]
Type=1
Class=COptionsDlg
ControlCount=4
Control1=IDC_TABS,SysTabControl32,1342177280
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDAPPLY,button,1342242816

[DLG:IDD_OPTIONS_GENERAL_PAGE]
Type=1
Class=CGeneralPageDlg
ControlCount=3
Control1=IDC_CHK_SAVE_ON_SWITCH,button,1342243843
Control2=IDC_CHK_CLEAR_OUTPUT,button,1342243843
Control3=IDC_CHK_RELOAD_MODIFIED,button,1342252035

[DLG:IDD_OPTIONS_EDITOR_PAGE]
Type=1
Class=CEditorPageDlg
ControlCount=9
Control1=IDC_STATIC,button,1342177287
Control2=IDC_LIST_SYNTAX,listbox,1352728913
Control3=IDC_STATIC,static,1342308352
Control4=IDC_COLOR_CHOOSE,button,1342242816
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,static,1342308352
Control7=IDC_CBO_FONT,combobox,1344340227
Control8=IDC_STATIC,static,1342308352
Control9=IDC_FONT_SIZE,edit,1350631552

[DLG:IDD_OPTIONS_OUTPUT_PAGE]
Type=1
Class=COutputPageDlg
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_OPTIONS_PROJECTMGR_PAGE]
Type=1
Class=CProjectMgrPageDlg
ControlCount=5
Control1=IDC_CHK_FOLDERS_FIRST,button,1342242819
Control2=IDC_STATIC,button,1342177287
Control3=IDC_CHK_DFP_PROJECTS,button,1342242819
Control4=IDC_CHK_DFP_FOLDERS,button,1342242819
Control5=IDC_CHK_DFP_FILES,button,1342242819

[DLG:IDD_OPTIONS_XMLTREE_PAGE]
Type=1
Class=CXMLTreePageDlg
ControlCount=10
Control1=IDC_CHK_SHOW_XML_ATTRIBUTES,button,1342243843
Control2=IDC_CHK_SHOW_XML_TEXT,button,1342243843
Control3=IDC_CHK_SHOW_XML_COMMENTS,button,1342252035
Control4=IDC_STATIC,button,1342177287
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,static,1342308352
Control7=IDC_OPTIONS_XMLTREE_FONT,combobox,1344340227
Control8=IDC_STATIC,static,1342308352
Control9=IDC_OPTIONS_XMLTREE_FONTSIZE,edit,1350631552
Control10=IDC_CHK_EDIT_XMLTREE,button,1342242819

[DLG:IDD_OPTIONS_VIEWS_PAGE]
Type=1
Class=CViewsPageDlg
ControlCount=11
Control1=IDC_LIST_FILEEXT,listbox,1353779457
Control2=IDC_FILEEXT,edit,1350631552
Control3=IDC_ADD_EDIT_FILEEXT,button,1342242816
Control4=IDC_DELETE_FILEEXT,button,1342242816
Control5=IDC_STATIC,static,1342308352
Control6=IDC_LIST_VIEWS,SysListView32,1350647949
Control7=IDC_MOVE_UP,button,1073807360
Control8=IDC_MOVE_DOWN,button,1073807360
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_COMBO_HIGHLIGHTING,combobox,1344339971

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVEALL
Command5=ID_FILE_PRINT
Command6=ID_EDIT_UNDO
Command7=ID_EDIT_REDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_VIEW_FULLSCREEN
Command12=ID_HELP_CONTENTS
CommandCount=12

[TB:IDR_TOOLBAR_PROJECT]
Type=1
Class=?
Command1=ID_PROJECT_NEW
Command2=ID_PROJECT_OPEN
Command3=ID_PROJECT_SAVE
Command4=ID_PROJECT_NEWFOLDER
Command5=ID_PROJECT_ADDFILESTOPROJECT
Command6=ID_PROJECT_ADDDIRTOPROJECT
Command7=ID_PROJECT_REMOVEFROMPROJECT
CommandCount=7

[TB:IDR_TOOLBAR_FULLSCREEN]
Type=1
Class=?
Command1=ID_VIEW_FULLSCREEN
CommandCount=1

[TB:IDR_TOOLBAR_FIND]
Type=1
Class=?
Command1=ID_EDIT_FIND
Command2=ID_EDIT_FIND_PREVIOUS
Command3=ID_EDIT_REPEAT
Command4=ID_EDIT_REPLACE
Command5=ID_EDIT_FINDINFILES
CommandCount=5

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_OPENFROMURL
Command4=ID_PROJECT_NEW
Command5=ID_PROJECT_OPEN
Command6=ID_PROJECT_SAVE
Command7=ID_PROJECT_SAVEAS
Command8=ID_FILE_SAVEALL
Command9=ID_FILE_PRINT_SETUP
Command10=ID_FILE_MRU_FILE1
Command11=ID_FILE_MRU_PROJECT1
Command12=ID_APP_EXIT
Command13=ID_VIEW_TOOLBAR
Command14=ID_VIEW_TOOLBAR_PROJECT
Command15=ID_VIEW_TOOLBAR_FIND
Command16=ID_VIEW_PROJECTEXPLORER
Command17=ID_VIEW_TAGPROPERTIES
Command18=ID_VIEW_OUTPUTWINDOW
Command19=ID_VIEW_STATUS_BAR
Command20=ID_VIEW_FULLSCREEN
Command21=ID_EDIT_FIND
Command22=ID_EDIT_REPEAT
Command23=ID_EDIT_FIND_PREVIOUS
Command24=ID_EDIT_REPLACE
Command25=ID_EDIT_FINDINFILES
Command26=ID_EDIT_GOTOLINE
Command27=ID_PROJECT_REFRESH
Command28=ID_PROJECT_NEWFOLDER
Command29=ID_PROJECT_ADDFILESTOPROJECT
Command30=ID_PROJECT_ADDDIRTOPROJECT
Command31=ID_PROJECT_REMOVEFROMPROJECT
Command32=ID_PROJECT_PROPERTIES
Command33=ID_PROJECT_CLOSE
Command34=ID_TOOLS_OPENDEFAULT
Command35=ID_TOOLS_LOCATEINEXPLORER
Command36=ID_TOOLS_OPTIONS
Command37=ID_HELP_CONTENTS
Command38=ID_HELP_INDEX
Command39=ID_HELP_SEARCH
Command40=ID_APP_ABOUT
Command41=ID_HELP_HOMEPAGE
CommandCount=41

[MNU:IDR_BONFIRETYPE]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_OPENFROMURL
Command4=ID_PROJECT_NEW
Command5=ID_PROJECT_OPEN
Command6=ID_FILE_SAVE
Command7=ID_FILE_SAVE_AS
Command8=ID_PROJECT_SAVE
Command9=ID_PROJECT_SAVEAS
Command10=ID_FILE_SAVEALL
Command11=ID_FILE_PRINT_PREVIEW
Command12=ID_FILE_PRINT_SETUP
Command13=ID_FILE_PRINT
Command14=ID_FILE_PROPERTIES
Command15=ID_FILE_MRU_FILE1
Command16=ID_FILE_MRU_PROJECT1
Command17=ID_APP_EXIT
Command18=ID_EDIT_UNDO
Command19=ID_EDIT_REDO
Command20=ID_EDIT_CUT
Command21=ID_EDIT_COPY
Command22=ID_EDIT_PASTE
Command23=ID_EDIT_SELECT_ALL
Command24=ID_EDIT_DELETE
Command25=ID_EDIT_TAB
Command26=ID_EDIT_UNTAB
Command27=ID_EDIT_UPPERCASESELECTION
Command28=ID_EDIT_LOWERCASESELECTION
Command29=ID_EDIT_GO_BOOKMARK0
Command30=ID_EDIT_GO_BOOKMARK1
Command31=ID_EDIT_GO_BOOKMARK2
Command32=ID_EDIT_GO_BOOKMARK3
Command33=ID_EDIT_GO_BOOKMARK4
Command34=ID_EDIT_GO_BOOKMARK5
Command35=ID_EDIT_GO_BOOKMARK6
Command36=ID_EDIT_GO_BOOKMARK7
Command37=ID_EDIT_GO_BOOKMARK8
Command38=ID_EDIT_GO_BOOKMARK9
Command39=ID_EDIT_TOGGLE_BOOKMARK0
Command40=ID_EDIT_TOGGLE_BOOKMARK1
Command41=ID_EDIT_TOGGLE_BOOKMARK2
Command42=ID_EDIT_TOGGLE_BOOKMARK3
Command43=ID_EDIT_TOGGLE_BOOKMARK4
Command44=ID_EDIT_TOGGLE_BOOKMARK5
Command45=ID_EDIT_TOGGLE_BOOKMARK6
Command46=ID_EDIT_TOGGLE_BOOKMARK7
Command47=ID_EDIT_TOGGLE_BOOKMARK8
Command48=ID_EDIT_TOGGLE_BOOKMARK9
Command49=ID_EDIT_READONLY
Command50=ID_EDIT_FIND
Command51=ID_EDIT_REPEAT
Command52=ID_EDIT_FIND_PREVIOUS
Command53=ID_EDIT_REPLACE
Command54=ID_EDIT_FINDINFILES
Command55=ID_EDIT_GOTOLINE
Command56=ID_VIEW_TOOLBAR
Command57=ID_VIEW_TOOLBAR_PROJECT
Command58=ID_VIEW_TOOLBAR_FIND
Command59=ID_VIEW_PROJECTEXPLORER
Command60=ID_VIEW_TAGPROPERTIES
Command61=ID_VIEW_OUTPUTWINDOW
Command62=ID_VIEW_STATUS_BAR
Command63=ID_VIEW_FULLSCREEN
Command64=ID_PROJECT_REFRESH
Command65=ID_PROJECT_NEWFOLDER
Command66=ID_PROJECT_ADDFILESTOPROJECT
Command67=ID_PROJECT_ADDDIRTOPROJECT
Command68=ID_PROJECT_REMOVEFROMPROJECT
Command69=ID_PROJECT_PROPERTIES
Command70=ID_PROJECT_CLOSE
Command71=ID_TOOLS_OPENDEFAULT
Command72=ID_TOOLS_LOCATEINEXPLORER
Command73=ID_TOOLS_VALIDATE_FILE
Command74=ID_TOOLS_OPTIONS
Command75=ID_WINDOW_CASCADE
Command76=ID_WINDOW_TILE_HORZ
Command77=ID_WINDOW_TILE_VERT
Command78=ID_WINDOW_ARRANGE
Command79=ID_FILE_CLOSE
Command80=ID_WINDOW_CLOSEALL
Command81=ID_HELP_CONTENTS
Command82=ID_HELP_INDEX
Command83=ID_HELP_SEARCH
Command84=ID_APP_ABOUT
Command85=ID_HELP_HOMEPAGE
CommandCount=85

[MNU:IDR_POPUP_FILE]
Type=1
Class=?
Command1=ID_FILE_SHOW
Command2=ID_TOOLS_OPENDEFAULT
Command3=ID_TOOLS_LOCATEINEXPLORER
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_RENAME
Command7=ID_PROJECT_REMOVEFROMPROJECT
Command8=ID_FILE_MOVETO_PROJECT1
Command9=ID_FILE_PROPERTIES
CommandCount=9

[MNU:IDR_POPUP_PROJECT]
Type=1
Class=?
Command1=ID_PROJECT_SAVE
Command2=ID_PROJECT_SAVEAS
Command3=ID_PROJECT_CLOSE
Command4=ID_PROJECT_NEWFOLDER
Command5=ID_PROJECT_ADDFILESTOPROJECT
Command6=ID_PROJECT_ADDDIRTOPROJECT
Command7=ID_PROJECT_RENAME
Command8=ID_PROJECT_PROPERTIES
CommandCount=8

[MNU:IDR_POPUP_FOLDER]
Type=1
Class=?
Command1=ID_TOOLS_LOCATEINEXPLORER
Command2=ID_PROJECT_NEWFOLDER
Command3=ID_PROJECT_ADDFILESTOPROJECT
Command4=ID_PROJECT_ADDDIRTOPROJECT
Command5=ID_FOLDER_RENAME
Command6=ID_PROJECT_REMOVEFROMPROJECT
Command7=ID_FILE_MOVETO_PROJECT1
CommandCount=7

[MNU:IDR_POPUP_EDITOR]
Type=1
Class=?
Command1=ID_EDIT_UNDO
Command2=ID_EDIT_REDO
Command3=ID_EDIT_CUT
Command4=ID_EDIT_COPY
Command5=ID_EDIT_PASTE
Command6=ID_EDIT_SELECT_ALL
Command7=ID_EDIT_FIND
Command8=ID_EDIT_GOTOLINE
CommandCount=8

[MNU:IDR_POPUP_XMLELEMENT]
Type=1
Class=?
Command1=ID_ELEMENT_EDIT_NAME
Command2=ID_ELEMENT_EXPAND_COLLAPSE
Command3=ID_ELEMENT_EXPANDALL
Command4=ID_ELEMENT_COLLAPSEALL
Command5=ID_ELEMENT_INSERT_ELEMENT
Command6=ID_ELEMENT_INSERT_ATTRIBUTE
Command7=ID_ELEMENT_INSERT_COMMENT
Command8=ID_ELEMENT_INSERT_TEXT
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_EDIT_DELETE
CommandCount=12

[MNU:IDR_POPUP_XMLATTRIBUTE]
Type=1
Class=?
Command1=ID_ATTRIBUTE_EDIT_NAME
Command2=ID_ATTRIBUTE_EDIT_VALUE
Command3=ID_EDIT_CUT
Command4=ID_EDIT_COPY
Command5=ID_EDIT_PASTE
Command6=ID_EDIT_DELETE
CommandCount=6

[MNU:IDR_POPUP_XMLTEXT]
Type=1
Class=?
Command1=ID_TEXTNODE_EDIT
Command2=ID_EDIT_CUT
Command3=ID_EDIT_COPY
Command4=ID_EDIT_PASTE
Command5=ID_EDIT_DELETE
CommandCount=5

[MNU:IDR_POPUP_XMLCOMMENT]
Type=1
Class=?
Command1=ID_COMMENT_EDIT
Command2=ID_EDIT_CUT
Command3=ID_EDIT_COPY
Command4=ID_EDIT_PASTE
Command5=ID_EDIT_DELETE
CommandCount=5

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_SAVEALL
Command2=ID_FILE_NEW
Command3=ID_PROJECT_NEW
Command4=ID_FILE_OPEN
Command5=ID_PROJECT_OPEN
Command6=ID_FILE_PRINT
Command7=ID_FILE_SAVE
Command8=ID_PROJECT_SAVE
Command9=ID_HELP_CONTENTS
Command10=ID_VIEW_FULLSCREEN
Command11=ID_TOOLS_OPENDEFAULT
Command12=ID_TOOLS_LOCATEINEXPLORER
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
Command15=ID_TOOLS_VALIDATE_FILE
CommandCount=15

[ACL:IDR_DEFAULT_ACCEL]
Type=1
Class=?
Command1=ID_EDIT_TOGGLE_BOOKMARK0
Command2=ID_EDIT_GO_BOOKMARK0
Command3=ID_EDIT_TOGGLE_BOOKMARK1
Command4=ID_EDIT_GO_BOOKMARK1
Command5=ID_EDIT_TOGGLE_BOOKMARK2
Command6=ID_EDIT_GO_BOOKMARK2
Command7=ID_EDIT_TOGGLE_BOOKMARK3
Command8=ID_EDIT_GO_BOOKMARK3
Command9=ID_EDIT_TOGGLE_BOOKMARK4
Command10=ID_EDIT_GO_BOOKMARK4
Command11=ID_EDIT_TOGGLE_BOOKMARK5
Command12=ID_EDIT_GO_BOOKMARK5
Command13=ID_EDIT_TOGGLE_BOOKMARK6
Command14=ID_EDIT_GO_BOOKMARK6
Command15=ID_EDIT_TOGGLE_BOOKMARK7
Command16=ID_EDIT_GO_BOOKMARK7
Command17=ID_EDIT_TOGGLE_BOOKMARK8
Command18=ID_EDIT_GO_BOOKMARK8
Command19=ID_EDIT_TOGGLE_BOOKMARK9
Command20=ID_EDIT_GO_BOOKMARK9
Command21=ID_EDIT_SELECT_ALL
Command22=ID_EDIT_COPY
Command23=ID_EDIT_FIND
Command24=ID_EDIT_GOTOLINE
Command25=ID_EDIT_REPLACE
Command26=ID_EDIT_PASTE
Command27=ID_EDIT_DELETE_BACK
Command28=ID_EDIT_DELETE_WORD_BACK
Command29=ID_EDIT_UNDO
Command30=ID_EDIT_DELETE
Command31=ID_EDIT_CUT
Command32=ID_EDIT_LINE_DOWN
Command33=ID_EDIT_SCROLL_DOWN
Command34=ID_EDIT_EXT_LINE_DOWN
Command35=ID_EDIT_LINE_END
Command36=ID_EDIT_TEXT_END
Command37=ID_EDIT_EXT_LINE_END
Command38=ID_EDIT_EXT_TEXT_END
Command39=ID_HELP_CONTENTS
Command40=ID_EDIT_GOTO_NEXT_BOOKMARK
Command41=ID_EDIT_TOGGLE_BOOKMARK
Command42=ID_EDIT_GOTO_PREV_BOOKMARK
Command43=ID_EDIT_CLEAR_ALL_BOOKMARKS
Command44=ID_EDIT_REPEAT
Command45=ID_EDIT_FIND_PREVIOUS
Command46=ID_EDIT_HOME
Command47=ID_EDIT_TEXT_BEGIN
Command48=ID_EDIT_EXT_HOME
Command49=ID_EDIT_EXT_TEXT_BEGIN
Command50=ID_EDIT_SWITCH_OVRMODE
Command51=ID_EDIT_COPY
Command52=ID_EDIT_PASTE
Command53=ID_EDIT_CHAR_LEFT
Command54=ID_EDIT_WORD_LEFT
Command55=ID_EDIT_EXT_CHAR_LEFT
Command56=ID_EDIT_EXT_WORD_LEFT
Command57=ID_EDIT_PAGE_DOWN
Command58=ID_EDIT_EXT_PAGE_DOWN
Command59=ID_EDIT_PAGE_UP
Command60=ID_EDIT_EXT_PAGE_UP
Command61=ID_EDIT_CHAR_RIGHT
Command62=ID_EDIT_WORD_RIGHT
Command63=ID_EDIT_EXT_CHAR_RIGHT
Command64=ID_EDIT_EXT_WORD_RIGHT
Command65=ID_EDIT_TAB
Command66=ID_EDIT_UNTAB
Command67=ID_EDIT_LINE_UP
Command68=ID_EDIT_SCROLL_UP
Command69=ID_EDIT_EXT_LINE_UP
Command70=ID_EDIT_CUT
Command71=ID_EDIT_REDO
Command72=ID_EDIT_UNDO
CommandCount=72

[DLG:IDD_EDIT_FIND]
Type=1
Class=?
ControlCount=9
Control1=IDC_STATIC,static,1342308352
Control2=IDC_FIND_TEXT_EX,combobox,1344471106
Control3=IDC_EDIT_WHOLE_WORD,button,1342373891
Control4=IDC_EDIT_MATCH_CASE,button,1342242819
Control5=IDC_STATIC,button,1342308359
Control6=IDC_EDIT_DIRECTION_UP,button,1342308361
Control7=IDC_EDIT_DIRECTION_DOWN,button,1342177289
Control8=IDOK,button,1342373889
Control9=IDCANCEL,button,1342242816

[DLG:IDD_EDIT_REPLACE]
Type=1
Class=?
ControlCount=13
Control1=IDC_STATIC,static,1342308352
Control2=IDC_FINDREP_TEXT_EX,combobox,1344340226
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT_REPLACE_WITH_EX,combobox,1344340226
Control5=IDC_EDIT_WHOLE_WORD,button,1342373891
Control6=IDC_EDIT_MATCH_CASE,button,1342242819
Control7=IDC_STATIC,button,1342308359
Control8=IDC_EDIT_SCOPE_SELECTION,button,1342308361
Control9=IDC_EDIT_SCOPE_WHOLE_FILE,button,1342177289
Control10=IDC_EDIT_SKIP,button,1342373889
Control11=IDC_EDIT_REPLACE,button,1342242816
Control12=IDC_EDIT_REPLACE_ALL,button,1342242816
Control13=IDCANCEL,button,1342242816

