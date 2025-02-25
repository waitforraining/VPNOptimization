#define _USE_MATH_DEFINES


#include "VPN/HeatMap.h"

using namespace std;
using namespace cv;

namespace ViewPointNetwork
{
	HeatMap::HeatMap()
	{
		m_x = 0;
		m_y = 0;
		m_cell = 0;
		m_width = 0;
		m_height = 0;
	}

	HeatMap::HeatMap(int height, int width, double cell)
	{
		m_cell = cell;
		m_height = height;
		m_width = width;
		m_value = vector<vector<double>>(m_width, vector<double>(m_height, 0.0));
	}

	HeatMap::HeatMap(const House& house, double cell)
	{
		m_width = std::ceil(house.getMaxX() / cell);
		m_height = std::ceil(house.getMaxY() / cell);
		m_cell = cell;
		if (house.getType() == HT_Outdoor) {
			m_width = m_width * 3;
			m_height = m_height * 3;
		}
		m_value = vector<vector<double>>(m_width, vector<double>(m_height));
	}
	

	double HeatMap::getValue(int indx, int indy) const
	{
		assert(indx >= 0 && indx < m_width);
		assert(indy >= 0 && indy < m_height);
		double v = m_value[indx][indy];

		return v;
	}

	void HeatMap::setValue(int indx, int indy, double v)
	{
		assert(indx >= 0 && indx < m_width);
		assert(indy >= 0 && indy < m_height);
		m_value[indx][indy] = v;
	}


	double HeatMap::getValue(double inx, double iny) const
	{
		int i = int(inx / m_cell);
		int j = int(iny / m_cell);
		return getValue(i, j);
	}

	void HeatMap::setValue(double inx, double iny, double v)
	{
		int i = int(inx / m_cell);
		int j = int(iny / m_cell);
		setValue(i, j, v);
	}

	double HeatMap::getValue(const Point2D & p) const
	{
		return getValue(p.X(), p.Y());
	}

	void HeatMap::setValue(const Point2D & p, double v)
	{
		setValue(p.X(), p.Y(), v);
	}


	bool HeatMap::loadGrayImg(const std::string& grayMapPath)
	{
		cv::Mat figure = cv::imread(grayMapPath, cv::IMREAD_GRAYSCALE);
		double pi_2 = 2.0 * M_PI;
		
		if (figure.empty()) {
			std::cerr << "no cache" << std::endl;
			return false;
		}
		cv::flip(figure, figure, 0);
		for (int i = 0; i < m_width; i++) {
			for (int j = 0; j < m_height; j++) {
				double score = static_cast<double>(figure.at<uchar>(j, i)) / 255.0;

				m_value[i][j] = score * pi_2;
			}
		}
		
	}

	void HeatMap::generate(House& house, double r_min, double r_max, double scoreThresh)
	{
		
		Station s;
		for (int i = 0; i < m_width; i++)
		{
			for (int j = 0; j < m_height; j++)
			{
				
				int ptType = house.position(i, j, m_cell);

				if (house.getType() == HT_Indoor && ptType != PT_INSIDE ) continue;
				if (house.getType() == HT_Outdoor && ptType != PT_OUTSIDE ) continue;
				s = Station(i * m_cell, j * m_cell, r_min, r_max);
				s.scan(house.getBspRoot().get());
				m_value[i][j] = s.getScanedScore();

				if (house.getType() == HT_Outdoor && m_value[i][j] <= scoreThresh) {
					m_value[i][j] = 0;
				}
			}
		}
	}
	// Generate a heat map for an edge
	void HeatMap::generateOneEdgeHeat(House& house, Edge2D& edge, double r_min, double r_max)
	{
		Station s;

		double maxV = 0, minV = INT_MAX;
		for (int i = 0; i < m_width; i++)
		{
			for (int j = 0; j < m_height; j++)
			{

				if (house.getType() == HT_Indoor && (house.position(i, j, m_cell) == PT_OUTSIDE || house.position(i, j, m_cell) == PT_ONWALL)) continue;
				if (house.getType() == HT_Outdoor && (house.position(i, j, m_cell) == PT_INSIDE || house.position(i, j, m_cell) == PT_ONWALL)) continue;

				s = Station(i * m_cell, j * m_cell, r_min, r_max);
				s.traversal(house.getBspRoot().get());

				for (auto scanedArea : s.getScanedEdges())
				{
					if (scanedArea.second.isOverlap(edge, 0.1))
					{
						m_value[i][j] += s.getScore(scanedArea.second);
					}
				}
				maxV = max(maxV, m_value[i][j]);
				minV = min(minV, m_value[i][j]);
			}
		}
		// normalization
		//for (int i = 0; i < m_width; i++)
		//{
		//	for (int j = 0; j < m_height; j++)
		//	{
		//		m_value[i][j] = (m_value[i][j] - minV) / (maxV - minV) * 2 * M_PI;
		//	}
		//}
	}

