#include "Skeleton/SkeletonGraph.h"

using namespace std;
using namespace cv;

namespace ViewPointNetwork
{

	bool SkeletonGraph::isCrossPnt(int x, int y) const
	{
		if (m_skeleton.at<uchar>(x, y) != 255) return false;
		int neighbors = 0;
		int nx, ny;

		for (int dx = -1; dx <= 1; dx++)
		{
			for (int dy = -1; dy <= 1; dy++)
			{
				if (dx == 0 && dy == 0) continue;
				nx = x + dx;
				ny = y + dy;
				if (nx >= 0 && ny >= 0 && nx < m_skeleton.rows && ny < m_skeleton.cols
					&& m_skeleton.at<uchar>(nx, ny) == 255)
					neighbors++;
			}
		}

		return neighbors >= 3;
	}

	// It is recommended to store all ridges
	void SkeletonGraph::getMidCrossPnt(int x, int y, int& midX, int& midY) const
	{
		double sumX = 0;
		double sumY = 0;
		int num = 0;
		stack<SkeletonJoint> stackCross;
		stackCross.emplace(x, y, JunctionType::JT_INIT);
		set<SkeletonJoint> setUsedCross;
		SkeletonJoint current;

		while (!stackCross.empty())
		{
			current = stackCross.top();
			stackCross.pop();
			
			//search 8 neighbors
			for (int dx = -1; dx <= 1; dx++)
			{
				for (int dy = -1; dy <= 1; dy++)
				{
					int nx = current.X() + dx;
					int ny = current.Y() + dy;
					if (nx >= 0 && ny >= 0 && nx < m_skeleton.rows && ny < m_skeleton.cols
						&& m_skeleton.at<uchar>(nx, ny) == 255)
					{
						if (setUsedCross.count(SkeletonJoint(nx, ny, JunctionType::JT_INIT))) continue;
						setUsedCross.emplace(nx, ny);

						if (isCrossPnt(nx, ny))
						{
							sumX += nx;
							sumY += ny;
							stackCross.emplace(nx, ny, JunctionType::JT_INIT);
							num++;
						}
					}
				}
			}
		}

		midX = sumX / num + 0.5;
		midY = sumY / num + 0.5;
	}

	bool SkeletonGraph::isMidCrossPnt(int x, int y) const
	{
		int midX, midY;
		if (!isCrossPnt(x, y)) return false;
		getMidCrossPnt(x, y, midX, midY);

		//if ( && (midX != x && midY != y)) return false;
		return x == midX && y == midY;
	}

	void SkeletonGraph::computeCrossPntConnect(Mat & visited, const SkeletonJoint & midCrossPnt)
	{
		stack<SkeletonJoint> stPnts;
		vector<SkeletonJoint> path;

		stPnts.emplace(midCrossPnt.X(), midCrossPnt.Y(), JunctionType::JT_INIT);

		int midX, midY;
		int currentCrossInd = point2CrossInd[midCrossPnt];

		while (!stPnts.empty())
		{
			SkeletonJoint current = stPnts.top();
			stPnts.pop();
			int cx = current.X();
			int cy = current.Y();
			if (!isCrossPnt(cx, cy))
				path.emplace_back(cx, cy, JunctionType::JT_INIT);
			bool pathFlag = true;
			visited.at<uchar>(cx, cy) = 1;

			for (int dx = -1; dx <= 1; dx++)
			{
				for (int dy = -1; dy <= 1; dy++)
				{
					if (dx == 0 && dy == 0) continue;
					int nx = cx + dx;
					int ny = cy + dy;
					if (nx < 0 || ny < 0 || nx >= m_skeleton.rows || ny >= m_skeleton.cols)
						continue;
					if (m_skeleton.at<uchar>(nx, ny) != 255) continue;
					if (visited.at<uchar>(nx, ny)) continue;
					//visited.at<uchar>(nx, ny) = 1;

					if (isCrossPnt(nx, ny))
					{
						getMidCrossPnt(nx, ny, midX, midY);
						if (midX == crossInd2Point[currentCrossInd].X() && midY == crossInd2Point[currentCrossInd].Y())
						{
							stPnts.push(SkeletonJoint(nx, ny));
							pathFlag = false;
						}
						else
						{
							SkeletonJoint nextMidCrossPnt = SkeletonJoint(midX, midY, JunctionType::JT_INIT);

							if (!point2CrossInd.count(nextMidCrossPnt))
							{
								int crossInd = point2CrossInd.size();
								point2CrossInd[nextMidCrossPnt] = crossInd;
								crossInd2Point[crossInd] = nextMidCrossPnt;
								m_vaildJointNum++;
							}
							int nextCrossInd = point2CrossInd[nextMidCrossPnt];

							paths[currentCrossInd][nextCrossInd].insert(paths[currentCrossInd][nextCrossInd].end(),
								path.begin(), path.end());

							paths[nextCrossInd][currentCrossInd].insert(paths[nextCrossInd][currentCrossInd].end(),
								path.rbegin(), path.rend());
							path.clear();
						}
					}
					else
					{
						pathFlag = false;
						stPnts.push(SkeletonJoint(nx, ny));
					}
				}
			}

			if (pathFlag)
				path.clear();
		}
	}

