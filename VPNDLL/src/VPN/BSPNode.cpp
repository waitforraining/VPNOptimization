#define _USE_MATH_DEFINES
#include "VPN/BSPNode.h"


using namespace std;
using namespace cv;

namespace ViewPointNetwork
{
	// Record the position relationship of the edge
	BSPNode::BSPNode()
	{
		m_edge = Edge2D();
		m_minScan = 0;
		m_minwall = 40;
	}

	BSPNode::BSPNode(const Edge2D& edge)
	{
		m_edge = edge;
		m_minScan = 0;
		m_minwall = 40;
	}

	const Edge2D& BSPNode::getEdge() const
	{
		return m_edge;
	}

	void BSPNode::setName(string name)
	{
		m_name = name;
	}

	string BSPNode::getName()
	{
		return m_name;
	}

	void partitionEdge(const vector<Edge2D>& edges, const Edge2D& edge, vector<Edge2D>& frontLines, vector<Edge2D>& backLines)
	{
		for (size_t i = 0; i < edges.size(); ++i) {
			const Edge2D& curEdge = edges[i];
			if (curEdge == edge) continue;
			int side0 = edge.isFrontOf(curEdge.getBegPoint());
			int side1 = edge.isFrontOf(curEdge.getEndPoint());

			if (side0 ^ side1)
			{
				if (side0 + side1 < 1) // One is -1;
				{
					if (side0 + side1 == 0) frontLines.push_back(curEdge);
					else backLines.push_back(curEdge);
					continue;
				}
				Point2D p = curEdge.getCrossPoint(edge);

				if (side0)
				{
					frontLines.emplace_back(curEdge.getBegPoint(), p);
					backLines.emplace_back(p, curEdge.getEndPoint());
				}
				else
				{
					backLines.emplace_back(curEdge.getBegPoint(), p);
					frontLines.emplace_back(p, curEdge.getEndPoint());
				}
			}
			else if (side0)  // Both on the line also put in the front
				frontLines.push_back(curEdge);
			else
				backLines.push_back(curEdge);
		}
	}

	BSPNode* buildBspTree(const std::vector<Edge2D>& edges, int partition, const std::string& name)
	{
		if (edges.empty())
			return nullptr;

		const Edge2D& nodeEdge = edges[partition];
		std::vector<Edge2D> frontLines, backLines;
		partitionEdge(edges, nodeEdge, frontLines, backLines);
		BSPNode* newNode = new BSPNode(edges[partition]);

		newNode->setName(name);
		newNode->frontNode = buildBspTree(frontLines, frontLines.size() / 2, name + "f");
		newNode->backNode = buildBspTree(backLines, backLines.size() / 2, name + "b");
		return newNode;

	}

}