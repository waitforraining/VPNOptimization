#ifndef __VPN_STATION_NET_H__
#define __VPN_STATION_NET_H__
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <iostream>
#include <iomanip>  
#include <fstream>

#include "Station.h"

namespace ViewPointNetwork
{
	class StationNet
	{
	public:
		StationNet();
		StationNet(std::vector<Station>&& stations,double minOverlapPercent = 0.3);
		StationNet(std::vector<Station>& stations,double minOverlapPercent = 0.3);
		~StationNet() {};

	public:
		int getStationsNum() const;

		const std::vector<Station>& getStations() const;
		const Station& getStation(int i) const;
		
		std::vector<std::vector<int>> getConnectMat();
		std::vector<std::vector<double>> getOverlapMat() { return m_overlapMat; }


		double getOverlapMat(int, int);
		double getOverlapMatOF() { return m_overlapOF; }
		double getenergy() { return m_energy; }
		double getCoveragePercent() { return m_coveragePercent; }

		void computeMat();
		void addStation(const Station&);
		void printmat();
		void printmat(const std::string& fileName);
		void writeStationOverlapMatrix(const std::string& matrixPath) const;
		void writeStationScannedEdges() const;
		void writeStationWeightOverlap(const std::string& outputPath, double overlapThre);

	private:
		void resizeVec();

	private:
		int m_stationSize;
		int m_capacity;
		std::vector<Station> m_stations, m_delList;
		std::vector<std::vector<bool>> m_adjacentMat, m_conMat;
		std::vector<std::vector<double>> m_overlapMat;
		double m_minOverlapPercent, m_maxOverlapPercent, m_coveragePercent, m_energy, m_overlapOF;
	};
	double writeOverlapPercentByLen(std::ofstream& scoreFile, const Station& s0, const Station& s1);

}
#endif