	void SkeletonGraph::writeCrossPnt()
	{
		Mat figure(m_skeleton.rows, m_skeleton.cols, CV_8UC3, Scalar(255, 255, 255));
		for (int i = 0; i < m_skeleton.rows; i++)
		{
			for (int j = 0; j < m_skeleton.cols; j++)
			{
				if (m_skeleton.at<uchar>(i, j) == 255)
				{
					figure.at<Vec3b>(i, j)[0] = 0;
					figure.at<Vec3b>(i, j)[1] = 0;
					figure.at<Vec3b>(i, j)[2] = 0;
				}

				int midX, midY;
				if (isCrossPnt(i, j))
				{
					figure.at<Vec3b>(i, j)[1] = 255;
					getMidCrossPnt(i, j, midX, midY);
					figure.at<Vec3b>(midX, midY)[2] = 255;
				}
			}
		}

		cv::imwrite("crossFine.png", figure);
	}

	void SkeletonGraph::getOneColor() {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dis(75, 225);

		stationColor.push_back(Scalar(dis(gen), dis(gen), dis(gen)));
	}

	void SkeletonGraph::initColor() {
		stationColor.clear();
		stationColor.push_back(Scalar(51, 153, 0));
		stationColor.push_back(Scalar(204, 102, 0));
		stationColor.push_back(Scalar(219, 219, 219));
	}

	void SkeletonGraph::setGreenColor() {
		stationColor.push_back(Scalar(51, 153, 0));
		stationColor.push_back(Scalar(51, 153, 0));
		stationColor.push_back(Scalar(51, 153, 0));
		stationColor.push_back(Scalar(51, 153, 0));
		stationColor.push_back(Scalar(51, 153, 0));
		stationColor.push_back(Scalar(51, 153, 0));
		stationColor.push_back(Scalar(51, 153, 0));
		stationColor.push_back(Scalar(51, 153, 0));
		stationColor.push_back(Scalar(51, 153, 0));
	}

	void SkeletonGraph::setConnectColor() {
		stationColor.push_back(Scalar(219, 219, 219));
	}

	void SkeletonGraph::clearColor() {
		stationColor.clear();
		color_type.clear();
	}


	cv::Mat SkeletonGraph::getSkeleton() const{
		return m_skeleton;
	}

	void SkeletonGraph::resizeVec()
	{
		color_type.resize(m_vaildJointNum * 2, 0);
		paths.resize(m_vaildJointNum * 2);
		for(int i = 0;i<paths.size();++i)
		{
			paths[i].resize(m_vaildJointNum * 2);
		}
		connectMat.resize(m_vaildJointNum * 2);
		for (int i = 0; i < connectMat.size(); ++i)
		{
			connectMat[i].resize(m_vaildJointNum * 2);
		}
		adjMat.resize(m_vaildJointNum * 2);
		for (int i = 0; i < adjMat.size(); ++i)
		{
			adjMat[i].resize(m_vaildJointNum * 2);
		}
	}

