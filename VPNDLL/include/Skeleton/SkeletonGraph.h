#pragma once
#include <iostream>
#include <unordered_map>
#include <stack>
#include <random>
#include <functional>
#include <set>
#include <iomanip>  

#include <opencv2/opencv.hpp>

#include "preCompiled.h"

#include "Shape/Edge2D.h"

#include "VPN/House.h"
#include "VPN/Station.h"
#include "VPN/BSPNode.h"
#include "VPN/VPNTypeDefinition.h"

#include "Skeleton/SkeletonJoint.h"

namespace ViewPointNetwork
{
	struct pair_hash {
		std::size_t operator () (const SkeletonJoint& sj) const {
			std::hash<int> int_hash;

			return int_hash(sj.X()) ^ int_hash(sj.Y());
		}
	};

	class VPN_API SkeletonGraph
	{
	public:
		SkeletonGraph() {};
		SkeletonGraph(const cv::Mat& iSkeleton) :m_skeleton(iSkeleton) {};
		SkeletonGraph& operator=(const SkeletonGraph& other) {
			if (this != &other) {  // Prevents self-assignment
				m_skeleton = other.m_skeleton;
			}
			return *this;
		}
		SkeletonGraph(const SkeletonGraph& other) {
			m_skeleton = other.m_skeleton;
		}
		~SkeletonGraph() {};

		cv::Mat getSkeleton() { return m_skeleton; }
		int getJointNum() { return m_vaildJointNum; }
		int getMatRow() { return m_skeleton.rows; }
		int getMatCol() { return m_skeleton.cols; }
		std::unordered_map<int, SkeletonJoint>& getCrossInd2Point() { return crossInd2Point; };

		void writeCrossPnt();
		void getOneColor();
		void setGreenColor();
		void initColor();
		void setConnectColor();
		void clearColor();
		void initMidCross();

		cv::Mat getSkeleton() const;

		void getMidCrossPath();
		void getSkeletonGraph();
		bool addNearbyCrossPoints(double threshold);
		void addNewStations(double threshold);
		double pointLineDistance(const SkeletonJoint& point,
			const SkeletonJoint& lineStart, const SkeletonJoint& lineEnd);
		cv::Mat creatRidge(const std::string& grayImgPath, const std::string& outresultpath);
		bool isIsolate(int ind) const;
		void resizeVec();
		void updatePathsByNewJoint(int i, int j,int maxDisInd,SkeletonJoint& newJoint);
		void expandPath(int i);
		void expandPaths();
		const std::vector<SkeletonJoint>& getPath(int i, int j);
		
		const std::vector<std::vector<std::vector<SkeletonJoint>>>& getPaths() { return paths; }
	private:
		bool isCrossPnt(int x, int y) const;
		bool isMidCrossPnt(int x, int y) const;
		void getMidCrossPnt(int x, int y, int& midX, int& midY) const;
		void computeCrossPntConnect(cv::Mat& visited, const SkeletonJoint& midCrossPnt);

	public:
		int m_vaildJointNum;
		std::vector<std::vector<std::vector<SkeletonJoint>>> paths;
		std::unordered_map<SkeletonJoint, int, pair_hash> point2CrossInd;
		std::unordered_map<int, SkeletonJoint> crossInd2Point;
		std::vector<std::vector<int>> connectMat;
		std::vector<std::vector<int>> adjMat;
		std::vector<cv::Scalar> stationColor;
		std::vector<int> color_type; //0 indicates the green initial site, 
		//1 indicates the farthest point, 
		//and 2 indicates the point where overlap is guaranteed
		
		// In the optimization diagram, 
		//-1 represents the connection point and others represent points in the corresponding cluster index number
		std::vector<Station> stations;

	private:
		cv::Mat m_skeleton;
	};

}