// ColourPickerPopup.cpp : implementation file
//

#include "stdafx.h"
#include "ColourPicker.h"
#include "ColourPickerPopup.h"
#include "PatternSheet.h"
#include "../GeoMap/CommonStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColourPickerPopup

#define PATTERN_BOX_VALUE -4
#define DEFAULT_BOX_VALUE -3
#define CUSTOM_BOX_VALUE  -2
#define INVALID_COLOUR    -1

#define MAX_COLOURS      100


ColourTableEntry CColourPickerPopup::m_crColours[] = 
{
	{ RGB(0x00, 0x00, 0x00),    _T("黑色")				},
    { RGB(0xA5, 0x2A, 0x00),    _T("褐色")				},
    { RGB(0x00, 0x40, 0x40),    _T("橄榄绿")			},
    { RGB(0x00, 0x55, 0x00),    _T("深绿")				},
    { RGB(0x00, 0x00, 0x5E),    _T("深灰蓝")			},
    { RGB(0x00, 0x00, 0x8B),    _T("深蓝")				},
    { RGB(0x4B, 0x00, 0x82),    _T("靛蓝")				},
    { RGB(0x28, 0x28, 0x28),    _T("灰色-80%")			},

    { RGB(0x8B, 0x00, 0x00),    _T("深红")				},
    { RGB(0xFF, 0x68, 0x20),    _T("桔黄")				},
    { RGB(0x8B, 0x8B, 0x00),    _T("深黄")				},
    { RGB(0x00, 0x93, 0x00),    _T("绿色")				},
    { RGB(0x38, 0x8E, 0x8E),    _T("青色")				},
    { RGB(0x00, 0x00, 0xFF),    _T("蓝色")				},
    { RGB(0x7B, 0x7B, 0xC0),    _T("蓝灰色")			},
    { RGB(0x66, 0x66, 0x66),    _T("灰色-50%")			},

    { RGB(0xFF, 0x00, 0x00),    _T("红色")				},
    { RGB(0xFF, 0xAD, 0x5B),    _T("浅桔黄")			},
    { RGB(0x32, 0xCD, 0x32),    _T("酸橙色")			}, 
    { RGB(0x3C, 0xB3, 0x71),    _T("海绿")				},
    { RGB(51, 204, 204),		_T("宝石蓝")			},
    { RGB(0x7D, 0x9E, 0xC0),    _T("浅蓝")				},
    { RGB(0x80, 0x00, 0x80),    _T("紫色")				},
    { RGB(0x7F, 0x7F, 0x7F),    _T("灰色-40%")			},

    { RGB(0xFF, 0xC0, 0xCB),    _T("粉红")				},
    { RGB(0xFF, 0xD7, 0x00),    _T("金色")				},
    { RGB(0xFF, 0xFF, 0x00),    _T("黄色")				},    
    { RGB(0x00, 0xFF, 0x00),    _T("鲜绿")				},
    { RGB(0x00, 0xFF, 0xFF),    _T("青绿")				},
    { RGB(0, 204, 255),			_T("天蓝")				},
    { RGB(234, 128, 102),		_T("梅红")              },
    { RGB(0xC0, 0xC0, 0xC0),    _T("灰色-25%")			},

    { RGB(0xFF, 0xE4, 0xE1),    _T("玫瑰红")			},
    { RGB(255, 254, 153),		_T("棕黄")				},
    { RGB(0xFF, 0xFF, 0xE0),    _T("浅黄")				},
    { RGB(204, 255, 204),		_T("浅绿")				},
    { RGB(204, 255, 255),		_T("浅青绿")			},
    { RGB(153, 204, 255),		_T("淡蓝")				},
    { RGB(204, 153, 255),		_T("淡紫")				},
    { RGB(0xFF, 0xFF, 0xFF),    _T("白色")				}
};

/////////////////////////////////////////////////////////////////////////////
// CColourPickerPopup

CColourPickerPopup::CColourPickerPopup()
{
    Initialise();
}

CColourPickerPopup::CColourPickerPopup(CPoint p, COLORREF crColour, CWnd* pParentWnd,
                           LPCTSTR szDefaultText /* = NULL */,
                           LPCTSTR szCustomText  /* = NULL */,
						   LPCTSTR szPatternText /* = NULL */,
						   LPPATTERNSTYLE pPatternStyle/* = NULL*/)
{
    Initialise();

    m_crColour       = m_crInitialColour = crColour;
    m_pParent        = pParentWnd;
    m_strDefaultText = (szDefaultText)? szDefaultText : _T("");//缺省区文本，如“自动”
    m_strCustomText  = (szCustomText)?  szCustomText  : _T("");//定制区文本，如“其它颜色”
	m_strPatternText = (szPatternText)?  szPatternText  : _T("");//图案区文本，如“图案样式”	
	if(pPatternStyle)
	{
		m_pPatternStyle = new PATTERNSTYLE;

		memcpy(m_pPatternStyle, pPatternStyle, sizeof(PATTERNSTYLE));
	}

    CColourPickerPopup::Create(p, crColour, pParentWnd, szDefaultText, szCustomText, szPatternText, pPatternStyle);
}

