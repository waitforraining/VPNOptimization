#define _USE_MATH_DEFINES
#include <cassert>

#include "Shape/Edge2D.h"

using namespace std;

namespace ViewPointNetwork
{

	Edge2D::Edge2D()
	{
		m_line = Line2D(0.0, 0.0, 0.0);
	}

	Edge2D::Edge2D(const Edge2D& e)
	{
		m_line = Line2D(0.0, 0.0, 0.0);
		*this = e;
	}

	Edge2D::Edge2D(const Point2D& p0, const Point2D& p1)
	{
		m_begPnt = p0;
		m_endPnt = p1;
		//len = p1.distance(p2);
		if (m_begPnt.distance(m_endPnt) == 0.0)
			m_line = Line2D(0.0, 0.0, 0.0);
		else
			m_line = Line2D(p0, p1);
	}

	Edge2D& Edge2D::operator=(const Edge2D& v)
	{
		if (this == &v) return *this;

		m_begPnt = v.m_begPnt;
		m_endPnt = v.m_endPnt;
		m_line = v.m_line;

		return *this;
	}

	//判断两条边是否是同一条边
	bool Edge2D::operator==(const Edge2D& e) const
	{
		return (m_begPnt == e.m_begPnt && m_endPnt == e.m_endPnt);
	}

	//将两条重叠的边合并
	Edge2D Edge2D::operator+(const Edge2D & e)
	{
		//assert(false);
		//TODO
		//assert(isOverlap(e));

		Edge2D rst = Extend(e, 0.0001);
		return rst;
	}

	//将边的端点信息输出到流
	ostream& operator<<(ostream & out, const Edge2D & e)
	{
		out << "[(" << e.getBegPoint().X() << "," << e.getBegPoint().Y() << "), ("
			<< e.getEndPoint().X() << "," << e.getEndPoint().X() << ")]";

		return out;
	}
	//无具体意义，只用来生成map
	bool Edge2D::operator<(const Edge2D& other) const
	{
		if (getBegPoint() != other.getBegPoint())
			return getBegPoint() < other.getBegPoint();
		return getEndPoint() < other.getEndPoint();
	}

	bool Edge2D::operator>(const Edge2D& other) const
	{
		if (getBegPoint() != other.getBegPoint())
			return getBegPoint() > other.getBegPoint();
		return getEndPoint() > other.getEndPoint();
	}

	void Edge2D::setBegPoint(const Point2D& p)
	{
		m_begPnt = p;
		m_line = Line2D(m_begPnt, m_endPnt);
	}

	void Edge2D::setEndPoint(const Point2D& p)
	{
		m_endPnt = p;
		m_line = Line2D(m_begPnt, m_endPnt);
	}

	void Edge2D::setPoints(const Point2D& p0, const Point2D& p1)
	{
		m_begPnt = p0;
		m_endPnt = p1;
		m_line = Line2D(m_begPnt, m_endPnt);
	}

	//获取边的长度
	double Edge2D::Length() const
	{
		return m_begPnt.distance(m_endPnt);
	}

	//判断边是否竖直
	bool Edge2D::isVertical(double err) const
	{
		return m_line.IsVertical(err);
	}

	//判断边是否水平
	bool Edge2D::isHorizontal(double err) const
	{
		return m_line.IsHorizontal(err);
	}

	//the output densified points is with endpoints, 
	//and always make sure real step is equal or less than step
	std::vector<Point2D> Edge2D::densify(double step, bool withEndPnts) const
	{
		assert(step > 0.0);
		double len = Length();
		assert(len > 0.0);

		int nStep = std::ceil(len / step);
		return densify(nStep, withEndPnts);
	}

	std::vector<Point2D> Edge2D::densify(int nStep, bool withEndPnts) const
	{
		assert(nStep > 0);
		double len = Length();
		assert(len > 0.0);

		double realStep = len / nStep;
		double s0 = ScalarBegin();
		std::vector<Point2D> vecPnts;
		if (withEndPnts) vecPnts.push_back(getBegPoint());

		for (int i = 1; i < nStep; i++)
			vecPnts.push_back(m_line.Position(s0 + i * realStep));

		if (withEndPnts) vecPnts.push_back(getEndPoint());

		return vecPnts;
	}

