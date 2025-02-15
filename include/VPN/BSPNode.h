#ifndef __VPN_BSP_NODE_H__
#define __VPN_BSP_NODE_H__

#include <memory>
#include <vector>

#include <opencv2/opencv.hpp>

#include "Shape/Edge2D.h"
#include "IO/io.h"

namespace ViewPointNetwork
{
	class BSPNode
	{
	public:
		BSPNode();
		BSPNode(const Edge2D&);
		BSPNode(Edge2D&);
		~BSPNode() {};

		const Edge2D& getEdge() const;
		void setName(std::string);
		std::string getName();
		double getminScan() { return m_minScan; };

		BSPNode* frontNode;
		BSPNode* backNode;
	private:
		std::string m_name; // Traversal order ffb fbb fbbf fbbfb and so on
		Edge2D m_edge;
		double m_minScan;
		double m_minwall; 

	};
	BSPNode* buildBspTree(const std::vector<Edge2D>& edage, int partition, const std::string& name = "r");
	static void deleteNode(BSPNode* node) 
	{
		if (node) {
			deleteNode(node->frontNode);
			deleteNode(node->backNode);
			delete node;
			node = nullptr;
		}
	}
}
#endif