void CColourPickerPopup::Initialise()
{    
	m_pPatternStyle = NULL;	

	m_pColorDlg = NULL;
	m_pPropSheet = NULL;

	m_nNumColours       = sizeof(m_crColours)/sizeof(ColourTableEntry);
    ASSERT(m_nNumColours <= MAX_COLOURS);
    if (m_nNumColours > MAX_COLOURS)
        m_nNumColours = MAX_COLOURS;

    m_nNumColumns       = 0;
    m_nNumRows          = 0;
    m_nBoxSize          = 18;
    m_nMargin           = ::GetSystemMetrics(SM_CXEDGE);
    m_nCurrentSel       = INVALID_COLOUR;
    m_nChosenColourSel  = INVALID_COLOUR;
	m_pParent           = NULL;
    m_crColour          = m_crInitialColour = RGB(0,0,0);

    // Idiot check: Make sure the colour square is at least 5 x 5;
    if (m_nBoxSize - 2*m_nMargin - 2 < 5) m_nBoxSize = 5 + 2*m_nMargin + 2;

    // Create the font
	OSVERSIONINFO osvi; 
	memset(&osvi, 0, sizeof(OSVERSIONINFO)); 
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO); 
	GetVersionEx (&osvi);

	NONCLIENTMETRICS ncm;
	int size;

	if( osvi.dwMajorVersion > 5)
	{
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		size = sizeof(NONCLIENTMETRICS);
	}
	else
	{
		ncm.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(ncm.iPaddedBorderWidth);
		size = sizeof(NONCLIENTMETRICS) - sizeof(ncm.iPaddedBorderWidth);
	}

	VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0));
    m_Font.CreateFontIndirect(&(ncm.lfMessageFont));

    // Create the palette
    struct {
        LOGPALETTE    LogPalette;
        PALETTEENTRY  PalEntry[MAX_COLOURS];
    } pal;

    LOGPALETTE* pLogPalette = (LOGPALETTE*) &pal;
    pLogPalette->palVersion    = 0x300;
    pLogPalette->palNumEntries = (WORD) m_nNumColours; 

    for (int i = 0; i < m_nNumColours; i++)
    {
        pLogPalette->palPalEntry[i].peRed   = GetRValue(m_crColours[i].crColour);
        pLogPalette->palPalEntry[i].peGreen = GetGValue(m_crColours[i].crColour);
        pLogPalette->palPalEntry[i].peBlue  = GetBValue(m_crColours[i].crColour);
        pLogPalette->palPalEntry[i].peFlags = 0;
    }

    m_Palette.CreatePalette(pLogPalette);
}

CColourPickerPopup::~CColourPickerPopup()
{
    m_Font.DeleteObject();
    m_Palette.DeleteObject();
	if(m_pPatternStyle)
		delete m_pPatternStyle;
	if(m_pPropSheet)
		delete m_pPropSheet;
	if(m_pColorDlg)
		delete m_pColorDlg;	
}

BOOL CColourPickerPopup::Create(CPoint p, COLORREF crColour, CWnd* pParentWnd,
                          LPCTSTR szDefaultText /* = NULL */,
                          LPCTSTR szCustomText  /* = NULL */,
						  LPCTSTR szPatternText /*= NULL*/,
						  LPPATTERNSTYLE pPatternStyle/* = NULL*/)
{
    ASSERT(pParentWnd && ::IsWindow(pParentWnd->GetSafeHwnd()));
    ASSERT(pParentWnd->IsKindOf(RUNTIME_CLASS(CColourPicker)));

    m_pParent  = pParentWnd;
    m_crColour = m_crInitialColour = crColour;

    // Get the class name and create the window
    CString szClassName = AfxRegisterWndClass(CS_CLASSDC|CS_SAVEBITS|CS_HREDRAW|CS_VREDRAW,
                                              0, (HBRUSH)GetStockObject(LTGRAY_BRUSH),0);

    if (!CWnd::CreateEx(0, szClassName, _T(""), WS_VISIBLE|WS_POPUP, 
                        p.x, p.y, 100, 100, // size updated soon
                        pParentWnd->GetSafeHwnd(), 0, NULL))
        return FALSE;

    // Store the Custom text
    if (szCustomText != NULL) 
        m_strCustomText = szCustomText;

    // Store the Default Area text
    if (szDefaultText != NULL) 
        m_strDefaultText = szDefaultText;

	// Store the Pattern Area text
    if (szPatternText != NULL) 
        m_strPatternText = szPatternText;

	// Store the Pattern
	if ( pPatternStyle != NULL )
		memcpy(m_pPatternStyle, pPatternStyle, sizeof(PATTERNSTYLE));
        
    // Set the window size
    SetWindowSize();

    // Create the tooltips
    CreateToolTips();

    // Find which cell (if any) corresponds to the initial colour
    FindCellFromColour(crColour);

    // Capture all mouse events for the life of this window
    SetCapture();

    return TRUE;
}

BEGIN_MESSAGE_MAP(CColourPickerPopup, CWnd)
    //{{AFX_MSG_MAP(CColourPickerPopup)
    ON_WM_NCDESTROY()
    ON_WM_LBUTTONUP()
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
    ON_WM_QUERYNEWPALETTE()
    ON_WM_PALETTECHANGED()
	ON_WM_KILLFOCUS()
	ON_WM_ACTIVATEAPP()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColourPickerPopup message handlers

// For tooltips
BOOL CColourPickerPopup::PreTranslateMessage(MSG* pMsg) 
{
    m_ToolTip.RelayEvent(pMsg);
    return CWnd::PreTranslateMessage(pMsg);
}

