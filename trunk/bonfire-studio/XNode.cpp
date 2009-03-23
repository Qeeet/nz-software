// XNode.cpp: implementation of the CXNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Bonfire.h"
#include "XNode.h"
#include "MainFrm.h"
#include <COMDEF.H>
#include <cderr.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
extern CMainFrame* g_pMainFrame;
extern CBonfireApp theApp;

CXNode::CXNode()
{
	this->m_hTreeItem	= NULL;
	this->m_pParent		= NULL;
	this->m_pProject	= NULL;
	this->m_enNodeType	= XNODE_UNKNOWN;
}

CXNode::CXNode(const CXNode& _src)
{
	*this = _src;
}

CXNode& CXNode::operator=(const CXNode& _src)
{
	this->m_hTreeItem	= NULL;
	this->m_strName		= _src.m_strName;
	this->m_strPath		= _src.m_strPath;
	this->m_pProject	= _src.m_pProject;
	this->m_pParent		= _src.m_pParent;
	this->m_enNodeType	= _src.m_enNodeType;
	return *this;
}

CXNode* CXNode::Clone()
{
	CXNode* pNewNode = new CXNode(*this);
	return pNewNode;
}

CXNode::~CXNode()
{

}

int CXNode::CompareNodes(CXNode* pNode1, CXNode* pNode2)
{
	int cmp = pNode1->m_strName.CompareNoCase(pNode2->m_strName);
	
	// if the function thinks they are equal, make sure they really are
	if (cmp == 0 && pNode1 != pNode2)
		return -1;
	else
		return cmp;
}

// the compare callback function for sorting the tree
int CXNode::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	try
	{
		CXNode* pNode1 = (CXNode*)lParam1;
		CXNode* pNode2 = (CXNode*)lParam2;
		
		// check if the node types are identical
		if (pNode1->m_enNodeType == pNode2->m_enNodeType)
			// use regular node name comparison
			return pNode1->m_strName.CompareNoCase(pNode2->m_strName);
		else
		{
			// display folders or files first
			if (theApp.m_opOptions.projbar.dwShowFoldersBeforeFiles)
			{
				return (pNode1->m_enNodeType == XNODE_FOLDER) ? -1 : 1;
			}
			else
			{
				return (pNode1->m_enNodeType == XNODE_FILE) ? -1 : 1;
			}
		}
	}
	catch (...)
	{
		TRACE0("Exception in Compare Function");
	}
	
	return 0;
}

// get the full path of the node
CString CXNode::FullPath()
{
	return AddSlash(m_strPath) + m_strName;
}

//////////////////////////////////////////////////////////////////////

CXFile::CXFile()
{
	this->m_bSaved		= true;
	this->m_bNew		= true;
	this->m_bReadOnly	= false;
	this->m_pDoc		= NULL;
	this->m_enNodeType	= XNODE_FILE; 
}

CXFile::CXFile(const CXFile& _src)
{
	*this = _src;
}

CXFile& CXFile::operator=(const CXFile& _src)
{
	CXNode::operator=(_src);
	this->m_bReadOnly	= _src.m_bReadOnly;
	this->m_pDoc		= _src.m_pDoc;
	this->m_bNew		= _src.m_bNew;
	this->m_bSaved		= _src.m_bSaved;
	return *this;
}

CXFile* CXFile::Clone()
{
	CXFile* pNewNode = new CXFile(*this);
	return pNewNode;
}

CXFile::~CXFile()
{

}

void CXFile::Update(CString strFullPath, bool bSaved)
{
	this->m_bNew	= false;
	this->m_bSaved	= bSaved;
	this->m_strName = GetFileTitle(strFullPath);
	this->m_strPath = GetFilePath(strFullPath);
	g_pMainFrame->m_wndProjectBar.UpdateNode(this, FALSE);
}

//////////////////////////////////////////////////////////////////////

CXFolder::CXFolder()
{
	this->m_enNodeType	= XNODE_FOLDER;
	this->m_bVirtual	= false;

	this->m_arrFiles2.SetCompareFunction(CompareNodes);
}

CXFolder::CXFolder(const CXFolder& _src)
{
	*this = _src;
}

