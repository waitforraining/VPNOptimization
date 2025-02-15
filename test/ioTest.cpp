#include <gtest/gtest.h>
#include <filesystem>
#include "IO/io.h"
#include <boost/program_options.hpp>
#include "VPN/VpnOptimizerBySkeleton.h"

using namespace ViewPointNetwork;
using namespace std;
namespace VPN = ViewPointNetwork;
//using namespace VPN;
//bsp搜索功能 测试两个数据 用固定参数，固定站点测试bsp扫描的边数长度是否一致 
//生成热力图 固定输入平面图 与之前代码中热力图矩阵中的值进行比较
//骨架线
//站点位置

bool PauseArguments(int argc, char** argv, std::string& houseName, VPN::OptSkelParam& p)
{
	try
	{
		int houseType;
		namespace po = boost::program_options;
		po::options_description desc("Options");
		desc.add_options()
			("help,h", "Print help messages")
			("name,i", po::value<std::string>(&houseName)->required(), "input house name")
			("type,t", po::value<int>(&houseType)->default_value(0), "house type, 0:indoor, 1:outdoor, default value: 0")
			("cell,c", po::value<double>(&p.cell)->default_value(0.025), "heatmap cell size, default value: 0.025m")
			("radiusMin,r", po::value<double>(&p.stationRadiusMin)->default_value(0.8), "statioin min radius, default value: 0.8m")
			("RadiusMax,R", po::value<double>(&p.stationRadiusMax)->default_value(30.0), "statioin max radius, default value: 30.0m")
			("overlap,l", po::value<double>(&p.overlapThresh)->default_value(0.4), "statioin overlap threshold, default value: 0.4")
			("score,s", po::value<double>(&p.scoreThresh)->default_value(1.0), "heatmap score threshold, default value: 1.0")
			("densify,d", po::value<double>(&p.denseStep)->default_value(0.2), "densify wall for overlap check, default value: 0.2m");

		po::variables_map vm;
		try
		{
			po::store(po::parse_command_line(argc, argv, desc), vm); // can throw 

			if (vm.count("help"))
			{
				std::cout << desc << std::endl;
				return true;
			}

			po::notify(vm);
		}
		catch (po::error& e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			return false;
		}

		p.houseType = static_cast<VPN::HouseType>(houseType);
	}
	catch (std::exception& e)
	{
		std::cerr << "Unhandled Exception reached the top of main: "
			<< e.what() << ", application will now exit" << std::endl;
		return false;
	}

	return true;
}

std::vector<Edge2D> ConstructRectangle()
{
	//Construct data for test overlap
	std::vector<Edge2D> edges;
	edges.emplace_back(Point2D(0, 0), Point2D(0, 1));
	edges.emplace_back(Point2D(0, 1), Point2D(1, 1));
	edges.emplace_back(Point2D(1, 1), Point2D(1, 0));
	edges.emplace_back(Point2D(1, 0), Point2D(0, 0));

	return edges;
}

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

std::string houseName;
VPN::OptSkelParam param;

// Main function for tests with command line arguments
int main(int argc, char** argv) {
	// First, parse custom arguments before running tests
	if (!PauseArguments(argc, argv, houseName, param)) {
		std::cerr << "Failed to parse arguments" << std::endl;
		return 1;
	}

	// Now run Google Test with any remaining command line args
	::testing::InitGoogleTest(&argc, argv);  // This will handle gtest-related command line options
	int result = RUN_ALL_TESTS();

	return result;
}

TEST(ReadTest, readTop) 
{
	std::vector<Edge2D> edges;
	std::vector<double> expectedLengths;
	readEdges(houseName, edges, "top");

	std::string filePath = houseName + "/readTop.csv";
	// save in csv file
	if (!std::filesystem::exists(filePath)) {
		std::ofstream outFile(filePath);
		outFile << "Edge Index,Length\n";
		for (size_t i = 0; i < edges.size(); ++i) {
			outFile << i << "," << edges[i].Length() << "\n";
		}
		outFile.close();
	}

	std::ifstream file(filePath);
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

TEST(ReadTest, readDxf)
{
	std::vector<Edge2D> edges;
	std::vector<double> expectedLengths;
	readEdges(houseName, edges, "dxf");

	std::string filePath = houseName + "/readDxf.csv";
	// save in csv file
	if (!std::filesystem::exists(filePath)) {
		std::ofstream outFile(filePath);
		outFile << "Edge Index,Length\n";
		for (size_t i = 0; i < edges.size(); ++i) {
			outFile << i << "," << edges[i].Length() << "\n";
		}
		outFile.close();
	}

	std::ifstream file(filePath);
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
		/*ASSERT_FLOAT_EQ(edges[i].getBegPoint().X(), rEdges[i].getBegPoint().X(), 1e-9);
		ASSERT_FLOAT_EQ(edges[i].getEndPoint().X(), rEdges[i].getEndPoint().X(), 1e-9);*/
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
		/*ASSERT_FLOAT_EQ(edges[i].getBegPoint().X(), rEdges[i].getBegPoint().X(), 1e-9);
		ASSERT_FLOAT_EQ(edges[i].getEndPoint().X(), rEdges[i].getEndPoint().X(), 1e-9);*/
	}
}

