#ifndef __VPN_OPTIMIZER_BY_SKELETION__
#define __VPN_OPTIMIZER_BY_SKELETION__

#include <vector>
#include <unordered_set>
#include <map>
#include <boost/property_tree/xml_parser.hpp>

#include "Edge2D.h"
#include "SkeletonGraph.h"
#include "Station.h"
#include "House.h"
#include "StationNet.h"
#include "BSPNode.h"
#include "StationNet.h"
#include "HeatMap.h"
#include "RidgeFilter.h"
#include "io.h"
#include "Draw/Draw.h"

namespace ViewPointNetwork
{
	struct OptSkelParam
	{
	public:
		OptSkelParam();
		OptSkelParam(HouseType hType, double inCell,
			double stationRadMin, double stationRadMax,
			double ovlpThresh, double dnsStep);
		std::string& toString(const std::string& prefix = "", const std::string& format = "tcrRlsd") const;
	public:
		HouseType houseType;
		double cell;
		double stationRadiusMin;
		double stationRadiusMax;
		double overlapThresh;
		double scoreThresh;
		double denseStep;//for densify edges
	};

	class VpnOptimizerBySkeleton
	{
	public:
		VpnOptimizerBySkeleton();
		VpnOptimizerBySkeleton(const std::vector<Edge2D>& vecHouseEdges, const OptSkelParam& param = OptSkelParam());
		VpnOptimizerBySkeleton(const std::string& houseName, const OptSkelParam& param = OptSkelParam());
		~VpnOptimizerBySkeleton();

	public:
		void doOptimize();
		void setHouseName(const std::string& name) { m_houseName = name; }
		std::string getHouseName() const { return m_houseName; }
		
		void setParameter(const OptSkelParam& param) { m_param = param; }
		OptSkelParam getParameter() { return m_param; }
		
		void writeStation2HeilosXml(const std::string& resultFile, const std::string& scenePath,
			const std::string& platformPath, const std::string& scanner, const std::string& surveyName = "test");
		void writeStation2Txt(const std::string& outputTxtPath);


	private:
		void initialize();
		void getScanGroups();
		void initializeHouse();
		void initializeHeatMap();
		void initializeSklGraph();
		void initializeNet();
		void initializeVec();

		void groupConnect(std::set<int>& remainStations);
		void addStationForConnecting();
		void computePathsAndMat();
		void updateScanMat(int maxScanInd);

		void drawWholeHouse(const std::string& outresultpath) const;

		void closeNet();
		void getMaxScanEdgeCount(std::set<int>& stationId, std::vector<int>& maxScanStationId, int& maxScanCnt);
		void updateAdjcentStation(int curInd, std::set<int>& remainStations,
			std::set<int>& adjStations);

		double getMaxOverlap(int curInd);
		double getGroupOverlapMaxStation(int group0, std::array<int, 2>& maxOverlapInd);
		int selectBestStation(const std::vector<int>& maxScanInds);

		bool isIsolate(int ind);
	private:
		std::string m_houseName;
		OptSkelParam m_param;
		House m_house;
		HeatMap m_heatMap;
		StationNet m_stationNet;
		SkeletonGraph m_sklGraph;

		std::vector<Edge2D> m_vecEdges;
		std::vector<Edge2D> m_denseEdges;
		std::vector<Edge2D> m_offestVecEdges;//如果室外扫描，偏移量不为0

		std::vector<Station> m_stations;
		std::unordered_map<int, std::vector<int>> m_groupStationUMap;
		std::vector<int> m_usedStationInds;

		std::vector<bool> m_edgeScanned;
		std::vector<std::vector<bool>> m_scanMat;//row: stations, col: edges
		std::vector<std::vector<bool>> m_conMat;
		std::vector<std::vector<std::vector<int>>> m_stationPaths;
		
		int m_unScanedCnt;
		int m_groupInd;
	};
}

#endif