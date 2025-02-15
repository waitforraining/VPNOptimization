#include "VPN/VpnOptimizerBySkeleton.h"

using namespace std;
namespace ViewPointNetwork
{
	OptSkelParam::OptSkelParam()
	{
		houseType = HT_Indoor;
		cell = 0.025;
		stationRadiusMin = 0.8;
		stationRadiusMax = 30.0;
		overlapThresh = 0.4;
		scoreThresh = 1.0;
		denseStep = 0.2;
	}

	OptSkelParam::OptSkelParam(HouseType hType, double inCell,
		double stationRadMin, double stationRadMax,
		double ovlpThresh, double dnsStep)
	{
		houseType = hType;
		cell = inCell;
		stationRadiusMin = stationRadMin;
		stationRadiusMax = stationRadMax;
		overlapThresh = ovlpThresh;
		denseStep = dnsStep;
	}
	
	std::string& OptSkelParam::toString(const string& prefix,const string& format) const 
	{
		stringstream ss;
		ss << setprecision(3) << prefix;
		if (format.find('t') != format.npos)
			ss << "-t" << houseType;
		if (format.find('c') != format.npos)
			ss << "-c" << cell;
		if (format.find('r') != format.npos)
			ss << "-r" << stationRadiusMin;
		if (format.find('R') != format.npos)
			ss << "-R" << stationRadiusMax;
		if (format.find('l') != format.npos)
			ss << "-l" << overlapThresh;
		if (format.find('s') != format.npos)
			ss << "-s" << scoreThresh;
		if (format.find('d') != format.npos)
			ss << "-d" << denseStep;
		return ss.str();
	}

	VpnOptimizerBySkeleton::VpnOptimizerBySkeleton()
	{
		m_houseName = "000";
	}
	
	VpnOptimizerBySkeleton::VpnOptimizerBySkeleton(const std::string& houseName, const OptSkelParam& param)
	{
		m_param = param;
		m_houseName = houseName;
		//readEdge(houseName + "_move_min.objpts", houseName + "_move_min.top" ,m_vecEdges);
		readEdges(houseName, m_vecEdges,"top");
		//writeEdge("test", m_vecEdges);
	}

	VpnOptimizerBySkeleton::VpnOptimizerBySkeleton(const std::vector<Edge2D>& vecHouseEdges, const OptSkelParam& param)
	{
		m_param = param;
		m_vecEdges = vecHouseEdges;
	}

	VpnOptimizerBySkeleton::~VpnOptimizerBySkeleton()
	{
	}

	void VpnOptimizerBySkeleton::initialize()
	{
		initializeHouse();
		initializeHeatMap();
		initializeSklGraph();
		initializeNet();
		initializeVec();

	}


	void VpnOptimizerBySkeleton::getScanGroups()
	{
		set<int> remainStations, adjStations;
		for (int i = 0; i < m_stationNet.getStationsNum(); i++)
			remainStations.insert(i);
		int loop = 0;
		int maxScanCnt = 0;
		vector<int> maxScanInds;
		m_groupInd = 0;
		while (true) {
			if (loop == 0)
				getMaxScanEdgeCount(remainStations, maxScanInds, maxScanCnt);
			else
				getMaxScanEdgeCount(adjStations, maxScanInds, maxScanCnt);

			if (maxScanCnt == 0)
			{
				if (loop == 0) break;
				groupConnect(remainStations);
				m_groupInd++;
				adjStations.clear();
				loop = 0;
				continue;
			}

			// The best station was selected, 
			//taking into account the score and average overlap with all currentStations
			int maxInd = selectBestStation(maxScanInds);

			// Update the scan matrix
			updateScanMat(maxInd);
			updateAdjcentStation(maxInd, remainStations, adjStations);

			m_unScanedCnt -= maxScanCnt;
			m_groupStationUMap[m_groupInd].push_back(maxInd);
			m_usedStationInds.push_back(maxInd);
			remainStations.erase(maxInd);
			
			cout << "--------------------" << endl;
			std::cout << "site optimization the " << m_groupInd << " clustered and the " << loop++ << " wheel." << endl;
			std::cout << "have been added the initial site: no " << maxInd << "." << endl;
			std::cout << "Gets the maximum number of scans for a single site: " << maxScanCnt << "    Number of remaining unscanned edges: " << m_unScanedCnt << endl;
			std::cout << "Number of optimized sites currently: " << m_usedStationInds.size() << endl;
		}
		
		
		if (m_house.getType() == HT_Outdoor || m_usedStationInds.size() < 10) {
			// Calculate the path and connectivity matrix
			computePathsAndMat();
			// Close the network
			closeNet();
		}
		std::cout << "Site optimization complete." << endl;
		std::cout << "********************" << endl;
	}