CXFolder& CXFolder::operator=(const CXFolder& _src)
{
	CXNode::operator=(_src);
	this->m_bVirtual = _src.m_bVirtual;

	//clear any existing folders
	m_arrFolders.clear();

	// clone the children folders
	int i	 = 0;
	int size = _src.m_arrFolders.size();
	for (i = 0; i < size; i++)
	{
		// recursively clone each child folder
		CXFolder* pFolder = (CXFolder*)_src.m_arrFolders[i];
		CXFolder* pNewFolder = pFolder->Clone();
		pNewFolder->m_pParent = this;
		m_arrFolders.push_back( pNewFolder );
	}

	// clear any existing files
	m_arrFiles.clear();

	// clone the children files
	size = _src.m_arrFiles.size();
	for (i = 0; i < size; i++)
	{
		CXFile* pFile = (CXFile*)_src.m_arrFiles[i];
		CXFile* pNewFile = pFile->Clone();
		pNewFile->m_pParent = this;
		m_arrFiles.push_back( pNewFile );
	}
	return *this;
}

CXFolder* CXFolder::Clone()
{
	CXFolder* pNewNode = new CXFolder(*this);
	return pNewNode;
}

// get the full path of the node
CString CXFolder::FullPath()
{
	return AddSlash(AddSlash(m_strPath) + m_strName);
}

// recursively generate xml code for the current folder
BOOL CXFolder::GenerateXML(CString &strSoFar)
{
	try
	{
		// get all the fodlers and their children
		for (int folders=0; folders<(int)this->m_arrFolders.size(); folders++)
		{
			CXFolder* pChildFolder = this->m_arrFolders[folders];
			if (pChildFolder == NULL)
				continue;
			strSoFar += "<folder name=\"" + EncodeString(pChildFolder->m_strName) + "\"";

			//if (pChildFolder->m_bVirtual)
			strSoFar += " path=\"" + EncodeString(AddSlash(pChildFolder->m_strPath)) + "\"";

			strSoFar += ">\n";
			pChildFolder->GenerateXML(strSoFar);
			strSoFar += "</folder>\n";
		}

		// get all the files
		int size = this->m_arrFiles.size();
		for (int files=0; files < size; files++)
		{
			//CXFile* pChildFile = (CXFile*)this->m_arrFiles2[files];
			CXFile* pChildFile = this->m_arrFiles[files];
			if (pChildFile == NULL)
				continue;
			strSoFar += "<file name=\"" + EncodeString(pChildFile->m_strName) + "\"";
			strSoFar += " path=\"" + EncodeString(AddSlash(pChildFile->m_strPath)) + "\"";
			strSoFar += "/>\n";
		}
	}
	catch (...)
	{
		AfxMessageBox("Error in recursive GenerateXML()", MB_OK | MB_ICONEXCLAMATION,0);
		return FALSE;
	}
	return TRUE;
}


CXFolder::~CXFolder()
{

}

//////////////////////////////////////////////////////////////////////

CXProject::CXProject()
{
	this->m_bReadOnly	= false;
	this->m_bMisc		= false;
	this->m_bNew		= true;
	this->m_bSaved		= true;
	this->m_pProject	= this;
	this->m_enNodeType	= XNODE_PROJECT;
}

CXProject::CXProject(bool bMisc, bool bNew, bool bSaved)
{
	this->m_bReadOnly	= false;
	this->m_bMisc		= bMisc;
	this->m_bNew		= bNew;
	this->m_bSaved		= bSaved;
	this->m_pProject	= this;
	this->m_enNodeType	= XNODE_PROJECT;
}

CXProject::CXProject(const CXProject& _src)
{
	*this = _src;
}

CXProject& CXProject::operator=(const CXProject& _src)
{
	CXFolder::operator=(_src);
	this->m_bReadOnly	= _src.m_bReadOnly;
	this->m_bMisc		= _src.m_bMisc;
	this->m_bNew		= _src.m_bNew;
	this->m_bSaved		= _src.m_bSaved;
	return *this;
}

CXProject* CXProject::Clone()
{
	CXProject* pNewNode = new CXProject(*this);
	return pNewNode;
}

// get the full path of the node
CString CXProject::FullPath()
{
	return AddSlash(m_strPath) + m_strName;
}

