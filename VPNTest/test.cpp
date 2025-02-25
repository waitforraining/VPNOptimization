#include <boost/program_options.hpp>
#include <filesystem>
#include <memory>

#include <map>
#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include "IO/io.h"
#include "VPN/VpnOptimizerBySkeleton.h"


using namespace ViewPointNetwork;
using namespace std;
namespace VPN = ViewPointNetwork;
namespace fs = std::filesystem;

//bsp搜索功能 测试两个数据 用固定参数，固定站点测试bsp扫描的边数长度是否一致 
//生成热力图 固定输入平面图 与之前代码中热力图矩阵中的值进行比较
//骨架线
//站点位置

fs::path currentFilePath = __FILE__;
fs::path currentDir = currentFilePath.parent_path();
fs::path parentDir = currentDir.parent_path();


struct TestData {
	shared_ptr<VpnOptimizerBySkeleton> opt;
	std::string houseName;
};


class RoomReadTest : public ::testing::TestWithParam<TestData> {};

static vector<TestData> vecData, vecScanData;
static void readData()
{
	std::ifstream file(parentDir / "data/dataMap.csv");
	std::string line;
	std::getline(file, line); //skip first line
	while (std::getline(file, line)) {
		if (line.empty()) continue;
		std::stringstream ss(line);
		std::string token;
		std::getline(ss, token, ',');  
		auto houseName = token;
		std::getline(ss, token, ',');  
		auto paramStr = token;
		vecData.push_back(TestData{
				make_shared< VpnOptimizerBySkeleton>((parentDir / "data" / houseName / houseName).string(),
													 OptSkelParam(paramStr)),
				houseName
			});
	}
}
static void readScanData(int count = 1000)
{
	std::ifstream file(parentDir / "data/dataMap.csv");
	std::string line;
	std::getline(file, line); //skip first line
	int i = 0;
	while (std::getline(file, line)) {
		if (line.empty()) continue;
		if (i++ >= count) break;
		std::stringstream ss(line);
		std::string token;
		std::getline(ss, token, ',');
		auto houseName = token;
		std::getline(ss, token, ',');
		auto paramStr = token;
		vecScanData.push_back(TestData{
				make_shared< VpnOptimizerBySkeleton>((parentDir / "data" / houseName / houseName).string(),
													 OptSkelParam(paramStr)),
				houseName
			});
		vecScanData.back().opt->doOptimize();
	}
}

std::vector<Edge2D> ConstructRectangle()
{
	std::vector<Edge2D> edges;
	edges.emplace_back(Point2D(0, 0), Point2D(0, 1));
	edges.emplace_back(Point2D(0, 1), Point2D(1, 1));
	edges.emplace_back(Point2D(1, 1), Point2D(1, 0));
	edges.emplace_back(Point2D(1, 0), Point2D(0, 0));

	return edges;
}

INSTANTIATE_TEST_SUITE_P(
	RoomRead, RoomReadTest,
	::testing::ValuesIn(vecData),
	[](const testing::TestParamInfo<TestData>& info) {
		// Can use info.param here to generate the test suffix
		std::string name = info.param.houseName;
		return name;
	});


