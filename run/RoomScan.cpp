#include <boost/program_options.hpp>

#include "VPN/VpnOptimizerBySkeleton.h"

using namespace std;
namespace VPN = ViewPointNetwork;

bool PauseArguments(int argc, char** argv, std::string& houseName, VPN::OptSkelParam& param);

int main(int argc, char** argv)
{
	string houseName;
	VPN::OptSkelParam param;
	//PauseArguments(argc, argv, houseName, param);
	std::vector<VPN::Edge2D> edges;
	VPN::readEdges("C:\\Users\\3DV\\Desktop\\ourresult\\Zeng_SceneA\\Zeng_SceneA", edges, "Dxf");
	VPN::writeEdges("C:\\Users\\3DV\\Desktop\\ourresult\\Zeng_SceneA\\test", edges, "dxf");

	//VPN::VpnOptimizerBySkeleton optimizer(houseName, param);
	//optimizer.doOptimize();
	//optimizer.writeStation2Txt("refine2.txt");
	//optimizer.writeStation2HeilosXml("test.xml", "scene.xml#test", "D:\\platforms.xml#tripod", "D:\\scanners_tls.xml#faroFM70");

	return 0;
}

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
			("cell,c", po::value<double>(&p.cell)->default_value(0.01), "heatmap cell size, default value: 0.01m")
			("radiusMin,r", po::value<double>(&p.stationRadiusMin)->default_value(0.8), "statioin min radius, default value: 0.8m")
			("RadiusMax,R", po::value<double>(&p.stationRadiusMax)->default_value(30.0), "statioin max radius, default value: 30.0m")
			("overlap,l", po::value<double>(&p.overlapThresh)->default_value(0.5), "statioin overlap threshold, default value: 0.5")
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
