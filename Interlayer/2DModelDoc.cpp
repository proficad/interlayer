// 2DModelDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "2DModelDoc.h"
#include "MainFrm.h"
#include <float.h>
#include "vfc/InverseDist.h"

// C2DModelDoc

// 比较交点位置
int compareDepth( const void *arg1, const void *arg2 )
{	
	tagCellDepth *pS1, *pS2;
	pS1 = (tagCellDepth*)arg1;
	pS2 = (tagCellDepth*)arg2;
	if( pS1->m_dDepth < pS2->m_dDepth )
		return -1;
	else if( pS1->m_dDepth > pS2->m_dDepth )
		return 1;
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////
// C2DModelDoc

IMPLEMENT_DYNCREATE(C2DModelDoc, CDocument)

C2DModelDoc::C2DModelDoc()
{
	m_dMinValue = DBL_MAX;
	m_dMaxValue = -DBL_MAX;			// 数据最小最大值
	
	m_dDispMinValue = DBL_MAX;
	m_dDispMaxValue = -DBL_MAX;	

	m_clrMax = RGB(255, 0, 0);
	m_clrMin = RGB(255,255,255);	

	m_pCellDefault = RUNTIME_CLASS(CFieldCell);

	m_sizeGridMap.cx = 1;
	m_sizeGridMap.cy = 1;

	m_lGridMapScale = 25000;

	m_strModelFileName = _T("");

	GetPrinterPageSize(m_sizeDoc);
}

BOOL C2DModelDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

C2DModelDoc::~C2DModelDoc()
{
	DeleteGrid();
}

void C2DModelDoc::GetPrinterPageSize(CSize &pageSize)
{
	CPrintInfo PrintInfo;
	pageSize = CSize(2100, 2960);		// default to A4
	if (AfxGetApp()->GetPrinterDeviceDefaults(&PrintInfo.m_pPD->m_pd))
	{
		if (PrintInfo.m_pPD->CreatePrinterDC() != NULL)
		{
			if (PrintInfo.m_pPD->m_pd.hDC == NULL)
				PrintInfo.m_pPD->CreatePrinterDC();

			// attach printer dc and retreive docSize
			CDC dcPrint;
			dcPrint.Attach(PrintInfo.m_pPD->m_pd.hDC);
			pageSize.cx = 10 * dcPrint.GetDeviceCaps(HORZSIZE);
			pageSize.cy = 10 * dcPrint.GetDeviceCaps(VERTSIZE);
		}
	}
}

CSize C2DModelDoc::GetPictureSize() const//图幅大小
{
	CSize size = m_sizeGridMap;
	// 2*边框+内框距井的距离50*2+页边距
	size.cx += 200;
	size.cy += 200;

	return size;
}

CSize C2DModelDoc::GetGridMapSize() const//图形大小
{
	CSize  size(1,1);

	double xWidth, yHeight;
	xWidth = (m_gridModel.m_dMaxX - m_gridModel.m_dMinX)*100.0*100.0/m_lGridMapScale;
	yHeight =(m_gridModel.m_dMaxY - m_gridModel.m_dMinY)*100.0*100.0/m_lGridMapScale;
	size.cx = xWidth;
	size.cy = yHeight;
	return size;
}

long C2DModelDoc::RealToLogicX(double x)
{
	CSize size = m_sizeGridMap;
	return (x - m_gridModel.m_dMinX)*100*100.0/m_lGridMapScale- size.cx /2;
}

long C2DModelDoc::RealToLogicY(double y)
{
	CSize size = m_sizeGridMap;
	return (y - m_gridModel.m_dMinY)*100*100.0/m_lGridMapScale - size.cy /2;
}

double C2DModelDoc::LogicToRealX(long x)
{
	CSize size = m_sizeGridMap;

	return MulDiv((x + size.cx/2) , m_lGridMapScale, 10000) + m_gridModel.m_dMinX;
}

double C2DModelDoc::LogicToRealY(long y)
{
	CSize size = m_sizeGridMap;
	return MulDiv((y + size.cy/2) , m_lGridMapScale , 10000) + m_gridModel.m_dMinY;
}

//////////////////////////////////////////////////////////////////////////
void C2DModelDoc::CreateGrid(int index)		//
{
	//DeleteGrid();

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CString strMsg;
	strMsg = "正在计算网格模型数据，请稍等...";
	pMF->GetStatusBar().SetPaneText(0, strMsg);	

	if( m_gridModel.m_bCornerPointGrid )
		m_arDepth.RemoveAll();

	for(int j=0;j<m_gridModel.m_nGridY;j++)
	{
		for (int i=0;i<m_gridModel.m_nGridX;i++)
		{
			CFieldCell * pCell = GetCell(i, j);
			pCell->m_i = i;
			pCell->m_j = j;
			pCell->m_k = index;
			pCell->m_nX = m_gridModel.m_nGridX-1;
			pCell->m_nY = m_gridModel.m_nGridY-1;
			pCell->m_nZ = m_gridModel.m_nGridZ-1;

			pCell->m_cornerPoint[0] = m_gridModel.GetCornerPoint(i,j,index,0);
			pCell->m_cornerPoint[1] = m_gridModel.GetCornerPoint(i,j,index,1);
			pCell->m_cornerPoint[2] = m_gridModel.GetCornerPoint(i,j,index,2);
			pCell->m_cornerPoint[3] = m_gridModel.GetCornerPoint(i,j,index,3);

			// 求网格中心
			CPoint2D pt1[2],pt2[2];

			pt1[0].x = (pCell->m_cornerPoint[0].x+pCell->m_cornerPoint[1].x+pCell->m_cornerPoint[3].x)/3;
			pt1[0].y = (pCell->m_cornerPoint[0].y+pCell->m_cornerPoint[1].y+pCell->m_cornerPoint[3].y)/3;
			pt1[1].x = (pCell->m_cornerPoint[1].x+pCell->m_cornerPoint[2].x+pCell->m_cornerPoint[3].x)/3;
			pt1[1].y = (pCell->m_cornerPoint[1].y+pCell->m_cornerPoint[2].y+pCell->m_cornerPoint[3].y)/3;

			pt2[0].x = (pCell->m_cornerPoint[0].x+pCell->m_cornerPoint[1].x+pCell->m_cornerPoint[2].x)/3;
			pt2[0].y = (pCell->m_cornerPoint[0].y+pCell->m_cornerPoint[1].y+pCell->m_cornerPoint[2].y)/3;
			pt2[1].x = (pCell->m_cornerPoint[0].x+pCell->m_cornerPoint[2].x+pCell->m_cornerPoint[3].x)/3;
			pt2[1].y = (pCell->m_cornerPoint[0].y+pCell->m_cornerPoint[2].y+pCell->m_cornerPoint[3].y)/3;

			CPoint2D ptC = pCell->intersect(pt1, pt2); // 求网格中心Z值

			pCell->m_centerPoint[0].x = ptC.x;
			pCell->m_centerPoint[0].y = ptC.y;
			pCell->m_centerPoint[0].z = (pCell->m_cornerPoint[0].z
				+ pCell->m_cornerPoint[1].z
				+ pCell->m_cornerPoint[2].z
				+ pCell->m_cornerPoint[3].z)/4.0;

			pCell->m_cornerPoint[4] = m_gridModel.GetCornerPoint(i,j,index,4);
			pCell->m_cornerPoint[5] = m_gridModel.GetCornerPoint(i,j,index,5);
			pCell->m_cornerPoint[6] = m_gridModel.GetCornerPoint(i,j,index,6);
			pCell->m_cornerPoint[7] = m_gridModel.GetCornerPoint(i,j,index,7);

			pCell->m_centerPoint[1].x = (pCell->m_cornerPoint[4].x
				+ pCell->m_cornerPoint[5].x
				+ pCell->m_cornerPoint[6].x
				+ pCell->m_cornerPoint[7].x)/4.0;
			pCell->m_centerPoint[1].y = (pCell->m_cornerPoint[4].y
				+ pCell->m_cornerPoint[5].y
				+ pCell->m_cornerPoint[6].y
				+ pCell->m_cornerPoint[7].y)/4.0;

			pt1[0].x = (pCell->m_cornerPoint[4].x+pCell->m_cornerPoint[5].x+pCell->m_cornerPoint[7].x)/3;
			pt1[0].y = (pCell->m_cornerPoint[4].y+pCell->m_cornerPoint[5].y+pCell->m_cornerPoint[7].y)/3;
			pt1[1].x = (pCell->m_cornerPoint[5].x+pCell->m_cornerPoint[6].x+pCell->m_cornerPoint[7].x)/3;
			pt1[1].y = (pCell->m_cornerPoint[5].y+pCell->m_cornerPoint[6].y+pCell->m_cornerPoint[7].y)/3;


			pt2[0].x = (pCell->m_cornerPoint[4].x+pCell->m_cornerPoint[5].x+pCell->m_cornerPoint[6].x)/3;
			pt2[0].y = (pCell->m_cornerPoint[4].y+pCell->m_cornerPoint[5].y+pCell->m_cornerPoint[6].y)/3;
			pt2[1].x = (pCell->m_cornerPoint[4].x+pCell->m_cornerPoint[6].x+pCell->m_cornerPoint[7].x)/3;
			pt2[1].y = (pCell->m_cornerPoint[4].y+pCell->m_cornerPoint[6].y+pCell->m_cornerPoint[7].y)/3;

			ptC = pCell->intersect(pt1, pt2);

			pCell->m_centerPoint[1].x = ptC.x;
			pCell->m_centerPoint[1].y = ptC.y;

			pCell->m_centerPoint[1].z = (pCell->m_cornerPoint[4].z
				+ pCell->m_cornerPoint[5].z
				+ pCell->m_cornerPoint[6].z
				+ pCell->m_cornerPoint[7].z)/4.0;

			pCell->m_dValue = m_values[index*m_gridModel.m_nGridX*m_gridModel.m_nGridY + j*m_gridModel.m_nGridX + i];

			if( m_gridModel.m_bCornerPointGrid )
			{
				tagCellDepth cellDepth;
				cellDepth.m_i = i;
				cellDepth.m_j = j;
				cellDepth.m_dDepth = pCell->m_cornerPoint[0].z;
				m_arDepth.Add(cellDepth);
			}
		}
	}

	if( m_gridModel.m_bCornerPointGrid )
		qsort(m_arDepth.GetData(),m_arDepth.GetSize(),sizeof(tagCellDepth),compareDepth);

	m_sizeGridMap = GetGridMapSize();//图形大小
	m_sizePicture = GetPictureSize();

	strMsg = "计算完成!";
	pMF->GetStatusBar().SetPaneText(0, strMsg);	

	m_nCurLayer = index;
}

void C2DModelDoc::DeleteGrid()
{
	if( m_fieldDat.GetSize() < 1 ) 
		return;

	for (int row = 0; row < m_gridModel.m_nGridX; row++)
	{
		FIELD_ROW* pRow = m_fieldDat[row];
		for (int j=0;j<m_gridModel.m_nGridY;j++)
		{
			CFieldCell *pCell = pRow->GetAt(j);
			delete pCell;
		}
		delete pRow;
	}

	// Remove all rows
	m_fieldDat.RemoveAll();
}

BEGIN_MESSAGE_MAP(C2DModelDoc, CDocument)
	//{{AFX_MSG_MAP(C2DModelDoc)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C2DModelDoc diagnostics

#ifdef _DEBUG
void C2DModelDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void C2DModelDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// C2DModelDoc serialization

void C2DModelDoc::Serialize(CArchive& ar)
{
	CWaitCursor wait;
	if (ar.IsStoring())
	{
		// TODO: add storing code here

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

		CString strMsg;
		strMsg = _T("正在保存数据，请稍等...");
		pMF->GetStatusBar().SetPaneText(0, strMsg);	

		ar << m_strModelFileName;

		int size = m_gridModel.m_nGridX*m_gridModel.m_nGridY*m_gridModel.m_nGridZ;
		
		ar << size;

		for( int i = 0; i< size; i++)
		{
			ar << m_values[i];
		}

		ar << m_clrMax;			// 最大值颜色
		ar << m_clrMin;			// 最小值颜色
		ar << m_dDispMinValue;
		ar << m_dDispMaxValue;

		strMsg = _T("保存数据成功!");
		pMF->GetStatusBar().SetPaneText(0, strMsg);	
	}
	else
	{
		// TODO: add loading code here

		CWaitCursor wait;
		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

		CString strMsg;
		strMsg = _T("正在读取数据，请稍等...");
		pMF->GetStatusBar().SetPaneText(0, strMsg);	
		
		ar >> m_strModelFileName;

		CString strFileName = ar.GetFile()->GetFilePath();

		CSplitPath path(strFileName);
		strFileName = path.GetDrive();
		strFileName += path.GetDirectory();
		strFileName += m_strModelFileName;
		CFile gridfile;
		if( gridfile.Open(strFileName, CFile::modeRead | CFile::typeBinary ) )
		{
			CArchive ar(&gridfile,CArchive::load);
			m_gridModel.Serialize(ar);
			ar.Close();
			gridfile.Close();
		}

		double v;
		int size;
		ar >> size;
		for( int i = 0; i< size; i++)
		{
			ar >> v;			
			m_values.push_back(v);
			if( m_dMinValue > v)
				m_dMinValue = v;
			if( m_dMaxValue < v)
				m_dMaxValue = v;
		}

		ar >> m_clrMax;			// 最大值颜色
		ar >> m_clrMin;			// 最小值颜色
		ar >> m_dDispMinValue;
		ar >> m_dDispMaxValue;

		m_fieldDat.SetSize(m_gridModel.m_nGridX);

		for (int row = 0; row < m_gridModel.m_nGridX; row++)
		{		
			m_fieldDat[row] = new FIELD_ROW;
			m_fieldDat[row]->SetSize(m_gridModel.m_nGridY);
			for (int col = 0; col < m_gridModel.m_nGridY; col++)
			{
				FIELD_ROW* pRow = m_fieldDat[row];
				pRow->SetAt(col, CreateCell(row, col));
			}
		}

		if( m_dDispMinValue > m_dDispMaxValue ) // 如果是初始的
		{
			m_dDispMinValue = m_dMinValue;
			m_dDispMaxValue = m_dMaxValue;
		}

		CreateGrid(0);

		strMsg = _T("读取数据成功!");
		pMF->GetStatusBar().SetPaneText(0, strMsg);	
	}
}

/////////////////////////////////////////////////////////////////////////////
// C2DModelDoc commands


CFieldCell* C2DModelDoc::CreateCell(int nRow, int nCol)
{
	if (!m_pCellDefault || !m_pCellDefault->IsDerivedFrom(RUNTIME_CLASS(CFieldCell)))
	{
		ASSERT( FALSE);
		return NULL;
	}

	CFieldCell* pCell = (CFieldCell*) m_pCellDefault->CreateObject();

	if (!pCell)
		return NULL;

	return pCell;
}

CFieldCell* C2DModelDoc::GetCell(int nRow, int nCol) const
{
	if (nRow < 0 || nRow >= m_gridModel.m_nGridX || nCol < 0 || nCol >= m_gridModel.m_nGridY) 
		return NULL;

	FIELD_ROW* pRow = m_fieldDat[nRow];

	if (!pRow) 
		return NULL;

	return pRow->GetAt(nCol);
}

void C2DModelDoc::SetCellValue(int nX, int nY, int nZ, double v)
{
	m_values[nZ*m_gridModel.m_nGridX*m_gridModel.m_nGridY + nY*m_gridModel.m_nGridX + nX ] = v;
}

void C2DModelDoc::DeleteContents()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDocument::DeleteContents();

	DeleteGrid();
}

void C2DModelDoc::SetTitle(LPCTSTR lpszTitle)
{
	// TODO: 在此添加专用代码和/或调用基类

	CDocument::SetTitle(lpszTitle);
}

BOOL C2DModelDoc::SaveModified()
{
	// TODO: 在此添加专用代码和/或调用基类

	if (!IsModified())
		return TRUE;        // ok to continue

	// get name/title of document
	CString name = _T("[");
	name += m_strTitle;
	name += _T("]");

	CString prompt;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoFileSave())
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}
