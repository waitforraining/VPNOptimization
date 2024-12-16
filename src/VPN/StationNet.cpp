#define _USE_MATH_DEFINES

#include "StationNet.h"
using namespace std;

namespace ViewPointNetwork
{
	StationNet::StationNet()
	{
		m_energy = 0;
		m_minOverlapPercent = 0.3;
	}

	StationNet::StationNet(vector<Station>&& stations,double minOverlapPercent)
	{
		m_energy = 0;
		m_minOverlapPercent = minOverlapPercent;

		m_stations = stations;
		m_stationSize = m_stations.size();
		m_capacity = m_stationSize;
		resizeVec();
	}

	StationNet::StationNet(vector<Station>& stations, double minOverlapPercent)
	{
		m_energy = 0;
		m_minOverlapPercent = minOverlapPercent;

		m_stations = stations;
		m_stationSize = m_stations.size();
		m_capacity = m_stationSize;
		resizeVec();
	}

	int StationNet::getStationsNum() const
	{
		return m_stations.size();
	}


	void StationNet::computeMat()
	{
		//邻接矩阵  需要对称性的overlapPercent
		double overlapPercent;
		for (size_t i = 0; i < m_stationSize; i++) 
		{
			const Station& s1 = m_stations[i];
			m_overlapMat[i][i] = 1;
			for (size_t j = i + 1; j < m_stationSize; j++) 
			{
				
				const Station& s2 = m_stations[j];
				overlapPercent = s1.getOverlapPercent(s2);

				m_overlapMat[i][j] = overlapPercent;
				m_overlapMat[j][i] = overlapPercent;
				
				m_adjacentMat[i][j] = overlapPercent >= m_minOverlapPercent;
				m_adjacentMat[j][i] = overlapPercent >= m_minOverlapPercent;
			}
		}

		//连通矩阵
		m_conMat = m_adjacentMat;
		vector<bool> vis(m_stationSize);
		for (size_t i = 0; i < m_stationSize; ++i)
		{
			if (vis[i]) continue;
			queue<int> q;
			q.push(i);
			vector<int> connectInd;
			while (!q.empty())
			{
				auto m = q.front();
				q.pop();
				connectInd.push_back(m);
				vis[m] = true;
				for (size_t j = 0; j < m_stationSize; ++j)
				{
					if (vis[j] || !m_conMat[m][j]) continue;
					q.push(j);
				}
			}

			for (auto ind0 : connectInd)
			{
				for (auto ind1 : connectInd)
				{
					m_conMat[ind0][ind1] = 1;
					m_conMat[ind1][ind0] = 1;
				}
			}
		}
	}

	const vector<Station>& StationNet::getStations() const 
	{ 
		return m_stations;
	};
	const Station& StationNet::getStation(int i) const
	{
		return m_stations[i];
	};
	void StationNet::resizeVec()
	{
		m_capacity *= 2;
		m_overlapMat.resize(m_capacity);
		for (int i = 0; i < m_overlapMat.size(); ++i)
		{
			m_overlapMat[i].resize(m_capacity);
		}
		m_adjacentMat.resize(m_capacity);
		for (int i = 0; i < m_adjacentMat.size(); ++i)
		{
			m_adjacentMat[i].resize(m_capacity);
		}
		m_conMat.resize(m_capacity);
		for (int i = 0; i < m_conMat.size(); ++i)
		{
			m_conMat[i].resize(m_capacity);
		}
	}


	void StationNet::addStation(const Station & s0) {

		int addStationInd = m_stationSize;
		m_stations.push_back(s0);
		m_stationSize++;
		if (m_stationSize >= m_capacity)
			resizeVec();

		double overlapPercent;
		for (size_t i = 0; i < addStationInd; ++i)
		{
			const Station& s = m_stations[i];
			overlapPercent = s0.getOverlapPercent(s);
			//对称
			m_overlapMat[addStationInd][i] = overlapPercent;
			m_overlapMat[i][addStationInd] = overlapPercent;

			if (overlapPercent >= m_minOverlapPercent)
			{
				// 临接矩阵
				m_adjacentMat[addStationInd][i] = true;
				m_adjacentMat[i][addStationInd] = true;
			}
			else
			{
				//更新连通矩阵
				for (size_t j = 0; j < addStationInd; ++j) {
					if (i == j) continue;
					if (m_conMat[addStationInd][j] && m_conMat[j][i]) {
						m_conMat[addStationInd][i] = true;
						m_conMat[i][addStationInd] = true;
					}
				}
			}
		}

		m_overlapMat[addStationInd][addStationInd] = 1.0;
		m_adjacentMat[addStationInd][addStationInd] = true;
		m_conMat[addStationInd][addStationInd] = true;

	}

