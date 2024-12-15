#pragma once
#include <string>

#include "VPNTypeDefinition.h"
#include "House.h"
#include "HeatMap.h"
#include "RidgeFilter.h"
#include "SkeletonGraph.h"

#include <opencv2/core.hpp>

namespace ViewPointNetwork
{
	class RidgeDetection
	{
	public:
		RidgeDetection(int ddepth = CV_32FC1, int dx = 1, int dy = 1, int ksize = 3, int out_dtype = CV_8UC1,
			double scale = 1, double delta = 0, int borderType = cv::BorderTypes::BORDER_DEFAULT);

		//利用opencv绘制骨架线图像
		// 处理图像并提取骨架
		cv::Mat getRidge(House& house, HeatMap& heat,
			cv::Mat image, const std::string& outresultpath,
			int distType, int markSize, int HouseType = HT_Indoor);
		cv::Mat getRidge(cv::Mat image, const std::string& outresultpath,
			int distType, int markSize, int HouseType = HT_Indoor);
		cv::Mat creatRidge(House& house, HeatMap& heat,
			const std::string& grayImgPath, const std::string& outresultpath, int houseType = HT_Indoor);
	private:
		void getRoughRidge(cv::InputArray _img, cv::OutputArray out);

	public:
		int _ddepth, _dx, _dy, _ksize;
		double _scale, _delta;
		int _borderType;
		int _out_dtype;
	};
	cv::Mat fastSkeletonize(cv::Mat image);
	void showImage(cv::Mat image, const std::string& windowName);

}