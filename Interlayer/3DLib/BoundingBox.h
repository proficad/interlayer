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

class CBoundingBox  // ���޺���

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
	CVertex3D Center() const;					// ���ĵ�
	double BoundingRadius() const;				// ���޵İ뾶
	bool Contains(const CVertex3D& P) const;		// �Ƿ�����˵�
	void AddBox(const CBoundingBox&);			// �ϲ�������
	void Serialize(CArchive& ar);
private:
	double itsXMax;		// X���ֵ
	double itsXMin;		// X��Сֵ
	double itsYMax;		// Y���ֵ
	double itsYMin;		// Y��Сֵ
	double itsZMax;		// Z���ֵ
	double itsZMin;		// Z��Сֵ
};

#endif // !defined(AFX_BOUNDINGBOX_H__098639F6_5719_4B89_8478_0A0F51851F73__INCLUDED_)
