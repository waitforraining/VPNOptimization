#ifndef _VPN_LINE_2D_h_
#define _VPN_LINE_2D_h_

#include "Shape/Point2D.h"
#include "Shape/Vector2D.h"

#include "preCompiled.h"

namespace ViewPointNetwork
{
	class VPN_API Line2D
	{
	public:
		Line2D() ;
		Line2D(double sine, double cosine, double distance);
		Line2D(const Point2D& p0, const Point2D& p1);
		Line2D(const Line2D& line);
		~Line2D() {};
		Line2D& operator=(const Line2D&);

		friend bool operator ==(const Line2D& line0, const Line2D& line1);
		friend bool operator !=(const Line2D& line0, const Line2D& line1) { return !(line0 == line1); }

	public:
		
		double Getcosphi() const { return m_cosphi; }
		double Getsinphi() const { return m_sinphi; }
		double GetDisto() const { return d; }

		double DistanceToOrigin() const { return d; }
		void SetDistanceToOrigin(double dist) { d = dist; }

		double Distance(const Point2D& p) const;
		double SignedDiatance(const Point2D& p) const;

		// Test if a point is on the line		
		bool Online(const Point2D& pt, double err = 0.0001) const;

		// Return the angle of the line with X-axis
		double AngleOx() const;
		bool IsHorizontal(double err = M_PI * 5.0 / 180.0) const;
		bool IsVertical(double err = M_PI * 5.0 / 180.0) const;

		void FindParallelLine(const Point2D& p, Line2D& line) const;
		Line2D PerpendicularLine(const Point2D& p) const;

		// Return the normal direction in radians according to the line definition
		double NormalAngle() const { return(atan2(m_sinphi, m_cosphi)); }

		// Return the angle between two lines in radians
		friend double Angle2Lines(const Line2D& line0, const Line2D& line1);

		// Determine the intersection point of two lines
		/** @param line0 First line
		@param line1 Second line
		@param p  Constructed intersection point
		@return 0 - Parallel lines, no intersection, 1 - Success
		*/
		friend bool Intersection2Lines(const Line2D& line0,
			const Line2D& line1, Point2D& p);

		// Fast intersection of two lines, no test for parallel lines!
		friend void Intersection2NonParallelLines(const Line2D& line0,
			const Line2D& line1, Point2D& p);

		// Construct the bisector of two lines
		friend Line2D Bisector(const Line2D& line1, const Line2D& line2);

		// Check if two lines are parallel
		/** @param line0      First line
		@param line1      Second line
		@param err_angle Allowed angle between the two lines
		*/
		friend bool ParallelLines(const Line2D& line0,
			const Line2D& line1, double err_angle);

		// Check if two lines are collinear         			     
		/** @param line0      First line
		@param line1      Second line
		@param err_angle Allowed angle between the two lines
		@param err_dist  Allowed difference between distance to origin
		*/
		friend bool CollinearLines(const Line2D& line0,
			const Line2D& line1, double err_angle, double err_dist);

		// Test on equality of all class parameters
		friend bool operator==(const Line2D& line0, const Line2D& line1);

		// Print all class parameters to stdout
		void Print() const;

		// Additional functions with footpoints and directions

		// Return the line's footpoint (point nearest to origin)
		Point2D FootPoint() const;

		// Return the line's direction (perpendicular to normal direction)
		Vector2D Direction() const;

		// Return the line's normal direction
		Vector2D Normal() const;

		// Project a point onto the line
		Point2D Project(const Point2D& p) const;

		// Return the scalar of a point's projection onto the line
		/** The scalar is defined as the signed distance of a point to the
		origin in the direction of the line.
		*/
		double Scalar(const Point2D& p) const;

		// Position of a point on the line derived from the scalar
		/** The scalar is defined as the signed distance of a point to the
		origin in the direction of the line. For scalar 0, the point equals
		the foot point of the line.
		*/
		Point2D Position(double scalar) const;

		std::pair<Point2D, Point2D> intersectCircle(double x0, double y0, double r) const;

		// Swap the normal direction
		void SwapNormal();
		friend bool Intersection2Lines(const Line2D& lin1, const Line2D& lin2, Point2D& pos);
	protected:
		double m_sinphi, m_cosphi;
		double d;
	};
}

#endif
