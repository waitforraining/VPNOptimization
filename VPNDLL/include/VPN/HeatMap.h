#ifndef __VPN_HEAT_MAP_H__
#define __VPN_HEAT_MAP_H__

#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <iomanip>  

#include "preCompiled.h"

#include "Shape/Point2D.h"
#include "Shape/Edge2D.h"

#include "VPN/House.h"
#include "VPN/Station.h"
#include "VPN/BSPNode.h"
#include "VPN/VPNTypeDefinition.h"

#include "Skeleton/SkeletonGraph.h"


namespace ViewPointNetwork
{
	class VPN_API HeatMap
	{
	public:
		HeatMap();
		HeatMap(int height, int width, double cell);
		HeatMap(const House& house, double cell);
		~HeatMap() {};

		double getCell() const { return m_cell; }
		double getValue(int indx, int indy) const;
		double getValue(double x, double y) const;
		double getValue(const Point2D& p) const;

		int getXLen() const { return m_width; }
		int getYLen() const { return m_height; }

		void setCell(double c) { m_cell = c; }
		void setXLen(int xLen) { m_width = xLen; }
		void setYLen(int yLen) { m_height = yLen; }
		void setValue(int indx, int indy, double v);
		void setValue(double x, double y, double v);
		void setValue(const Point2D& p, double v);

		bool loadGrayImg(const std::string& grayMapPath);
		void generate(House& house, double r_min, double r_max, double scoreThresh = 1.0);
		void generateOneEdgeHeat(House& house, Edge2D& edge, double r_min,double r_max);
		cv::Mat getGRAYImg();
		cv::Mat getHeatImg(const House& house);

		void normalization(House, std::vector<std::vector<int>>);

		void show(House&, std::string path, std::string  name, bool show = true, bool station = false, 
			int num = 0, bool merge = false, int num1 = 0, int num2 = 0, 
			std::vector<BSPNode> tmpEdge = std::vector<BSPNode>(), bool lineshow = false);

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

	void drawStations(cv::Mat& figure, const std::vector<Station>& vecStations, double cell,
		const cv::Scalar& color, int radius, int thickness = 1, int lineType = cv::LINE_8, int shift = 0);

	void drawHouseLines(cv::Mat& figure, const House& house, double cell);
	void showHeatBGR(cv::Mat& figure, const HeatMap& heatMap);
	void showSkeleton(cv::Mat& figure, std::vector<int> stations, std::vector<std::vector<int>> skeletonConMat,
		std::unordered_map<int, std::pair<int, int>> crossInd2Point);
	void showAddEdges(cv::Mat& figure, House& house, std::vector<Edge2D> lineSegs,
		std::vector<int> addEdgesId, double cell, cv::Scalar color);
}
#endif