// If an arrow key is pressed, then move the selection
void CColourPickerPopup::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    int row = GetRow(m_nCurrentSel),
        col = GetColumn(m_nCurrentSel);
    int nSelection = m_nCurrentSel;

    if (nChar == VK_DOWN) 
    {
        if (row == DEFAULT_BOX_VALUE) 
            row = col = 0; 
        else
		{
			if (row == CUSTOM_BOX_VALUE)
			{
				if (m_strPatternText.GetLength())
					row = col = PATTERN_BOX_VALUE;
				else if (m_strDefaultText.GetLength())
					row = col = DEFAULT_BOX_VALUE;
				else
					row = col = 0;
			}
			else if (row == PATTERN_BOX_VALUE)
			{
				if (m_strDefaultText.GetLength())
					row = col = DEFAULT_BOX_VALUE;
				else
					row = col = 0;
			}
			else
			{
				row++;
				if (GetIndex(row,col) < 0)
				{
					if (m_strCustomText.GetLength())
						row = col = CUSTOM_BOX_VALUE;
					else
					{
						
						if (m_strPatternText.GetLength())
							row = col = PATTERN_BOX_VALUE;
						else if (m_strDefaultText.GetLength())
							row = col = DEFAULT_BOX_VALUE;
						else 
							row = col = 0;
					}
				}
			}
		}
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_UP) 
    {
        if (row == DEFAULT_BOX_VALUE)
        {
			if (m_strPatternText.GetLength())
                row = col = PATTERN_BOX_VALUE;            
			else if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;            
            else
           { 
                row = GetRow(m_nNumColours-1); 
                col = GetColumn(m_nNumColours-1); 
            }
        }
		else if (row == PATTERN_BOX_VALUE)
		{
			if (m_strCustomText.GetLength())
				row = col = CUSTOM_BOX_VALUE;
			else
			{
				row = GetRow(m_nNumColours-1); 
				col = GetColumn(m_nNumColours-1); 
			}
		}
        else if (row == CUSTOM_BOX_VALUE)
        { 
            row = GetRow(m_nNumColours-1); 
            col = GetColumn(m_nNumColours-1); 
        }
        else if (row > 0) row--;
        else /* row == 0 */
        {
            if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
			else if (m_strPatternText.GetLength())
                row = col = PATTERN_BOX_VALUE;
            else if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;
            else
            { 
                row = GetRow(m_nNumColours-1); 
                col = GetColumn(m_nNumColours-1); 
            }
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_RIGHT) 
    {
        if (row == DEFAULT_BOX_VALUE) 
            row = col = 0; 
        else if (row == PATTERN_BOX_VALUE)
        {
            if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
            else
                row = col = 0;
        }
		else if (row == CUSTOM_BOX_VALUE)
        {
            if (m_strPatternText.GetLength())
                row = col = PATTERN_BOX_VALUE;
			else if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
            else
                row = col = 0;
        }
        else if (col < m_nNumColumns-1) 
            col++;
        else 
        { 
            col = 0; row++;
        }

        if (GetIndex(row,col) == INVALID_COLOUR)
        {
            if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;
			else if (m_strPatternText.GetLength())
				row = col = PATTERN_BOX_VALUE;
            else if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
            else
                row = col = 0;
        }

        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_LEFT) 
    {
        if (row == DEFAULT_BOX_VALUE)
        {
            if (m_strPatternText.GetLength())
                row = col = PATTERN_BOX_VALUE;
            else
           { 
				if (m_strCustomText.GetLength())
					row = col = CUSTOM_BOX_VALUE;
				else
				{
					row = GetRow(m_nNumColours-1); 
					col = GetColumn(m_nNumColours-1); 
				}
            }
        }
		else if (row == PATTERN_BOX_VALUE)
        {
            if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;
            else
           { 
				row = GetRow(m_nNumColours-1); 
				col = GetColumn(m_nNumColours-1); 				
            }
        }
        else if (row == CUSTOM_BOX_VALUE)
        { 
            row = GetRow(m_nNumColours-1); 
            col = GetColumn(m_nNumColours-1); 
        }
        else if (col > 0) col--;
        else /* col == 0 */
        {
            if (row > 0) { row--; col = m_nNumColumns-1; }
            else 
            {
                if (m_strDefaultText.GetLength())
                    row = col = DEFAULT_BOX_VALUE;
				else if (m_strPatternText.GetLength())
                    row = col = PATTERN_BOX_VALUE;
                else if (m_strCustomText.GetLength())
                    row = col = CUSTOM_BOX_VALUE;
                else
                { 
                    row = GetRow(m_nNumColours-1); 
                    col = GetColumn(m_nNumColours-1); 
                }
            }
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_ESCAPE) 
    {
        m_crColour = m_crInitialColour;
        EndSelection(CPN_SELENDCANCEL);
        return;
    }

    if (nChar == VK_RETURN || nChar == VK_SPACE)
    {
        EndSelection(CPN_SELENDOK);
        return;
    }

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

// auto-deletion
void CColourPickerPopup::OnNcDestroy() 
{
    CWnd::OnNcDestroy();
    delete this;
}

void CColourPickerPopup::OnPaint() 
{
    CPaintDC dc(this); // device context for painting

    // Draw the Default Area text
    if (m_strDefaultText.GetLength())
        DrawCell(&dc, DEFAULT_BOX_VALUE);
 
    // Draw colour cells
    for (int i = 0; i < m_nNumColours; i++)
        DrawCell(&dc, i);
    
    // Draw custom text
    if (m_strCustomText.GetLength())
        DrawCell(&dc, CUSTOM_BOX_VALUE);

	if (m_strPatternText.GetLength())
        DrawCell(&dc, PATTERN_BOX_VALUE);

    // Draw raised window edge (ex-window style WS_EX_WINDOWEDGE is sposed to do this,
    // but for some reason isn't
    CRect rect;
    GetClientRect(rect);
    dc.DrawEdge(rect, EDGE_RAISED, BF_RECT);
}

void CColourPickerPopup::OnMouseMove(UINT nFlags, CPoint point) 
{
    int nNewSelection = INVALID_COLOUR;

    // Translate points to be relative raised window edge
    point.x -= m_nMargin;
    point.y -= m_nMargin;

    // First check we aren't in text box
    if (m_strCustomText.GetLength() && m_CustomTextRect.PtInRect(point))
        nNewSelection = CUSTOM_BOX_VALUE;
    else
	{
		if (m_strDefaultText.GetLength() && m_DefaultTextRect.PtInRect(point))
			nNewSelection = DEFAULT_BOX_VALUE;
		else
		{
			if (m_strPatternText.GetLength() && m_PatternTextRect.PtInRect(point))
				nNewSelection = PATTERN_BOX_VALUE;
			else
			{
				// Take into account text box
				if (m_strDefaultText.GetLength()) 
					point.y -= m_DefaultTextRect.Height();  
				
				// Get the row and column
				nNewSelection = GetIndex(point.y / m_nBoxSize, point.x / m_nBoxSize);
				
				// In range? If not, default and exit
				if (nNewSelection < 0 || nNewSelection >= m_nNumColours)
				{
					CWnd::OnMouseMove(nFlags, point);
					return;
				}
			}
		}
	}

    // OK - we have the row and column of the current selection (may be CUSTOM_BOX_VALUE)
    // Has the row/col selection changed? If yes, then redraw old and new cells.

	if (nNewSelection != m_nCurrentSel)
		ChangeSelection(nNewSelection, nFlags&MK_LBUTTON?TRUE:FALSE);
	
    CWnd::OnMouseMove(nFlags, point);
}

// End selection on LButtonUp
void CColourPickerPopup::OnLButtonUp(UINT nFlags, CPoint point) 
{    
    CWnd::OnLButtonUp(nFlags, point);

    DWORD pos = GetMessagePos();
    point = CPoint(LOWORD(pos), HIWORD(pos));

    if (m_WindowRect.PtInRect(point))
        EndSelection(CPN_SELENDOK);
    else
        EndSelection(CPN_SELENDCANCEL);
}

/////////////////////////////////////////////////////////////////////////////
// CColourPickerPopup implementation

int CColourPickerPopup::GetIndex(int row, int col) const
{ 
    if ((row == CUSTOM_BOX_VALUE || col == CUSTOM_BOX_VALUE) && m_strCustomText.GetLength())
        return CUSTOM_BOX_VALUE;
    else if ((row == DEFAULT_BOX_VALUE || col == DEFAULT_BOX_VALUE) && m_strDefaultText.GetLength())
        return DEFAULT_BOX_VALUE;
	else if ((row == PATTERN_BOX_VALUE || col == PATTERN_BOX_VALUE) && m_strPatternText.GetLength())
		return PATTERN_BOX_VALUE;
    else if (row < 0 || col < 0 || row >= m_nNumRows || col >= m_nNumColumns)
        return INVALID_COLOUR;
    else
    {
        if (row*m_nNumColumns + col >= m_nNumColours)
            return INVALID_COLOUR;
        else
            return row*m_nNumColumns + col;
    }
}

int CColourPickerPopup::GetRow(int nIndex) const               
{ 
    if (nIndex == CUSTOM_BOX_VALUE && m_strCustomText.GetLength())
        return CUSTOM_BOX_VALUE;
    else if (nIndex == DEFAULT_BOX_VALUE && m_strDefaultText.GetLength())
        return DEFAULT_BOX_VALUE;
	else if (nIndex == PATTERN_BOX_VALUE && m_strPatternText.GetLength())
        return PATTERN_BOX_VALUE;
    else if (nIndex < 0 || nIndex >= m_nNumColours)
        return INVALID_COLOUR;
    else
        return nIndex / m_nNumColumns; 
}

int CColourPickerPopup::GetColumn(int nIndex) const            
{ 
    if (nIndex == CUSTOM_BOX_VALUE && m_strCustomText.GetLength())
        return CUSTOM_BOX_VALUE;
    else if (nIndex == DEFAULT_BOX_VALUE && m_strDefaultText.GetLength())
        return DEFAULT_BOX_VALUE;
	else if (nIndex == PATTERN_BOX_VALUE && m_strPatternText.GetLength())
        return PATTERN_BOX_VALUE;
    else if (nIndex < 0 || nIndex >= m_nNumColours)
        return INVALID_COLOUR;
    else
        return nIndex % m_nNumColumns; 
}

void CColourPickerPopup::FindCellFromColour(COLORREF crColour)
{
    if (crColour == CLR_DEFAULT && m_strDefaultText.GetLength())
    {
        m_nChosenColourSel = DEFAULT_BOX_VALUE;
        return;
    }

    for (int i = 0; i < m_nNumColours; i++)
    {
        if (GetColour(i) == crColour)
        {
            m_nChosenColourSel = i;
            return;
        }
    }

    if (m_strCustomText.GetLength())
        m_nChosenColourSel = CUSTOM_BOX_VALUE;
    else
        m_nChosenColourSel = INVALID_COLOUR;
}

// Gets the dimensions of the colour cell given by (row,col)
BOOL CColourPickerPopup::GetCellRect(int nIndex, const LPRECT& rect)
{
    if (nIndex == CUSTOM_BOX_VALUE)
    {
        ::SetRect(rect, 
                  m_CustomTextRect.left,  m_CustomTextRect.top,
                  m_CustomTextRect.right, m_CustomTextRect.bottom);
        return TRUE;
    }
    else
	{
		if (nIndex == DEFAULT_BOX_VALUE)
		{
			::SetRect(rect, 
				m_DefaultTextRect.left,  m_DefaultTextRect.top,
				m_DefaultTextRect.right, m_DefaultTextRect.bottom);
			return TRUE;
		}
		else if (nIndex == PATTERN_BOX_VALUE)
		{
			::SetRect(rect, 
				m_PatternTextRect.left,  m_PatternTextRect.top,
				m_PatternTextRect.right, m_PatternTextRect.bottom);
			return TRUE;
		}
	}

    if (nIndex < 0 || nIndex >= m_nNumColours)
        return FALSE;

    rect->left = GetColumn(nIndex) * m_nBoxSize + m_nMargin;
    rect->top  = GetRow(nIndex) * m_nBoxSize + m_nMargin;

    // Move everything down if we are displaying a default text area
    if (m_strDefaultText.GetLength()) 
        rect->top += (m_nMargin + m_DefaultTextRect.Height());

    rect->right = rect->left + m_nBoxSize;
    rect->bottom = rect->top + m_nBoxSize;

    return TRUE;
}

// Works out an appropriate size and position of this window
void CColourPickerPopup::SetWindowSize()
{
    CSize TextSize;

    // If we are showing a custom or default text area, get the font and text size.
    if (m_strCustomText.GetLength() || m_strDefaultText.GetLength() || m_strPatternText.GetLength())
    {
        CClientDC dc(this);
        CFont* pOldFont = (CFont*) dc.SelectObject(&m_Font);

        // Get the size of the custom text (if there IS custom text)
        TextSize = CSize(0,0);
        if (m_strCustomText.GetLength())
            TextSize = dc.GetTextExtent(m_strCustomText);

        // Get the size of the default text (if there IS default text)
        if (m_strDefaultText.GetLength())
        {
            CSize DefaultSize = dc.GetTextExtent(m_strDefaultText);
            if (DefaultSize.cx > TextSize.cx) TextSize.cx = DefaultSize.cx;
            if (DefaultSize.cy > TextSize.cy) TextSize.cy = DefaultSize.cy;
        }

		if (m_strPatternText.GetLength())
        {
            CSize PatternSize = dc.GetTextExtent(m_strPatternText);
            if (PatternSize.cx > TextSize.cx) TextSize.cx = PatternSize.cx;
            if (PatternSize.cy > TextSize.cy) TextSize.cy = PatternSize.cy;
        }

        dc.SelectObject(pOldFont);
        TextSize += CSize(2*m_nMargin,2*m_nMargin);

        // Add even more space to draw the horizontal line
        TextSize.cy += 2*m_nMargin + 2;
    }

    // Get the number of columns and rows
    //m_nNumColumns = (int) sqrt((double)m_nNumColours);    // for a square window (yuk)
    m_nNumColumns = 8;
    m_nNumRows = m_nNumColours / m_nNumColumns;
    if (m_nNumColours % m_nNumColumns) m_nNumRows++;

    // Get the current window position, and set the new size
    CRect rect;
    GetWindowRect(rect);

    m_WindowRect.SetRect(rect.left, rect.top, 
                         rect.left + m_nNumColumns*m_nBoxSize + 2*m_nMargin,
                         rect.top  + m_nNumRows*m_nBoxSize + 2*m_nMargin);

    // if custom text, then expand window if necessary, and set text width as
    // window width
    if (m_strDefaultText.GetLength()) 
    {
        if (TextSize.cx > m_WindowRect.Width())
            m_WindowRect.right = m_WindowRect.left + TextSize.cx;
        TextSize.cx = m_WindowRect.Width()-2*m_nMargin;

        // Work out the text area
        m_DefaultTextRect.SetRect(m_nMargin, m_nMargin, 
                                  m_nMargin+TextSize.cx, 2*m_nMargin+TextSize.cy);
        m_WindowRect.bottom += m_DefaultTextRect.Height() + 2*m_nMargin;
    }

    // if custom text, then expand window if necessary, and set text width as
    // window width
    if (m_strCustomText.GetLength()) 
    {
        if (TextSize.cx > m_WindowRect.Width())
            m_WindowRect.right = m_WindowRect.left + TextSize.cx;
        TextSize.cx = m_WindowRect.Width()-2*m_nMargin;

        // Work out the text area
        m_CustomTextRect.SetRect(m_nMargin, m_WindowRect.Height(), 
                                 m_nMargin+TextSize.cx, 
                                 m_WindowRect.Height()+m_nMargin+TextSize.cy);
        m_WindowRect.bottom += m_CustomTextRect.Height() + 2*m_nMargin;
   }

	if (m_strPatternText.GetLength()) 
    {
        if (TextSize.cx > m_WindowRect.Width())
            m_WindowRect.right = m_WindowRect.left + TextSize.cx;
        TextSize.cx = m_WindowRect.Width()-2*m_nMargin;
		
        // Work out the text area
        m_PatternTextRect.SetRect(m_nMargin, m_WindowRect.Height(), 
			m_nMargin+TextSize.cx, 
			m_WindowRect.Height()+m_nMargin+TextSize.cy);
        m_WindowRect.bottom += m_PatternTextRect.Height() + 2*m_nMargin;
	}

    // Need to check it'll fit on screen: Too far right?
    CSize ScreenSize(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
    if (m_WindowRect.right > ScreenSize.cx)
        m_WindowRect.OffsetRect(-(m_WindowRect.right - ScreenSize.cx), 0);

    // Too far left?
    if (m_WindowRect.left < 0)
        m_WindowRect.OffsetRect( -m_WindowRect.left, 0);

    // Bottom falling out of screen?
    if (m_WindowRect.bottom > ScreenSize.cy)
    {
        CRect ParentRect;
        m_pParent->GetWindowRect(ParentRect);
        m_WindowRect.OffsetRect(0, -(ParentRect.Height() + m_WindowRect.Height()));
    }

    // Set the window size and position
    MoveWindow(m_WindowRect, TRUE);
}

void CColourPickerPopup::CreateToolTips()
{
    // Create the tool tip
    if (!m_ToolTip.Create(this)) return;

    // Add a tool for each cell
    for (int i = 0; i < m_nNumColours; i++)
    {
        CRect rect;
        if (!GetCellRect(i, rect)) continue;
            m_ToolTip.AddTool(this, GetColourName(i), rect, 1);
    }
}

void CColourPickerPopup::ChangeSelection(int nIndex, BOOL bDown)
{
    CClientDC dc(this);        // device context for drawing
	
    if (nIndex > m_nNumColours)
        nIndex = CUSTOM_BOX_VALUE; 	

    if ((m_nCurrentSel >= 0 && m_nCurrentSel < m_nNumColours) ||
        m_nCurrentSel == CUSTOM_BOX_VALUE || m_nCurrentSel == DEFAULT_BOX_VALUE || m_nCurrentSel == PATTERN_BOX_VALUE)
    {
        // Set Current selection as invalid and redraw old selection (this way
        // the old selection will be drawn unselected)
		int OldSel = m_nCurrentSel;
		m_nCurrentSel = INVALID_COLOUR;
		if(bDown)  DrawDownCell(&dc, OldSel);
		else DrawCell(&dc, OldSel);
    }

    // Set the current selection as row/col and draw (it will be drawn selected)
    m_nCurrentSel = nIndex;
	if(bDown)  DrawDownCell(&dc, m_nCurrentSel);
		else  DrawCell(&dc, m_nCurrentSel);
	
    // Store the current colour
    if (m_nCurrentSel == CUSTOM_BOX_VALUE)
        m_pParent->SendMessage(CPN_SELCHANGE, (WPARAM) m_crInitialColour, (LPARAM) m_pPatternStyle);
	else if (m_nCurrentSel == PATTERN_BOX_VALUE)
		m_pParent->SendMessage(CPN_SELCHANGE, (WPARAM) m_crInitialColour, (LPARAM) m_pPatternStyle);
    else if (m_nCurrentSel == DEFAULT_BOX_VALUE)
    {
        m_crColour = CLR_DEFAULT;
        m_pParent->SendMessage(CPN_SELCHANGE, (WPARAM) CLR_DEFAULT, (LPARAM) m_pPatternStyle);
    }
    else
    {
        m_crColour = GetColour(m_nCurrentSel);
        m_pParent->SendMessage(CPN_SELCHANGE, (WPARAM) m_crColour, (LPARAM) m_pPatternStyle);
    }
}

void CColourPickerPopup::EndSelection(int nMessage)
{
    ReleaseCapture();

	if(m_pColorDlg) return;
	if( m_pPropSheet )return;

    // If custom text selected, perform a custom colour selection
    if (nMessage != CPN_SELENDCANCEL && m_nCurrentSel == CUSTOM_BOX_VALUE)
    {		
        m_pColorDlg = new CColorDialog(m_crInitialColour, CC_FULLOPEN | CC_ANYCOLOR, this);
		
        if (m_pColorDlg->DoModal() == IDOK)
            m_crColour = m_pColorDlg->GetColor();
        else
            m_crColour = m_crInitialColour;
		
		if(m_pPatternStyle) 
			delete m_pPatternStyle;

		m_pPatternStyle = NULL;

	}
	if (nMessage != CPN_SELENDCANCEL && m_nCurrentSel == PATTERN_BOX_VALUE)
    {        
		m_crColour = m_crInitialColour;

		if(!m_pPatternStyle)
			m_pPatternStyle = new PATTERNSTYLE;	

		m_pPropSheet = new CPatternSheet(m_strPatternText.Left(m_strPatternText.GetLength()-7));
		
		m_pPropSheet->patternStyle = *m_pPatternStyle;
		if(m_pPropSheet->DoModal() == IDOK)
		{
			m_pPatternStyle->m_nFill = m_pPropSheet->patternStyle.m_nFill;
			m_pPatternStyle->m_GradientFill = m_pPropSheet->patternStyle.m_GradientFill;
			m_pPatternStyle->m_PatternFill = m_pPropSheet->patternStyle.m_PatternFill;
			m_pPatternStyle->m_PictureFill = m_pPropSheet->patternStyle.m_PictureFill;						
		}
		else
		{
			if(m_pPatternStyle) 
				delete m_pPatternStyle;
			m_pPatternStyle = NULL;
		}

	} 

	if (nMessage != CPN_SELENDCANCEL && m_nCurrentSel == DEFAULT_BOX_VALUE)
    {
        if(m_pPatternStyle) 
			delete m_pPatternStyle;

		m_pPatternStyle = NULL;
	}

    if (nMessage == CPN_SELENDCANCEL)
	{
        m_crColour = m_crInitialColour;
		
		if(m_pPatternStyle) 
			delete m_pPatternStyle;
		
		m_pPatternStyle = NULL;
	}

    m_pParent->SendMessage(nMessage, (WPARAM) m_crColour, (LPARAM) m_pPatternStyle);
    
    DestroyWindow();
}

void CColourPickerPopup::DrawCell(CDC* pDC, int nIndex)
{
    // For the Custom Text area
    if (m_strCustomText.GetLength() && nIndex == CUSTOM_BOX_VALUE)
    {
        // The extent of the actual text button
        CRect TextButtonRect = m_CustomTextRect;
        TextButtonRect.top += 2*m_nMargin;

        // Fill background
        pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw horizontal line
        pDC->FillSolidRect(m_CustomTextRect.left+2*m_nMargin, m_CustomTextRect.top,
                           m_CustomTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DSHADOW));
        pDC->FillSolidRect(m_CustomTextRect.left+2*m_nMargin, m_CustomTextRect.top+1,
                           m_CustomTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DHILIGHT));

        TextButtonRect.DeflateRect(1,1);

        // fill background
        if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DLIGHT));
        else
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw button
        if (m_nCurrentSel == nIndex) 
            pDC->DrawEdge(TextButtonRect, BDR_RAISEDINNER, BF_RECT);
        else if (m_nChosenColourSel == nIndex)
            pDC->DrawEdge(TextButtonRect, BDR_SUNKENINNER, BF_RECT);

        // Draw custom text
        CFont *pOldFont = (CFont*) pDC->SelectObject(&m_Font);
        pDC->SetBkMode(TRANSPARENT);
        pDC->DrawText(m_strCustomText, TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(pOldFont);

        return;
    }

	// For the Pattern Text area
	if (m_strPatternText.GetLength() && nIndex == PATTERN_BOX_VALUE)
    {
        // The extent of the actual text button
        CRect TextButtonRect = m_PatternTextRect;
        TextButtonRect.top += 2*m_nMargin;

        // Fill background
        pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw horizontal line
        pDC->FillSolidRect(m_PatternTextRect.left+2*m_nMargin, m_PatternTextRect.top,
                           m_PatternTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DSHADOW));
        pDC->FillSolidRect(m_PatternTextRect.left+2*m_nMargin, m_PatternTextRect.top+1,
                           m_PatternTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DHILIGHT));

        TextButtonRect.DeflateRect(1,1);

        // fill background
        if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DLIGHT));
        else
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw button
        if (m_nCurrentSel == nIndex) 
            pDC->DrawEdge(TextButtonRect, BDR_RAISEDINNER, BF_RECT);
        else if (m_nChosenColourSel == nIndex)
            pDC->DrawEdge(TextButtonRect, BDR_SUNKENINNER, BF_RECT);

        // Draw custom text
        CFont *pOldFont = (CFont*) pDC->SelectObject(&m_Font);
        pDC->SetBkMode(TRANSPARENT);
        pDC->DrawText(m_strPatternText, TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(pOldFont);

        return;
    }

    // For the Default Text area
    if (m_strDefaultText.GetLength() && nIndex == DEFAULT_BOX_VALUE)
    {
        // Fill background
        pDC->FillSolidRect(m_DefaultTextRect, ::GetSysColor(COLOR_3DFACE));

        // The extent of the actual text button
        CRect TextButtonRect = m_DefaultTextRect;
        TextButtonRect.DeflateRect(1,1);

        // fill background
        if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DLIGHT));
        else
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw thin line around text
        CRect LineRect = TextButtonRect;
        LineRect.DeflateRect(2*m_nMargin,2*m_nMargin);
        CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
        CPen* pOldPen = pDC->SelectObject(&pen);
        pDC->SelectStockObject(NULL_BRUSH);
        pDC->Rectangle(LineRect);
        pDC->SelectObject(pOldPen);

        // Draw button
        if (m_nCurrentSel == nIndex) 
            pDC->DrawEdge(TextButtonRect, BDR_RAISEDINNER, BF_RECT);
        else if (m_nChosenColourSel == nIndex)
            pDC->DrawEdge(TextButtonRect, BDR_SUNKENOUTER, BF_RECT);

        // Draw custom text
        CFont *pOldFont = (CFont*) pDC->SelectObject(&m_Font);
        pDC->SetBkMode(TRANSPARENT);
        pDC->DrawText(m_strDefaultText, TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(pOldFont);

        return;
    }        

    CRect rect;
    if (!GetCellRect(nIndex, rect)) return;

    // Select and realize the palette
    CPalette* pOldPalette;
    if (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
    {
        pOldPalette = pDC->SelectPalette(&m_Palette, FALSE);
        pDC->RealizePalette();
    }

    // fill background
    if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DHILIGHT));
    else
       pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));

    // Draw button
    if (m_nCurrentSel == nIndex) 
        pDC->DrawEdge(rect, BDR_RAISEDINNER, BF_RECT);
    else if (m_nChosenColourSel == nIndex)
	    pDC->DrawEdge(rect, BDR_SUNKENOUTER , BF_RECT);
	
    CBrush brush(PALETTERGB(GetRValue(GetColour(nIndex)), 
                            GetGValue(GetColour(nIndex)), 
                            GetBValue(GetColour(nIndex)) ));
    CPen   pen;
    pen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));

    CBrush* pOldBrush = (CBrush*) pDC->SelectObject(&brush);
    CPen*   pOldPen   = (CPen*)   pDC->SelectObject(&pen);

    // Draw the cell colour
    rect.DeflateRect(m_nMargin+1, m_nMargin+1);

    pDC->Rectangle(rect);

    // restore DC and cleanup
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
    brush.DeleteObject();
    pen.DeleteObject();

    if (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
        pDC->SelectPalette(pOldPalette, FALSE);
}

