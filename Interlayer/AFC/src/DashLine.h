// DashLine.h : main header file for the CDashLine
////////////////////////////////////////////////////////////////////////////
// CDashLine

#ifndef __DASHLINE__H
#define  __DASHLINE__H


class AFX_EXT_CLASS CDashLine
{
public:
	CDashLine(CDC* dc, unsigned* pattern, unsigned count);
	CDashLine();
	~CDashLine();
	void SetPattern(unsigned* pattern, unsigned count);
	enum {	DL_SOLID, DL_DASH, DL_DOT, DL_DASHDOT, DL_DASHDOTDOT, DL_DASHDOTDOTDOT, 
			DL_DASH_GAP, DL_DOT_GAP, DL_DASHDOT_GAP, DL_DASHDOTDOT_GAP, DL_DASHDOTDOTDOT_GAP}; 
// Returns count of elements (dash/dot and gaps)
// You must be careful to pass in enough memory for pattern
// It is probably safest to always have an array of [8]
	static unsigned GetPattern(unsigned* pattern, BOOL round, unsigned pensize, unsigned style);

protected:
	CDC *m_pDC;
	unsigned int	m_CurPat;
	unsigned int	m_Count;
	unsigned int	m_CurStretch;
	unsigned int*	m_Pattern;
	CPoint			m_CurPos;

	void Reset();
	void Bresenham(LONG x, LONG y);

public:
	void SetDC(CDC *dc);
	void Rectangle( LPCRECT lpRect );
	void Rectangle( int x1, int y1, int x2, int y2 );
	void Polygon( LPPOINT lpPoints, int nCount );
	void Polyline( LPPOINT lpPoints, int nCount );
	void MoveTo(const POINT& p) {MoveTo(p.x, p.y);}
	void MoveTo(int x, int y);
	void LineTo(const POINT& p) {LineTo(p.x, p.y);}
	void LineTo(int x, int y);
};

#endif // !defined( __DASHLINE__H)