	//normalization
	void HeatMap::normalization(House house, vector<vector<int>> tmp)
	{
		double maxV = 0, minV = INT_MAX;
		for (int i = 0; i < m_value.size(); i++)
			for (int j = 0; j < m_value[0].size(); j++)
			{
				if (m_value[i][j] <= 0)
					continue;
				if (m_value[i][j] < minV)
					minV = m_value[i][j];
				if (m_value[i][j] > maxV)
					maxV = m_value[i][j];
			}
		for (int i = 0; i < m_width; i++)
			for (int j = 0; j < m_height; j++)
				if (m_value[i][j] > 0)
					m_value[i][j] = m_value[i][j] / 2 / M_PI;
		//m_value[i][j] = (m_value[i][j] - minV) / (maxV - minV);
	}

	void HeatMap::show(House & house, string path, string  name, bool show, bool station,
		int num, bool merge, int num1, int num2, vector<BSPNode> tmpEdge, bool lineshow)
	{
		Mat figure(m_height + 40, m_width + 40, CV_8UC3, Scalar(255, 255, 255));

		for (int i = 0; i < m_height; i++)
		{
			for (int j = 0; j < m_width; j++)
			{
				if (m_value[j][i] == -1)
				{
					figure.at<Vec3b>(i, j)[0] = 0;
					figure.at<Vec3b>(i, j)[1] = 0;
					figure.at<Vec3b>(i, j)[2] = 0;
				}
				else if (m_value[j][i] == 0)
				{
					figure.at<Vec3b>(i, j)[0] = 255;
					figure.at<Vec3b>(i, j)[1] = 255;
					figure.at<Vec3b>(i, j)[2] = 255;
				}
				else if (m_value[j][i] < 0.25)
				{
					figure.at<Vec3b>(i, j)[0] = 255;
					figure.at<Vec3b>(i, j)[1] = int(255 * 4 * m_value[j][i]);
					figure.at<Vec3b>(i, j)[2] = 0;
				}
				else if (m_value[j][i] < 0.5)
				{
					figure.at<Vec3b>(i, j)[0] = 255;
					figure.at<Vec3b>(i, j)[1] = 255;
					figure.at<Vec3b>(i, j)[2] = int(255 * 4 * (m_value[j][i] - 0.25));
				}
				else if (m_value[j][i] < 0.75)
				{
					figure.at<Vec3b>(i, j)[0] = int(255 * (1 - 4 * (m_value[j][i] - 0.5)));
					figure.at<Vec3b>(i, j)[1] = 255;
					figure.at<Vec3b>(i, j)[2] = 255;
				}
				else
				{
					figure.at<Vec3b>(i, j)[0] = 0;
					figure.at<Vec3b>(i, j)[1] = int(255 * (1 - 4 * (m_value[j][i] - 0.75)));
					figure.at<Vec3b>(i, j)[2] = 255;
				}
			}
		}
		double _cell = m_cell;
		drawEdges(figure, tmpEdge, _cell, Scalar(0, 255, 0), 1);

		drawEdges(figure, house.getEdges(), _cell, Scalar(0, 0, 0), 1);

		if (station)
		{
			vector<Station> stations;
			for (int i = 0; i < stations.size(); i++)
			{
				Station station = stations[i];
				Point s(station.X() / _cell, station.Y() / _cell);
				//circle(figure, s, max(m_x, m_height) / (_cell * 80), Scalar(0, 0, 0), -1);
				circle(figure, s, 5, Scalar(0, 0, 0), -1);

				// cv::putText(figure, to_string(i+1), s + Point(10,0), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 0, 0),0.5);
			}

			if (lineshow)
			{
				map<vector<double>, Edge2D> tmp = stations[num1].getScanedEdges();
				Point s(stations[num1].X() / _cell, stations[num1].Y() / _cell);
				for (map<vector<double>, Edge2D>::iterator j = tmp.begin(); j != tmp.end(); j++)
				{
					Point p1 = Point(j->second.getBegPoint().X() / _cell, j->second.getBegPoint().Y() / _cell);
					Point p2 = Point(j->second.getEndPoint().X() / _cell, j->second.getEndPoint().Y() / _cell);
					cv::line(figure, s, p1, Scalar(0, 0, 255));
					cv::line(figure, s, p2, Scalar(0, 0, 255));//
					//cv::putText(figure, to_string(double((int)(j->second.getScore(stations[num1]) * 100)) / 100), (p1 + p2) / 2 + Point(1, 0), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 125), 0.3);//
					//cv::putText(figure, to_string(((int)(station.scanEdge(j->second) * 1000)) / 1000), (p1 + p2) / 2 + Point(3, 0), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 125), 1);
				}
			}
		}
		if (num)
		{
			if (merge)
			{
				if (show) {
					imshow(to_string(num) + "( " + to_string(num1) + ", " + to_string(num2) + ")", figure);
				}
				//cv::imwrite(path + "/" + name + to_string(num) + "( " + to_string(num1) + ", " + to_string(num2) + ")" "_heat.png", figure);
			}
			else
			{
				if (show) {
					imshow(to_string(num), figure);
				}
				//cv::imwrite(path + "/" + to_string(num) + "_heat.png", figure);
			}
		}
		else
		{
			cv::flip(figure, figure, 0); // 0 indicates upside down

			vector<Station> stations;
			for (int i = 0; i < stations.size(); i++)
			{
				Station station = stations[i];
				Point s(station.X() / _cell, station.Y() / _cell);
				//circle(figure, s, max(m_x, m_height) / (_cell * 80), Scalar(0, 0, 0), -1);
				//auto p = s + Point(10, 0);
				auto p = s + Point(10, 0);
				p.y = figure.rows - p.y;
				//cv::putText(figure, to_string(i + 1), p, FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 0), 0.5);
				cv::putText(figure, to_string(i + 1), p, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 0.5);
			}

			if (show) {
				imshow("heat", figure);
			}
			cout << path << endl;
			//cv::imwrite(path + "_heat.png", figure);

			cv::imwrite(path + name + "_heat.png", figure);
		}
	}


