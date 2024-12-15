#define _USE_MATH_DEFINES
#include "Edge2D.h"
#include "Vector2D.h"
#include "Point2D.h"
#include "../VPN/Station.h"
#include "../VPN/BSPNode.h"

#include <math.h>
#include <assert.h>

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

	bool Edge2D::isInInterBlindArea(const Station& s, double r_min) const
	{
		double d = m_line.Distance(s);
		if (d >= r_min) {
			return false;
		}
		auto crosses_min = m_line.intersectCircle(s.X(), s.Y(), r_min);
		Edge2D circleEdge_min(crosses_min.first, crosses_min.second);
		
		if (!circleEdge_min.isCover(*this)) {
			return false;
		}
		else {
			//看到部分也算补充边
			return true;
		}
	}
	bool Edge2D::isInOuterBlindArea(const Station& s, double r_max) const
	{
		double d = m_line.Distance(s);
		if (d >= r_max) {
			return true;
		}
		auto crosses_max = m_line.intersectCircle(s.X(), s.Y(), r_max);
		Edge2D circleEdge_max(crosses_max.first, crosses_max.second);

		if (Overlap(circleEdge_max).Length() == 0.0) {
			return true;
		}
		else {
			//看到部分也算补充边
			return false;
		}
	}

	bool Edge2D::isInValidArea(const Station& s, double r_min, double r_max) const
	{
		return !(isInInterBlindArea(s, r_min) || isInOuterBlindArea(s, r_max));
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




	//返回边所在直线的系数a，b，c
	double* Edge2D::getLine() const
	{
		//double m_line[3] = {0};
		double* m_line = new double[3];
		m_line[0] = getBegPoint().Y() - getEndPoint().Y();
		m_line[1] = getEndPoint().X() - getBegPoint().X();
		m_line[2] = getBegPoint().X() * getEndPoint().Y() - getEndPoint().X() * getBegPoint().Y();
		
		return m_line;
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

	//获取站点s对边的得分，该函数需确保边在有效扫描区域内
	double Edge2D::getScore(const Station& s) const
	{
		double angle = Angle2D(getBegPoint(), s, getEndPoint());
		return angle;
	}

	//考虑内盲区
	double Edge2D::getScore(const Station& s, double r1) const
	{
		double angle = Angle2D(getBegPoint(), s, getEndPoint());

		// 视场角
		double d = m_line.Distance(s);
		if (d >= r1)
			return angle;
		
		auto crosses = m_line.intersectCircle(s.X(), s.Y(), r1);
		Edge2D circleEdge(crosses.first, crosses.second);
		Edge2D overlapEdge = Overlap(circleEdge);
		if (overlapEdge.Length() == 0.0)
			return angle;

		double overlapAngle = Angle2D(overlapEdge.getBegPoint(), s, overlapEdge.getEndPoint());
		return max(angle - overlapAngle, 0.0);
	}

	//考虑外盲区和内盲区
	double Edge2D::getScore(const Station& s, double r_min, double r_max) const
	{
		// 视场角
		double d = m_line.Distance(s);
		if (d >= r_max) {
			return 0.0;
		}
		else if (d >= r_min) {
			//直线与外圆交线
			auto crosses = m_line.intersectCircle(s.X(), s.Y(), r_max);
			Edge2D circleEdge(crosses.first, crosses.second);
			Edge2D overlapEdge = Overlap(circleEdge);

			if (overlapEdge.Length() == 0.0)
			{
				return 0.0;
			}

			double overlapAngle = Angle2D(overlapEdge.getBegPoint(), s, overlapEdge.getEndPoint());
			return max(overlapAngle, 0.0);
		}
		else {
			//直线与内圆交线

			auto crosses_min = m_line.intersectCircle(s.X(), s.Y(), r_min);
			Edge2D circleEdge_min(crosses_min.first, crosses_min.second);

			//直线与外圆交线
			auto crosses_max = m_line.intersectCircle(s.X(), s.Y(), r_max);
			Edge2D circleEdge_max(crosses_max.first, crosses_max.second);

			Edge2D overlapEdgeMax = Overlap(circleEdge_max);

			if (overlapEdgeMax.Length() == 0.0) {
				return 0.0;
			}

			double overlapAngleMax = Angle2D(overlapEdgeMax.getBegPoint(), s, overlapEdgeMax.getEndPoint());
			Edge2D overlapEdgeMin = Overlap(circleEdge_min);

			if (overlapEdgeMin.Length() == 0.0) {
				return overlapAngleMax;
			}

			double overlapAngleMin = Angle2D(overlapEdgeMin.getBegPoint(), s, overlapEdgeMin.getEndPoint());
			return max(overlapAngleMax - overlapAngleMin, 0.0);

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

		if (fabs(d) > 0.0001) return false;
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

	std::map<Edge2D, bool> Edge2D::getValidScanedEdges(const Station& s, double r_min, double r_max) const
	{
		//改进版，采用线段与圆的截点来获取不同的分割部分
		//灵活使用isCover和Overlap囊括所有位置情况
		std::map<Edge2D, bool> validEdgesMap;
		validEdgesMap.clear();
		//std::vector<Edge2D> validEdges;

		// 计算站点与边的距离
		double d = m_line.Distance(s);



		// distance >= r_max，返回空的有效边段
		if (d >= r_max) {
			//return validEdges;
			validEdgesMap[*this] = false;
			return validEdgesMap;
		}
		//外圆
		auto crosses_max = m_line.intersectCircle(s.X(), s.Y(), r_max);
		Edge2D circleEdge_max(crosses_max.first, crosses_max.second);
		vector<Point2D> croMaxPnts;
		Point2D croMaxP1 = crosses_max.first;
		Point2D croMaxP2 = crosses_max.second;
		croMaxPnts.push_back(croMaxP1);
		croMaxPnts.push_back(croMaxP2);

		int maxCnt = 0;
		Point2D tmpMaxPnt;
		//判断外圆两截点是否位于线段上，分为三种情况
		for each (auto & pnt in croMaxPnts) {
			if (isCover(pnt)) {
				maxCnt++;
				tmpMaxPnt = pnt;
			}
		}
		// r_min <= d < r_max
		if (d >= r_min) {
			if (maxCnt == 0) {
				if (Overlap(circleEdge_max).Length() == 0.0) {
					validEdgesMap[*this] = false;
				}
				else {
					validEdgesMap[*this] = true;
				}
			}
			else if (maxCnt == 1) {
				Edge2D tmpe = *this;
				Edge2D segOut, segIn;
				//穿过外圆分割为两部分,端点可能位于外圆上
				if (s.distance(getBegPoint()) >= r_max) {
					segOut = Edge2D(getBegPoint(), tmpMaxPnt);
					segIn = Edge2D(tmpMaxPnt, getEndPoint());
				}
				else {
					segOut = Edge2D(getEndPoint(), tmpMaxPnt);
					segIn = Edge2D(tmpMaxPnt, getBegPoint());
				}
				if (segOut.Length() != 0.0) validEdgesMap[segOut] = false;
				if (segIn.Length() != 0.0) validEdgesMap[segIn] = true;

				/*if (getBegPoint() == Point2D(0.0, 0.0) && getEndPoint() == Point2D(0.0, 4.3297031327937212)|| 
					getEndPoint() == Point2D(0.0, 0.0) && getBegPoint() == Point2D(0.0, 4.3297031327937212)) {
					cout << "1" << endl;
				}*/
				//({ size = 2 }, { m_begPnt = {m_x = 0.0000000000000000 m_y = 4.3297031327937212 } m_endPnt = {m_x = 0.0000000000000000 m_y = 2.9423377254580196 } ... })
				/*if (getBegPoint() == Point2D(0.0, 0.0) && getEndPoint() == Point2D(0.0, 4.4328571000000005) ||
					getEndPoint() == Point2D(0.0, 0.0) && getBegPoint() == Point2D(0.0, 4.4328571000000005)) {
					cout << "1" << endl;
				}*/
				/*if (validEdgesMap.size() != 2)
				{
					if (segOut.Length() != 0.0) cout << "out+1" << endl;
					if (segIn.Length() != 0.0) cout << "in+1" << endl;
					cout << validEdgesMap[segOut] << endl;
					cout << validEdgesMap[segIn] << endl;
					cout << segOut.Length() << endl;
					cout << segIn.Length() << endl;
					cout << "error" << endl;
				}*/
				//{m_x = 0.0000000000000000 m_y = 4.4328571000000005 }
				//{m_begPnt = { m_x = 0.0000000000000000 m_y = 4.3297031327937212 } m_endPnt = { m_x = 0.0000000000000000 m_y = 0.0000000000000000 } ...}
			}
			//maxCnt = 2
			else {
				if (getBegPoint().distance(croMaxP1) < getBegPoint().distance(croMaxP2)) {
					if(getBegPoint().distance(croMaxP1) != 0.0) validEdgesMap[Edge2D(getBegPoint(), croMaxP1)] = false;
				}
				else {
					if (getBegPoint().distance(croMaxP2) != 0.0) validEdgesMap[Edge2D(getBegPoint(), croMaxP2)] = false;
				}

				if (getEndPoint().distance(croMaxP1) < getEndPoint().distance(croMaxP2)) {
					if(getEndPoint().distance(croMaxP1) != 0.0) validEdgesMap[Edge2D(getEndPoint(), croMaxP1)] = false;
				}
				else {
					if (getEndPoint().distance(croMaxP2) != 0.0) validEdgesMap[Edge2D(getEndPoint(), croMaxP2)] = false;
				}
				validEdgesMap[circleEdge_max] = true;
			}
		}
		// d < r_min
		else {
			//内圆
			auto crosses_min = m_line.intersectCircle(s.X(), s.Y(), r_min);
			Edge2D circleEdge_min(crosses_min.first, crosses_min.second);
			/*vector<Point2D> croMinPnts;
			Point2D croMinP1 = crosses_min.first;
			Point2D croMinP2 = crosses_min.second;
			croMinPnts.push_back(croMinP1);
			croMinPnts.push_back(croMinP2);*/
			// 外圆截线与内圆截线三部分
			std::map<Edge2D, bool> splitedEdges = circleEdge_max.getOverlapEdgeSegs(circleEdge_min);
			if (maxCnt == 0) {
				//cout << "maxCnt = 0" << endl;
				if (Overlap(circleEdge_max).Length() == 0.0) {
					validEdgesMap[*this] = false;
				}
				else {
					for each (auto & oneSplitedEdge in splitedEdges) {
						Edge2D tmpOverlapEdge = Overlap(oneSplitedEdge.first);
						if (tmpOverlapEdge.Length() == 0.0) continue;
						if (oneSplitedEdge.second == true) {
							validEdgesMap[tmpOverlapEdge] = true;
						}
						else {
							validEdgesMap[tmpOverlapEdge] = false;
						}

					}
				}
			}
			else if (maxCnt == 2) {
				//cout << "maxCnt = 2" << endl;
				if (getBegPoint().distance(croMaxP1) < getBegPoint().distance(croMaxP2)) {
					if (getBegPoint().distance(croMaxP1) != 0.0) validEdgesMap[Edge2D(getBegPoint(), croMaxP1)] = false;
				}
				else {
					if (getBegPoint().distance(croMaxP2) != 0.0) validEdgesMap[Edge2D(getBegPoint(), croMaxP2)] = false;
				}

				if (getEndPoint().distance(croMaxP1) < getEndPoint().distance(croMaxP2)) {
					if (getEndPoint().distance(croMaxP1) != 0.0) validEdgesMap[Edge2D(getEndPoint(), croMaxP1)] = false;
				}
				else {
					if (getEndPoint().distance(croMaxP2) != 0.0) validEdgesMap[Edge2D(getEndPoint(), croMaxP2)] = false;
				}

				for each (auto & oneSplitedEdge in splitedEdges) {
					if (oneSplitedEdge.second == true) {
						validEdgesMap[oneSplitedEdge.first] = true;
					}
					else {
						validEdgesMap[oneSplitedEdge.first] = false;
					}

				}

			}
			else {
				//cout << "maxCnt = 1" << endl;
				//处理外盲区部分
				if (s.distance(getBegPoint()) >= r_max) {
					Edge2D segOut = Edge2D(getBegPoint(), tmpMaxPnt);
					if (segOut.Length() != 0.0) validEdgesMap[segOut] = false;
				}
				else {
					Edge2D segOut = Edge2D(getEndPoint(), tmpMaxPnt);
					if (segOut.Length() != 0.0) validEdgesMap[segOut] = false;
				}

				for each (auto & oneSplitedEdge in splitedEdges) {
					Edge2D tmpOverlapEdge = Overlap(oneSplitedEdge.first);
					if (tmpOverlapEdge.Length() == 0.0) continue;
					if (oneSplitedEdge.second == true) {
						validEdgesMap[tmpOverlapEdge] = true;
					}
					else {
						validEdgesMap[tmpOverlapEdge] = false;
					}

				}
			}
		}
		return validEdgesMap;
		
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

		for each (const auto & v in vecEdges)
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

		for each (const auto & e in vecEdges)
		{
			vecPnts = e.densify(step, true);

			for (size_t i = 0; i < vecPnts.size() - 1; i++)
				densifiedEdges.push_back(Edge2D(vecPnts[i], vecPnts[i + 1]));
		}

		return densifiedEdges;
	}
}