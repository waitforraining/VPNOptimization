#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#include "Shape/Vector2D.h"
#include "Shape/Point2D.h"


namespace ViewPointNetwork
{
	//from p0 to p1
	Vector2D::Vector2D(const Point2D& p0, const Point2D& p1)
	{
		x[0] = p1.X() - p0.X();
		x[1] = p1.Y() - p0.Y();
	}

	Vector2D& Vector2D::operator=(const Vector2D& v)
	{
		if (this == &v) return *this;
		x[0] = v.x[0];
		x[1] = v.x[1];
		return *this;
	}

	double& Vector2D::operator[] (int i)
	{
		assert(i >= 0 && i < 2); return x[i];
	}

	double Vector2D::operator[] (int i) const
	{
		assert(i >= 0 && i < 2);
		return x[i];
	}

	double Vector2D::SqLength() const
	{
		return x[0] * x[0] + x[1] * x[1];
	}

	double Vector2D::Length() const
	{
		return sqrt(SqLength());
	}

	Vector2D Vector2D::PartialDeriv(int param) const
	{
		Vector2D deriv;
		deriv[param] = 1.0;
		return deriv;
	}

	double Vector2D::DotProduct(const Vector2D& v) const
	{
		return x[0] * v.X() + x[1] * v.Y();
	}

	Vector2D Vector2D::Align(const Vector2D& vec) const
	{
		double len = SqLength();
		if (len == 0.0) return Vector2D();
		return (*this) * (DotProduct(vec) / SqLength());
	}

	int Vector2D::AreaSign(const Vector2D& b, const Vector2D& c) const
	{
		double area2;

		area2 = (b.X() - X()) * (double)(c.Y() - Y()) -
			(c.X() - X()) * (double)(b.Y() - Y());

		if (area2 > 0) return 1;
		else if (area2 < 0) return -1;
		else { return 0; };
	}

	// Obtain the polar Angle (radian) of the vector (counterclockwise from the positive X-axis)
	double Vector2D::getTheta() const
	{
		if (x[0] == 0.0) {
			if (x[1] > 0.0)
				return M_PI / 2;
			else
				return M_PI * 3 / 2;
		}

		double k = x[1] / x[0];
		double theta = atan(k);

		if (x[0] < 0.0)
			theta += M_PI;
		else if (x[0] > 0.0 && x[1] < 0.0)
			theta += 2 * M_PI;

		return theta;
	}

	const Vector2D& Vector2D::operator += (const Vector2D& p)
	{
		x[0] += p.X();
		x[1] += p.Y();
		return *this;
	}

	const Vector2D& Vector2D::operator -= (const Vector2D& p)
	{
		x[0] -= p.X();
		x[1] -= p.Y();
		return *this;
	}

	const Vector2D& Vector2D::operator *= (double d)
	{
		x[0] *= d;
		x[1] *= d;
		return *this;
	}

	const Vector2D& Vector2D::operator /= (double d)
	{
		assert(d != 0.0);
		x[0] /= d;
		x[1] /= d;
		return *this;
	}

	Vector2D operator +(const Vector2D& p1, const Vector2D& p2)
	{
		Vector2D p;
		p.X() = p1.X() + p2.X();
		p.Y() = p1.Y() + p2.Y();
		return p;
	}

	Vector2D operator -(const Vector2D& p1, const Vector2D& p2)
	{
		Vector2D p;
		p.X() = p1.X() - p2.X();
		p.Y() = p1.Y() - p2.Y();
		return p;
	}

	Vector2D operator *(const Vector2D& p1, double d)
	{
		Vector2D p;
		p.X() = p1.X() * d;
		p.Y() = p1.Y() * d;
		return p;
	}

	Vector2D operator *(double d, const Vector2D& v1)
	{
		return v1 * d;
	}

	Vector2D operator /(const Vector2D& p1, double d)
	{
		assert(d != 0.0);
		Vector2D p;
		p.X() = p1.X() / d;
		p.Y() = p1.Y() / d;
		return p;
	}

	Point2D operator +(const Point2D& p, const Vector2D& v)
	{
		return Point2D(p.X() + v.X(), p.Y() + v.Y());
	}

	Point2D operator +(const Vector2D& v, const Point2D& p)
	{
		return Point2D(p.X() + v.X(), p.Y() + v.Y());
	}

	bool operator ==(const Vector2D& p1, const Vector2D& p2)
	{
		return p1.X() == p2.X() && p1.Y() == p2.Y();
	}

	std::istream& operator >> (std::istream& is, Vector2D& v)
	{
		return is >> v.X() >> v.Y();
	}

	std::ostream& operator << (std::ostream& os, const Vector2D& v)
	{
		return os << v.X() << ' ' << v.Y();
	}

	// angle is from p1 to p2, positive anticlockwise
	// result is between -pi and pi
	double Angle2D(const Vector2D& v0, const Vector2D& v1)
	{
		double lLen = v0.Length();
		double rLen = v1.Length();
		if (lLen < 0.0000001 || rLen < 0.0000001)
			return 0.0;

		double value = v0.DotProduct(v1) / (rLen * lLen);
		if (value > 1.0) 
			value = 1.0;
		else if (value < -1.0) 
			value = -1.0;
		
		double angle = acos(value);

		return angle;
	}

	//the angle of a_p_b
	double Angle2D(const Point2D& a, const Point2D& p, const Point2D& b)
	{
		Vector2D v0(p, a);
		Vector2D v1(p, b);
		double angle = Angle2D(v0, v1);
		
		return angle;
	}

	Vector2D Vector2D::Normalize()const
	{
		if (!SqLength())
			return(Vector2D(0.0, 0.0));

		Vector2D normal = *this;
		return(normal / normal.Length());
	}

	// Normal vector
	Vector2D Vector2D::Normal() const
	{
		Vector2D normal;

		if (!SqLength())
			return(Vector2D(0.0, 0.0));

		normal = Vector2D(Y(), -X());
		return(normal / normal.Length());
	}
}