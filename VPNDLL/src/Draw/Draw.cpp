#include "Draw/Draw.h"

using namespace std;
using namespace cv;
namespace ViewPointNetwork
{
	void drawHouseLines(cv::Mat& figure, const House& house, double cell)
	{
		for (size_t i = 0; i < house.getEdges().size(); i++) {
			auto endP0 = house.getEdge(i).getBegPoint();
			auto endP1 = house.getEdge(i).getEndPoint();
			auto p0 = cv::Point(endP0.X() / cell, figure.rows - endP0.Y() / cell);
			auto p1 = cv::Point(endP1.X() / cell, figure.rows - endP1.Y() / cell);
			cv::line(figure, p0, p1, cv::Scalar(0, 0, 0), 1);
		}
	}

	void drawSkeletonGraph(cv::Mat& figure, const SkeletonGraph& sg)
	{
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dis(75, 225);

		const auto& paths = sg.paths;
		for (int i = 0; i < paths.size(); i++)
		{
			Vec3b color(dis(gen), dis(gen), 225);
			for (int j = i + 1; j < paths.size(); j++)
			{
				for (auto path : paths[i][j])
				{
					int x = path.X();
					int y = path.Y();
					for (int dx = -1; dx <= 1; dx++) {
						for (int dy = -1; dy <= 1; dy++) {
							if (x + dx >= 0 && x + dx < figure.rows && y + dy >= 0 && y + dy < figure.cols)
							{
								figure.at<Vec3b>(x + dx, y + dy) = color;
							}
						}
					}
				}
			}
		}
	}

	void drawStation(cv::Mat& figure, const Station& station, double cell,
		std::string text, cv::Scalar circleScalar, cv::Scalar textScalar,
		int textThickness, int radius)
	{
		cv::Point s(station.X() / cell, station.Y() / cell);
		s.y = figure.rows - s.y;
		cv::putText(figure, text, s, FONT_HERSHEY_SIMPLEX, 1.0, textScalar, 1);
		circle(figure, s, radius, circleScalar, -1);
	}

	void drawStations(cv::Mat& figure, const SkeletonGraph& sg,
		const vector<Station>& iStations, double cell, int radius)
	{
		for (size_t i = 0; i < iStations.size(); i++)
		{
			const Station& station = iStations[i];
			cv::Point s(station.X() / cell, station.Y() / cell);
			auto p = s;
			p.y = figure.rows - p.y;
			s.y = figure.rows - s.y;
			cv::putText(figure, to_string(i), p, FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 0), 1.0);

			int colorType = sg.color_type[i];
			circle(figure, s, radius, sg.stationColor[colorType], -1);
		}
	}
}
