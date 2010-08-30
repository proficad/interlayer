//############################################################################
//
// BoundingBox.h: interface for the CBoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOUNDINGBOX_H__098639F6_5719_4B89_8478_0A0F51851F73__INCLUDED_)
#define AFX_BOUNDINGBOX_H__098639F6_5719_4B89_8478_0A0F51851F73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CVertex3D;

class CBoundingBox  // 界限盒子

{
public:
	CBoundingBox();
	CBoundingBox(const double&, const double&, const double&, const double&, const double&, const double&);
	virtual ~CBoundingBox();
	void SetLimits(const double&, const double&, const double&, const double&, const double&, const double&);
	double XMax() const
	{
		return itsXMax;
	}
	double XMin() const
	{
		return itsXMin;
	}
	double YMax() const
	{
		return itsYMax;
	}
	double YMin() const
	{
		return itsYMin;
	}
	double ZMax() const
	{
		return itsZMax;
	}
	double ZMin() const
	{
		return itsZMin;
	}
	CVertex3D Center() const;					// 中心点
	double BoundingRadius() const;				// 界限的半径
	bool Contains(const CVertex3D& P) const;		// 是否包含此点
	void AddBox(const CBoundingBox&);			// 合并最大界限
	void Serialize(CArchive& ar);
private:
	double itsXMax;		// X最大值
	double itsXMin;		// X最小值
	double itsYMax;		// Y最大值
	double itsYMin;		// Y最小值
	double itsZMax;		// Z最大值
	double itsZMin;		// Z最小值
};

#endif // !defined(AFX_BOUNDINGBOX_H__098639F6_5719_4B89_8478_0A0F51851F73__INCLUDED_)