	std::vector<cv::Vec3b> interpolateColors(const std::vector<cv::Vec3b>& colors, int steps)
	{
		std::vector<cv::Vec3b> gradient;

		for (size_t i = 0; i < colors.size() - 1; ++i) {
			const cv::Vec3b& startColor = colors[i];
			const cv::Vec3b& endColor = colors[i + 1];

			for (int t = 0; t < steps; ++t) {
				float alpha = static_cast<float>(t) / (steps - 1);
				cv::Vec3b interpolatedColor(
					static_cast<uchar>((1 - alpha) * startColor[0] + alpha * endColor[0]),
					static_cast<uchar>((1 - alpha) * startColor[1] + alpha * endColor[1]),
					static_cast<uchar>((1 - alpha) * startColor[2] + alpha * endColor[2])
				);
				gradient.push_back(interpolatedColor);
			}
		}

		return gradient;
	}

	void HeatMap::saveBGR1(const House & house, const std::vector<cv::Vec3b> & colors,
		const std::string & name, double scalar, bool station)
	{
		Mat figure(m_height, m_width, CV_8UC3, Scalar(255, 255, 255));
		std::vector<cv::Vec3b> interPolate = interpolateColors(colors, 101);
		double pi_2 = 2.0 * M_PI;
		double score;
		int ind;
		for (int i = 0; i < m_height; i++)
		{
			for (int j = 0; j < m_width; j++)
			{
				if (house.getType() == HT_Indoor) {
					if (house.position(j, i, m_cell) == PT_OUTSIDE || house.position(j, i, m_cell) == PT_ONWALL) continue;
					score = m_value[j][i] / pi_2;
					ind = int(score * 100 * (colors.size() - 1));
					figure.at<Vec3b>(i, j) = interPolate[ind];
				}
				else 
				{
					if (house.position(j, i, m_cell) == PT_INSIDE || house.position(j, i, m_cell) == PT_ONWALL) continue;
					score = m_value[j][i] / pi_2;
					ind = int(score * 100 * (colors.size() - 1));
					figure.at<Vec3b>(i, j) = interPolate[ind];
				}
			}
		}

		drawEdges(figure, *this, house.getEdges(), m_cell, Scalar(0, 0, 0), 1, house.getType());

		cv::flip(figure, figure, 0); // 0 indicates upside down
		cv::imwrite(name + "_RGBheat.png", figure);
	}

