#define _USE_MATH_DEFINES

using namespace std;
#include <cmath>


#include "VPN/Station.h"
#include "VPN/BSPNode.h"

namespace ViewPointNetwork
{
	//Station class initialization function, default function, initializes the parent class first
	Station::Station() :Point2D()
	{
		m_scanedLen = 0.0;
		m_scanedScore = 0.0;
		m_scanedFin = false;
		m_rMin = 0.0;
		m_rMax = 0.0;
		m_maxScanedAngle = 0.0;
	}

	//Station class initialization function, passing the two-dimensional coordinates of the point, 
	//first initialize the parent class
	Station::Station(double x, double y, double r_min, double r_max) :Point2D(x, y)
	{
		m_scanedLen = 0;
		m_scanedScore = 0;
		m_scanedFin = false;
		m_rMin = r_min;
		m_rMax = r_max;
		m_maxScanedAngle = 0.0;
	}

	//Station class initialization function, passed a superclass object, 
	//first initialize the superclass
	Station::Station(const Point2D& p, double r_min, double r_max) :Point2D(p)
	{
		m_scanedLen = 0;
		m_scanedScore = 0;
		m_scanedFin = false;
		m_rMin = r_min;
		m_rMax = r_max;
		m_maxScanedAngle = 0.0;
	}

	// Initialize the properties of the Station object
	void Station::initial()
	{
		m_edgeScanned.clear();
		m_scanedFin = false;
		m_scanedLen = 0;
	}

	bool Station::operator ==(const Station& s) const
	{
		return abs(X() - s.X()) < 1e-5 && abs(Y() - s.Y()) < 1e-5;
	}

	void Station::scan(BSPNode* root)
	{
		traversal(root);
		//mergeOnlineScanEdge();
		computeScanAngle();
		computeScanLength();
	}

	double Station::getScanLen() const
	{
		return m_scanedLen;
	}

	std::vector<Edge2D> Station::constructStationCircle() const
	{
		int nStep = 36;
		std::vector<Edge2D> rst;
		rst.reserve(nStep);
		double ang = 2.0 * M_PI / nStep;
		double r = 0.1 * GetRadiusMin();
		Point2D p0, p1;
		p0.X() = X() + r * cos(0.0);
		p0.Y() = Y() + r * sin(0.0);

		for (int i = 1 ; i <= nStep; i++)
		{
			p1.X() = X() + r * cos(ang * i);
			p1.Y() = Y() + r * sin(ang * i);
			rst.emplace_back(p0, p1);
			p0 = p1;
		}

		return rst;
	}

	void Station::writeScannedEdges(const std::string& prefix) const
	{
		std::vector<Edge2D> vecEdges = constructStationCircle();

		for(const auto & var : m_edgeScanned)
			vecEdges.push_back(var.second);
		writeEdges(prefix, vecEdges);
	}

	void Station::writeInResolutionEdges(const std::string& prefix, vector<Edge2D> denseEdges, double minScanAngle)
	{
		std::vector<Edge2D> vecEdges = constructStationCircle();

		for (auto j = m_edgeScanned.begin(); j != m_edgeScanned.end(); j++) {
			const auto& curEdge = j->second;

			for (size_t k = 0; k < denseEdges.size(); k++) {

				if (denseEdges[k].Overlap(curEdge).Length() > denseEdges[k].Length() * 0.9 &&
					isInValidArea(denseEdges[k]) &&
					isInValidResolutionArea(denseEdges[k], minScanAngle))
				{
					vecEdges.push_back(denseEdges[k]);
				}
			}
		}

		auto startEdge = m_edgeScanned.begin()->second;
		auto endEdge = m_edgeScanned.rbegin()->second;
		if (ParallelLines(startEdge.getLine2D(), endEdge.getLine2D(), 0.001 * M_PI / 180.0))
		{
			auto mergeEdge = startEdge + endEdge;
			for (size_t k = 0; k < denseEdges.size(); k++) {

				if (denseEdges[k].Overlap(mergeEdge).Length() > denseEdges[k].Length() * 0.9 &&
					isInValidArea(denseEdges[k]) &&
					isInValidResolutionArea(denseEdges[k], minScanAngle))
				{
					vecEdges.push_back(denseEdges[k]);
				}
			}
		}

		writeEdges(prefix, vecEdges);
	}

