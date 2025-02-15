#include "IO/io.h"
using namespace std;
using namespace ViewPointNetwork;

#define comment_char '#'

#define NUMBER_H    "%c Number of points in this file : %d\n"

#define O_CONTENTS_H    "%c This file contains object points\n"
#define O_EXPL_H   "%c                       coordinates                                   variances                                    covariances\n\
%c point        x                y              z              x              y              z               xy             xz             yz\n"
#define O_LINE_H   "%c-----------------------------------------------------------------------------------------------------------------------------------------------\n"
#define O_FORMAT   " %4d%17.7lf%17.7lf%15.7lf%16.7le%15.7le%15.7le%16.7le%15.7le%15.7le\n"
#define O_FORMAT2   " %4d%16.7lf%16.7lf%15.7lf\n"

#define OP_EXPL_H   "%c                 coordinates                    variances                  covariances\n\
%cpoint    x           y           z        x        y        z        xy        xz        yz\n"
#define OP_LINE_H   "%c-----------------------------------------------------------------------------------------------\n"
#define OP_FORMAT  "%4d%11.5lf,%11.5lf,%11.5lf%9.2le%9.2le%9.2le%10.2le%10.2le%10.2le\n"

#define M_CONTENTS_H    "%c This file contains model points\n"
#define M_EXPL_H   "%c                       coordinates                                   variances                                    covariances\n\
%c point        x                y              z              x              y              z               xy             xz             yz\n"
#define M_LINE_H   "%c-----------------------------------------------------------------------------------------------------------------------------------------------\n"
#define M_FORMAT   " %4d%17.7lf%17.7lf%15.7lf%16.7le%15.7le%15.7le%16.7le%15.7le%15.7le\n"

#define MP_EXPL_H   "%c                 coordinates                    variances                  covariances\n\
%cpoint    x           y           z        x        y        z        xy        xz        yz\n"
#define MP_LINE_H   "%c-----------------------------------------------------------------------------------------------\n"
#define MP_FORMAT  "%4d%11.5lf,%11.5lf,%11.5lf%9.2le%9.2le%9.2le%10.2le%10.2le%10.2le\n"

#define C_CONTENTS_H   "%c This file contains control points\n"
#define C_STATUS_EXPL_H "%c The status field has the following meaning :\n\
%c\t0 -> Full control point (x, y and z known)\n\
%c\t1 -> Planimetric control point (x and y known, z is not to be used)\n\
%c\t2 -> Height control point (only z is known and can be used)\n\
%c\t3 -> No control point, all coordinates are unknown\n"
#define C_EXPL_H   "%c Pointnumber         x               y              z        status\n"
#define C_LINE_H   "%c------------- -------------- --------------- -------------- --------\n"
#define C_FORMAT  " %9d%16.3lf%16.3lf%15.3lf%9d\n"

namespace ViewPointNetwork
{
	void readPointsFromObjpts(const std::string& objptsPath, vector<Point2D>& points)
	{
		std::ifstream inputFile(objptsPath);

		string line;
		std::getline(inputFile, line);
		int tmp;
		double x, y;
		while (std::getline(inputFile, line)) {
			if (line[0] == '#') {
				continue;  // Skip lines starting with #
			}
			std::istringstream iss(line);
			if (iss >> tmp >> x >> y) {
				points.emplace_back(x, y);
			}
		}
		inputFile.close();
	}


	void IOTop::readEdges(const std::string& inFileName, vector<Edge2D>& edges)
	{
		vector<Point2D> points;
		readPointsFromObjpts(inFileName + ".objpts", points);
		std::ifstream inputFile(inFileName + ".top");
		if (!inputFile || points.empty())
		{
			return;
		}
		string line;
		std::getline(inputFile, line);
		int lineNumber, pointNums, attributeNums;
		int tmp;
		int ind0, ind1;
		while (std::getline(inputFile, line)) {
			if (line[0] == '#') {
				continue;  // Skip lines starting with #
			}
			std::istringstream iss(line);
			if (iss >> lineNumber >> pointNums >> attributeNums) {
				for (int i = 0; i < attributeNums; i++)
				{
					inputFile >> tmp >> tmp;
				}
				inputFile >> ind0 >> ind1;
				edges.emplace_back(points[ind0], points[ind1]);
			}
		}
		inputFile.close();
	}