	void VpnOptimizerBySkeleton::initializeHouse()
	{
		MoveMinPoint2Origin(m_vecEdges);
		m_house = House(m_vecEdges, m_param.houseType);
	}

	void VpnOptimizerBySkeleton::initializeHeatMap()
	{
		cout << "construct heatMap..." << endl;
		m_heatMap = HeatMap(m_house, m_param.cell);
		bool cache = false;
		if (!cache || !m_heatMap.loadGrayImg(m_houseName + "_" + to_string(m_param.cell) + "_GRAYheat.png"))
		{
			// not use cache
			m_heatMap.generate(m_house,
				m_param.stationRadiusMin, m_param.stationRadiusMax, m_param.scoreThresh);

			m_heatMap.saveBGR_YellowOrange(m_house, m_houseName, false);

			m_heatMap.saveGRAY(m_house, m_houseName + "_" + to_string(m_param.cell));
		}
	}


	void VpnOptimizerBySkeleton::initializeSklGraph()
	{
		//cv::Mat grayImg = m_heatMap.getGRAYImg();
		//debug
		cv::Mat grayImg = cv::imread(m_houseName + "_" + to_string(m_param.cell) + "_GRAYheat.png", cv::IMREAD_GRAYSCALE);
		RidgeDetection rd;
		cv::Mat skeletonImg = rd.getRidge(m_house, m_heatMap, grayImg, m_houseName, RT_L2, 5, m_param.houseType);

		m_sklGraph = SkeletonGraph(skeletonImg);
		m_sklGraph.getSkeletonGraph();
		m_sklGraph.initColor();
		m_sklGraph.addNewStations(5);
	}

	void VpnOptimizerBySkeleton::initializeNet()
	{
		auto& crossInd2Point = m_sklGraph.getCrossInd2Point();
		vector<Station> scanedStations;
		for (int i = 0; i < crossInd2Point.size(); i++)
		{
			Station s(crossInd2Point[i].Y() * m_heatMap.getCell(),
				(1.0 * m_heatMap.getYLen() - crossInd2Point[i].X()) * m_heatMap.getCell(),
				m_param.stationRadiusMin,
				m_param.stationRadiusMax);

			s.scan(m_house.getBspRoot().get());
			scanedStations.push_back(s);
		}

		m_stationNet = StationNet(std::move(scanedStations),m_param.overlapThresh);
		m_stationNet.computeMat();
		addStationForConnecting();
	}

