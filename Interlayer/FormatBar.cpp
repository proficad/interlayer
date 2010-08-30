// FormatBar.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "FormatBar.h"


// CFormatBar

static CSize GetBaseUnits(CFont* pFont)
{
	ASSERT(pFont != NULL);
	ASSERT(pFont->GetSafeHandle() != NULL);
	CDC m_dcScreen;

	m_dcScreen.Attach(::GetDC(NULL));
	
	pFont = m_dcScreen.SelectObject(pFont);
	TEXTMETRIC tm;
	VERIFY(m_dcScreen.GetTextMetrics(&tm));

	m_dcScreen.SelectObject(pFont);

	::ReleaseDC(NULL, m_dcScreen.Detach());

	return CSize(tm.tmAveCharWidth, tm.tmHeight);
}

IMPLEMENT_SERIAL(CFormatBar, CMFCToolBar, 1)

CFormatBar::CFormatBar()
{
	DWORD dwVersion = ::GetVersion();
	
	BOOL m_bWin4 = (BYTE)dwVersion >= 4;
	
	int m_nDefFont = (m_bWin4) ? DEFAULT_GUI_FONT : ANSI_VAR_FONT;
	
	CFont fnt;
	fnt.Attach(GetStockObject(m_nDefFont));
	m_szBaseUnits = GetBaseUnits(&fnt);
	CMFCToolBarFontComboBox::m_nFontHeight = m_szBaseUnits.cy;
	m_nNumColours = 0;
}

BEGIN_MESSAGE_MAP(CFormatBar, CMFCToolBar)
	//{{AFX_MSG_MAP(CFormatBar)
	//}}AFX_MSG_MAP
	// Global help commands
END_MESSAGE_MAP()

void CFormatBar::OnReset ()
{
	// Insert font name combo:
 	CMFCToolBarFontComboBox* pFontButton = CreateFontComboButton ();
 	ASSERT_VALID (pFontButton);
 
 	int iIndex = CommandToIndex (IDC_FONTNAME);
 	RemoveButton (iIndex);
 
 	if (iIndex > GetCount ())
 	{
 		iIndex = GetCount ();
 	}
 
 	InsertButton (*pFontButton, iIndex);
 	delete pFontButton;

	// Insert font size combo:
	CMFCToolBarFontSizeComboBox comboButtonFontSize (IDC_FONTSIZE,
		afxCommandManager->GetCmdImage (IDC_FONTSIZE, FALSE),
		WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWN,
		10*m_szBaseUnits.cx + 10);

	iIndex = CommandToIndex (IDC_FONTSIZE);
	RemoveButton (iIndex);

	if (iIndex > GetCount ())
	{
		iIndex = GetCount ();
	}

	InsertButton (comboButtonFontSize, iIndex);

	// Insert the color menu button:
	CMFCColorMenuButton* pColorButton = CreateFontColor ();
	ReplaceButton (ID_CHAR_COLOR, *pColorButton);
	delete pColorButton;
}
//***************************************************************************************
void CFormatBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
 	USES_CONVERSION;
 	CMFCToolBar::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);
	
	return;
}
//***************************************************************************************
CMFCToolBarFontComboBox* CFormatBar::CreateFontComboButton ()
{
	return new CMFCToolBarFontComboBox (IDC_FONTNAME, 
		afxCommandManager->GetCmdImage (IDC_FONTNAME, FALSE),
		TRUETYPE_FONTTYPE | DEVICE_FONTTYPE,
		DEFAULT_CHARSET,
		WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWN |
		CBS_AUTOHSCROLL | CBS_HASSTRINGS | CBS_OWNERDRAWFIXED,
		(3*LF_FACESIZE*m_szBaseUnits.cx)/2);
}

// To hold the colours and their names
typedef struct {
    COLORREF crColour;
    TCHAR    *szName;
} ColourTableEntry;

#define MAX_COLOURS      100