	double StationNet::getOverlapMat(int i, int j)
	{
		if (i < 0 || j < 0 || i >= m_overlapMat.size() || j >= m_overlapMat.size()) return 0.0;
		return m_overlapMat[i][j];
	}

	void StationNet::printmat()
	{
		cout << "conMat" << endl;
		for (int i = 0; i < m_stations.size(); i++)
		{
			for (int j = 0; j < m_stations.size(); j++)
			{
				cout << m_conMat[i][j] << " ";
			}
			cout << endl;
		}
		cout << "overlap" << endl;
		for (int i = 0; i < m_stations.size(); i++)
		{
			for (int j = 0; j < m_stations.size(); j++)
			{
				cout << m_overlapMat[i][j] << " ";
			}
			cout << "--" << endl;
		}

	}

	vector<vector<int>> StationNet::getConnectMat()
	{
		vector<vector<int>> connectMat(m_stations.size(), vector<int>(m_stations.size(), false));
		for (int i = 0; i < m_stations.size(); i++)
		{
			for (int j = 0; j < m_stations.size(); j++)
			{
				connectMat[i][j] = getOverlapMat(i, j) > 0.3;
			}
		}

		return connectMat;
	}


	void StationNet::printmat(const std::string& filename)
	{
		std::ofstream outFile(filename);

		if (!outFile.is_open()) {
			std::cerr << "Error opening file: " << filename << std::endl;
			return;
		}

		const size_t stationSize = m_stations.size();

		// Print conMat header
		outFile << "conMat" << std::endl;
		outFile << ",";
		for (size_t j = 0; j < stationSize; ++j) {
			outFile << j + 1;
			if (j < stationSize - 1) {
				outFile << ",";
			}
		}
		outFile << std::endl;

		for (size_t i = 0; i < stationSize; ++i) {
			outFile << i + 1 << ",";
			for (size_t j = 0; j < stationSize; ++j) {
				outFile << m_conMat[i][j];
				if (j < stationSize - 1) {
					outFile << ",";
				}
			}
			outFile << std::endl;
		}

		// Print overlap header
		outFile << "overlap" << std::endl;
		outFile << ",";
		for (size_t j = 0; j < stationSize; ++j) {
			outFile << j + 1;
			if (j < stationSize - 1) {
				outFile << ",";
			}
		}
		outFile << std::endl;

		for (size_t i = 0; i < stationSize; ++i) {
			outFile << i + 1 << ",";
			for (size_t j = 0; j < stationSize; ++j) {
				outFile << std::fixed << std::setprecision(3) << m_overlapMat[i][j];
				if (j < stationSize - 1) {
					outFile << ",";
				}
			}
			outFile << std::endl;
		}

		outFile.close();
	}


	void writeScoreMatrix(ofstream& ofs, const std::vector<std::vector<double>>& scoreMatrix)
	{
		for (size_t i = 0; i < scoreMatrix.size(); i++)
			ofs << " ," << i;
		ofs << std::endl;

		for (size_t i = 0; i < scoreMatrix.size(); i++) {
			ofs << i << " ,";

			for (size_t j = 0; j < scoreMatrix[i].size(); j++)
				ofs << scoreMatrix[i][j] << " ,";

			ofs << std::endl;
		}
	}