	void VpnOptimizerBySkeleton::initializeVec()
	{
		m_denseEdges = Densify(m_house.getEdges(), m_param.denseStep);
		int edgeCount = m_denseEdges.size();
		int stationCount = m_stationNet.getStations().size();

		m_scanMat = vector<vector<bool>>(stationCount, vector<bool>(edgeCount, false));
		m_edgeScanned = vector<bool>(edgeCount, false);
		m_unScanedCnt = edgeCount;
		double minPointDis = 0.025;
		const auto& stations = m_stationNet.getStations();
		vector<Edge2D> mm,mm1;
		for (size_t i = 0; i < stations.size(); i++) {
			const auto& curStation = stations[i];
			const auto& scannedEdges = curStation.getScanedEdges();
			std::vector<Edge2D> vecEdges = curStation.constructStationCircle();
			if (scannedEdges.empty()) continue;
			for (auto j = scannedEdges.begin(); j != scannedEdges.end(); j++) {
				const auto& curEdge = j->second;

				mm1.push_back(curEdge);
				for (size_t k = 0; k < m_denseEdges.size(); k++) {

					if (m_denseEdges[k].Overlap(curEdge).Length() > m_denseEdges[k].Length() * 0.9 &&
						curStation.isInValidArea(m_denseEdges[k]) &&
						curStation.isInValidResolutionArea(m_denseEdges[k], minPointDis))
					{
						mm.push_back(m_denseEdges[k]);
						m_scanMat[i][k] = true;
					}
				}				
			}

			auto startEdge = scannedEdges.begin()->second;
			auto endEdge = scannedEdges.rbegin()->second;
			if (ParallelLines(startEdge.getLine2D(), endEdge.getLine2D(), 0.001 * M_PI / 180.0))
			{
				auto mergeEdge = startEdge + endEdge;
				for (size_t k = 0; k < m_denseEdges.size(); k++) {
					if (m_denseEdges[k].Overlap(mergeEdge).Length() > m_denseEdges[k].Length() * 0.9 &&
						curStation.isInValidArea(m_denseEdges[k]) &&
						curStation.isInValidResolutionArea(m_denseEdges[k], minPointDis))
					{
						mm.push_back(m_denseEdges[k]);
						m_scanMat[i][k] = true;
					}
				}
			}
			//writeEdges(to_string(i) + "sation", mm);
			//mm.clear();

		}
	}

	void VpnOptimizerBySkeleton::doOptimize()
	{
		initialize();
		getScanGroups();
		m_stations.clear();
		for (int i = 0; i < m_usedStationInds.size(); i++)
			m_stations.push_back(m_stationNet.getStation(m_usedStationInds[i]));

		drawWholeHouse(m_houseName);
		
		StationNet netSn(m_stations);
		netSn.writeStationOverlapMatrix(m_houseName + "_overlap.csv");

	}
	void VpnOptimizerBySkeleton::drawWholeHouse(const string& outresultpath) const
	{
		// Show the initial site house map
		cv::Mat heatRgbScore = cv::imread(m_houseName + "_RGBheat.png", cv::IMREAD_COLOR);
		if (heatRgbScore.empty()) {
			std::cerr << "Error: Could not read the image." << std::endl;
			return;
			// Handle error situations
		}
		cv::Mat figure;

		figure = heatRgbScore.clone();
		drawSkeletonGraph(figure, m_sklGraph);
		drawStations(figure, m_sklGraph, m_stations, m_heatMap.getCell(), 5);

		stringstream ss;
		ss << setprecision(3)
			<< outresultpath
			<< "-t" << m_param.houseType
			<< "-c" << m_param.cell
			<< "-r" << m_param.stationRadiusMin
			<< "-R" << m_param.stationRadiusMax
			<< "-l" << m_param.overlapThresh
			<< "-s" << m_param.scoreThresh
			<< "-d" << m_param.denseStep;

		cv::imwrite(ss.str() + "_refinedStations.png", figure);
		
	
	}
	void VpnOptimizerBySkeleton::addStationForConnecting()
	{
		double overlapThresh = m_param.overlapThresh;
		double r_min = m_param.stationRadiusMin; 
		double r_max = m_param.stationRadiusMax;
		cout << "Add connected sites to ensure overlap..." << endl;

		bool flag = false;

		queue<pair<int, int>> pq;
		int jointNum = m_sklGraph.getJointNum();
		vector<vector<double>> stationOverlapMat = m_stationNet.getOverlapMat();
		for (size_t i = 0; i < jointNum; ++i)
		{
			for (size_t j = i + 1; j < jointNum; ++j)
			{
				if (m_sklGraph.adjMat[i][j] == 0
					|| m_stationNet.getOverlapMat(i, j) >= overlapThresh)
					continue;
				
				int pathSize = m_sklGraph.paths[i][j].size();

				pq.emplace(i, j);
			}
		}

		while (!pq.empty())
		{
			auto jointNum = pq.front();
			pq.pop();
			auto i = jointNum.first;
			auto j = jointNum.second;
			int pathSize = m_sklGraph.getPath(i, j).size();

			if (m_stationNet.getOverlapMat(i, j) >= overlapThresh
				|| pathSize <= 3) continue;

			SkeletonJoint& p1 = m_sklGraph.crossInd2Point[i];
			SkeletonJoint& p2 = m_sklGraph.crossInd2Point[j];

			SkeletonJoint connectStation = m_sklGraph.getPath(i, j)[pathSize / 2];
			connectStation.setJunctionType(JunctionType::JT_ADDOVERLAP);// Path center point added to ensure overlap

			m_sklGraph.updatePathsByNewJoint(i, j, pathSize / 2, connectStation);
			// Update stationNet
			Station s(connectStation.Y() * m_heatMap.getCell(),
				(1.0 * m_heatMap.getYLen() - connectStation.X()) * m_heatMap.getCell(),
				r_min, r_max);
			s.scan(m_house.getBspRoot().get());
			m_stationNet.addStation(s);
			pq.emplace(i, m_sklGraph.getJointNum() - 1);
			pq.emplace(j, m_sklGraph.getJointNum() - 1);

			cout << "p1: " << m_sklGraph.point2CrossInd[p1] << "-(" << p1.X() << "," << p1.Y() << ")"
			<< " p2: " << m_sklGraph.point2CrossInd[p2] << "-(" << p2.X() << "," << p2.Y() << ")"
			<< " connectStation: " << m_sklGraph.getJointNum() - 1 << "-(" << connectStation.X() << "," << connectStation.Y() << ")" << endl;
		}
		cout << "Connection site added complete." << endl;
	}


