
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��
#include <afxrich.h>            // MFC Rich Edit ��

#include <afx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <afxdlgs.h>

#include "GridCtrl/GridLib.h"

#include "AFC/ResizableLib/ResizableDialog.h"
#include "AFC/ResizableLib/ResizableFormView.h"
#include "afc/src/CSVFile.h"
#include "afc/src/SplitPath.h"
#include "afc/src/Ini.h"
#include "afc/src/Spline3.h"
#include "afc/src/Spline.h"
#include "afc/src/ParseToken.h"
#include "afc/src/DashLine.h"
#include "afc/src/Point2D.h"
#include "afc/src/Point3D.h"
#include "afc/src/RectEx.h"
#include "afc/Command/Command.h"
#include "afc/Command/CommandHistory.h"
#include "afc/Command/CustomCommandHistory.h"
#include "afc/Command/RefObject.h"
#include "afc/Convert.h"
#include "cximage599c/CxImage/ximage.h"

#include "AdoX/ado2.h"
#include <afxhtml.h>
#include <afxrich.h>
#include <afxole.h>
#include <afxdhtml.h>

//#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//#endif