	//得到两条边交点的坐标，注意会扩展为直线
	Point2D Edge2D::getCrossPoint(const Edge2D& e) const
	{
		Point2D cross;

		//be carefully, the two m_lines maybe parallel
		if (Intersection2Lines(m_line, e.m_line, cross))
			return cross;
		else
			return Point2D();
	}

	double Edge2D::distance(const Point2D & p) const
	{
		double sBeg = m_line.Scalar(m_begPnt);
		double sEnd = m_line.Scalar(m_endPnt);
		double sCur = m_line.Scalar(p);

		if (sBeg < sEnd) {
			if (sCur < sBeg)
				return p.distance(m_begPnt);
			else if (sCur > sEnd)
				return p.distance(m_endPnt);
			else
				return m_line.Distance(p);
		}
		else {
			if (sCur < sEnd)
				return p.distance(m_endPnt);
			else if (sCur > sBeg)
				return p.distance(m_begPnt);
			else
				return m_line.Distance(p);
		}
	}

	bool Edge2D::isOverlap(const Edge2D& e, double distErr) const
	{
		//if (m_line != e.m_line) return false;
		if (!m_line.Online(e.m_begPnt, distErr) || !m_line.Online(e.m_endPnt, distErr))
			return false;

		double s00 = m_line.Scalar(m_begPnt);
		double s01 = m_line.Scalar(m_endPnt);

		double s10 = e.m_line.Scalar(e.m_begPnt);
		double s11 = e.m_line.Scalar(e.m_endPnt);

		if (s00 > s01) std::swap(s00, s01);
		if (s10 > s11) std::swap(s10, s11);
		//return s10 >= s00 && s10 <= s01;
		return (s10 >= s00 && s10 < s01) || (s10 < s00 && s11 > s00);
	}

	//get the overlapped edge, if not overlapped, return an empty edge with both endpoint to be original point
	Edge2D Edge2D::Overlap(const Edge2D& e, double distErr) const
	{
		if (!m_line.Online(e.m_begPnt, distErr) || !m_line.Online(e.m_endPnt, distErr))
			return Edge2D();

		double s00 = m_line.Scalar(m_begPnt);
		double s01 = m_line.Scalar(m_endPnt);
		if (s00 > s01) std::swap(s00, s01);

		double s10 = m_line.Scalar(e.m_begPnt);
		double s11 = m_line.Scalar(e.m_endPnt);
		if (s10 > s11) std::swap(s10, s11);

		double sBeg = std::max(s00, s10);
		double sEnd = std::min(s01, s11);
		//if (sBeg > sEnd)
		if (sBeg >= sEnd)
			return Edge2D();

		Point2D p0 = m_line.Position(sBeg);
		Point2D p1 = m_line.Position(sEnd);
		return Edge2D(p0, p1);
	}

	//get the extended edge of the two edges
	Edge2D Edge2D::Extend(const Edge2D& e, double distErr) const
	{
		if (!m_line.Online(e.m_begPnt, distErr) || !m_line.Online(e.m_endPnt, distErr))
			return *this;
	
		if (Length() == 0.0)
			return e;
		if (e.Length() == 0.0)
			return *this;

		double s00 = m_line.Scalar(m_begPnt);
		double s01 = m_line.Scalar(m_endPnt);
		if (s00 > s01) std::swap(s00, s01);

		double s10 = m_line.Scalar(e.m_begPnt);
		double s11 = m_line.Scalar(e.m_endPnt);
		if (s10 > s11) std::swap(s10, s11);

		double sBeg = std::min(s00, s10);
		double sEnd = std::max(s01, s11);

		Point2D posBeg = m_line.Position(sBeg);
		Point2D posEnd = m_line.Position(sEnd);
		Edge2D rst(posBeg, posEnd);

		return rst;
	}

	bool Edge2D::OnLine(const Point2D& p, double err) const
	{ 
		return m_line.Online(p, err); 
	}

	bool Edge2D::OnLine(const Edge2D& p, double err) const
	{
		return m_line.Online(p.m_begPnt, err) && m_line.Online(p.m_endPnt, err);
	}

	//老的方式：
	//-1表示在延长线上
	//1表示在左侧
	//0表示在右侧
	int Edge2D::isFrontOf(const Point2D& p) const
	{
		double d = m_line.SignedDiatance(p);
		
		if (fabs(d) < 0.0001)
			return -1;
		else if (d > 0.0)
			return 1;
		else
			return 0;

		//return d > 0.0;//
	}