	// scene = "data/scenes/scene/03401.xml#test" platform = "data/platforms.xml#tripod" scanner = "data/scanners_tls.xml#faroFM70">
	void VpnOptimizerBySkeleton::writeStation2HeilosXml(const std::string& resultFile, const std::string& scenePath,
		const std::string& platformPath, const std::string& scanner,
		const std::string& surveyName)
	{
		//auto stations = m_stations;
		//boost::property_tree::ptree tree;
		//// scanSetting
		//auto& scansetting = tree.add("document.scannerSettings", "");

		//scansetting.put("<xmlattr>.id", "scanner1");
		//scansetting.put("<xmlattr>.active", "true");
		//scansetting.put("<xmlattr>.pulseFreq_hz", "600000");
		//scansetting.put("<xmlattr>.scanFreq_hz", "83");
		//scansetting.put("<xmlattr>.headRotatePerSec_deg", "15");
		//scansetting.put("<xmlattr>.verticalAngleMin_deg", "-60");
		//scansetting.put("<xmlattr>.verticalAngleMax_deg", "240");
		//scansetting.put("<xmlattr>.headRotateStart_deg", "0");
		//scansetting.put("<xmlattr>.headRotateStop_deg", "360");

		//// surveyNode
		//auto& surveyNode = tree.add("document.survey", "");
		//surveyNode.put("<xmlattr>.name", surveyName);
		//surveyNode.put("<xmlattr>.scene", scenePath);
		//surveyNode.put("<xmlattr>.platform", platformPath);
		//surveyNode.put("<xmlattr>.scanner", scanner);

		//for (int i = 0; i < stations.size(); i++)
		//{
		//	auto& legNode = surveyNode.add("leg", "");
		//	legNode.put("<xmlattr>.stripId", i);

		//	auto& platformSettingsNode = legNode.add("platformSettings", "");
		//	platformSettingsNode.put("<xmlattr>.x", stations[i].X());
		//	platformSettingsNode.put("<xmlattr>.y", stations[i].Y());
		//	platformSettingsNode.put("<xmlattr>.z", "-1");
		//	platformSettingsNode.put("<xmlattr>.onGround", "false");

		//	auto& scannerSettingsNode = legNode.add("scannerSettings", "");
		//	scannerSettingsNode.put("<xmlattr>.template", "scanner1");
		//}
		//boost::property_tree::xml_writer_settings<string> settings('\t', 4);
		//// Ð´³ö XML ÎÄ¼þ
		//try {
		//	boost::property_tree::write_xml(resultFile, tree, std::locale(), settings);
		//}
		//catch (...)
		//{
		//	cout << "error write to " << resultFile << endl;
		//}
	}