static ColourTableEntry crColours[] = 
{
    { RGB(0x00, 0x00, 0x00),    _T("Black")             },
    { RGB(0xA5, 0x2A, 0x00),    _T("Brown")             },
    { RGB(0x00, 0x40, 0x40),    _T("Dark Olive Green")  },
    { RGB(0x00, 0x55, 0x00),    _T("Dark Green")        },
    { RGB(0x00, 0x00, 0x5E),    _T("Dark Teal")         },
    { RGB(0x00, 0x00, 0x8B),    _T("Dark Blue")         },
    { RGB(0x4B, 0x00, 0x82),    _T("Indigo")            },
    { RGB(0x28, 0x28, 0x28),    _T("Gray-80%")          },

    { RGB(0x8B, 0x00, 0x00),    _T("Dark Red")          },
    { RGB(0xFF, 0x68, 0x20),    _T("Orange")            },
    { RGB(0x8B, 0x8B, 0x00),    _T("Dark Yellow")       },
    { RGB(0x00, 0x93, 0x00),    _T("Green")             },
    { RGB(0x38, 0x8E, 0x8E),    _T("Teal")              },
    { RGB(0x00, 0x00, 0xFF),    _T("Blue")              },
    { RGB(0x7B, 0x7B, 0xC0),    _T("Blue-Gray")         },
    { RGB(0x66, 0x66, 0x66),    _T("Gray-50%")          },

    { RGB(0xFF, 0x00, 0x00),    _T("Red")               },
    { RGB(0xFF, 0xAD, 0x5B),    _T("Light Orange")      },
    { RGB(0x32, 0xCD, 0x32),    _T("Lime")              }, 
    { RGB(0x3C, 0xB3, 0x71),    _T("Sea Green")         },
    { RGB(51, 204, 204),		_T("Aqua")              },
    { RGB(0x7D, 0x9E, 0xC0),    _T("Light Blue")        },
    { RGB(0x80, 0x00, 0x80),    _T("Violet")            },
    { RGB(0x7F, 0x7F, 0x7F),    _T("Gray-40%")          },

    { RGB(0xFF, 0xC0, 0xCB),    _T("Pink")              },
    { RGB(0xFF, 0xD7, 0x00),    _T("Gold")              },
    { RGB(0xFF, 0xFF, 0x00),    _T("Yellow")            },    
    { RGB(0x00, 0xFF, 0x00),    _T("Bright Green")      },
    { RGB(0x00, 0xFF, 0xFF),    _T("Turquoise")         },
    { RGB(0, 204, 255),			_T("Sky Blue")          },
    { RGB(234, 128, 102),		_T("Plum")              },
    { RGB(0xC0, 0xC0, 0xC0),    _T("Gray-25%")          },

    { RGB(0xFF, 0xE4, 0xE1),    _T("Rose")              },
    { RGB(255, 254, 153),		_T("Tan")               },
    { RGB(0xFF, 0xFF, 0xE0),    _T("Light Yellow")      },
    { RGB(204, 255, 204),		_T("Light Green ")      },
    { RGB(204, 255, 255),		_T("Light Turquoise")	},
    { RGB(153, 204, 255),		_T("Pale Blue")         },
    { RGB(204, 153, 255),		_T("Lavender")          },
    { RGB(0xFF, 0xFF, 0xFF),    _T("White")             }
};

CMFCColorMenuButton* CFormatBar::CreateFontColor ()
{
	if (m_palColorPicker.GetSafeHandle () == NULL)
	{
		m_nNumColours = sizeof (crColours)/sizeof(ColourTableEntry);
		ASSERT(m_nNumColours <= MAX_COLOURS);
		if (m_nNumColours > MAX_COLOURS)
			m_nNumColours = MAX_COLOURS;

		// Create the palette
		struct 
		{
			LOGPALETTE    LogPalette;
			PALETTEENTRY  PalEntry[MAX_COLOURS];
		}pal;

		LOGPALETTE* pLogPalette = (LOGPALETTE*) &pal;
		pLogPalette->palVersion    = 0x300;
		pLogPalette->palNumEntries = (WORD) m_nNumColours; 

		for (int i = 0; i < m_nNumColours; i++)
		{
			pLogPalette->palPalEntry[i].peRed   = GetRValue(crColours[i].crColour);
			pLogPalette->palPalEntry[i].peGreen = GetGValue(crColours[i].crColour);
			pLogPalette->palPalEntry[i].peBlue  = GetBValue(crColours[i].crColour);
			pLogPalette->palPalEntry[i].peFlags = 0;
		}

		m_palColorPicker.CreatePalette (pLogPalette);
	}

	CMFCColorMenuButton* pColorButton = new 
		CMFCColorMenuButton (ID_CHAR_COLOR, _T("字体颜色..."), &m_palColorPicker);

	pColorButton->EnableAutomaticButton (_T("自动"), RGB (0, 0, 0));
	pColorButton->EnableOtherButton (_T("更多..."));
	pColorButton->EnableDocumentColors (_T("Document's Colors"));
	pColorButton->SetColumnsNumber (8);
//	pColorButton->EnableTearOff (ID_COLOR_TEAROFF, 5, 2);

	// Initialize color names:
	for (int i = 0; i < m_nNumColours; i++)
	{
		CMFCColorMenuButton::SetColorName (crColours[i].crColour, crColours[i].szName);
	}

	return pColorButton;
}