	void SkeletonGraph::expandPath(int i)
	{
		for (int k = 0; k < m_vaildJointNum; k++)
		{
			for (int j = 0; j < m_vaildJointNum; j++)
			{
				if (i == j) continue;
				if (k == i || k == j) continue;
				// If the path from i to k and from k to j exists
				if (connectMat[i][k] && connectMat[k][j])
				{
					// Update the paths matrix and merge paths
					if (paths[i][j].empty() || paths[i][k].size() + paths[k][j].size() < paths[i][j].size())
					{
						paths[i][j] = paths[i][k];  // Copy the path from i to k
						paths[i][j].push_back(SkeletonJoint(crossInd2Point[k].X(), crossInd2Point[k].Y())); // 添加中间点 k
						paths[i][j].insert(paths[i][j].end(), paths[k][j].begin(), paths[k][j].end()); // 拼接 k 到 j 的路径
						paths[j][i] = vector<SkeletonJoint>(paths[i][j].rbegin(), paths[i][j].rend());

						// Update the connection status from i to j
						connectMat[i][j] = 1;
						connectMat[j][i] = 1;
					}
				}
			}
		}
	}

	const std::vector<SkeletonJoint>& SkeletonGraph::getPath(int i, int j)
	{
		if (i < 0 || j < 0 || i >= paths.size() || j >= paths[i].size())
		{
			static std::vector<SkeletonJoint> emptyPath;
			return emptyPath;
		}
		return paths[i][j];
	}

	void SkeletonGraph::expandPaths()
	{
		for (int k = 0; k < m_vaildJointNum; k++)
		{
			for (int i = 0; i < m_vaildJointNum; i++)
			{
				for (int j = 0; j < m_vaildJointNum; j++)
				{
					if (i == j) continue;
					if (k == i || k == j) continue;
					// If the path from i to k and from k to j exists
					if (connectMat[i][k] && connectMat[k][j])
					{
						// Update the paths matrix and merge paths
						if (paths[i][j].empty() || paths[i][k].size() + paths[k][j].size() < paths[i][j].size())
						{
							paths[i][j] = paths[i][k];  // Copy the path from i to k
							paths[i][j].push_back(SkeletonJoint(crossInd2Point[k].X(), crossInd2Point[k].Y())); // 添加中间点 k
							paths[i][j].insert(paths[i][j].end(), paths[k][j].begin(), paths[k][j].end()); // 拼接 k 到 j 的路径
							paths[j][i] = vector<SkeletonJoint>(paths[i][j].rbegin(), paths[i][j].rend());

							// Update the connection status from i to j
							connectMat[i][j] = 1;
							connectMat[j][i] = 1;
						}
					}
				}
			}
		}
	}
	void SkeletonGraph::getMidCrossPath()
	{
		Mat visited = Mat::zeros(m_skeleton.size(), CV_8U);
		for (auto it = crossInd2Point.begin(); it != crossInd2Point.end(); it++)
			computeCrossPntConnect(visited, it->second);

		for (int i = 0; i < m_vaildJointNum; i++)
		{
			for (int j = i; j < m_vaildJointNum; j++)
			{
				if (i == j) {
					connectMat[i][j] = 1;
					adjMat[i][j] = 1;
				}
				else
				{
					if (!paths[i][j].empty())
					{
						connectMat[i][j] = 1;
						connectMat[j][i] = 1;

						adjMat[i][j] = 1;
						adjMat[j][i] = 1;
					}
				}
			}
		}
	}
	void SkeletonGraph::initMidCross()
	{
		m_vaildJointNum = 0;
		for (int i = 0; i < m_skeleton.rows; i++)
		{
			for (int j = 0; j < m_skeleton.cols; j++)
			{
				if (isMidCrossPnt(i, j))
				{
					SkeletonJoint midCrossPnt = SkeletonJoint(i, j, JunctionType::JT_INIT);
					int crossInd = point2CrossInd.size();
					point2CrossInd[midCrossPnt] = crossInd;
					crossInd2Point[crossInd] = midCrossPnt;
					m_vaildJointNum++;
				}
			}
		}
	}

	void SkeletonGraph::getSkeletonGraph()
	{
		initMidCross();
		resizeVec();
		getMidCrossPath();
		expandPaths();
	}


