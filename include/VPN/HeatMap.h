#ifndef __VPN_HEAT_MAP_H__
#define __VPN_HEAT_MAP_H__

#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

#include "HeatMap.h"

#include "Point2D.h"
#include "Edge2D.h"
#include "House.h"
#include "Station.h"
#include "BSPNode.h"
#include "VPNTypeDefinition.h"
#include "SkeletonGraph.h"

namespace ViewPointNetwork
{
	class HeatMap
	{
	public:
		HeatMap();
		HeatMap(int height, int width, double cell);
		HeatMap(const House& house, double cell);
		~HeatMap() {};

		double getCell() const { return m_cell; }
		void setCell(double c) { m_cell = c; }
		int getXLen() const { return m_width; }
		int setXLen(int xLen) { m_width = xLen; }
		int getYLen() const { return m_height; }
		int setYLen(int yLen) { m_height = yLen; }

		double getValue(int indx, int indy) const;
		void setValue(int indx, int indy, double v);
		double getValue(double x, double y) const;
		void setValue(double x, double y, double v);
		double getValue(const Point2D& p) const;
		void setValue(const Point2D& p, double v);

		void generate(House& house, double r_min, double r_max, double scoreThresh = 1.0);
		void generateOneEdgeHeat(House& house, Edge2D& edge, double r_min,double r_max);
		cv::Mat HeatMap::getGRAYImg();
		
		void normalization(House, std::vector<std::vector<int>>);

		void show(House&, std::string path, std::string  name, bool show = true, bool station = false, 
			int num = 0, bool merge = false, int num1 = 0, int num2 = 0, 
			std::vector<BSPNode> tmpEdge = std::vector<BSPNode>(), bool lineshow = false);

		void saveBGR(House& house, const std::string& name, double scalar, bool station = false) const;
		void saveBGR1(const House& house, const std::vector<cv::Vec3b>& colors, 
			const std::string& name,
			double scalar, bool station = false);
		void saveBGR_YellowOrange(const House& house,  const std::string& name, 
			bool station = false);
		void saveGRAY(House& house, std::string name, bool station = false);

	private:
		double m_x, m_y, m_cell;
		std::vector<std::vector<double>> m_value;
		int m_width, m_height;
	};

	HeatMap computeHeatMap(const std::vector<Edge2D>& vecEdges, const std::string& outPath, 
		double cell = 0.1, double r_min = 0.8, double r_max = 30.0);
	
	void drawEdges(cv::Mat& figure, HeatMap& heat, const Edge2D& edge, double cell, const cv::Scalar& color,
		int thickness = 1, int lineType = cv::LINE_8, int shift = 0, HouseType houseType = HT_Indoor);

	void drawEdges(cv::Mat& figure, HeatMap& heat, const std::vector<Edge2D>& vecEdges, double cell, const cv::Scalar& color,
		int thickness = 1, int lineType = cv::LINE_8, int shift = 0, HouseType houseType = HT_Indoor);

	void drawEdges(cv::Mat& figure, HeatMap& heat, const std::vector<BSPNode>& vecNodes, double cell, const cv::Scalar& color,
		int thickness = 1, int lineType = cv::LINE_8, int shift = 0, HouseType houseType = HT_Indoor);

	void drawEdges(cv::Mat& figure,const Edge2D& edge, double cell, const cv::Scalar& color,
		int thickness = 1, int lineType = cv::LINE_8, int shift = 0);

	void drawEdges(cv::Mat& figure, const std::vector<Edge2D>& vecEdges, double cell, const cv::Scalar& color,
		int thickness = 1, int lineType = cv::LINE_8, int shift = 0);

	void drawEdges(cv::Mat& figure, const std::vector<BSPNode>& vecNodes, double cell, const cv::Scalar& color,
		int thickness = 1, int lineType = cv::LINE_8, int shift = 0);

	void drawStation(cv::Mat& figure, const std::vector<Station>& vecStations, double cell,
		const cv::Scalar& color, int radius, int thickness = 1, int lineType = cv::LINE_8, int shift = 0);

	void drawHouseLines(cv::Mat& figure, const House& house, double cell);
	void showHeatBGR(cv::Mat& figure, const HeatMap& heatMap);
	void showSkeleton(cv::Mat& figure, std::vector<int> stations, std::vector<std::vector<int>> skeletonConMat,
		std::unordered_map<int, std::pair<int, int>> crossInd2Point);
	void showAddEdges(cv::Mat& figure, House& house, std::vector<Edge2D> lineSegs,
		std::vector<int> addEdgesId, double cell, cv::Scalar color);

	void drawStations(cv::Mat& figure, const SkeletonGraph& sg, const std::vector<Station>& iStations, 
		double cell, int radius);
}
#endif