#define _USE_MATH_DEFINES 
#include <math.h>
#include <assert.h>

#include "Shape/Line2D.h"
#include "Shape/Vector2D.h"
#include "Shape/Point2D.h"

namespace ViewPointNetwork
{
	Line2D::Line2D()
	{
		m_sinphi = 0.0;
		m_cosphi = 0.0;
		d = 0.0;
	}

	// Construct from given values
	Line2D::Line2D(double sine, double cosine, double distance)
	{
		double length = sine * sine + cosine * cosine;
		if (length != 0.0) {
			length = sqrt(length);
			m_sinphi = sine / length;
			m_cosphi = cosine / length;
		}
		d = distance;
	}

	// Construct from two 2D points
	Line2D::Line2D(const Point2D& p0, const Point2D& p1)
	{
		double l = p0.distance(p1);
		assert(l != 0.0);

		m_sinphi = -(p1.X() - p0.X()) / l;
		m_cosphi = (p1.Y() - p0.Y()) / l;
		d = p0.X() * m_cosphi + p0.Y() * m_sinphi;
	}
	Line2D::Line2D(const Line2D& line)
	{
		m_sinphi = line.Getsinphi();
		m_cosphi = line.Getcosphi();
		d = line.GetDisto();
	}

	Line2D & Line2D::operator=(const Line2D & line)
	{
		if (this == &line) return *this;
		m_sinphi = line.m_sinphi;
		m_cosphi = line.m_cosphi;
		d = line.d;
		return *this;
	}

	bool operator ==(const Line2D& line0, const Line2D& line1)
	{
		if (line0.m_sinphi == line1.m_sinphi && line0.m_cosphi == line1.m_cosphi && line0.d == line1.d)
			return true;
		else if (line0.m_sinphi == -line1.m_sinphi && line0.m_cosphi == -line1.m_cosphi && line0.d == -line1.d)
			return true;
		else
			return false;
	}

	void Line2D::FindParallelLine(const Point2D & p, Line2D & line) const
	{
		line.m_sinphi = m_sinphi;
		line.m_cosphi = m_cosphi;
		line.d = p.X() * m_cosphi + p.Y() * m_sinphi;
	}

	Line2D Line2D::PerpendicularLine(const Point2D & p) const
	{
		Line2D line;
		line.m_sinphi = m_cosphi;
		line.m_cosphi = -m_sinphi;
		line.d = p.X() * line.m_cosphi + p.Y() * line.m_sinphi;
		return line;
	}

	void Line2D::Print() const
	{
		printf("m_sinphi = %15.6f m_cosphi = %15.6f  d =%15.6f\n", m_sinphi, m_cosphi, d);
	}

	double Line2D::Distance(const Point2D & p) const
	{
		return fabs(p.X() * m_cosphi + p.Y() * m_sinphi - d);
	}

	double Line2D::SignedDiatance(const Point2D & p) const
	{
		return p.X()* m_cosphi + p.Y() * m_sinphi - d;
	}

	bool Line2D::Online(const Point2D & p, double err) const
	{
		return (Distance(p) < err);
	}

	double Line2D::AngleOx() const
	{
		double angle = atan2(-m_cosphi, m_sinphi);
		if (angle < -M_PI / 2) angle += M_PI;
		if (angle > M_PI / 2)  angle -= M_PI;
		return angle;
	}

	//in radiance
	bool Line2D::IsHorizontal(double err) const
	{
		Vector2D hor(1.0, 0.0);
		Vector2D dir = Direction();
		if (dir.X() < 0.0) dir *= -1.0;
		double angle = Angle2D(hor, dir);
		return (fabs(angle) < err);
	}

	bool Line2D::IsVertical(double err) const
	{
		Vector2D ver(0.0, 1.0);
		Vector2D dir = Direction();
		if (dir.Y() < 0.0) dir *= -1.0;
		double angle = Angle2D(ver, dir);
		return (fabs(angle) < err);
	}

	//[0-pi/2)
	double Angle2Lines(const Line2D & line0, const Line2D & line1)
	{
		double dotProd = line0.Direction().DotProduct(line1.Direction());

		if (dotProd > 1.0)
			dotProd = 1.0;
		else if (dotProd < -1.0)
			dotProd = -1.0;

		double angle = acos(dotProd);
		if (angle > M_PI / 2.0) angle = M_PI - angle;// GV

		return angle;
	}

	bool ParallelLines(const Line2D & line0, const Line2D & line1, double err_angle)
	{
		return (Angle2Lines(line0, line1) < err_angle);
	}