	void SkeletonGraph::updatePathsByNewJoint(int i, int j, int maxDisInd, SkeletonJoint& newJoint)
	{
		int addJointInd = m_vaildJointNum;
		crossInd2Point[addJointInd] = newJoint;
		point2CrossInd[newJoint] = addJointInd;

		m_vaildJointNum++;
		if (m_vaildJointNum >= color_type.size())
			resizeVec();

		// Update the path
		for (int m = 0; m < paths[i][j].size(); m++) {
			if (paths[i][j][m] != crossInd2Point[i] && paths[i][j][m] != crossInd2Point[j]) {
				if (m < maxDisInd) {
					paths[i][addJointInd].push_back(paths[i][j][m]);
				}
				else if (m > maxDisInd) {
					paths[addJointInd][j].push_back(paths[i][j][m]);
				}
			}
		}

		paths[addJointInd][i] = vector<SkeletonJoint>(paths[i][addJointInd].rbegin(),
			paths[i][addJointInd].rend());

		paths[j][addJointInd] = vector<SkeletonJoint>(paths[addJointInd][j].rbegin(),
			paths[addJointInd][j].rend());


		adjMat[i][j] = 0;
		adjMat[j][i] = 0;
		adjMat[i][addJointInd] = 1;
		adjMat[addJointInd][i] = 1;
		adjMat[j][addJointInd] = 1;
		adjMat[addJointInd][j] = 1;
		adjMat[addJointInd][addJointInd] = 1;

		connectMat[i][addJointInd] = 1;
		connectMat[addJointInd][i] = 1;
		connectMat[j][addJointInd] = 1;
		connectMat[addJointInd][j] = 1;
		connectMat[addJointInd][addJointInd] = 1;
		color_type[addJointInd] = static_cast<int>(newJoint.getType());
		expandPath(addJointInd);
	}

	//TODO
	// Refactoring, split into smaller functions
	bool SkeletonGraph::addNearbyCrossPoints(double threshold)
	{
		bool sign = false;
		int preVaildJointNum = m_vaildJointNum;
		for (int i = 0; i < preVaildJointNum - 1; ++i) {
			//if (isIsolate(i)) continue;
			for (int j = i + 1; j < preVaildJointNum; ++j) {
				if (adjMat[i][j] == 0)
					continue;

				SkeletonJoint& p1 = crossInd2Point[i];
				SkeletonJoint& p2 = crossInd2Point[j];

				double maxDist = 0;
				int maxIndex = -1;
				for (int k = 0; k < paths[i][j].size(); ++k) {
					double distance = pointLineDistance(paths[i][j][k], p1, p2);
					if (distance > maxDist) {
						maxDist = distance;
						maxIndex = k;
					}
				}

				if (maxDist > threshold && maxIndex != -1) {
					SkeletonJoint newJoint = paths[i][j][maxIndex];
					newJoint.setJunctionType(JunctionType::JT_PATHMID);
					cout << "p1: " << point2CrossInd[p1] << "-(" << p1.X() << "," << p1.Y() << ")"
						<< " p2: " << point2CrossInd[p2] << "-(" << p2.X() << "," << p2.Y() << ")"
						<< " newStation: " << crossInd2Point.size() << "-(" << newJoint.X() << "," << newJoint.Y() << ")" << endl;

					updatePathsByNewJoint(i, j, maxIndex, newJoint);

					sign = true;
				}
			}
		}
		return sign;
	}

	void SkeletonGraph::addNewStations(double threshold) 
	{
		cout << "Start adding a new site to the initial site..." << endl;
		bool sign = false;
		do {
			sign = addNearbyCrossPoints(threshold);
		} while (sign);
		cout << "Finished adding." << endl;
	}

	double SkeletonGraph::pointLineDistance(const SkeletonJoint & point, const SkeletonJoint & lineStart, const SkeletonJoint & lineEnd)
	{
		double A = lineEnd.Y() - lineStart.Y();
		double B = lineStart.X() - lineEnd.X();
		double C = lineEnd.X() * lineStart.Y() - lineStart.X() * lineEnd.Y();

		return abs(A * point.X() + B * point.Y() + C) / sqrt(A * A + B * B);
	}

	bool SkeletonGraph::isIsolate(int ind) const
	{
		int connectCnt = 0;
		for (size_t i = 0; i < connectMat.size(); ++i) {
			if (i == ind) continue;
			if (connectMat[ind][i]) {
				connectCnt++;
			}
		}

		if (connectCnt < crossInd2Point.size() / 10) {
			return true;
		}
		else {
			return false;
		}
	}

}