#define _USE_MATH_DEFINES

using namespace std;

#include "Station.h"
#include "BSPNode.h"

namespace ViewPointNetwork
{
	//Station类初始化函数，缺省函数，先初始化父类
	Station::Station() :Point2D()
	{
		m_scanedLen = 0.0;
		m_scanedScore = 0.0;
		m_scanedFin = false;
		m_rMin = 0.0;
		m_rMax = 0.0;
		m_maxScanedAngle = 0.0;
	}

	//Station类初始化函数，传入点的二维坐标，先初始化父类
	Station::Station(double x, double y, double r_min, double r_max) :Point2D(x, y)
	{
		m_scanedLen = 0;
		m_scanedScore = 0;
		m_scanedFin = false;
		m_rMin = r_min;
		m_rMax = r_max;
		m_maxScanedAngle = 0.0;
	}

	//Station类的初始化函数，传入一个父类对象，先初始化父类
	Station::Station(const Point2D& p, double r_min, double r_max) :Point2D(p)
	{
		m_scanedLen = 0;
		m_scanedScore = 0;
		m_scanedFin = false;
		m_rMin = r_min;
		m_rMax = r_max;
		m_maxScanedAngle = 0.0;
	}

	//Station对象的属性初始化
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

	void Station::scan(std::shared_ptr<BSPNode> root)
	{
		traversal(root);
		mergeOnlineScanEdge();
		computeScanAngle();
		computeScanLength();
	}

	double Station::getEdgeDist(const Edge2D& edge, bool real) const
	{
		double d = 0.0;

		if (real)
			d = edge.getLine2D().Distance(*this);
		else
			d = edge.distance(*this);

		return d;
	}

	//添加站点到某边的距离
	void Station::addEdgeDist(double d)
	{
		m_dist2Edge.push_back(d);
	}

	//
	double Station::getScanLen() const
	{
		return m_scanedLen;
	}

	//获取站点扫描最小角
	double Station::getSmallestAng() const
	{
		return m_smallestAng;
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

	bool Station::wirteScannedEdges(const std::string& prefix) const
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2);
		ss << prefix << X() << "_" << Y();
		std::string name = ss.str();

		//std::vector<Edge2D> vecEdges;
		//station point
		std::vector<Edge2D> vecEdges = constructStationCircle();

		//scanned edges
		for(const auto & var : m_edgeScanned)
			vecEdges.push_back(var.second);

		//WriteEdges2LineTop(vecEdges, name + ".top");
		return true;
	}

	//获取站点对边的扫描角度
	vector<vector<double>> Station::getEdgeAngle(const Edge2D & edge) const
	{
		vector<double> theta;
		theta.push_back(Vector2D(*this, edge.getBegPoint()).getTheta());
		theta.push_back(Vector2D(*this, edge.getEndPoint()).getTheta());
		//for (int i = 0; i < 2; i++)
		//{
		//	Vector2D v = Vector2D(*this, edge.getEndpoint(i));
		//	theta.push_back(v.getTheta());
		//}

		if (abs(theta[0] - theta[1]) > M_PI)
		{
			vector<double> a1;
			a1.push_back(0);
			a1.push_back(min(theta[0], theta[1]));

			vector<double> a2;
			a2.push_back(max(theta[0], theta[1]));
			a2.push_back(2 * M_PI);

			vector<vector<double>> angle;
			//if (a1[0] != a1[1]) angle.push_back(a1);
			angle.push_back(a1);
			//if (a2[0] != a2[1]) angle.push_back(a2);
			angle.push_back(a2);

			return angle;
		}
		else
		{
			vector<double> a;
			a.push_back(min(theta[0], theta[1]));
			a.push_back(max(theta[0], theta[1]));

			vector<vector<double>> angle;
			//if (a[0] != a[1]) angle.push_back(a);
			angle.push_back(a);
			return angle;
		}
	}

	//获取站点对扫描到的边区域
	map<vector<double>, Edge2D> Station::getScanedEdges() const
	{
		return m_edgeScanned;
	}

	//返回站点到所有边的距离
	vector<double> Station::getDist2Edge() const
	{
		return m_dist2Edge;
	}

	//获取站点扫描得分
	double Station::getScanedScore() const
	{
		return m_scanedScore;
	}

	//通过扫描角计算扫描到的某条边的临界点（即边的一小段）
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

	//通过扫描角计算扫描到的某条边的临界点（即边的一小段）
	Point2D Station::getPoint(const Edge2D& edge, double theta) const
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

		return Point2D(x, y);
	}


	void Station::scanEdge(const Edge2D& edge)
	{
		double s = 0.0;
		if (edge.getLine2D().Online(*this))
			return ;

		vector<vector<double>> angles = getEdgeAngle(edge);//整条边的扫描角度
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

	//获取站点扫描长度
	void Station::computeScanLength()
	{
		m_scanedLen = 0;
		for (auto itr = m_edgeScanned.begin(); itr != m_edgeScanned.end(); itr++)
			m_scanedLen += itr->second.Length();
	}

	//获取某站点的对所有扫描边的得分总和
	void Station::computeScanAngle()
	{
#ifdef _DEBUG
		if (X() == 42.0 && Y() == 8.0)
			wirteScannedEdges();

		std::vector<double> vecAngles;
		vecAngles.reserve(m_edgeScanned.size());
#endif

		double score = 0, scoreTem;
		m_smallestAng = 180;
		double angle;

		for (auto itr = m_edgeScanned.begin(); itr != m_edgeScanned.end(); itr++) {
			if (itr->second.Length() == 0.0) continue;
			scoreTem = getScore(itr->second);
			score += scoreTem;

#ifdef _DEBUG
			vecAngles.push_back(180.0 * scoreTem / M_PI);
#endif

			//m_smallestAng = m_smallestAng < abs(itr->first[0] - itr->first[1]) ? m_smallestAng : abs(itr->first[0] - itr->first[1]);
			angle = itr->first[1] - itr->first[0];
			if (m_smallestAng > angle)
				m_smallestAng = angle;
		}
		m_scanedScore = score;
	}

	void Station::traversal(std::shared_ptr<BSPNode> root)
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

	//考虑外盲区和内盲区
	double Station::getScore(const Edge2D& e) const
	{
		// 视场角
		double d = e.getLine2D().Distance(*this);
		if (d >= m_rMax) {
			return 0.0;
		}
		else if (d >= m_rMin) {
			//直线与外圆交线
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
			//直线与内圆交线

			auto crosses_min = e.getLine2D().intersectCircle(m_x, m_y, m_rMin);
			Edge2D circleEdge_min(crosses_min.first, crosses_min.second);

			//直线与外圆交线
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
			//看到部分也算补充边
			return true;
		}
	}

	bool Station::isInValidArea(const Edge2D& e) const
	{
		return !(isInterArea(e, m_rMin) || isInterArea(e, m_rMax));
	}

}