	bool CollinearLines(const Line2D & line0, const Line2D & line1,
		double err_angle, double err_dist)
	{
		double a;// GV
		if (!ParallelLines(line0, line1, err_angle))
			return false;
		if (line0.Direction().DotProduct(line1.Direction()) > 0.0)// GV
			a = fabs(line0.d - line1.d);
		else// GV
			a = fabs(line0.d + line1.d);// GV
		return (a < err_dist);
	}

	bool Intersection2Lines(const Line2D & line0, const Line2D & line1, Point2D & p)
	{
		double err = 0.001 * M_PI / 180.0;
		if (Angle2Lines(line0, line1) < err)// parallel
			return false;

		Intersection2NonParallelLines(line0, line1, p);
		return true;
	}

	void Intersection2NonParallelLines(const Line2D & line0, const Line2D & line1, Point2D & p)
	{
		double det = line0.m_cosphi * line1.m_sinphi - line0.m_sinphi * line1.m_cosphi;
		p.X() = ((line0.d * line1.m_sinphi - line1.d * line0.m_sinphi) / det);
		p.Y() = ((line1.d * line0.m_cosphi - line0.d * line1.m_cosphi) / det);
	}

	Point2D Line2D::FootPoint() const
	{
		return(Point2D(d * m_cosphi, d * m_sinphi));
	}

	Vector2D Line2D::Direction() const
	{
		return(Vector2D(-m_sinphi, m_cosphi));
	}

	Vector2D Line2D::Normal() const
	{
		return(Vector2D(m_cosphi, m_sinphi));
	}

	Point2D Line2D::Project(const Point2D & p) const
	{
		Point2D projection;
		double distance;

		distance = SignedDiatance(p);
		projection.X() = (p.X() - distance * m_cosphi);
		projection.Y() = (p.Y() - distance * m_sinphi);

		return(projection);
	}

	double Line2D::Scalar(const Point2D & p) const
	{
		Point2D projection;

		projection = Project(p);
		if (fabs(Direction().X()) > fabs(Direction().Y()))
			return((projection.X() - FootPoint().X()) / Direction().X());
		else
			return((projection.Y() - FootPoint().Y()) / Direction().Y());
	}

	Point2D Line2D::Position(double scalar) const
	{
		Vector2D dir = Direction();
		Point2D p = FootPoint() + scalar * dir;

		return p;
		//return Point2D(FootPoint().getX() + scalar * Direction().getX(),
		//	FootPoint().getY() + scalar * Direction().getY());
	}

	void Line2D::SwapNormal()
	{
		m_sinphi = -m_sinphi;
		m_cosphi = -m_cosphi;
		d = -d;
	}

	// Construct the bisector of the smallest angle between two lines

	Line2D Bisector(const Line2D & line1, const Line2D & line2)
	{
		Point2D intersection;
		Line2D bisector, normal;
		double  dir1, dir2, dir_bisector;

		// Check for parallel lines
		if (line1.m_cosphi * line2.m_sinphi == line1.m_sinphi * line2.m_cosphi) {
			bisector = line1;
			bisector.d = (line1.d + line2.d) / 2.0;
		}
		else {
			dir1 = line1.AngleOx();
			dir2 = line2.AngleOx();

			if (dir1 < 0) dir1 += M_PI;
			if (dir2 < 0) dir2 += M_PI;
			if (dir1 > dir2) std::swap(dir1, dir2);
			//if (dir2-dir1 >= M_PI) dir2 -= M_PI;
			if (dir2 - dir1 >= M_PI / 2.0) dir2 -= M_PI;
			dir_bisector = (dir1 + dir2) / 2.0;
			normal = Line2D(sin(dir_bisector), cos(dir_bisector), 0.0);
			Intersection2NonParallelLines(line1, line2, intersection);
			bisector = normal.PerpendicularLine(intersection);
		}

		return bisector;
	}

	
	std::pair<Point2D, Point2D> Line2D::intersectCircle(double x0, double y0, double r) const
	{
		// Rewrite the line equation as d = x * cos_phi + y * sin_phi
		double dx = d - x0 * m_cosphi - y0 * m_sinphi;

		// Based on geometric derivation, calculate delta to determine whether there are intersection points
		double delta = r * r - dx * dx;
		if (delta < 0) {
			throw std::runtime_error("No intersection, the m_line does not intersect the circle.");
		}

		// Calculate the intersection coordinates
		double sqrt_delta = std::sqrt(delta);

		Point2D p1, p2;

		p1.X() = x0 + dx * m_cosphi - sqrt_delta * m_sinphi;
		p1.Y() = y0 + dx * m_sinphi + sqrt_delta * m_cosphi;

		p2.X() = x0 + dx * m_cosphi + sqrt_delta * m_sinphi;
		p2.Y() = y0 + dx * m_sinphi - sqrt_delta * m_cosphi;

		return std::make_pair(p1, p2);
	}
}