	// Get the scanning Angle of the opposite side of the site
	vector<vector<double>> Station::getEdgeAngle(const Edge2D & edge) const
	{
		vector<double> theta;
		theta.push_back(Vector2D(*this, edge.getBegPoint()).getTheta());
		theta.push_back(Vector2D(*this, edge.getEndPoint()).getTheta());

		if (abs(theta[0] - theta[1]) > M_PI)
		{
			vector<double> a1;
			a1.push_back(0);
			a1.push_back(min(theta[0], theta[1]));

			vector<double> a2;
			a2.push_back(max(theta[0], theta[1]));
			a2.push_back(2 * M_PI);

			vector<vector<double>> angle;
			angle.push_back(a1);
			angle.push_back(a2);

			return angle;
		}
		else
		{
			vector<double> a;
			a.push_back(min(theta[0], theta[1]));
			a.push_back(max(theta[0], theta[1]));

			vector<vector<double>> angle;
			angle.push_back(a);
			return angle;
		}
	}

	// Gets the scanned side area of the site pair
	const map<vector<double>, Edge2D>& Station::getScanedEdges() const
	{
		return m_edgeScanned;
	}

	// Return the distance from the site to all sides
	vector<double> Station::getDist2Edge() const
	{
		return m_dist2Edge;
	}

	// Get site scan score
	double Station::getScanedScore() const
	{
		return m_scanedScore;
	}

	// Calculate the critical point of a scanned edge (i.e. a small section of an edge) by scanning the Angle.
	Point2D Station::getPoint(const Edge2D & edge, double theta, double r1) const
	{
		vector<Point2D> vecPnts;
		vecPnts.push_back(edge.getBegPoint());
		vecPnts.push_back(edge.getEndPoint());

		for (int i = 0; i < 2; i++)
		{
			Vector2D v = Vector2D(*this, vecPnts[i]);
			if (abs(theta - v.getTheta()) < 1e-5)
			{
				//point[0] = vecPnts[i].X();
				//point[1] = vecPnts[i].Y();
				return vecPnts[i];
			}
		}

		double x, y;
		if (edge.isVertical())
		{
			x = edge.getBegPoint().X();
			y = tan(theta) * (x - m_x) + m_y;
		}
		else if (edge.isHorizontal())
		{
			y = edge.getBegPoint().Y();
			x = (y - m_y) / tan(theta) + m_x;
		}
		else
		{
			double tmp1 = tan(theta) * m_x - m_y + edge.getBegPoint().Y();
			double tmp2 = tan(theta) * m_x - m_y + edge.getEndPoint().Y();
			double tmp3 = edge.getEndPoint().X() - edge.getBegPoint().X();
			double tmp4 = edge.getBegPoint().Y() - edge.getEndPoint().Y();
			x = (edge.getEndPoint().X() * tmp1 - edge.getBegPoint().X() * tmp2) / (tmp3 * tan(theta) + tmp4);
			y = tan(theta) * (x - m_x) + m_y;
		}

		//point[0] = x; point[1] = y;
		return Point2D(x, y);
	}

	// Calculate the critical point of a scanned edge (i.e. a small section of an edge) by scanning the Angle.
	Point2D Station::getPoint(const Edge2D& edge, double theta) const
	{
		Line2D line0(-cos(theta), sin(theta), sin(theta) * m_x - cos(theta) * m_y);
		Line2D line1 = edge.getLine2D();
		
		double err = 0.001 * M_PI / 180.0;
		if (Angle2Lines(line0, line1) < err)// parallel
			return edge.getBegPoint();

		Point2D crossPnt;
		Intersection2NonParallelLines(line0, line1, crossPnt);
		return crossPnt;
	}


	void Station::scanEdge(const Edge2D& edge)
	{
		double s = 0.0;
		if (edge.getLine2D().Online(*this))
			return ;

		vector<vector<double>> angles = getEdgeAngle(edge);// Scanning Angle of the entire edge
		vector<vector<double>> addScanAngles;
		vector<double> tmpAngle(2);;
		auto scanIt = m_edgeScanned.begin();
		Point2D p0, p1;
		for (int i = 0; i < angles.size(); i++)
		{
			auto curAngle = angles[i];
			vector<vector<double>> addScanAngle;
			while (scanIt != m_edgeScanned.end() && scanIt->first[0] <= curAngle[1])
			{
				auto scanAngle = scanIt->first;
				if (scanAngle[0] - curAngle[0] > 1e-5)
				{
					tmpAngle[0] = curAngle[0];
					tmpAngle[1] = scanAngle[0];
					addScanAngles.push_back(tmpAngle);

				}
				curAngle[0] = max(curAngle[0], scanAngle[1]);
				scanIt++;
			}

			if (curAngle[1] - curAngle[0] > 1e-5)
			{
				tmpAngle[0] = curAngle[0];
				tmpAngle[1] = curAngle[1];
				addScanAngles.push_back(tmpAngle);
			}

			for (const auto& addScanAngle : addScanAngles)
			{
				p0 = getPoint(edge, addScanAngle[0]);
				p1 = getPoint(edge, addScanAngle[1]);
				Edge2D e(p0, p1);
				m_edgeScanned.insert(pair<vector<double>, Edge2D>(addScanAngle, e));
			}

		}
	}

