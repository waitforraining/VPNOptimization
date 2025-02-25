#ifndef __VPN_HOUSE_H__
#define __VPN_HOUSE_H__
#include <vector>
#include <memory>

#include "preCompiled.h"

#include "Shape/Point2D.h"
#include "Shape/Edge2D.h"

#include "VPN/Station.h"
#include "VPN/BSPNode.h"
#include "VPN/VPNTypeDefinition.h"

namespace ViewPointNetwork
{
	class VPN_API House
	{
	public:
		House();
		House(const std::vector<Edge2D>& vecEdges, HouseType t = HT_Indoor);
		~House() {};

	public:
		double getMaxX() const { return m_maxX; }
		double getMaxY() const { return m_maxY; }

		HouseType getType() const { return m_type; }
		void setType(HouseType t) { m_type = t; }

		const std::vector<Edge2D>& getEdges() const { return m_edges; }
		const Edge2D& getEdge(int i) const { return m_edges[i]; }
		std::shared_ptr<BSPNode> getBspRoot() { return m_bspRoot; }

		size_t getEdgesNum() { return m_edges.size(); };
		PositionType position(int indX, int indY, double cell) const;//-1 outside 0 onWall 1 inside
		PositionType position(const Point2D& p) const;

		Point2D getCentre() const { return m_type == HT_Indoor ? Point2D(m_maxX / 2.0, m_maxY / 2.0) :
			Point2D(m_maxX / 2.0 + m_maxX * 1.0, m_maxY / 2.0 + m_maxY * 1.0); }

	private:
		int getNearstEdgeInd(const Point2D& p) const;
		void initializeBspTree();
		void saveBspTree(BSPNode* node, std::ofstream& outFile);
		void saveBspToCsv();
	private:
		std::vector<Edge2D> m_edges;
		std::vector<Edge2D> m_denseEdges;
		Point2D m_centre;
		double m_maxX, m_maxY;// Width and height of the area
		HouseType m_type;
		std::shared_ptr<BSPNode> m_bspRoot;
	};
}
#endif