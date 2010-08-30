#ifndef GRID_LIBRARY
#define GRID_LIBRARY

#ifdef _GRID_EXPORT
#define _GRID_API __declspec(dllexport)
#else
#define _GRID_API __declspec(dllimport)
#endif

#ifndef _GRID_NOAUTOLIB
#ifdef _DEBUG
//#pragma comment(lib, "..\\lib\\GRID.lib")
#else
//#pragma comment(lib, "..\\lib\\GRID.lib")
#endif
#endif

#include <afxtempl.h>
#include "GridCtrl_Src/amsEdit.h"
#include "GridCtrl_Src/CellRange.h"
#include "GridCtrl_Src/GridCell.h"
#include "GridCtrl_Src/GridCellBase.h"
#include "GridCtrl_Src/GridCtrl.h"
#include "GridCtrl_Src/GridDropTarget.h"
#include "GridCtrl_Src/InPlaceEdit.h"
#include "GridCtrl_Src/MemDC.h"
#include "GridCtrl_Src/TitleTip.h"

#include "NewCellTypes/CheckComboBox.h"
#include "NewCellTypes/GridCellCheck.h"
#include "NewCellTypes/GridCellCheckCombo.h"
#include "NewCellTypes/GridCellCombo.h"
#include "NewCellTypes/gridcellcurrency.h"
#include "NewCellTypes/GridCellDateTime.h"
#include "NewCellTypes/GridCellDouble.h"
#include "NewCellTypes/GridCellMask.h"
#include "NewCellTypes/GridCellNumeric.h"
#include "NewCellTypes/GridURLCell.h"
#include "NewCellTypes/GridCellEditFile.h"

void _GRID_API AFXAPI DDX_GridControl(CDataExchange* pDX, int nIDC, CGridCtrl& rControl);

#endif //GRID_LIBRARY