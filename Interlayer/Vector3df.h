//############################################################################
//
// Vector3d.h: interface for the CVector3DFf class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CVector3DF_FLOAT_
#define _CVector3DF_FLOAT_

class CVertex3D;
class CMatrix33;
class COneAxis;
class CPlane;

class CVector3DF
{
	friend std::istream& operator>>(std::istream&, CVector3DF&);
	friend std::ostream& operator<<(std::ostream&, CVector3DF&);
public:
	CVector3DF();
	CVector3DF(float x, float y, float z);
	CVector3DF(const CVertex3D&);
	CVector3DF(const CVector3DF&, const CVector3DF&);
	CVector3DF(const CVertex3D&, const CVertex3D&);
	CVector3DF(CVertex3D* pVertex1, CVertex3D* pVertex2);
	~CVector3DF();

	void SetX(float x)
	{
		this->x = x;
	} //Set The X Component Value
	void SetY(float y)
	{
		this->y = y;
	} //Set The Y Component Value
	void SetZ(float z)
	{
		this->z = z;
	} //Set The Z Component Value
	void SetParam(float x, float y, float z)
	{
		this->x = x; this->y = y;this->z = z;
	} //Set The X Y and Z Values
	void SetParam(const CVector3DF& vec)
	{
		x = vec.GetX(); y = vec.GetY();z = vec.GetZ();
	} //Set The X Y and Z Values
	float GetX() const
	{
		return x;
	}// Returns The X Component
	float GetY() const
	{
		return y;
	}// Returns The Y Component
	float GetZ() const
	{
		return z;
	}// Returns The Z Component

	// cross product
	const CVector3DF CrossProduct(const CVector3DF &vec) const
	{
		return CVector3DF(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
	}

	// dot product
	const float DotProduct(const CVector3DF &vec) const
	{
		return x*vec.x + y*vec.y + z*vec.z;
	}	

	float Dot(const CVector3DF& V1) const;//Calculate Dot Product 点积
	float DotCross(const CVector3DF& V1, const CVector3DF& V2);//Calculate Scalar Triple Product 纯量矢量三重积
	void Cross(const CVector3DF& V1);//Calculate Cross Product Of' this' And Other 叉积
	CVector3DF Crossed(const CVector3DF& V1) const;//Return The Cross Product Vector 矢积
	void CrossCross(const CVector3DF& V1, const CVector3DF& V2);//Calculate The Vector Triple Product 矢量三重积
	CVector3DF CrossCrossed(const CVector3DF& V1, const CVector3DF& V2);//Return Vector Triple Product 矢量三重积
	void Reverse();//Reverse The Direction Of 'this' 相反矢量
	CVector3DF Reversed();//Return The Reversed Vector Of 'this'相反矢量

	CVector3DF operator +(const CVector3DF& V1);//Return Addition Of 'this' And Other 相加	
	CVector3DF operator -(const CVector3DF& V1);//Return Subtraction Of 'this' And Other
	CVector3DF operator *(const float& scalar);//Scalar Multiple With Other Vector
	CVector3DF operator *(const CMatrix33& M);//Multiply With Matrix
	CVector3DF operator /(const float& scalar);//Scalar Division

	CVector3DF operator ^(const CVector3DF& V1);//Return The Cross Product

	CVector3DF operator=(const CVector3DF& V1); // Setting

	void operator +=(const CVector3DF& V1);//Add Other To 'this'
	void operator +=(CVector3DF* pVector);

	void operator -=(const CVector3DF& V1);//Subtract Other From 'this'

	void operator *=(const float& scalar);//Multiply 'this' With Other
	void operator *=(const CMatrix33& M);//Multiply 'this With Matrix

	void operator /=(const float& scalar);//Divide 'this' By Scalar

	void operator ^=(const CVector3DF& V1);//Calculate Cross Product Of' this' And Other
	bool operator ==(const CVector3DF& V) const;//Check Equality
	bool operator !=(const CVector3DF& V) const;//Check Equality

	float Magnitude() const;// Calculate 'this' Vector's Magnitude
	float SqrMagnitude() const;// Calculate 'this' Vector's Square Magnitude
	float CrossMagnitude(const CVector3DF& V);// Calculate 'this' Vector's Cross Magnitude
	float CrossSqrMagnitude(const CVector3DF& V1);// Calculate 'this' Vector's Cross Square Magnitude
	float Modulus();// Return Modulus
	CVector3DF Unit();//Return Normalized Vector Of 'this'
	CVertex3D Point();//Return Component Triplet As Point
	float Angle(const CVector3DF&) const;//Angle Bet'n 'this' And Other
	float Angle(const CVector3DF&, const CVector3DF&) const;//Angle Bet'n 'this' And Other WRT Direction
	bool IsNull() const;//Check For Null Vector
	bool IsParallel(const CVector3DF&) const;//Check If Parallel To Other
	bool IsOpposite(const CVector3DF&) const;//Check If Opposite Direction To Other
	bool IsNormal(const CVector3DF&) const;//Check If Normal To Other	
	operator CVertex3D();//Convertion Cast To Point;	
	void Normalize();//Normalize 'this' Vector
	virtual void Translate(const COneAxis&, const float&);
	virtual void Translate(float dx, float dy, float dz);
	virtual void Translate(const CVector3DF&);
	virtual void Translate(const CVertex3D&, const CVertex3D&);
	virtual void Rotate(const COneAxis&, float);
	virtual void Scale(const CVertex3D&, float);
	virtual void Mirror(const CVertex3D&);
	virtual void Mirror(const COneAxis&);
	virtual void Mirror(const CPlane&);
	static CVector3DF Origin();

	//-----------------------------------------------------------------------
	void Inner(CVector3DF& v);
	static CVector3DF Inner(CVector3DF* u, CVector3DF* v); // Stupid c++
	static CVector3DF Inner(const CVector3DF& u, const CVector3DF& v); // Stupid c++

	// Misc
	void NormalizeL2(void);
	void NormalizeL2(float val);
	float GetNormL2(void) const;
	float GetNormL2Square(void) const;
	int Collinear(CVector3DF* pVector);
	int Collinear(CVector3DF& v);

	void Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			ar << x;
			ar << y;
			ar << z;
		}
		else
		{
			ar >> x;
			ar >> y;
			ar >> z;
		}
	}
//private:
	float x;
	float y;
	float z;
};

#endif