/////////////////////////////////////////////////////////////////////////////
// load the project from given path
/////////////////////////////////////////////////////////////////////////////
bool CXProject::LoadProject(LPCTSTR strProjectPath, OutputMessage* pMsg)
{
	try
	{
		//read XML and insert files/folders
		HRESULT				hr;
		VARIANT_BOOL		vbSuccess;
		IXMLDOMDocument*	pXMLDoc;
		IXMLDOMNode*		pXDN;
		IXMLDOMElement*		pXE;

		// initialize msxml.dll
		if (CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&pXMLDoc) == REGDB_E_CLASSNOTREG)
		{
			pMsg->enResult = MSG_ERROR;
			pMsg->strResult = "Microsoft XML Library not found! Please make sure you have msxml3.dll registered.";
			return false;
		}
		hr = pXMLDoc->QueryInterface(IID_IXMLDOMNode,(void**)&pXDN);

		// open the xml file
		FILE* fiXML = fopen(strProjectPath, "rb");
		if (!fiXML)
		{
			pMsg->enResult = MSG_ERROR;
			pMsg->strResult = "File IO Error";
			return false;
		}

		// read the file and convert to UNICODE
		size_t sztfile = filelength(fileno(fiXML));
		char* data = (char*)malloc(sztfile + 1);
		data[sztfile] = 0; // null terminate
		fread(data,sizeof(char),sztfile,fiXML);
		fclose(fiXML);
		BSTR wcdata = (OLECHAR*)calloc(sztfile + 1,sizeof(OLECHAR));
		if (MultiByteToWideChar(CP_ACP,0,data,sztfile + 1,(unsigned short*)wcdata,(sztfile + 1)) == 0)
		{
			pMsg->enResult = MSG_ERROR;
			pMsg->strResult = "Error converting to UNICODE";
			return false;
		}
		free(data);

		// load the text into DOM
		hr = pXMLDoc->loadXML(wcdata, &vbSuccess);

		// free memory
		// DEBUG ERRORS
		SysFreeString(wcdata);
		
		if (!vbSuccess)
		{
			pMsg->enResult = MSG_ERROR;
			pMsg->strResult = "Error loading text file into XML DOM";
			return false;
		}

		if (pXMLDoc->get_documentElement(&pXE) != S_OK)
		{
			pMsg->enResult = MSG_ERROR;
			pMsg->strResult = "Error getting Document Element";
			return false;
		}
		
		// set project properties
		this->m_strName		= GetFileTitle(strProjectPath);
		this->m_strPath		= GetFilePath(strProjectPath);
		this->m_pParent		= NULL;

		// create project tree items
		CreateProjectTree(pXE, this);

		// draw all the nodes
		g_pMainFrame->m_wndProjectBar.PopulateFolderTree(this);

		return true;
	}
	catch (...)
	{
		pMsg->enResult = MSG_ERROR;
		pMsg->strResult = "Error (Exception)";
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// recursive function to populate the project tree
/////////////////////////////////////////////////////////////////////////////
void CXProject::CreateProjectTree(IXMLDOMNode* pXNode, CXFolder* fParent)
{
	try
	{
		IXMLDOMNode*	pXMLNext	= NULL;
		IXMLDOMNode*	pXMLTemp	= NULL;

		pXNode->get_firstChild(&pXMLNext);
		while (pXMLNext != NULL)
		{
			DWORD					hr = 0;
			IXMLDOMNamedNodeMap*	pXMLMap = NULL;
			IXMLDOMNode*			pXMLAttrib = NULL;
			BSTR					nodeType = NULL;
			CXNode*					pNewNode = NULL;
		
			// get the name of the node - <file> or <folder>
			pXMLNext->get_nodeName(&nodeType);
			if (_wcsicmp(nodeType,L"file") == 0) //file
			{
				pNewNode = new CXFile();
				((CXFile*)pNewNode)->m_bNew	= false;
			}
			else if (_wcsicmp(nodeType,L"folder") == 0) //folder
			{
				pNewNode = new CXFolder();
			}
			else //unknown
			{
				//error
				TRACE0("Error in DOM - Node neither folder or file. Ignoring...");
				throw "";
			}

			// set the path
			pNewNode->m_strName = "";
			pNewNode->m_strPath = (fParent->m_enNodeType == XNODE_FOLDER)
								? fParent->FullPath()
								: fParent->m_strPath;

			// get file/folder name
			pXMLNext->get_attributes(&pXMLMap);

			//BSTR bstrPath = ::SysAllocString(_T("path"));
			//BSTR bstrName = ::SysAllocString(_T("name"));

			try
			{
				hr = pXMLMap->getNamedItem(L"path", &pXMLAttrib);
				if (SUCCEEDED(hr) && pXMLAttrib)
				{
			
					BSTR path = NULL;
					CString strParentPath = "";

					// get the path from the XML
					pXMLAttrib->get_text(&path);

					// set the path noted in the project file
					pNewNode->m_strPath = path;

					if (pNewNode->m_enNodeType == XNODE_FOLDER)
					{
						// get the path for parent node
						if (fParent->m_enNodeType == XNODE_FOLDER)
							strParentPath = fParent->FullPath();
						else
							strParentPath = fParent->m_strPath;

						// verify each folder for "vurtuality"
						if (pNewNode->m_strPath.Compare(strParentPath) == 0)
							((CXFolder*)pNewNode)->m_bVirtual = false;
						else
							((CXFolder*)pNewNode)->m_bVirtual = true;
					}

					// free memory
					SysFreeString(path);
					if (pXMLAttrib) pXMLAttrib->Release();	pXMLAttrib = NULL;
				}
			}
			catch (...)
			{}



			try
			{
				hr = pXMLMap->getNamedItem(L"name", &pXMLAttrib);
				if (SUCCEEDED(hr) && pXMLAttrib)
				{
			
					BSTR name = NULL;
					
					// get the name from the XML
					pXMLAttrib->get_text(&name);

					// set the name noted in the project file
					pNewNode->m_strName = name;

					// free memory
					SysFreeString(name);
					if (pXMLAttrib) pXMLAttrib->Release();	pXMLAttrib = NULL;
				}
			}
			catch (...)
			{}

			// set the parent project of the node
			pNewNode->m_pProject = this->m_pProject;

			// add it to the parent
			g_pMainFrame->m_wndProjectBar.AddNodeToTree(pNewNode, fParent, CMA_NONE, FALSE);

			if  (pNewNode->m_enNodeType == XNODE_FOLDER)
				// recursively iterate through the new folder
				CreateProjectTree(pXMLNext, ((CXFolder*)pNewNode) );

			SysFreeString(nodeType);

			// free memory
			if (pXMLMap) pXMLMap->Release();		pXMLMap = NULL;

			// go to the next xml sibling node
			pXMLTemp = pXMLNext;
			pXMLTemp->get_nextSibling(&pXMLNext);

			// free memory
			pXMLTemp->Release();
			pXMLTemp = NULL;
		}
	}
	catch (...)
	{
		//AfxMessageBox("Error in recursive CreateProjectTree", MB_OK | MB_ICONEXCLAMATION,0);
		return;
	}
}

//////////////////////////////////////////////////////////////////////
// save project as
//////////////////////////////////////////////////////////////////////
bool CXProject::SaveProjectAs()
{
	CString projectFullPath;
	bool retVal = false;

	try
	{
		int nResult = SaveFileDialog(AfxGetMainWnd(),projectFullPath,NULL,
			CString("Save Project " + m_strName + " As"),
			GetFileFilter(FILTER_PROJECTS),1,GetDefExt(FILTER_PROJECTS));

		if (nResult > 0)
		{
			//if (projectFullPath.Find('.', 0) < 1)
			//	projectFullPath += ".xpr";

			CString strOutput;
			strOutput.Format("Saving project: \"%s\" as \"%s\" (%s)", 
					m_strName, GetFileTitle(projectFullPath), projectFullPath); 
			// create the message object
			OutputMessage* pMsg = new OutputMessage();
			pMsg->enType = OUTPUT_APPSTATUS;
			pMsg->enResult = MSG_PROCESSING;
			pMsg->strMessage = strOutput;
			g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);

			if (SaveProjectToPath(projectFullPath))
			{
				// update the new name for the project
				this->m_strPath = GetFilePath(projectFullPath);
				this->m_strName = GetFileTitle(projectFullPath);
				this->m_bNew	= false;
				this->m_bMisc	= false;
				g_pMainFrame->m_wndProjectBar.UpdateNode(this, TRUE);

				SetModified(FALSE);

				// add project to MRU list
				theApp.AddToRecentProjectList(projectFullPath);

				pMsg->enResult = MSG_SUCCESS;
				pMsg->strResult = "Success";
				g_pMainFrame->m_wndOutputBar.UpdateLast();
				retVal = true;
			}
			else
			{
				pMsg->enResult = MSG_ERROR;
				pMsg->strResult = "Failed. Project was not saved.";
				g_pMainFrame->m_wndOutputBar.UpdateLast();
				retVal = false;
			}
		}
		else
			retVal = false;
		
		// the project is not the misc project anymore
		if (retVal)
			m_bMisc = false;

		// update the "virtual" folders
		UpdateChildren();
	}
	catch (...)
	{
		retVal = false;
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////
// save project to current path
//////////////////////////////////////////////////////////////////////
bool CXProject::SaveProject()
{
	if (this->m_bSaved)	return true;
	bool retVal = false;
	
	// show the wait cursor until everything is done
	CWaitCursor wc;

	if (this->m_bNew)
	{
		retVal = SaveProjectAs();
	}
	else
	{
		CString projectFullPath = AddSlash(m_strPath) + m_strName;
		CString strOutput;
		strOutput.Format("Saving project: \"%s\" (%s)", m_strName, projectFullPath); 

		// create the message object
		OutputMessage* pMsg = new OutputMessage();
		pMsg->enType		= OUTPUT_APPSTATUS;
		pMsg->enResult		= MSG_PROCESSING;
		pMsg->strMessage	= strOutput;
		pMsg->nLine			= 0;
		pMsg->nCol			= 0;
		g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);

		// save the project using the full file path
		if (SaveProjectToPath(projectFullPath))
		{
			SetModified(FALSE);
			
			pMsg->enResult = MSG_SUCCESS;
			pMsg->strResult = "Success";
			g_pMainFrame->m_wndOutputBar.UpdateLast();
			
			retVal = true;
		}
		else
		{
			pMsg->enResult = MSG_ERROR;
			pMsg->strResult = "Error. Project was not saved.";
			g_pMainFrame->m_wndOutputBar.UpdateLast();

			retVal = false;
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////
// save project to given path
/////////////////////////////////////////////////////////////////////////////
bool CXProject::SaveProjectToPath(LPCTSTR strProjectPath)
{
	bool				retVal		= true;
	HRESULT				hr;
	VARIANT_BOOL		vbSuccess;
	IXMLDOMDocument*	pXMLDoc		= NULL;
	IXMLDOMNode*		pXDN		= NULL;
	try
	{
		// initialize msxml.dll
		if (CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&pXMLDoc) == REGDB_E_CLASSNOTREG)
		{
			AfxMessageBox("Microsoft XML Library not found! Please make sure you have msxml3.dll registered.", MB_OK | MB_ICONEXCLAMATION,0);
			throw "";
		}
		hr = pXMLDoc->QueryInterface(IID_IXMLDOMNode,(void**)&pXDN);
		if (hr != S_OK)
		{
			AfxMessageBox("Error in QueryInterface", MB_OK | MB_ICONEXCLAMATION,0);
			throw "";
		}
		
		CString projectXML = "<?xml version=\"1.0\"?><project>";
   		
		// create project tree items
		if ( !this->GenerateXML(projectXML) )
			throw "";
		projectXML += "</project>";

		// load the string into a UNICODE string and into XML DOM
		BSTR strXML = projectXML.AllocSysString();
		hr = pXMLDoc->loadXML(strXML, &vbSuccess);
		SysFreeString(strXML);
	
		// save the XML into a file
		BSTR strPath = ((CString)strProjectPath).AllocSysString();
		_variant_t varProjPath = strPath;//_T("c:\\saveproject.xml");
		hr = pXMLDoc->save(varProjPath);
		SysFreeString(strPath);
	}
	catch (const char*) { retVal = false; }
	catch (...)
	{
		g_pMainFrame->AddOutputError("Unexpected error occured while saving project.");
		retVal = false;
	}

	// free memory
	if (pXDN) pXDN->Release();			pXDN = NULL;
	if (pXMLDoc) pXMLDoc->Release();	pXMLDoc = NULL;

	return retVal;
}

CXProject::~CXProject()
{

}

// setting the modified flag on the project
void CXProject::SetModified(BOOL bModified)
{
	this->m_bSaved = !bModified;
}

void CXFolder::UpdateChildren()
{
	CString strPath = "";
	if (m_enNodeType == XNODE_FOLDER)
		strPath = FullPath();
	else if (m_enNodeType == XNODE_PROJECT)
		strPath = m_strPath;

	int i	 = 0;
	int size = 0;
	
	// update the child folders
	size = this->m_arrFolders.size();
	for (i = 0; i < size; i++)
	{
		CXFolder* pFolder = (CXFolder*)this->m_arrFolders[i];

		// update the project pointer
		pFolder->m_pProject = this->m_pProject;

		// verify each folder for "virtuality"
		if (pFolder->m_strPath.Compare(strPath) == 0)
			pFolder->m_bVirtual = false;
		else
			pFolder->m_bVirtual = true;

		// recursively update the children nodes
		pFolder->UpdateChildren();

		// update the tree node for the folder
		g_pMainFrame->m_wndProjectBar.UpdateNode(pFolder, TRUE);
	}

	// update the child files
	size = this->m_arrFiles.size();
	for (i = 0; i < size; i++)
	{
		CXFile* pFile = (CXFile*)this->m_arrFiles[i];

		// update the project pointer
		pFile->m_pProject = this->m_pProject;
	}
}

void CXFolder::SortChildren( CTreeCtrl* p_wndTreeCtrl )
{
	TVSORTCB sortStruct;
	sortStruct.hParent = this->m_hTreeItem;
	sortStruct.lpfnCompare = CXNode::CompareFunc;

	// call the sort function of the treectrl
	p_wndTreeCtrl->SortChildrenCB( &sortStruct );
}

