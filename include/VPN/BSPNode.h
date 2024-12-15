#ifndef __VPN_BSP_NODE_H__
#define __VPN_BSP_NODE_H__

#include <memory>
#include <vector>

#include <opencv2/opencv.hpp>

#include "Edge2D.h"


namespace ViewPointNetwork
{
	class BSPNode
	{
	public:
		BSPNode();
		BSPNode(const Edge2D&);
		~BSPNode() {};

		const Edge2D& getEdge() const;
		void setName(std::string);
		std::string getName();
		double getminScan() { return m_minScan; };

		std::shared_ptr<BSPNode> frontNode;
		std::shared_ptr<BSPNode> backNode;
	private:
		std::string m_name; //遍历顺序 f fb fbb fbbf fbbfb 以此累加
		Edge2D m_edge;
		double m_minScan;
		double m_minwall; 

	};
	std::shared_ptr<BSPNode> buildBspTree(const std::vector<Edge2D>& edage, int partition, const std::string& name = "r");

}
#endif
