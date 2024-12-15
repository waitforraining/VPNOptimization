#include "Point2D.h"

#include <math.h>
#include <assert.h>

namespace ViewPointNetwork
{
	Point2D::Point2D():
		m_x(0.0), m_y(0.0)
	{
	}

	Point2D::Point2D(double x, double y)
	{
		m_x = x;
		m_y = y;
	}

	Point2D::Point2D(const Point2D& p)
	{
		m_x = p.m_x;
		m_y = p.m_y;
	}

	double Point2D::distance(const Point2D& p) const
	{
		double dx = m_x - p.m_x;
		double dy = m_y - p.m_y;
		return sqrt(dx * dx + dy * dy);
	}

	bool Point2D::operator<(const Point2D& other) const {
		if (m_x != other.m_x) {
			return m_x < other.m_x;
		}
		else if (m_y != other.m_y) {
			return m_y < other.m_y;
		}
		else {
			return false;
		}
	}

	Point2D& Point2D::operator = (const Point2D& p)
	{
		if (this == &p) return *this;
		m_x = p.m_x;
		m_y = p.m_y;

		return *this;
	}

	bool Point2D::operator>(const Point2D& other) const {
		if (m_x != other.m_x) {
			return m_x > other.m_x;
		}
		else if (m_y != other.m_y) {
			return m_y > other.m_y;
		}
		else {
			return false;
		}
	}

	const Point2D& Point2D::operator += (const Point2D & p)
	{
		m_x += p.m_x;
		m_y += p.m_y;

		return *this;
	}

	const Point2D& Point2D::operator -= (const Point2D & p)
	{
		m_x -= p.m_x;
		m_y -= p.m_y;

		return *this;
	}

	const Point2D& Point2D::operator *= (double s)
	{
		m_x *= s;
		m_y *= s;

		return *this;
	}

	const Point2D& Point2D::operator /= (double s)
	{
		assert(s != 0.0);
		m_x /= s; 
		m_y /= s;

		return *this;
	}

	Point2D operator +(const Point2D & p0, const Point2D & p1)
	{
		return Point2D(p0.m_x + p1.m_x, p0.m_y + p1.m_y);
	}

	Point2D operator -(const Point2D & p0, const Point2D & p1)
	{
		return Point2D(p0.m_x - p1.m_x, p0.m_y - p1.m_y);
	}

	Point2D operator *(const Point2D & p, double s)
	{
		return Point2D(p.m_x * s, p.m_y * s);
	}

	Point2D operator *(double s, const Point2D & p)
	{
		return Point2D(p.m_x * s, p.m_y * s);
	}

	Point2D operator /(const Point2D & p, double s)
	{
		assert(s != 0.0);
		return Point2D(p.m_x / s, p.m_y / s);
	}

	bool operator == (const Point2D & p0, const Point2D & p1)
	{
		return (p0.m_x == p1.m_x && p0.m_y == p1.m_y);
		//return (fabs(p0.m_x - p1.m_x) < 1e-5 && fabs(p0.m_y - p1.m_y) < 1e-5);
	}
}