	void VpnOptimizerBySkeleton::writeStation2Txt(const string& outputTxtPath)
	{
		fstream outfile(outputTxtPath, ios::out);
		auto stations = m_stations;

		if (outfile.is_open()) {
			for (auto station : stations)
				outfile << fixed << setprecision(4) << station.X() << " " << station.Y() << endl;

			outfile.close();
		}
		else {
			std::cerr << "Failed to open the file: " << outputTxtPath << std::endl;
		}
	}

	void VpnOptimizerBySkeleton::updateScanMat(int maxScanInd)
	{
		auto maxScanEdges = m_scanMat[maxScanInd];
		for (int k = 0; k < maxScanEdges.size(); k++) 
		{
			if (maxScanEdges[k])
				m_edgeScanned[k] = true;
		}

		// Update the scan matrix
		for (auto& seeEdges : m_scanMat) {  // Use references to modify the original data
			for (int k = 0; k < maxScanEdges.size(); k++) {
				if (maxScanEdges[k]) {  
					seeEdges[k] = false;
				}
			}
		}
	}

	double VpnOptimizerBySkeleton::getMaxOverlap(int curInd)
	{
		const auto& stationOverlapMat = m_stationNet.getOverlapMat();
		if (m_usedStationInds.empty()) return 0.0;
		if (curInd < 0 || curInd >= stationOverlapMat.size())
			return 0.0;

		double maxOvlp = 0, temOvlp;
		int maxInd = 0;
		for (size_t i = 0; i < m_usedStationInds.size(); i++) {

			temOvlp = stationOverlapMat[curInd][m_usedStationInds[i]];

			if (temOvlp > maxOvlp)
			{
				maxOvlp = temOvlp;
				maxInd = m_usedStationInds[i];
			}
		}

		return maxOvlp;
	}

	// If the supplementary edge is the same, 
	// select the site with the highest overlap with the existing site group
	// It is best to choose the original skeleton point
	int VpnOptimizerBySkeleton::selectBestStation(const vector<int>& candidateInds)
	{
		if (candidateInds.size() == 1)
			return candidateInds[0];

		int temInd, maxInd = candidateInds[0];
		double maxScore = 0.0, temScore = 0.0;

		for (size_t i = 0; i < candidateInds.size(); i++) {
			temInd = candidateInds[i];

			if (m_usedStationInds.empty())// First selection, no seed station, with the maximum scan score, subsequent can use skeleton points
				temScore = m_stationNet.getStation(temInd).getScanedScore();
			else// If there are already seed stations, 
				//select the station with the greatest overlap with these seed stations
				temScore = getMaxOverlap(temInd);

			if (temScore > maxScore) {
				maxScore = temScore;
				maxInd = temInd;
			}

	
			
		}

		return maxInd;
	}

	void VpnOptimizerBySkeleton::getMaxScanEdgeCount(set<int>& stationId, vector<int>& maxScanStationId, int& maxScanCnt)
	{
		maxScanCnt = 0;
		maxScanStationId.clear();
		for (auto ind : stationId)
		{
			if (isIsolate(ind))
				continue;

			int scanCnt = 0;
			for (int k = 0; k < m_scanMat[ind].size(); k++) {
				if (m_scanMat[ind][k])
					scanCnt++;
			}
			//cout << ind << " " << scanCnt <<" "<<  m_stationNet.getStation(ind).X() << " " << m_stationNet.getStation(ind).Y() << endl;
			if (scanCnt < maxScanCnt) continue;
			
			if (scanCnt > maxScanCnt)
			{
				maxScanCnt = scanCnt;
				maxScanStationId.clear();
			}
			maxScanStationId.push_back(ind);
		}
	}

