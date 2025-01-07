#define _USE_MATH_DEFINES

#include "VPN/House.h"

using namespace cv;
using namespace std;

namespace ViewPointNetwork
{
	House::House()
	{
		m_type = HT_Indoor;
		m_maxX = 0;
		m_maxY = 0;
	}

	House::House(const std::vector<Edge2D> & vecEdges, HouseType t)
	{
		m_type = t;
		m_maxX = 0;
		m_maxY = 0;
		double x, y;
		m_edges = vecEdges;
		for (const auto& edge : vecEdges)
		{
			x = edge.getBegPoint().X();
			y = edge.getBegPoint().Y();

			m_maxX = max(m_maxX, x);
			m_maxY = max(m_maxY, y);

			x = edge.getEndPoint().X();
			y = edge.getEndPoint().Y();

			m_maxX = max(m_maxX, x);
			m_maxY = max(m_maxY, y);
		}

		if (t == HT_Outdoor)
		{
			for (auto& edge : m_edges)
			{
				edge = Edge2D(edge.getBegPoint() + Point2D(1.0 * m_maxX, 1.0 * m_maxY), 
					edge.getEndPoint() + Point2D(1.0 * m_maxX, 1.0 * m_maxY));
			}
		}
		writeEdges("move_min", m_edges);

		initializeBspTree();
	}
	void getBspEdges(vector<Edge2D>& edges,BSPNode* bspNode)
	{
		if (!bspNode) return;
		edges.push_back(bspNode->getEdge());
		getBspEdges(edges, bspNode->backNode);
		getBspEdges(edges, bspNode->frontNode);
	}
	void House::initializeBspTree()
	{
		m_bspRoot = shared_ptr<BSPNode>(buildBspTree(m_edges, 0),deleteNode);
		vector<Edge2D> edges;
		getBspEdges(edges, m_bspRoot.get());
		writeEdges("bsp", edges);
	}

	int House::getNearstEdgeInd(const Point2D & p) const
	{
		double minDist = std::numeric_limits<double>::max();
		double d;
		size_t ind = 0;

		for (size_t i = 0; i < m_edges.size(); i++)
		{
			d = m_edges[i].distance(p);

			if (d < minDist)
			{
				minDist = d;
				ind = i;
			}
		}

		return ind;
	}

	PositionType House::position(const Point2D& p) const
	{
		bool flag = false;
		size_t edgeCount = m_edges.size();

		for (size_t i = 0; i < edgeCount; i++) {
			const Edge2D& e = m_edges[i];
			const Point2D& p1 = e.getBegPoint();
			const Point2D& p2 = e.getEndPoint();

			if (e.isCover(p)) {
				return PT_ONWALL;
			}
			// 处理水平边：如果边平行于射线的水平线，跳过
			if (p1.Y() == p2.Y()) {
				continue;  // 如果是水平边且不在射线范围内，跳过该边
			}

			// 判断点是否在边的垂直方向范围内
			if ((p1.Y() > p.Y()) != (p2.Y() > p.Y())) {
				// 计算交点的横坐标
				double xIntersection = p1.X() + (p.Y() - p1.Y()) * (p2.X() - p1.X()) / (p2.Y() - p1.Y());

				// 如果交点在点的右侧，则算作一次交点
				if (xIntersection > p.X()) {
					flag = !flag;  // 每次交点出现，状态取反
				}
			}
		}

		return flag ? PT_INSIDE : PT_OUTSIDE;  // 如果交点数为奇数，返回 1（内部）；偶数，返回 0（外部）
	}

	PositionType House::position(int indX, int indY, double cell) const
	{
		Point2D p(indX * cell, indY * cell);
		return position(p);
	}
}