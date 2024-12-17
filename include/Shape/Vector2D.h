#ifndef _VPN_VECTOR_2D_H_
#define _VPN_VECTOR_2D_H_ 

#include <iostream>

#include "Point2D.h"

namespace ViewPointNetwork
{
	class Vector2D
	{
	protected:
		double x[2];

	public:
		Vector2D() { x[0] = x[1] = 0; }
		Vector2D(double xv, double yv) { x[0] = xv; x[1] = yv; }
		Vector2D(const double *v) { x[0] = v[0]; x[1] = v[1]; }
		Vector2D(const Vector2D &v) { x[0] = v.x[0]; x[1] = v.x[1]; }
		Vector2D(const Point2D& p0, const Point2D& p1);
		~Vector2D() {}

		Vector2D& operator=(const Vector2D &v);
		//double X(int i) const { return x[i]; }
		double X() const { return x[0]; }
		double Y() const { return x[1]; }
		double & operator[] (int i);
		double operator[] (int i) const;
		double &X() { return x[0]; }
		double &Y() { return x[1]; }

		// Squared length.
		double SqLength() const;
		double Length() const;
		Vector2D PartialDeriv(int param) const;
		const class Vector2D &vect() const { return *this; }
		Vector2D &vect() { return *this; }
		double DotProduct(const Vector2D &v) const;
		Vector2D Align(const Vector2D &v) const;
		int AreaSign(const Vector2D &p1, const Vector2D &p2) const;
		double getTheta() const;
		Vector2D Normalize() const;
		Vector2D Normal() const;

		const Vector2D &operator += (const Vector2D &v);
		const Vector2D &operator -= (const Vector2D &v);
		const Vector2D &operator *= (double d);
		const Vector2D &operator /= (double d);

		friend Vector2D operator +(const Vector2D &v1, const Vector2D &v2);
		friend Vector2D operator -(const Vector2D &v1, const Vector2D &v2);
		friend Vector2D operator *(const Vector2D &v1, double d);
		friend Vector2D operator *(double d, const Vector2D &v1);
		friend Vector2D operator /(const Vector2D &v1, double d);
		friend bool operator == (const Vector2D &v1, const Vector2D &v2);
		friend bool operator != (const Vector2D &v1, const Vector2D &v2) { return !(v1 == v2); }

		friend Point2D operator +(const Point2D& p, const Vector2D& v);
		friend Point2D operator +(const Vector2D& v, const Point2D& p);

		friend std::istream& operator >> (std::istream& is, Vector2D& v);
		friend std::ostream& operator << (std::ostream& os, const Vector2D& v);
	};
	//the angle from vector v1 to vector v2
	double Angle2D(const Vector2D& v1, const Vector2D& v2);
	double Angle2D(const Point2D& a, const Point2D& p, const Point2D& b);

}

#endif
