#pragma once
#include <random>
#include <vector>
#include <opencv2/opencv.hpp>

#include "VPN/Station.h"
#include "VPN/House.h"
#include "VPN/HeatMap.h"

#include "Skeleton/SkeletonGraph.h"

namespace ViewPointNetwork
{
	void drawHouseLines(cv::Mat& figure, const House& house, double cell);
	void drawSkeletonGraph(cv::Mat& figure, const SkeletonGraph& sg);
	void drawStation(cv::Mat& figure, const Station& station, double cell,
		std::string text, cv::Scalar circleScalar, cv::Scalar textScalar = cv::Scalar(0), 
		int textThickness = 1, int radius = 5);
	void drawStations(cv::Mat& figure, const SkeletonGraph& sg,
		const std::vector<Station>& iStations, double cell, int radius);
}