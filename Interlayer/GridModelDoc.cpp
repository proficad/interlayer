// GridModelDoc.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Interlayer.h"
#include "GridModelDoc.h"
#include "MainFrm.h"
#include "./3DLib/Vertex3D.h"

extern CString GetFileErrMsg(int nErr);

// CGridModelDoc

IMPLEMENT_DYNCREATE(CGridModelDoc, CDocument)

CGridModelDoc::CGridModelDoc()
{	
}

BOOL CGridModelDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CGridModelDoc::~CGridModelDoc()
{
}



BEGIN_MESSAGE_MAP(CGridModelDoc, CDocument)
END_MESSAGE_MAP()


// CGridModelDoc ���

#ifdef _DEBUG
void CGridModelDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CGridModelDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CGridModelDoc ���л�

void CGridModelDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

		CString strMsg;
		strMsg = "���ڱ�������ģ�����ݣ����Ե�...";
		pMF->GetStatusBar().SetPaneText(0, strMsg);	
		m_gridModel.Serialize(ar);

		strMsg = "�������ݳɹ���";
		pMF->GetStatusBar().SetPaneText(0, strMsg);	
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

		CString strMsg;

		strMsg = "���ڶ�ȡ����ģ�����ݣ����Ե�...";
		pMF->GetStatusBar().SetPaneText(0, strMsg);

		m_gridModel.Serialize(ar);

		strMsg = "��ȡ���ݳɹ���";
		pMF->GetStatusBar().SetPaneText(0, strMsg);	
	}
}
#endif

BOOL CGridModelDoc::ExportGridModel(LPCTSTR lpszGridFileName)
{
	// TODO: Add your command handler code here
	try
	{
		CStdioFile file;
		if( !file.Open(lpszGridFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeText))
			return FALSE;

		if( m_gridModel.m_bCornerPointGrid )
		{
			CString strTmp;
			file.WriteString("SPECGRID\n");
			strTmp.Format("%d %d %d 1 F /\n",m_gridModel.m_nGridX,m_gridModel.m_nGridY, m_gridModel.m_nGridZ);
			file.WriteString(strTmp);

			file.WriteString("\nCOORD\n");
			for (int j=0;j<m_gridModel.m_nGridY+1;j++)
			{
				for (int i=0;i<m_gridModel.m_nGridX+1; i++)
				{
					CVertex3D point = m_gridModel.m_coordGrid[j*(m_gridModel.m_nGridX+1)*2+2*i];
					strTmp.Format("%lf %lf %lf",point.GetX(), point.GetY(), point.GetZ());
					file.WriteString(strTmp);
					point = m_gridModel.m_coordGrid[j*2*(m_gridModel.m_nGridX+1)+2*i+1];
					strTmp.Format(" %lf %lf %lf\n",point.GetX(), point.GetY(), point.GetZ());
					file.WriteString(strTmp);
				}

				if( j<m_gridModel.m_nGridX )
					file.WriteString("\n");
			}
			file.WriteString("/\n\n");
			file.WriteString("ZCORN\n");

			int nCount = 0;
			for (int i=0;i<m_gridModel.m_nGridY*m_gridModel.m_nGridX*m_gridModel.m_nGridZ*8;i++)
			{
				strTmp.Format("%lf",m_gridModel.m_zcornGrid[i]);
				file.WriteString(strTmp);
				nCount ++;
				if( (i+1)%(2*m_gridModel.m_nGridX) == 0 )
				{
					file.WriteString("\n");
					nCount = 0;
				}
				else
				{
					if(nCount == 6)
					{
						file.WriteString("\n");
						nCount = 0;
					}
					else
						file.WriteString(" ");
				}			
			}
			file.WriteString("/\n\n");
		}
		else
		{
			CString strTmp;
			file.WriteString("SPECGRID\n");
			strTmp.Format("%d %d %d 1 F /\n",m_gridModel.m_nGridX,m_gridModel.m_nGridY, m_gridModel.m_nGridZ);
			file.WriteString(strTmp);
		}
		file.Close();

		return TRUE;
	}
	catch (CFileException* pe)
	{
		CString msg;
		msg.Format(_T("    �ļ��򿪴���: %s"), GetFileErrMsg(pe->m_cause));
		AfxMessageBox(msg);
		pe->Delete();
		return FALSE;
	}
	catch (...)
	{
		AfxMessageBox(_T("�޷�Ԥ�ϵĴ���!"));
		return FALSE;
	}
}

