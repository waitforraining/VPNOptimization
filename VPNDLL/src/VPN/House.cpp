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
		saveBspToCsv();
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

	void House::saveBspToCsv()
	{
		BSPNode* root = getBspRoot().get();

		std::ofstream outFile("bspEdges.csv");
		if (!outFile.is_open()) {
			std::cerr << "Failed to open file for writing\n";
			return;
		}

		// Writes to the header of the CSV file
		
		outFile << "Node Name, Edge Length\n";

		saveBspTree(root, outFile);

		outFile.close();
	}

	void House::saveBspTree(BSPNode* node, std::ofstream& outFile)
	{
		// If the node is empty, return it directly
		if (node == nullptr) {
			return;
		}

		// Calculates the edge length of the current node
		double edgeLength = node->getEdge().Length();

		outFile << node->getName() << "," << edgeLength << "\n";

		// Recursively traverse the left and right subtrees
		saveBspTree(node->frontNode, outFile);
		saveBspTree(node->backNode, outFile);
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
			// Handle horizontal edges: 
			//If the edge is parallel to the horizontal line of the ray, skip
			if (p1.Y() == p2.Y()) {
				continue;  // If it is a horizontal edge and not in the ray range, skip the edge
			}

			// Determine whether the point is within the vertical range of the edge
			if ((p1.Y() > p.Y()) != (p2.Y() > p.Y())) {
				// Calculate the abscissa of the intersection
				double xIntersection = p1.X() + (p.Y() - p1.Y()) * (p2.X() - p1.X()) / (p2.Y() - p1.Y());

				// If the intersection is to the right of the point, it counts as a single intersection
				if (xIntersection > p.X()) {
					flag = !flag;  // Every time an intersection occurs, the state is reversed
				}
			}
		}

		return flag ? PT_INSIDE : PT_OUTSIDE;  // If the number of intersections is odd, return 1 (inside); 
		//Even number, return 0 (external)
	}

	PositionType House::position(int indX, int indY, double cell) const
	{
		Point2D p(indX * cell, indY * cell);
		return position(p);
	}
}