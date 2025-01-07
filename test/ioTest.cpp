#include <gtest/gtest.h>
#include "IO/io.h"

using namespace ViewPointNetwork;
using namespace std;
//bsp搜索功能 测试两个数据 用固定参数，固定站点测试bsp扫描的边数长度是否一致 
//生成热力图 固定输入平面图 与之前代码中热力图矩阵中的值进行比较
//骨架线
//站点位置
std::vector<Edge2D> ConstructRectangle()
{
	//构造测试重叠度的数据
	std::vector<Edge2D> edges;
	edges.emplace_back(Point2D(0, 0), Point2D(0, 1));
	edges.emplace_back(Point2D(0, 1), Point2D(1, 1));
	edges.emplace_back(Point2D(1, 1), Point2D(1, 0));
	edges.emplace_back(Point2D(1, 0), Point2D(0, 0));

	return edges;
}

TEST(ReadTest, readTop) 
{
	std::vector<Edge2D> edges;
	readEdges("../data/noichl/noichl", edges, "top");
	for (const auto& edge : edges)
		ASSERT_FLOAT_EQ(edge.Length(), 0.0, 1e-8);
	ASSERT_TRUE(edges.size() > 0);
}

TEST(ReadTest, readDxf)
{

	std::vector<Edge2D> edges;
	readEdges("../data/noichl/noichl", edges, "dxf");
	for (const auto& edge : edges)
		ASSERT_FLOAT_EQ(edge.Length(), 0.0, 1e-8);
	ASSERT_TRUE(edges.size() > 0);
}

TEST(WriteTest, writeTop)
{
	std::vector<Edge2D> edges = ConstructRectangle();
	writeEdges("test", edges, "top");
	std::vector<Edge2D> rEdges;
	readEdges("test", rEdges, "top");
	ASSERT_EQ(edges.size(), rEdges.size());
	for (int i = 0; i < edges.size(); i++)
	{
		ASSERT_FLOAT_EQ(edges[i].getBegPoint().X(), rEdges[i].getBegPoint().X(), 1e-9);
		ASSERT_FLOAT_EQ(edges[i].getEndPoint().X(), rEdges[i].getEndPoint().X(), 1e-9);
	}
}

TEST(WriteTest, writeDxf)
{
	std::vector<Edge2D> edges = ConstructRectangle();
	writeEdges("test", edges, "dxf");
	std::vector<Edge2D> rEdges;
	readEdges("test", rEdges, "dxf");
	ASSERT_EQ(edges.size(), rEdges.size());
	for (int i = 0; i < edges.size(); i++)
	{
		ASSERT_FLOAT_EQ(edges[i].getBegPoint().X(), rEdges[i].getBegPoint().X(), 1e-9);
		ASSERT_FLOAT_EQ(edges[i].getEndPoint().X(), rEdges[i].getEndPoint().X(), 1e-9);
	}
}