	void Station::mergeOnlineScanEdge()
	{
		vector<double> mergeAngle(2);
		double maxScanAngle = 0.0;
		for (auto scanIt = m_edgeScanned.begin(); scanIt != m_edgeScanned.end();)
		{
			auto nextScanIt = std::next(scanIt);
			if (abs(scanIt->first[0] - maxScanAngle) < 1e-5)
				maxScanAngle = scanIt->first[1];
			if (nextScanIt == m_edgeScanned.end())
				break;
			if (nextScanIt->first[0] - scanIt->first[1] > 1e-5)
			{
				scanIt++;
				continue;
			}
			auto& e0 = scanIt->second;
			auto& e1 = nextScanIt->second;
			if (e0.OnLine(e1.getBegPoint()) && e0.OnLine(e1.getEndPoint()))
			{
				mergeAngle[0] = scanIt->first[0];
				mergeAngle[1] = nextScanIt->first[1];
				auto mergeEdge = e0 + e1;
				m_edgeScanned.erase(scanIt);
				m_edgeScanned.erase(nextScanIt);
				nextScanIt = m_edgeScanned.insert(make_pair(mergeAngle, mergeEdge)).first;
			}
			scanIt = nextScanIt;

		}
		if (abs(maxScanAngle - 2 * M_PI) < 1e-4)
			m_scanedFin = true;

	}

	// Get the site scan length
	void Station::computeScanLength()
	{
		m_scanedLen = 0;
		for (auto itr = m_edgeScanned.begin(); itr != m_edgeScanned.end(); itr++)
			m_scanedLen += itr->second.Length();
	}

	// Gets the total score of a site for all scanned edges
	void Station::computeScanAngle()
	{
		double score = 0, scoreTem;
		//double angle;

		for (auto itr = m_edgeScanned.begin(); itr != m_edgeScanned.end(); itr++) {
			if (itr->second.Length() == 0.0) continue;
			scoreTem = getScore(itr->second);
			score += scoreTem;

			//angle = itr->first[1] - itr->first[0];
		}
		m_scanedScore = score;
	}

	void Station::traversal(BSPNode* root)
	{
		if (!root || m_scanedFin)
			return;
		const Edge2D& edge = root->getEdge();

		if (edge.isFrontOf(*this))
		{
			traversal(root->frontNode);
			scanEdge(edge);
			mergeOnlineScanEdge();
			traversal(root->backNode);
		}
		else
		{
			traversal(root->backNode);
			scanEdge(edge);
			mergeOnlineScanEdge();
			traversal(root->frontNode);
		}
	}

	double Station::getOverlapPercent(const Station& s, OverlapScoreType ost) const
	{
		// assert(ost >= 0 && ost < vecScores.size());
		std::vector<double> vecScores = computeOverlap(s);
		return vecScores[ost];
	}

	std::vector<double> Station::computeOverlap(const Station& s) const//new one
	{
		const Station& s0 = *this;
		const Station& s1 = s;

#ifdef _DEBUG
		//s0.wirteScannedArea();
		//s1.wirteScannedArea();
#endif

		double len0 = s0.getScanLen();
		double ang0 = s0.getScanedScore();
		double len1 = s1.getScanLen();
		double ang1 = s1.getScanedScore();

		auto scanedEdges0 = s0.getScanedEdges();
		auto scanedEdges1 = s1.getScanedEdges();
		Edge2D overlapEdge;
		double overlapAng0 = 0.0, overlapAng1 = 0.0, overlapLen = 0.0;

		for (auto i = scanedEdges0.begin(); i != scanedEdges0.end(); i++) {
			const Edge2D& e0 = i->second;
			for (auto j = scanedEdges1.begin(); j != scanedEdges1.end(); j++) {
				const Edge2D& e1 = j->second;
				overlapEdge = e0.Overlap(e1);
				if (overlapEdge.Length() == 0.0) continue;

				overlapLen += overlapEdge.Length();
				overlapAng0 += s0.getScore(overlapEdge);
				overlapAng1 += s1.getScore(overlapEdge);
			}
		}

		double score_average_len = 2.0 * overlapLen / (len0 + len1);
		double score_minimum_len = overlapLen / min(len0, len1);
		double score_union_len = overlapLen / (len0 + len1 - overlapLen);
		double score_inter_sum_ang = (overlapAng0 + overlapAng1) / (ang0 + ang1);
		double score_intersect_ang = (overlapAng0 / ang0 + overlapAng1 / ang1) / 2.0;

		std::vector<double> rst;
		rst.push_back(score_average_len);
		rst.push_back(score_minimum_len);
		rst.push_back(score_union_len);
		rst.push_back(score_inter_sum_ang);
		rst.push_back(score_intersect_ang);
		return rst;
	}