	void IOTop::writeEdges(const std::string& outFileName, const std::vector<Edge2D>& edges)
	{
		FILE* fp;

		if ((fp = fopen(string(outFileName + ".objpts").c_str(), "w")) == NULL)
			return;

		/*------ Print the headers in the database file --------------------------*/
		fprintf(fp, "%d # file identifier\n", 702);
		fprintf(fp, O_CONTENTS_H, comment_char);
		fprintf(fp, NUMBER_H, comment_char, (int)edges.size() * 2);
		fprintf(fp, O_EXPL_H, comment_char, comment_char);
		fprintf(fp, O_LINE_H, comment_char);


		ofstream out(outFileName + ".top", ios::out);
		out << 716 << " # file ID" << "\n";
		out << "# Line topology database :" << "\n";
		out << "# Number of lines :" << "\t" << edges.size() << "\n";
		out << "# Line number" << "   \\   " << "Number of points" << "   \\   " << "Number of attributes" << "\n";
		out << "# Attributes Tag number" << "   \\   " << "Attribute value" << "\n";
		out << "# Point numbers" << "\n";
		out << "# -----------------------------------------------------------------------------------------------------------" << "\n";

		int pointInd = 0;
		for (int i = 0; i < edges.size(); i++)
		{
			auto p0 = edges[i].getBegPoint();
			fprintf(fp, O_FORMAT, pointInd, p0.X(), p0.Y(), 0.0,
				0, 0, 0, 0, 0, 0);
			p0 = edges[i].getEndPoint();
			fprintf(fp, O_FORMAT, pointInd + 1, p0.X(), p0.Y(), 0.0,
				0, 0, 0, 0, 0, 0);
			out << i << "\t" << 2 << "\t" << 0 << "\n";
			out << pointInd << "\t" << pointInd + 1 << "\n";
			pointInd += 2;
		}
		fclose(fp);
		out.close();
	}


