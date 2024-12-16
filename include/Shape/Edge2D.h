#ifndef _VPN_EDGE_2D_H_
#define _VPN_EDGE_2D_H_ 

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "Line2D.h"
#include "Point2D.h"
#include "Vector2D.h"

namespace ViewPointNetwork
{
	class Edge2D
	{
	public:
		Edge2D();
		Edge2D(const Edge2D& e);
		Edge2D(const Point2D& p0, const Point2D& p1);
		~Edge2D() {};

		Edge2D& operator=(const Edge2D& v);
		bool operator==(const Edge2D&) const;
		Edge2D operator+(const Edge2D&);
		bool operator<(const Edge2D& other) const;
		bool operator>(const Edge2D& other) const;
		friend std::ostream& operator<<(std::ostream& os, const Edge2D& e);

	public:
		void setBegPoint(const Point2D& p);
		Point2D getBegPoint() const { return m_begPnt; }
		void setEndPoint(const Point2D& p);
		Point2D getEndPoint() const { return m_endPnt; }
		void setPoints(const Point2D& p0, const Point2D& p1);

		double Length() const;
		double distance(const Point2D& p) const;
		const Line2D& getLine2D() const{ return m_line; }
		bool isVertical(double err = M_PI * 5.0 / 180.0) const;
		bool isHorizontal(double err = M_PI * 5.0 / 180.0) const;


		std::vector<Point2D> densify(double step, bool withEndPnts = true) const;
		std::vector<Point2D> densify(int nStep, bool withEndPnts = true) const;
		Point2D getCrossPoint(const Edge2D& e) const;
		
		bool isOverlap(const Edge2D& e, double distErr = 0.0001) const;
		Edge2D Overlap(const Edge2D& e, double distErr = 0.0001) const;
		Edge2D Extend(const Edge2D& e, double distErr) const;

		bool OnLine(const Point2D& p, double err = 0.0001) const;
		bool OnLine(const Edge2D& p, double err = 0.0001) const;
		int isFrontOf(const Point2D& p) const;

		bool isCover(const Point2D& p) const;
		bool isCover(const Edge2D& e) const;

		std::map<Edge2D, bool> getOverlapEdgeSegs(const Edge2D& e, double err = 0.0001) const;

		double ScalarBegin() const;
		double ScalarEnd() const;
		double Scalar(const Point2D& p) const;

	private:
		Point2D m_begPnt, m_endPnt;
		Line2D m_line;

	};

	Point2D ComputeMinimumPoint(const std::vector<Edge2D>& vecEdges);
	void MoveMinPoint2Origin(std::vector<Edge2D>& vecEdges);
	std::vector<Edge2D> Densify(const std::vector<Edge2D>& vecEdges, double step = 0.2);
}

#endif