	void HeatMap::saveBGR_YellowOrange(const House & house, const std::string & name, bool station)
	{
		double scalar = 0.25;
		std::vector<cv::Vec3b> colors;
		colors.push_back(cv::Vec3b(212, 255, 255));
		colors.push_back(cv::Vec3b(142, 217, 255));
		colors.push_back(cv::Vec3b(41, 155, 255));
		colors.push_back(cv::Vec3b(13, 91, 217));
		colors.push_back(cv::Vec3b(3, 51, 153));
	
		saveBGR1(house, colors, name, scalar, station);
	}

	cv::Mat HeatMap::getGRAYImg()
	{
		double pi_2 = 2.0 * M_PI;
		double score;
		cv::Mat img(m_height, m_width, CV_8UC1, cv::Scalar(0));

		for (int i = 0; i < m_height; i++)
		{
			for (int j = 0; j < m_width; j++)
			{
				score = m_value[j][i] / pi_2;
				score = score * 255;

				img.at<uchar>(i, j) = static_cast<uchar>(std::min(255.0, score));
			}
		}

		cv::flip(img, img, 0); // 0 indicates upside down
		return img;
	}

	cv::Mat HeatMap::getHeatImg(const House& house)
	{
		std::vector<cv::Vec3b> colors;
		colors.push_back(cv::Vec3b(212, 255, 255));
		colors.push_back(cv::Vec3b(142, 217, 255));
		colors.push_back(cv::Vec3b(41, 155, 255));
		colors.push_back(cv::Vec3b(13, 91, 217));
		colors.push_back(cv::Vec3b(3, 51, 153));
		Mat figure(m_height, m_width, CV_8UC3, Scalar(255, 255, 255));
		std::vector<cv::Vec3b> interPolate = interpolateColors(colors, 101);
		double pi_2 = 2.0 * M_PI;
		double score;
		int ind;
		for (int i = 0; i < m_height; i++)
		{
			for (int j = 0; j < m_width; j++)
			{
				if (house.getType() == HT_Indoor) {
					if (house.position(j, i, m_cell) == PT_OUTSIDE || house.position(j, i, m_cell) == PT_ONWALL) continue;
					score = m_value[j][i] / pi_2;
					ind = int(score * 100 * (colors.size() - 1));
					figure.at<Vec3b>(i, j) = interPolate[ind];
				}
				else
				{
					if (house.position(j, i, m_cell) == PT_INSIDE || house.position(j, i, m_cell) == PT_ONWALL) continue;
					score = m_value[j][i] / pi_2;
					ind = int(score * 100 * (colors.size() - 1));
					figure.at<Vec3b>(i, j) = interPolate[ind];
				}
			}
		}
		drawEdges(figure, *this, house.getEdges(), m_cell, Scalar(0, 0, 0), 1, house.getType());

		cv::flip(figure, figure, 0); // 0 indicates upside down
		return figure;
	}
	void HeatMap::saveGRAY(House & house, string name, bool station)
	{
		double pi_2 = 2.0 * M_PI;
		double score;
		cv::Mat figure(m_height, m_width, CV_8UC1, cv::Scalar(0));

		for (int i = 0; i < m_height; i++)
		{
			for (int j = 0; j < m_width; j++)
			{
				score = m_value[j][i] / pi_2;
				//score = m_value[j][i];
				score = score * 255;

				figure.at<uchar>(i, j) = static_cast<uchar>(std::min(255.0, score));
			}
		}

		cv::flip(figure, figure, 0); // 0 indicates upside down
		cv::imwrite(name + "_GRAYheat.png", figure);
	}