	// Consider external and internal blind spots
	double Station::getScore(const Edge2D& e) const
	{
		// Field of view Angle
		double d = e.getLine2D().Distance(*this);
		if (d >= m_rMax) {
			return 0.0;
		}
		else if (d >= m_rMin) {
			// The line intersects the outer circle
			auto crosses = e.getLine2D().intersectCircle(m_x, m_y, m_rMax);
			Edge2D circleEdge(crosses.first, crosses.second);
			Edge2D overlapEdge = e.Overlap(circleEdge);

			if (overlapEdge.Length() == 0.0)
			{
				return 0.0;
			}

			double overlapAngle = Angle2D(overlapEdge.getBegPoint(), *this, overlapEdge.getEndPoint());
			return max(overlapAngle, 0.0);
		}
		else {
			// The line intersects the inner circle

			auto crosses_min = e.getLine2D().intersectCircle(m_x, m_y, m_rMin);
			Edge2D circleEdge_min(crosses_min.first, crosses_min.second);

			// The line intersects the outer circle
			auto crosses_max = e.getLine2D().intersectCircle(m_x, m_y, m_rMax);
			Edge2D circleEdge_max(crosses_max.first, crosses_max.second);

			Edge2D overlapEdgeMax = e.Overlap(circleEdge_max);

			if (overlapEdgeMax.Length() == 0.0) {
				return 0.0;
			}

			double overlapAngleMax = Angle2D(overlapEdgeMax.getBegPoint(), *this, overlapEdgeMax.getEndPoint());
			Edge2D overlapEdgeMin = e.Overlap(circleEdge_min);

			if (overlapEdgeMin.Length() == 0.0) {
				return overlapAngleMax;
			}

			double overlapAngleMin = Angle2D(overlapEdgeMin.getBegPoint(), *this, overlapEdgeMin.getEndPoint());
			return max(overlapAngleMax - overlapAngleMin, 0.0);

		}
	}

	bool Station::isInterArea(const Edge2D& e,double r) const
	{
		double d = e.getLine2D().Distance(*this);
		if (d >= r) {
			return false;
		}
		auto crosses_min = e.getLine2D().intersectCircle(m_x, m_y, r);
		Edge2D circleEdge_min(crosses_min.first, crosses_min.second);

		if (!circleEdge_min.isCover(*this)) {
			return false;
		}
		else {
			// See part also counts as supplementary edge
			return true;
		}
	}

	bool Station::isInValidArea(const Edge2D& e) const
	{
		double s2Point0 = Point2D::distance(e.getBegPoint());
		double s2Point1 = Point2D::distance(e.getEndPoint());
		if (min(s2Point0, s2Point1) >= m_rMax) // Partial edge
			return false;
		if (max(s2Point0, s2Point1) <= m_rMin)
			return false;
		
		return true;
	}
	bool Station::isInValidResolutionArea(const Edge2D& e, double minPointDist) const
	{
		return true;
		if (e.OnLine(*this, 1e-3)) return false;
		Point2D projectPnt = e.getLine2D().Project(*this);
		double scanAngle = 5.0 / 83 / 180 * M_PI;
		double h = e.getLine2D().Distance(*this);
		double A = tan(scanAngle);
		double B = minPointDist / h * A;
		double C = -A - minPointDist / h;

		double discriminant = B * B - 4 * A * C;
		double x1 = (-B + sqrt(discriminant)) / (2 * A);
		double minScanAngle = abs(atan(x1));

		double angle1 = atan2(projectPnt.distance(e.getBegPoint()), h);
		double angle2 = atan2(projectPnt.distance(e.getEndPoint()), h);
		return max(angle1, angle2) <= minScanAngle;
	}
}