	void VpnOptimizerBySkeleton::computePathsAndMat()
	{
		//int n = m_stations.size(); // Number of sites
		int n = m_usedStationInds.size();
		// Initialize stationPaths as 3D vectors [n][n][]
		m_stationPaths.resize(n);
		for (int i = 0; i < n; ++i) {
			m_stationPaths[i].resize(n);
		}

		// Initializes conMat as a two-dimensional Boolean matrix [n][n] with a default value of false
		m_conMat.resize(n, std::vector<bool>(n, false));
		

		double cell = m_heatMap.getCell();
		vector<Station> allInitStations = m_stationNet.getStations();

		for (size_t i = 0; i < m_usedStationInds.size() - 1; ++i) {
			int srcInd = m_usedStationInds[i];
			for (size_t j = i + 1; j < m_usedStationInds.size(); ++j) {
				int dstInd = m_usedStationInds[j];

				bool flag = true;
				// Determine whether it is directly connected (there is no other site in the middle)
				vector<SkeletonJoint> SrcToDstPath = m_sklGraph.paths[srcInd][dstInd];

				vector<int> midStationInds;
				//cout << oriKeyInd << " " << newGroupKeyInd << endl;
				int staInd = -1;
				for (size_t m = 0; m < SrcToDstPath.size(); ++m) {
					/*for (size_t j = 0; j < usedStationInds.size(); ++j) {
						staInd = usedStationInds[j];*/
					for (size_t k = 0; k < allInitStations.size();++k){
						staInd = k;
						if (staInd == srcInd || staInd == dstInd) {
							continue;
						}
						
						Station s(SrcToDstPath[m].Y() * cell,
							(1.0 * m_sklGraph.getSkeleton().rows - SrcToDstPath[m].X()) * cell,
							m_param.stationRadiusMin, m_param.stationRadiusMax);
						/*Point2D sPnt = s.getThisPoint();
						if (sPnt == m_stations[staInd].getThisPoint()) {*/
						if (s == allInitStations[staInd]){
							//midStationInds.push_back(staInd);
							//DOTO solve the problem of why there are duplicate sites instead of deciding not to add them
							if (find(midStationInds.begin(), midStationInds.end(), staInd) == midStationInds.end()) {
								midStationInds.push_back(staInd);
							}
							//midStationInds.push_back(staInd);
							if (find(m_usedStationInds.begin(), m_usedStationInds.end(), staInd) != m_usedStationInds.end()) {
								flag = false;
							}
							break;
						}
					}
				}

				m_stationPaths[i][j] = midStationInds;
				reverse(midStationInds.begin(), midStationInds.end());
				m_stationPaths[j][i] = midStationInds;

				m_conMat[i][j] = flag;
				m_conMat[j][i] = flag;
			}
		}
 	}

