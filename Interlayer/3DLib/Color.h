#pragma once

class CColor	// ÑÕÉ«Àà
{
private :

	// Color
	unsigned char m_Red;
	unsigned char m_Green;
	unsigned char m_Blue;
	unsigned char m_Alpha;

public :

	// Constructor
	CColor()
	{
		m_Red = m_Green = m_Blue = m_Alpha = 255;
	}

	CColor(CColor& color)
	{
		m_Red = color.r();
		m_Green = color.g();
		m_Blue = color.b();
		m_Alpha = color.a();
	}

	~CColor()
	{
	}


	// Constructor
	CColor(unsigned char grey)
	{
		Set(grey);
	}
	CColor(unsigned char red, unsigned char green, unsigned char blue)
	{
		Set(red, green, blue);
	}
	CColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
	{
		Set(red, green, blue, alpha);
	}


	// Data access
	unsigned char GetR(void) const
	{
		return m_Red;
	} 
	unsigned char GetG(void) const
	{
		return m_Green;
	}  
	unsigned char GetB(void) const
	{
		return m_Blue;
	}  
	unsigned char GetA(void) const
	{
		return m_Alpha;
	}  

	unsigned char r(void) const
	{
		return m_Red;
	} 
	unsigned char g(void) const
	{
		return m_Green;
	}  
	unsigned char b(void) const
	{
		return m_Blue;
	}  
	unsigned char a(void) const
	{
		return m_Alpha;
	} 

	// Data setting
	void r(unsigned char red)
	{
		m_Red = red;
	} 
	void g(unsigned char green)
	{
		m_Green = green;
	}  
	void b(unsigned char blue)
	{
		m_Blue = blue;
	}  
	void a(unsigned char alpha)
	{
		m_Alpha = alpha;
	}  

	// Data setting
	void Set(CColor& c);
	void Set(unsigned char grey); 
	void Set(unsigned char red, unsigned char green, unsigned char blue); 
	void Set(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); 

	int operator==(CColor& color)
	{
		return (color.r() == m_Red) && (color.g() == m_Green) && (color.b() == m_Blue);
	}
	int operator==(CColor* pColor)
	{
		return (pColor->r() == m_Red) && (pColor->g() == m_Green) && (pColor->b() == m_Blue);
	}

	friend CArchive& AFXAPI operator<<(CArchive& ar, const CColor& clr)
	{
		ar << clr.m_Red;
		ar << clr.m_Green;
		ar << clr.m_Blue;
		ar << clr.m_Alpha;

		return ar;
	}

	friend CArchive& AFXAPI operator>>(CArchive& ar, CColor& clr)
	{
		ar >> clr.m_Red;
		ar >> clr.m_Green;
		ar >> clr.m_Blue;
		ar >> clr.m_Alpha;
		return ar;
	}
};