// Create a Test Fixture class
class RoomScanTest : public ::testing::Test {
protected:
	/*static string houseName;
	static VPN::OptSkelParam param;*/
	//std::vector<VPN::Edge2D> edges;

	static VPN::VpnOptimizerBySkeleton optimizer;
 
	// Called once at the beginning of the entire test suite to initialize shared resources
	static void SetUpTestSuite() {
		optimizer = VPN::VpnOptimizerBySkeleton(houseName, param);
		optimizer.doOptimize();
		// Initialize only once
	}

	// Called once at the end of the entire test suite to clean up resources
	static void TearDownTestSuite() {
		// If there are resources that need to be cleaned, they can be done here
		// In this example, no special cleaning is required
	}
	
};

// Static member initialization
//string RoomScanTest::houseName = "../data/Zeng_SceneA/Zeng_SceneA";
//VPN::OptSkelParam RoomScanTest::param;
//VPN::VpnOptimizerBySkeleton RoomScanTest::optimizer = VPN::VpnOptimizerBySkeleton(RoomScanTest::houseName, RoomScanTest::param);
VPN::VpnOptimizerBySkeleton RoomScanTest::optimizer;

TEST_F(RoomScanTest, testBsp) {
	//std::ifstream file(houseName +  "/bspEdges.csv");
	std::ifstream file("bspEdges.csv");
	std::vector<double> bspEdges;

	std::string line;

	std::getline(file, line); //Used to skip the header line

	while (std::getline(file, line)) {
		if (line.empty()) {
			break;
		}

		std::stringstream ss(line);
		std::string token;

		//Station station;
		double len;

		// Read the data for each row
		std::getline(ss, token, ','); // Skip the node name

		std::getline(ss, token, ','); // Read the side length
		len = std::stod(token);
		
		// Store the read site coordinates into the vector
		
		bspEdges.push_back(len);
	}

	BSPNode* root = optimizer.getHouse().getBspRoot().get();

	size_t index = 0;

	compareBsp(root, bspEdges,index);
};

TEST_F(RoomScanTest, testHeat) {
	// Read two images
	cv::Mat testHeat = cv::imread(houseName + "_RGBheat.png", cv::IMREAD_COLOR);
	cv::Mat groundTruth = cv::imread(houseName + "/RGBheat.png", cv::IMREAD_COLOR);

	// Check whether the image is loaded successfully
	ASSERT_FALSE(testHeat.empty());
	ASSERT_FALSE(groundTruth.empty());

	// Compare sizes
	ASSERT_EQ(testHeat.size(), groundTruth.size());

	// Compare image contents (pixel by pixel)
	ASSERT_TRUE(cv::countNonZero(testHeat != groundTruth) == 0);
};

TEST_F(RoomScanTest, testRidge) {
	// Read two images
	cv::Mat testRidge = cv::imread(houseName + "_zhang_skeleton.png",
		cv::IMREAD_COLOR);
	cv::Mat groundTruth = cv::imread(houseName + "/zhang_skeleton.png",
		cv::IMREAD_COLOR);

	// Check whether the image is loaded successfully
	ASSERT_FALSE(testRidge.empty());
	ASSERT_FALSE(groundTruth.empty());

	// Compare sizes
	ASSERT_EQ(testRidge.size(), groundTruth.size());

	// Compare image contents (pixel by pixel)
	ASSERT_TRUE(cv::countNonZero(testRidge != groundTruth) == 0);
};

TEST_F(RoomScanTest, testPos) {
	std::ifstream file(houseName + "/overlap.csv");
	std::vector<vector<double>> pos;
	//std::vector<Station> stations;

	std::string line;

	std::getline(file, line);// Used to skip the header line

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

	std::vector<Station> stations = optimizer.getStations();

	ASSERT_EQ(stations.size(), pos.size());

	for (size_t i = 0; i < stations.size(); ++i) {
		ASSERT_NEAR(stations[i].X(), pos[i][0], 1e-4);
		ASSERT_NEAR(stations[i].Y(), pos[i][1], 1e-4);
		/*ASSERT_FLOAT_EQ(stations[i].X(), pos[i][0], 1e-9);
		ASSERT_FLOAT_EQ(stations[i].Y(), pos[i][1], 1e-9);*/
	}

};