	void IODXF::readEdges(const std::string& inFileName, std::vector<Edge2D>& edges)
	{
		OGRRegisterAll();
		string dxfPath = inFileName + ".DXF";
		GDALDataset* poDS = (GDALDataset*)GDALOpenEx(dxfPath.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
		if (poDS == NULL) {
			std::cout << "Open failed.\n";
			return;
		}
		Point2D tmpP;
		for (int layer_id = 0; layer_id < poDS->GetLayerCount(); ++layer_id)
		{
			OGRLayer* poLayer = poDS->GetLayer(layer_id);
			if (poLayer == NULL) continue;

			OGRFeature* poFeature;
			poLayer->ResetReading();
			while ((poFeature = poLayer->GetNextFeature()) != NULL)
			{
				OGRGeometry* poGeometry = poFeature->GetGeometryRef();
				if (!poGeometry) continue;

				if (wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon ||
					wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon25D) {

					OGRPolygon* ptrPolygon = (OGRPolygon*)poGeometry;
					int inRingNum = ptrPolygon->getNumInteriorRings();
					//If have interior ring
					if (inRingNum != 0) {
						for (int i = 0; i < inRingNum; ++i) {
							OGRLineString* ptrInRing = ptrPolygon->getInteriorRing(i);
							int pntNum = ptrInRing->getNumPoints();
							for (int i = 0; i < pntNum - 1; ++i) {
								edges.emplace_back(
									Point2D(ptrInRing->getX(i), ptrInRing->getY(i)),
									Point2D(ptrInRing->getX(i + 1), ptrInRing->getY(i + 1)));
							}
						}

					}

					OGRLinearRing* ptrExtRing = ptrPolygon->getExteriorRing();
					if (!ptrExtRing) continue;

					int pntNum = ptrExtRing->getNumPoints();
					for (int i = 0; i < pntNum - 1; ++i) {
						edges.emplace_back(
							Point2D(ptrExtRing->getX(i), ptrExtRing->getY(i)),
							Point2D(ptrExtRing->getX(i + 1), ptrExtRing->getY(i + 1)));
					}

				}
				else if (wkbFlatten(poGeometry->getGeometryType()) == wkbLineString ||
					wkbFlatten(poGeometry->getGeometryType()) == wkbLineString25D)
				{

					OGRLineString* ptrPolyline = (OGRLineString*)poGeometry;

					for (int i = 0; i < ptrPolyline->getNumPoints() - 1; ++i)
					{
						edges.emplace_back(
							Point2D(ptrPolyline->getX(i), ptrPolyline->getY(i)),
							Point2D(ptrPolyline->getX(i + 1), ptrPolyline->getY(i + 1)));
					}
				}
				OGRFeature::DestroyFeature(poFeature);
			}
		}
		GDALClose(poDS);

	}

	/*void IODXF::writeEdges(const std::string& outFileName, const std::vector<Edge2D>& edges)
	{
		GDALDriver* poDriver;

		GDALAllRegister();

		poDriver = GetGDALDriverManager()->GetDriverByName("DXF");

		if (poDriver == NULL)
		{
			return;
		}

		GDALDataset* poDS;
		poDS = poDriver->Create((outFileName + ".DXF").c_str(), 0, 0, 0, GDT_Unknown, NULL);

		if (poDS == NULL)
		{
			printf("Creation of output file failed.\n");
			return;
		}
		OGRLayer* poLayer;

		poLayer = poDS->CreateLayer("line", NULL, wkbPoint, NULL);
		if (poLayer == NULL)
		{
			printf("Layer creation failed.\n");
			return;
		}

		for (const auto& edge : edges)
		{
			OGRLineString lineString;
			lineString.addPoint(edge.getBegPoint().X(), edge.getBegPoint().Y(), 0.0);
			lineString.addPoint(edge.getEndPoint().X(), edge.getEndPoint().Y(), 0.0);
			OGRFeature* feature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
			feature->SetGeometry(&lineString);

			OGRFeature::DestroyFeature(feature);
		}
		GDALClose(poDS);

	}*/

	void IODXF::writeEdges(const std::string& outFileName, const std::vector<Edge2D>& edges)
	{
		GDALDriver* poDriver;

		// 注册所有驱动
		GDALAllRegister();

		// 获取DXF驱动
		poDriver = GetGDALDriverManager()->GetDriverByName("DXF");
		if (poDriver == NULL)
		{
			printf("DXF driver not available.\n");
			return;
		}

		// create DXF file
		GDALDataset* poDS = poDriver->Create((outFileName + ".DXF").c_str(), 0, 0, 0, GDT_Unknown, NULL);
		
		if (poDS == NULL)
		{
			printf("Creation of output file failed.\n");
			return;
		}

		OGRLayer* poLayer = poDS->CreateLayer("lines", NULL, wkbLineString, NULL);
		if (poLayer == NULL)
		{
			printf("Layer creation failed.\n");
			GDALClose(poDS);
			return;
		}

		for (size_t i = 0; i < edges.size(); ++i)
		{
			const Edge2D& edge = edges[i];

			OGRLineString lineString;
			lineString.addPoint(edge.getBegPoint().X(), edge.getBegPoint().Y(), 0.0);  // 添加起点
			lineString.addPoint(edge.getEndPoint().X(), edge.getEndPoint().Y(), 0.0);  // 添加终点

			OGRFeature* feature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
			feature->SetGeometry(&lineString);

			if (poLayer->CreateFeature(feature) != OGRERR_NONE)
			{
				printf("Failed to create feature for edge %zu\n", i);
			}

			OGRFeature::DestroyFeature(feature);
			
		}
		
		GDALClose(poDS);

		// add the end flag
		FILE* file = fopen((outFileName + ".DXF").c_str(), "a"); // append mode
		if (file)
		{
			fprintf(file, "  0\nSEQEND\n");
			fprintf(file, "  0\nENDSEC\n");
			fprintf(file, "  0\nEOF\n");
			fclose(file);
		}
	}






	void readEdges(const std::string& inFileName, std::vector<Edge2D>& edges, const std::string& className)
	{
		auto io = IOFactory::createIO(className);
		if (io) {
			io->readEdges(inFileName, edges);
		}
		else {
			std::cerr << "Invalid class name for I/O!" << std::endl;
		}
	}

	void writeEdges(const std::string& outFileName, std::vector<Edge2D>& edges, const std::string& className)
	{
		auto io = IOFactory::createIO(className);
		if (io) {
			io->writeEdges(outFileName, edges);
		}
		else {
			std::cerr << "Invalid class name for I/O!" << std::endl;
		}
	}
}