	HeatMap computeHeatMap(const std::vector<Edge2D> & vecEdges, const std::string & outPath,
		double cell, double r_min, double r_max)
	{
		cout << "¿ªÊ¼¼ÆËã..." << endl;
		double stationRadiusMin = r_min;
		double stationRadiusMax = r_max;
		House house(vecEdges);
		//Initialize BSP, root= the edge closest to the center point

		HeatMap heatMap(house, cell);
		heatMap.generate(house, stationRadiusMin, stationRadiusMax);

		stringstream ss;
		ss << std::fixed << setprecision(3);
		ss << "_" << cell << "_" << r_min;
		ss << "_to_" << r_max;
		string name = outPath + ss.str();


		heatMap.saveBGR_YellowOrange(house, name, false);
		heatMap.saveGRAY(house, name);

		return heatMap;
	}

	void drawEdges(cv::Mat & figure, HeatMap & heat, const Edge2D & edge, double cell,
		const cv::Scalar & color, int thickness, int lineType, int shift, HouseType houseType)
	{
		assert(cell > 0.0);
		const auto& p0 = edge.getBegPoint();
		const auto& p1 = edge.getEndPoint();

		cv::Point cvP0 = Point(p0.X() / cell, p0.Y() / cell);
		cv::Point cvP1 = Point(p1.X() / cell, p1.Y() / cell);
		cv::line(figure, cvP0, cvP1, color, thickness, lineType, shift);
	}

	void drawEdges(cv::Mat & figure, HeatMap & heat, const vector<Edge2D> & vecEdges, double cell,
		const cv::Scalar & color, int thickness, int lineType, int shift, HouseType houseType)
	{
		for(const Edge2D & e : vecEdges)
			drawEdges(figure, heat, e, cell, color,
				thickness, lineType, shift, houseType);
	}

	void drawEdges(cv::Mat & figure, HeatMap & heat, const vector<BSPNode> & vecNodes, double cell,
		const cv::Scalar & color, int thickness, int lineType, int shift,
		HouseType houseType)
	{
		for(const auto & v : vecNodes)
			drawEdges(figure, heat, v.getEdge(), cell, color,
				thickness, lineType, shift, houseType);
	}

	void drawEdges(cv::Mat & figure, const Edge2D & edge, double cell,
		const cv::Scalar & color, int thickness, int lineType, int shift)
	{
		assert(cell > 0.0);
		const auto& p0 = edge.getBegPoint();
		const auto& p1 = edge.getEndPoint();
		cv::Point cvP0 = Point(p0.X() / cell, p0.Y() / cell);
		cv::Point cvP1 = Point(p1.X() / cell, p1.Y() / cell);
		cv::line(figure, cvP0, cvP1, color, thickness, lineType, shift);
	}

	void drawEdges(cv::Mat & figure,const vector<Edge2D> & vecEdges, double cell,
		const cv::Scalar & color, int thickness, int lineType, int shift)
	{
		for(const Edge2D & e : vecEdges)
			drawEdges(figure,e, cell, color,
				thickness, lineType, shift);
	}

	void drawEdges(cv::Mat & figure, const vector<BSPNode> & vecNodes, double cell,
		const cv::Scalar & color, int thickness, int lineType, int shift)
	{
		for(const auto & v : vecNodes)
			drawEdges(figure, v.getEdge(), cell, color,
				thickness, lineType, shift);
	}

	void drawStations(cv::Mat & figure, const vector<Station> & vecStations, double cell,
		const cv::Scalar & color, int radius, int thickness, int lineType, int shift)
	{
		assert(cell != 0);
		cv::Point s;

		for(const auto & v : vecStations)
		{
			s = cv::Point(v.X() / cell, v.Y() / cell);
			auto p = s + Point(1, 0);
			p.y = figure.rows - p.y;
			s.y = figure.rows - s.y;
			//cv::putText(figure, to_string(i + 1), p, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 0.5);
			circle(figure, s, radius, color, -1);
		}
	}


