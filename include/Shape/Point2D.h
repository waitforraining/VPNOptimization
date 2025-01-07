#ifndef _VPN_POINT_2D_h_
#define _VPN_POINT_2D_h_

#include "Shape/Point2D.h"

namespace ViewPointNetwork
{
	class Point2D
	{
	public:
		Point2D();
		Point2D(double x, double y);
		Point2D(const Point2D& p);
		~Point2D() {};

	public:
		//void X(double inX) { x = inX; }
		double& X() { return m_x; }
		double X() const { return m_x; }
		//void Y(double inY) { y = inY; }
		double& Y() { return m_y; }
		double Y() const { return m_y; }
		Point2D getThisPoint() const { return *this; }
		double distance(const Point2D& p) const;

	public:
		Point2D& operator = (const Point2D& p);

		bool operator<(const Point2D& other) const;
		bool operator>(const Point2D& other) const;

		const Point2D& operator += (const Point2D& p);
		const Point2D& operator -= (const Point2D& p);
		const Point2D& operator *= (double s);
		const Point2D& operator /= (double s);

		friend Point2D operator +(const Point2D& p0, const Point2D& p1);
		friend Point2D operator -(const Point2D& p0, const Point2D& p1);
		friend Point2D operator *(const Point2D& p0, double s);
		friend Point2D operator *(double s, const Point2D& p0);
		friend Point2D operator /(const Point2D& p0, double s);

		friend bool operator == (const Point2D& p0, const Point2D& p1);
		friend bool operator != (const Point2D& p0, const Point2D& p1) { return !(p0 == p1); }

	protected:
		double m_x, m_y;
	};
	//the angle of a_p_b
}

#endif 
