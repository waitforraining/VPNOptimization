#ifndef __VPN_STATION_H__
#define __VPN_STATION_H__

#include <string>
#include <vector>
#include <memory>
#include <list>
#include <map>
#include <iomanip>  

#include "Point2D.h"
#include "Edge2D.h"
#include "BSPNode.h"
#include "VPNTypeDefinition.h"
#include "Vector2D.h"
#include "io.h"


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
		void scan(BSPNode* bspNode);
		void SetRadiusMin(double r) { m_rMin = r; }
		void SetRadiusMax(double r) { m_rMax = r; }
		double GetRadiusMin() const { return m_rMin; }
		double GetRadiusMax() const { return m_rMax; }

		bool operator ==(const Station& s) const;
	public:
		std::vector<std::vector<double>> getEdgeAngle(const Edge2D&) const;
		const std::map<std::vector<double>, Edge2D>& getScanedEdges() const;

		double getOverlapPercent(const Station& s, OverlapScoreType ost = AVERAGE_LEN) const;//old one
		std::vector<double> computeOverlap(const Station& s) const;//new one
		std::vector<double> getDist2Edge() const;
		Point2D getPosition() const { return Point2D(X(), Y()); }
		void initial();
		void traversal(BSPNode* root);

		double getScanLen() const;
		double getScanedScore() const;

		void writeScannedEdges(const std::string& prefix = "") const;
		void writeInResolutionEdges(const std::string& prefix, std::vector<Edge2D> denseEdges, double minScanAngle);
		std::vector<Edge2D> constructStationCircle() const;
	private:
		
		Point2D getPoint(const Edge2D& edge, double theta, double r1) const;
		Point2D getPoint(const Edge2D& edge, double theta) const;
		void scanEdge(const Edge2D&);
		void computeScanAngle();
		void computeScanLength();
		void mergeOnlineScanEdge();

	public:
		double getScore(const Edge2D& e) const;

		bool isInterArea(const Edge2D& e,double r) const;
		bool isInValidArea(const Edge2D& s) const;
		bool isInValidResolutionArea(const Edge2D& s,double minPointDist) const;

	private:
		double m_scanedLen, m_scanedScore, m_smallestAng, m_maxOverlap;
		std::vector<double> m_dist2Edge;
		std::map<std::vector<double>, Edge2D> m_edgeScanned;
		bool m_scanedFin;
		double m_rMin; //scan radius
		double m_rMax;
		double m_maxScanedAngle;
	};

}
#endif