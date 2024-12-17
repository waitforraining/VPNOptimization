#define _USE_MATH_DEFINES

#include "House.h"

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
		Point2D p1, p2;
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
				edge = Edge2D(p1 + Point2D(1.0 * m_maxX, 1.0 * m_maxY), p2 + Point2D(1.0 * m_maxX, 1.0 * m_maxY));
			}
		}

		initializeBspTree();
	}

	void House::initializeBspTree()
	{
		m_bspRoot = shared_ptr<BSPNode>(buildBspTree(m_edges, 0),deleteNode);
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
			// ����ˮƽ�ߣ������ƽ�������ߵ�ˮƽ�ߣ�����
			if (p1.Y() == p2.Y()) {
				continue;  // �����ˮƽ���Ҳ������߷�Χ�ڣ������ñ�
			}

			// �жϵ��Ƿ��ڱߵĴ�ֱ����Χ��
			if ((p1.Y() > p.Y()) != (p2.Y() > p.Y())) {
				// ���㽻��ĺ�����
				double xIntersection = p1.X() + (p.Y() - p1.Y()) * (p2.X() - p1.X()) / (p2.Y() - p1.Y());

				// ��������ڵ���Ҳ࣬������һ�ν���
				if (xIntersection > p.X()) {
					flag = !flag;  // ÿ�ν�����֣�״̬ȡ��
				}
			}
		}

		return flag ? PT_INSIDE : PT_OUTSIDE;  // ���������Ϊ���������� 1���ڲ�����ż�������� 0���ⲿ��
	}

	PositionType House::position(int indX, int indY, double cell) const
	{
		Point2D p(indX * cell, indY * cell);
		return position(p);
	}
}