BOOL CColourPickerPopup::OnQueryNewPalette() 
{
    Invalidate();    
    return CWnd::OnQueryNewPalette();
}

void CColourPickerPopup::OnPaletteChanged(CWnd* pFocusWnd) 
{
    CWnd::OnPaletteChanged(pFocusWnd);

    if (pFocusWnd->GetSafeHwnd() != GetSafeHwnd())
        Invalidate();
}

void CColourPickerPopup::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

    ReleaseCapture();
    //DestroyWindow(); - causes crash when Custom colour dialog appears.
}

// KillFocus problem fix suggested by Paul Wilkerson.
void CColourPickerPopup::OnActivateApp(BOOL bActive, DWORD hTask) 
{
	CWnd::OnActivateApp(bActive, hTask);

	// If Deactivating App, cancel this selection
	if (!bActive)
		 EndSelection(CPN_SELENDCANCEL);
}


void CColourPickerPopup::DrawDownCell(CDC *pDC, int nIndex)
{
	// For the Custom Text area
    if (m_strCustomText.GetLength() && nIndex == CUSTOM_BOX_VALUE)
    {
        // The extent of the actual text button
        CRect TextButtonRect = m_CustomTextRect;
        TextButtonRect.top += 2*m_nMargin;

        // Fill background
        pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw horizontal line
        pDC->FillSolidRect(m_CustomTextRect.left+2*m_nMargin, m_CustomTextRect.top,
                           m_CustomTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DSHADOW));
        pDC->FillSolidRect(m_CustomTextRect.left+2*m_nMargin, m_CustomTextRect.top+1,
                           m_CustomTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DHILIGHT));

        TextButtonRect.DeflateRect(1,1);

        // fill background
        if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DLIGHT));
        else
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw button
        if (m_nCurrentSel == nIndex) 
            pDC->DrawEdge(TextButtonRect, EDGE_SUNKEN, BF_RECT);
        else if (m_nChosenColourSel == nIndex)
            pDC->DrawEdge(TextButtonRect, BDR_SUNKENINNER, BF_RECT);

        // Draw custom text
        CFont *pOldFont = (CFont*) pDC->SelectObject(&m_Font);
        pDC->SetBkMode(TRANSPARENT);
        pDC->DrawText(m_strCustomText, TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(pOldFont);

        return;
    }

	// For the Pattern Text area
	if (m_strPatternText.GetLength() && nIndex == PATTERN_BOX_VALUE)
    {
        // The extent of the actual text button
        CRect TextButtonRect = m_PatternTextRect;
        TextButtonRect.top += 2*m_nMargin;

        // Fill background
        pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw horizontal line
        pDC->FillSolidRect(m_PatternTextRect.left+2*m_nMargin, m_PatternTextRect.top,
                           m_PatternTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DSHADOW));
        pDC->FillSolidRect(m_PatternTextRect.left+2*m_nMargin, m_PatternTextRect.top+1,
                           m_PatternTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DHILIGHT));

        TextButtonRect.DeflateRect(1,1);

        // fill background
        if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DLIGHT));
        else
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw button
        if (m_nCurrentSel == nIndex) 
            pDC->DrawEdge(TextButtonRect, EDGE_SUNKEN , BF_RECT);
        else if (m_nChosenColourSel == nIndex)
            pDC->DrawEdge(TextButtonRect, BDR_SUNKENINNER, BF_RECT);

        // Draw custom text
        CFont *pOldFont = (CFont*) pDC->SelectObject(&m_Font);
        pDC->SetBkMode(TRANSPARENT);
        pDC->DrawText(m_strPatternText, TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(pOldFont);

        return;
    }

    // For the Default Text area
    if (m_strDefaultText.GetLength() && nIndex == DEFAULT_BOX_VALUE)
    {
        // Fill background
        pDC->FillSolidRect(m_DefaultTextRect, ::GetSysColor(COLOR_3DFACE));

        // The extent of the actual text button
        CRect TextButtonRect = m_DefaultTextRect;
        TextButtonRect.DeflateRect(1,1);

        // fill background
        if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DLIGHT));
        else
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw thin line around text
        CRect LineRect = TextButtonRect;
        LineRect.DeflateRect(2*m_nMargin,2*m_nMargin);
        CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
        CPen* pOldPen = pDC->SelectObject(&pen);
        pDC->SelectStockObject(NULL_BRUSH);
        pDC->Rectangle(LineRect);
        pDC->SelectObject(pOldPen);

        // Draw button
        if (m_nCurrentSel == nIndex) 
            pDC->DrawEdge(TextButtonRect, EDGE_SUNKEN, BF_RECT);
        else if (m_nChosenColourSel == nIndex)
            pDC->DrawEdge(TextButtonRect, BDR_SUNKENOUTER, BF_RECT);

        // Draw custom text
        CFont *pOldFont = (CFont*) pDC->SelectObject(&m_Font);
        pDC->SetBkMode(TRANSPARENT);
        pDC->DrawText(m_strDefaultText, TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(pOldFont);

        return;
    }        

    CRect rect;
    if (!GetCellRect(nIndex, rect)) return;

    // Select and realize the palette
    CPalette* pOldPalette;
    if (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
    {
        pOldPalette = pDC->SelectPalette(&m_Palette, FALSE);
        pDC->RealizePalette();
    }

    // fill background
    if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DHILIGHT));
    else
       pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));

    // Draw button
    if (m_nCurrentSel == nIndex) 
        pDC->DrawEdge(rect, EDGE_SUNKEN, BF_RECT);
    else if (m_nChosenColourSel == nIndex)
	    pDC->DrawEdge(rect, BDR_SUNKENOUTER , BF_RECT);
	
    CBrush brush(PALETTERGB(GetRValue(GetColour(nIndex)), 
                            GetGValue(GetColour(nIndex)), 
                            GetBValue(GetColour(nIndex)) ));
    CPen   pen;
    pen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));

    CBrush* pOldBrush = (CBrush*) pDC->SelectObject(&brush);
    CPen*   pOldPen   = (CPen*)   pDC->SelectObject(&pen);

    // Draw the cell colour
    rect.DeflateRect(m_nMargin+1, m_nMargin+1);

    pDC->Rectangle(rect);

    // restore DC and cleanup
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
    brush.DeleteObject();
    pen.DeleteObject();

    if (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
        pDC->SelectPalette(pOldPalette, FALSE);
}