int main(int argc, char** argv) {
	readData();
	readScanData(1);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST_P(RoomReadTest, readTop)
{
	auto& testData = GetParam();
	auto houseName = testData.houseName;
	cout << "test readTop:" << houseName << endl;
	std::vector<Edge2D> edges;
	readEdges((parentDir / "data" / houseName / houseName).string(), edges, "top");
	std::ifstream file(parentDir / "data" / houseName / houseName / "readTop.csv");
	std::string line;
	std::getline(file, line); //skip first line

	std::vector<double> expectedLengths;
	while (std::getline(file, line)) {
		if (line.empty()) continue;
		std::stringstream ss(line);
		std::string token;
		std::getline(ss, token, ',');  // skip index
		std::getline(ss, token, ',');  // read length
		expectedLengths.push_back(std::stod(token));
	}

	ASSERT_TRUE(edges.size() > 0);
	ASSERT_EQ(edges.size(), expectedLengths.size());
	for (size_t i = 0; i < edges.size(); ++i) {
		ASSERT_NEAR(edges[i].Length(), expectedLengths[i], 1e-4);
	}
}

TEST_P(RoomReadTest, readDxf)
{
	auto& testData = GetParam();
	auto houseName = testData.houseName;

	cout << "test readDxf:" << houseName << endl;
	std::vector<Edge2D> edges;
	std::vector<double> expectedLengths;
	readEdges((parentDir / "data" / houseName / houseName).string(), edges, "dxf");

	std::ifstream file(parentDir / "data" / houseName / houseName / "readDxf.csv");
	std::string line;
	std::getline(file, line); //skip first line

	while (std::getline(file, line)) {
		if (line.empty()) continue;

		std::stringstream ss(line);
		std::string token;
		std::getline(ss, token, ',');  // skip index
		std::getline(ss, token, ',');  // read length
		expectedLengths.push_back(std::stod(token));
	}

	ASSERT_TRUE(edges.size() > 0);
	ASSERT_EQ(edges.size(), expectedLengths.size());
	for (size_t i = 0; i < edges.size(); ++i) {
		ASSERT_NEAR(edges[i].Length(), expectedLengths[i], 1e-4);
	}
}

TEST(WriteTest, writeTop)
{
	std::vector<Edge2D> edges = ConstructRectangle();
	writeEdges("test", edges, "top");
	std::vector<Edge2D> rEdges;
	readEdges("test", rEdges, "top");
	ASSERT_EQ(edges.size(), rEdges.size());
	for (int i = 0; i < edges.size(); i++)
	{
		ASSERT_NEAR(edges[i].getBegPoint().X(), rEdges[i].getBegPoint().X(), 1e-4);
		ASSERT_NEAR(edges[i].getEndPoint().X(), rEdges[i].getEndPoint().X(), 1e-4);
	}
}

TEST(WriteTest, writeDxf)
{
	std::vector<Edge2D> edges = ConstructRectangle();
	writeEdges("test", edges, "dxf");
	std::vector<Edge2D> rEdges;
	readEdges("test", rEdges, "dxf");
	ASSERT_EQ(edges.size(), rEdges.size());
	for (int i = 0; i < edges.size(); i++)
	{
		ASSERT_NEAR(edges[i].getBegPoint().X(), rEdges[i].getBegPoint().X(), 1e-4);
		ASSERT_NEAR(edges[i].getEndPoint().X(), rEdges[i].getEndPoint().X(), 1e-4);
	}
}


class RoomScanTest : public ::testing::TestWithParam<TestData> {
public:
	RoomScanTest() {}
	
};


void compareBsp(BSPNode* node, const std::vector<double>& bspEdges, size_t& index)
{
	if (!node) {
		return;
	}

	double len = node->getEdge().Length();

	ASSERT_NEAR(len, bspEdges[index], 1e-4);

	++index;

	compareBsp(node->frontNode, bspEdges, index);
	compareBsp(node->backNode, bspEdges, index);
}


TEST_P(RoomScanTest, testBsp) {
	//std::ifstream file(houseName +  "/bspEdges.csv");
	auto& testData = GetParam();
	auto houseName = testData.houseName;
	
	std::ifstream file(parentDir / "data" / houseName / houseName / "bspEdges.csv");
	std::vector<double> bspEdges;

	std::string line;

	std::getline(file, line); //Used to skip the header line

	while (std::getline(file, line)) {
		if (line.empty()) {
			break;
		}

		std::stringstream ss(line);
		std::string token;

		double len;

		std::getline(ss, token, ',');
		std::getline(ss, token, ','); 
		len = std::stod(token);

		bspEdges.push_back(len);
	}
	ASSERT_FALSE(bspEdges.empty());

	BSPNode* root = testData.opt->getHouse().getBspRoot().get();

	size_t index = 0;
	compareBsp(root, bspEdges, index);
	
};

TEST_P(RoomScanTest, testHeat) {
	// Read two images
	auto& testData = GetParam();
	auto houseName = testData.houseName;
	auto opt = testData.opt;
	string path = (parentDir / "data" / houseName / houseName).string();
	cv::Mat testHeat = opt->getHeatMap().getHeatImg(opt->getHouse());
	cv::Mat groundTruth = cv::imread(path + "/RGBheat.png", cv::IMREAD_UNCHANGED);

	ASSERT_FALSE(testHeat.empty());
	ASSERT_FALSE(groundTruth.empty());

	ASSERT_EQ(testHeat.size(), groundTruth.size());

	ASSERT_TRUE(cv::countNonZero(testHeat != groundTruth) == 0);
};

TEST_P(RoomScanTest, testRidge) {
	// Read two images

	auto& testData = GetParam();
	auto houseName = testData.houseName;
	auto opt = testData.opt;
	string path = (parentDir / "data" / houseName / houseName).string();

	cv::Mat testRidge = opt->getSkeletonGraph().getSkeleton();
	cv::Mat groundTruth = cv::imread(path + "/zhang_skeleton.png",
		cv::IMREAD_UNCHANGED);

	// Check whether the image is loaded successfully
	ASSERT_FALSE(testRidge.empty());
	ASSERT_FALSE(groundTruth.empty());

	// Compare sizes
	ASSERT_EQ(testRidge.size(), groundTruth.size());

	// Compare image contents (pixel by pixel)
	ASSERT_TRUE(cv::countNonZero(testRidge != groundTruth) == 0);
};

TEST_P(RoomScanTest, testPos) {
	auto& testData = GetParam();
	auto houseName = testData.houseName;

	std::ifstream file(parentDir / "data" / houseName / houseName / "overlap.csv");

	std::vector<vector<double>> pos;

	std::string line;

	std::getline(file, line);

	while (std::getline(file, line)) {
		if (line.empty()) {
			break;
		}

		std::stringstream ss(line);
		std::string token;

		//Station station;
		vector<double> tmpPos;

		// Read the coordinate data of each row
		std::getline(ss, token, ','); // Skip the site number

		std::getline(ss, token, ','); // Read the x coordinates
		tmpPos.push_back(std::stod(token));

		std::getline(ss, token, ',');// Read the y coordinate
		tmpPos.push_back(std::stod(token));

		// Store the read site coordinates into the vector
		pos.push_back(tmpPos);
	}

	std::vector<Station> stations = testData.opt->getStations();

	ASSERT_EQ(stations.size(), pos.size());

	for (size_t i = 0; i < stations.size(); ++i) {
		ASSERT_NEAR(stations[i].X(), pos[i][0], 1e-4);
		ASSERT_NEAR(stations[i].Y(), pos[i][1], 1e-4);
	}

};


INSTANTIATE_TEST_SUITE_P(
	RoomScan, RoomScanTest,
	::testing::ValuesIn(vecScanData),
	[](const testing::TestParamInfo<TestData>& info) {
		std::string name = info.param.houseName;
		return name;
	});