	void VpnOptimizerBySkeleton::closeNet()
	{
		cout << "--------------------" << endl;
		cout << "********************" << endl;
		cout << "Start closing the site network..." << endl;
		const auto& stationOverlapMat = m_stationNet.getOverlapMat();
		size_t oriSize = m_usedStationInds.size();
		vector<int> midStationInds;
		int startInd = 0;
		int nextStartInd = 0;
		int conInd = -1;
		vector<int> conInds;
		bool pathVisited = false;

		for (size_t i = 0; i < oriSize - 1; ++i) {
			int srcInd = m_usedStationInds[i];
			int curInd = srcInd;
			for (size_t j = i + 1; j < oriSize; ++j) {
				int dstInd = m_usedStationInds[j];
				// Connectivity, overlap may not be enough
				if (m_conMat[i][j]) {
					double srcToDstOverlap = stationOverlapMat[srcInd][dstInd];
					cout << "overlap" << i << " to " << j << ": " << srcToDstOverlap << endl;
					if (srcToDstOverlap >= m_param.overlapThresh) { continue; }

					startInd = 0;
					nextStartInd = 0;
					curInd = srcInd;
					conInd = -1;
					pathVisited = false;

					//vector<int> midStationInds = stationPaths[srcInd][dstInd];
					midStationInds = m_stationPaths[i][j];

					if (midStationInds.size() == 0) {
						continue;
					}
					/*if (i == 12 && j == 21) {
						cout << "do" << endl;
					}*/
					while (true) {
						//nextStartInd Indicates the start index number for the next loop to find the join point
						double maxOverlapToDes = 0.0;
						for (; startInd < midStationInds.size(); startInd++) {
							int midInd = midStationInds[startInd];

							/*cout << "cur: " << curInd << endl;
							cout << "mid: " << midInd << endl;*/
							double overlapSrcToMid = stationOverlapMat[curInd][midInd];
							double overlapMidToDes = stationOverlapMat[midInd][dstInd];

							/*cout << "over1: " << overlapSrcToMid << endl;
							cout << "over2: " << overlapMidToDes << endl << endl;*/
							cout << "src: " << curInd << " mid: " << midInd << " dst: " << dstInd << endl;
							cout << "over1: " << overlapSrcToMid << "    " << "over2: " << overlapMidToDes << endl;

							if (overlapSrcToMid >= m_param.overlapThresh && overlapMidToDes >= maxOverlapToDes) {
								maxOverlapToDes = overlapMidToDes;
								conInd = midInd;
								nextStartInd = startInd + 1;
							}
						}

						if (find(m_usedStationInds.begin(), m_usedStationInds.end(), conInd) != m_usedStationInds.end()) {
							pathVisited = true;
							break;
						}
						// Adding the same point repeatedly causes an endless loop. TODO resolves the problem
						if (!conInds.empty() && conInd == conInds[conInds.size() - 1]) {
							break;
						}
						if (conInd == -1) {
							cout << "No closure found on path.Exit." << endl;
							break;
						}
						//cout << "conInd:" << conInd << endl;
						conInds.push_back(conInd);
						curInd = conInd;
						startInd = nextStartInd;
						if (maxOverlapToDes >= m_param.overlapThresh) {
							break;
						}
						//cout << "1" << endl;
					}

					if (pathVisited) {
						break;
					}

					for (size_t i = 0; i < conInds.size(); ++i) {
						m_usedStationInds.push_back(conInds[i]);
						m_sklGraph.color_type.push_back(0);
						cout << "Add closed site number: " << conInds[i] << "    Number of currently optimized sites: " << m_usedStationInds.size() << endl;
					}
					midStationInds.clear();
					conInds.clear();

				}
			}
		}//
		cout << "********************" << endl;
	}

	double VpnOptimizerBySkeleton::getGroupOverlapMaxStation(int group0, 
		std::array<int, 2>& maxOverlapInd)
	{
		const auto& stationOverlapMat = m_stationNet.getOverlapMat();
		double maxOverlap = -1.0;
		for (int preGroupId = 0; preGroupId < m_groupInd; preGroupId++)
		{
			for (auto stationInd0 : m_groupStationUMap[preGroupId])
			{
				for (auto stationInd1 : m_groupStationUMap[group0])
				{
					double tmpOverlap = stationOverlapMat[stationInd0][stationInd1];

					if (tmpOverlap > maxOverlap) {
						maxOverlap = tmpOverlap;
						maxOverlapInd[0] = stationInd0;
						maxOverlapInd[1] = stationInd1;
					}
				}
			}
		}
		return maxOverlap;
	}