	void showScanArea(cv::Mat & figure, const House & house, double cell)
	{
		vector<Station> stations;
		for (int i = 0; i < stations.size(); i++)
		{
			auto scanArea = stations[i].getScanedEdges();
		
			for (auto c = scanArea.begin(); c != scanArea.end(); c++)
			{
				cv::line(figure, cv::Point(c->second.getBegPoint().X() / cell, figure.rows - c->second.getBegPoint().Y() / cell),
					cv::Point(c->second.getEndPoint().X() / cell, figure.rows - c->second.getEndPoint().Y() / cell), cv::Scalar(0, 255, 0), 2);
			}
		}
	}

	void showAddEdges(cv::Mat & figure, House & house, std::vector<Edge2D> lineSegs,
		std::vector<int> addEdgesId, double cell, cv::Scalar color)
	{
		vector<Station> stations;
		for (int i = 0; i < lineSegs.size(); i++)
		{
			if (!addEdgesId[i]) continue;
			auto scanEdge = lineSegs[i];

			cv::line(figure, cv::Point(scanEdge.getBegPoint().X() / cell + 5, figure.rows - scanEdge.getBegPoint().Y() / cell - 5),
				cv::Point(scanEdge.getEndPoint().X() / cell + 5, figure.rows - scanEdge.getEndPoint().Y() / cell - 5), color, 2);
		}
	}

	void showSkeleton(cv::Mat & figure, vector<int> stations, vector<vector<int>> skeletonConMat,
		std::unordered_map<int, std::pair<int, int>> crossInd2Point)
	{
		for (int i = 0; i < stations.size(); i++)
		{
			cv::Point s1(crossInd2Point[stations[i]].second, crossInd2Point[stations[i]].first);
			for (int j = 0; j < stations.size(); j++)
			{
				if (i == j) continue;
				if (!skeletonConMat[stations[i]][stations[j]]) continue;
				cv::Point s(crossInd2Point[stations[j]].second, crossInd2Point[stations[j]].first);
				line(figure, s, s1, Scalar(125, 0, 125), 2);
			}
		}
	}

	void showHeatBGR(cv::Mat & figure, const HeatMap & heatMap)
	{
		int figureRows = figure.rows;
		for (int i = 0; i < heatMap.getXLen(); i++)
		{
			for (int j = 0; j < heatMap.getYLen(); j++)
			{
				//auto heatValue = heatMap.value[i][heatMap.height - j];
				double heatValue = heatMap.getValue(i, heatMap.getYLen() - j);
				if (heatValue == -1)
				{
					figure.at<Vec3b>(j, i)[0] = 0;
					figure.at<Vec3b>(j, i)[1] = 0;
					figure.at<Vec3b>(j, i)[2] = 0;
				}
				else if (heatValue == 0)
				{
					figure.at<Vec3b>(j, i)[0] = 255;
					figure.at<Vec3b>(j, i)[1] = 255;
					figure.at<Vec3b>(j, i)[2] = 255;
				}
				else if (heatValue < 0.25)
				{
					figure.at<Vec3b>(j, i)[0] = 255;
					figure.at<Vec3b>(j, i)[1] = int(255 * heatValue / 0.25);
					figure.at<Vec3b>(j, i)[2] = 0;
				}
				else if (heatValue < 0.5)
				{
					figure.at<Vec3b>(j, i)[0] = 255;
					figure.at<Vec3b>(j, i)[1] = 255;
					figure.at<Vec3b>(j, i)[2] = int(255 * (heatValue - 0.25) / 0.25);
				}
				else if (heatValue < 0.75)
				{
					figure.at<Vec3b>(j, i)[0] = int(255 * (1 - (heatValue - 0.5) / 0.25));
					figure.at<Vec3b>(j, i)[1] = 255;
					figure.at<Vec3b>(j, i)[2] = 255;
				}
				else
				{
					figure.at<Vec3b>(j, i)[0] = 0;
					figure.at<Vec3b>(j, i)[1] = int(255 * (1 - 4 * (heatValue - 0.75)));
					figure.at<Vec3b>(j, i)[2] = 255;
				}
			}
		}
	}


}