	bool Edge2D::isCover(const Point2D& p) const
	{
		double d = m_line.SignedDiatance(p);

		if (fabs(d) > 0.00001) return false;
		double sBeg = Scalar(m_begPnt);
		double sEnd = Scalar(m_endPnt);
		double sTem = Scalar(p);

		if (sBeg > sEnd) std::swap(sBeg, sEnd);
		return sTem >= sBeg && sTem <= sEnd;
	}

	bool Edge2D::isCover(const Edge2D& e) const
	{
		return isCover(e.m_begPnt) && isCover(e.m_endPnt);
	}
	//获取一条线段包含另一条线段的三部分：两条未重叠边，一条重叠边
	std::map<Edge2D, bool> Edge2D::getOverlapEdgeSegs(const Edge2D& e, double distErr) const {
		//assert(*this.isCover(e));
		std::map<Edge2D, bool> splitedEdges;

		double s00 = m_line.Scalar(m_begPnt);
		double s01 = m_line.Scalar(m_endPnt);
		if (s00 > s01) std::swap(s00, s01);

		double s10 = m_line.Scalar(e.m_begPnt);
		double s11 = m_line.Scalar(e.m_endPnt);
		if (s10 > s11) std::swap(s10, s11);

		double e1Beg = std::min(s00, s10);
		double e1End = std::max(s00, s10);

		double e3Beg = std::min(s01, s11);
		double e3End = std::max(s01, s11);

		Point2D e1P0 = m_line.Position(e1Beg);
		Point2D e1P1 = m_line.Position(e1End);
		Edge2D e1 = Edge2D(e1P0, e1P1);

		Point2D e3P0 = m_line.Position(e3Beg);
		Point2D e3P1 = m_line.Position(e3End);
		Edge2D e3 = Edge2D(e3P0, e3P1);

		Edge2D e2 = Edge2D(e1P1, e3P0);

		if (e1.Length() != 0.0) splitedEdges[e1] = true;
		if (e2.Length() != 0.0) splitedEdges[e2] = false;
		if (e3.Length() != 0.0) splitedEdges[e3] = true;

		return splitedEdges;
	}

	double Edge2D :: ScalarBegin() const
	{
		return Scalar(m_begPnt);
	}

	double Edge2D::ScalarEnd() const
	{
		return Scalar(m_endPnt);
	}

	double Edge2D::Scalar(const Point2D& p) const
	{
		return m_line.Scalar(p);
	}

	Point2D ComputeMinimumPoint(const std::vector<Edge2D>& vecEdges)
	{
		double minX = std::numeric_limits<double>::max();
		double minY = std::numeric_limits<double>::max();
		Point2D p0, p1;

		for  (const auto & v : vecEdges)
		{
			p0 = v.getBegPoint();
			if (minX > p0.X()) minX = p0.X();
			if (minY > p0.Y()) minY = p0.Y();

			p1 = v.getEndPoint();
			if (minX > p1.X()) minX = p1.X();
			if (minY > p1.Y()) minY = p1.Y();
		}

		return Point2D(minX, minY);
	}

	void MoveMinPoint2Origin(std::vector<Edge2D>& vecEdges)
	{
		Point2D minPnt = ComputeMinimumPoint(vecEdges);
		Point2D p0, p1;

		for (size_t i = 0; i < vecEdges.size(); i++)
		{
			Edge2D& e = vecEdges[i];
			p0 = e.getBegPoint();
			p1 = e.getEndPoint();
			p0 -= minPnt;
			p1 -= minPnt;
			e.setPoints(p0, p1);
		}
	}

	std::vector<Edge2D> Densify(const std::vector<Edge2D>& vecEdges, double step)
	{
		std::vector<Edge2D> densifiedEdges;
		densifiedEdges.reserve(vecEdges.size() * 5);
		vector<Point2D> vecPnts;

		for (auto & e : vecEdges)
		{
			vecPnts = e.densify(step, true);

			for (size_t i = 0; i < vecPnts.size() - 1; i++)
				densifiedEdges.push_back(Edge2D(vecPnts[i], vecPnts[i + 1]));
		}

		return densifiedEdges;
	}
}