	void VpnOptimizerBySkeleton::groupConnect(std::set<int>& remainStations)
	{
		if (m_groupInd == 0) return;
		std::array<int, 2> maxOverlapInd;

		cout << "Add connection points between clusters..." << endl;
		double maxOverlap = getGroupOverlapMaxStation(m_groupInd, maxOverlapInd);
		if (maxOverlap >= m_param.overlapThresh
			|| maxOverlap < 0) return;

		int stationInd0 = maxOverlapInd[0];
		int stationInd1 = maxOverlapInd[1];

		vector<SkeletonJoint> SrcToDesPath = m_sklGraph.paths[stationInd0][stationInd1];
		vector<int> midStationInds;
		double pathX, pathY;
		for (size_t i = 0; i < SrcToDesPath.size(); ++i) {

			for(auto stationInd : remainStations)
			{
				pathX = 1.0 * SrcToDesPath[i].Y() * m_heatMap.getCell();
				pathY =  (1.0 * m_heatMap.getYLen() - SrcToDesPath[i].X()) * m_heatMap.getCell();

				if (abs(pathX - m_stationNet.getStation(stationInd).X())<1e-5
				    && abs(pathY - m_stationNet.getStation(stationInd).Y()) < 1e-5) 
				{
					midStationInds.push_back(stationInd);
					break;
				}
			}
		}

		if (midStationInds.empty()) {
			cout << "Cluster connection point not found." << endl;
			return;
		}
		const auto& stationOverlapMat = m_stationNet.getOverlapMat();
		int startInd = 0;
		int nextStartInd = 0;
		int curInd = stationInd0;
		vector<int> conInds;
		while (true) {
			//nextStartInd Indicates the start index number for the next loop to find the join point
			double maxOverlapToDes = 0.0;
			int conInd = -1;

			for (; startInd < midStationInds.size(); startInd++) {
				int midInd = midStationInds[startInd];

				double overlapSrcToMid = stationOverlapMat[curInd][midInd];
 				double overlapMidToDes = stationOverlapMat[midInd][stationInd1];

				if (overlapSrcToMid >= m_param.overlapThresh && overlapMidToDes >= maxOverlapToDes) {
					maxOverlapToDes = overlapMidToDes;
					conInd = midInd;
					nextStartInd = startInd + 1;
				}
			}
			// Adding the same point repeatedly causes an endless loop. TODO resolves the problem
			if (!conInds.empty() && conInd == conInds[conInds.size() - 1]) {
				break;
			}
			if (conInd == -1) {
				cout << "No join point found on path.Exit." << endl;
				break;
			}

			conInds.push_back(conInd);
			curInd = conInd;
			startInd = nextStartInd;
			if (maxOverlapToDes >= m_param.overlapThresh) {
				break;
			}
				
		}
		for (size_t i = 0; i < conInds.size(); ++i) {
			m_usedStationInds.push_back(conInds[i]);
			m_groupStationUMap[m_groupInd].push_back(conInds[i]);
			remainStations.erase(conInds[i]);
			cout << "Add the connection site: no " << conInds[i] << endl;
		}

		cout << "Cluster connection point added complete." << endl;
		cout << "Number of optimized sites currently: " << m_usedStationInds.size() << endl;
		std::cout << "********************" << endl;
	}

	bool VpnOptimizerBySkeleton::isIsolate(int ind)
	{
		int connectCnt = 0;
		for (size_t i = 0; i < m_sklGraph.connectMat.size(); ++i) {
			if (i == ind) continue;
			if (m_sklGraph.connectMat[ind][i]) {
				connectCnt++;
			}
		}

		return connectCnt < m_stationNet.getStationsNum() / 10;
	}

	void VpnOptimizerBySkeleton::updateAdjcentStation(int curInd, std::set<int>& remainStations,
		std::set<int>& adjStations)
	{
		if (adjStations.count(curInd))
			adjStations.erase(curInd);

		const auto& stationOverlapMat = m_stationNet.getOverlapMat();
		// Finds the adjacency of the current site and adds it to the adjacency sequence
		for (auto remainStationInd: remainStations) {
			if (remainStationInd == curInd) continue;
			if (adjStations.count(remainStationInd)) continue;
			if (stationOverlapMat[curInd][remainStationInd] < m_param.overlapThresh) continue;

			adjStations.insert(remainStationInd);
		}
	}
}