	void StationNet::writeStationOverlapMatrix(const std::string& matrixPath) const
	{
		ofstream ofs(matrixPath);
		ofs << std::fixed << std::setprecision(3);

		//////////////////////////////////////////////////////////////////////////
		//station position
		ofs << "Station Positions: \n";
		for (size_t i = 0; i < m_stations.size(); i++)
			ofs << i << "," << m_stations[i].X() << "," << m_stations[i].Y() << std::endl;
		ofs << std::endl;

		//////////////////////////////////////////////////////////////////////////
		std::vector<std::vector<double>> score_average_len(m_stations.size(), std::vector<double>(m_stations.size(), 0.0));
		std::vector<std::vector<double>> score_minimum_len = score_average_len;
		std::vector<std::vector<double>> score_union_len = score_average_len;
		std::vector<std::vector<double>> score_inter_sum_ang = score_average_len;
		std::vector<std::vector<double>> score_intersect_ang = score_average_len;
		std::vector<double> overlaps;

		for (size_t i = 0; i < m_stations.size(); i++) {
			for (size_t j = i; j < m_stations.size(); j++) {
				overlaps = m_stations[i].computeOverlap(m_stations[j]);
				score_average_len[i][j] = overlaps[0];
				score_minimum_len[i][j] = overlaps[1];
				score_union_len[i][j] = overlaps[2];
				score_inter_sum_ang[i][j] = overlaps[3];
				score_intersect_ang[i][j] = overlaps[4];
			}

			for (size_t j = 0; j < i; j++) {
				score_average_len[i][j] = score_average_len[j][i];
				score_minimum_len[i][j] = score_minimum_len[j][i];
				score_union_len[i][j] = score_union_len[j][i];
				score_inter_sum_ang[i][j] = score_inter_sum_ang[j][i];
				score_intersect_ang[i][j] = score_intersect_ang[j][i];
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//overlap
		ofs << "\n score_average_len: \n";
		writeScoreMatrix(ofs, score_average_len);
		ofs << "\n score_minimum_len: \n";
		writeScoreMatrix(ofs, score_minimum_len);
		ofs << "\n score_union_len: \n";
		writeScoreMatrix(ofs, score_union_len);
		ofs << "\n score_inter_sum_ang: \n";
		writeScoreMatrix(ofs, score_inter_sum_ang);
		ofs << "\n score_intersect_ang: \n";
		writeScoreMatrix(ofs, score_intersect_ang);
		ofs.close();

	}

	void StationNet::writeStationScannedEdges() const
	{
		for (size_t i = 0; i < m_stations.size(); i++) {
			std::stringstream ss;
			ss << std::setw(2) << std::setfill('0') << i << "_";

			m_stations[i].wirteScannedEdges(ss.str());
		}

	}

	void StationNet::writeStationWeightOverlap(const std::string& outputPath, double overlapThre)
	{
		int n = m_stations.size();

		vector<vector<double>> dist(n, vector<double>(n, 100));
		double overlapPercent;
		ofstream ofs(outputPath);

		for (size_t i = 0; i < n; i++) {
			const Station& s1 = m_stations[i];
			for (size_t j = 0; j < n; j++) {
				if (i == j) continue;

				const Station& s2 = m_stations[j];
				overlapPercent = s1.getOverlapPercent(s2);
				if (overlapPercent >= overlapThre)
					dist[i][j] = 1 - overlapPercent;
			}
		}
		ofs << std::fixed << std::setprecision(3);
		ofs << "\n func(overlapPercent): \n";
		writeScoreMatrix(ofs, dist);

		for (int k = 0; k < n; ++k) {
			for (int i = 0; i < n; ++i) {
				for (int j = 0; j < n; ++j) {
					if (i == j) continue;
					if (dist[i][k] < INT_MAX && dist[k][j] < INT_MAX) {
						dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
					}
				}
			}
		}

		ofs << "\n shortPath: \n";
		writeScoreMatrix(ofs, dist);
		double weightAvgLen = 0.0;
		for (int i = 0; i < n; ++i)
			for (int j = i + 1; j < n; ++j)
				weightAvgLen += dist[i][j];

		weightAvgLen = weightAvgLen / n / (n - 1);
		ofs << std::fixed << std::setprecision(5);

		ofs << "\n weightAvgLen: \n";
		ofs << weightAvgLen << endl;

		ofs.close();
	}

	double writeOverlapPercentByLen(ofstream& ofsScores, const Station& s0, const Station& s1)
	{
		double len0 = s0.getScanLen();
		double ang0 = s0.getScanedScore();
		double len1 = s1.getScanLen();
		double ang1 = s1.getScanedScore();

		auto scanedEdges0 = s0.getScanedEdges();
		auto scanedEdges1 = s1.getScanedEdges();
		Edge2D overlapEdge;
		double overlapAng0 = 0.0, overlapAng1 = 0.0, overlapLen = 0.0;

		for (auto i = scanedEdges0.begin(); i != scanedEdges0.end(); i++) {
			const Edge2D& e0 = i->second;

			for (auto j = scanedEdges1.begin(); j != scanedEdges1.end(); j++) {
				const Edge2D& e1 = j->second;
				overlapEdge = e0.Overlap(e1);
				if (overlapEdge.Length() == 0.0) continue;

				overlapLen += overlapEdge.Length();
				overlapAng0 += s0.getScore(overlapEdge);
				overlapAng1 += s0.getScore(overlapEdge);
			}
		}

		double score_average_len = 2.0 * overlapLen / (len0 + len1);
		double score_minimum_len = overlapLen / min(len0, len1);
		double score_union_len = overlapLen / (len0 + len1 - overlapLen);
		double score_inter_sum_ang = (overlapAng0 + overlapAng1) / (ang0 + ang1);
		double score_intersect_ang = (overlapAng0 / ang0 + overlapAng1 / ang1) / 2.0;

		ofsScores << std::fixed << std::setprecision(3);
		ofsScores << s1.X() << ',' << s1.Y() << ","
			<< score_average_len << ','
			<< score_minimum_len << ','
			<< score_union_len << ','
			<< score_inter_sum_ang << ','
			<< score_intersect_ang << ','
			<< len0 << ',' << ang0 << ','
			<< len1 << ',' << ang1 << ','
			<< overlapLen << ',' << overlapAng0 << ',' << overlapAng1 << '\n';

		return 0.0;
	}
}

