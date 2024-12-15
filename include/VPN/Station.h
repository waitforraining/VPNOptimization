#ifndef __VPN_STATION_H__
#define __VPN_STATION_H__

#include <string>
#include <vector>
#include <memory>
#include <list>
#include <map>

#include "BSPNode.h"
#include "Edge2D.h"
#include "Vector2D.h"
#include "Point2D.h"
#include "VPNTypeDefinition.h"

namespace ViewPointNetwork
{
	class Station :public Point2D
	{
	public:
		Station();
		Station(double inx, double iny, double r_min = 0.8, double r_max = 30.0);
		Station(const Point2D& p, double r_min = 0.8, double r_max = 30.0);
		~Station() {};

	public:
		void scan(std::shared_ptr<BSPNode> bspNode);
		void SetRadiusMin(double r) { m_r1 = r; }
		void SetRadiusMax(double r) { m_r2 = r; }
		double GetRadiusMin() const { return m_r1; }
		double GetRadiusMax() const { return m_r2; }

		bool operator ==(const Station& s) const;
	public:
		std::vector<std::vector<double>> getEdgeAngle(const Edge2D&) const;
		std::map<std::vector<double>, Edge2D> getScanedEdges() const;
		double getOverlapPercent(const Station& s, OverlapScoreType ost = AVERAGE_LEN) const;//old one
		std::vector<double> computeOverlap(const Station& s) const;//new one
		
		void addEdgeDist(double);
		double getEdgeDist(const Edge2D&, bool real = false) const;
		std::vector<double> getDist2Edge() const;
		Point2D getPosition() const { return Point2D(X(), Y()); }
		void initial();
		void traversal(std::shared_ptr<BSPNode> root);

		double getScanLen() const;
		double getScanedScore() const;
		double getSmallestAng() const;

		bool wirteScannedEdges(const std::string& prefix = "") const;

	private:
		std::vector<Edge2D> constructStationCircle() const;
		Point2D getPoint(const Edge2D& edge, double theta, double r1) const;
		Point2D getPoint(const Edge2D& edge, double theta) const;
		void scanEdge(const Edge2D&);
		void computeScanAngle();
		void computeScanLength();
		void mergeOnlineScanEdge();

	private:
		double m_scanedLen, m_scanedScore, m_smallestAng, m_maxOverlap;
		std::vector<double> m_dist2Edge;
		std::map<std::vector<double>, Edge2D> m_edgeScanned;
		bool m_scanedFin;
		double m_r1; //scan radius
		double m_r2;
		double m_maxScanedAngle;
	};

}
#endif