void CColourPickerPopup::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nNewSelection = INVALID_COLOUR;

    // Translate points to be relative raised window edge
    point.x -= m_nMargin;
    point.y -= m_nMargin;

    // First check we aren't in text box
    if (m_strCustomText.GetLength() && m_CustomTextRect.PtInRect(point))
        nNewSelection = CUSTOM_BOX_VALUE;
    else
	{
		if (m_strDefaultText.GetLength() && m_DefaultTextRect.PtInRect(point))
			nNewSelection = DEFAULT_BOX_VALUE;
		else
		{
			if (m_strPatternText.GetLength() && m_PatternTextRect.PtInRect(point))
				nNewSelection = PATTERN_BOX_VALUE;
			else
			{
				// Take into account text box
				if (m_strDefaultText.GetLength()) 
					point.y -= m_DefaultTextRect.Height();  
				
				// Get the row and column
				nNewSelection = GetIndex(point.y / m_nBoxSize, point.x / m_nBoxSize);
				
				// In range? If not, default and exit
				if (nNewSelection < 0 || nNewSelection >= m_nNumColours)
				{
					CWnd::OnLButtonDown(nFlags, point);
					return;
				}
			}
		}
	}

    // OK - we have the row and column of the current selection (may be CUSTOM_BOX_VALUE)
    // Has the row/col selection changed? If yes, then redraw old and new cells.

	ChangeSelection(nNewSelection, TRUE);
	
	CWnd::OnLButtonDown(nFlags, point);
}


