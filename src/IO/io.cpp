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


void readPointsFromObjpts(std::string objptsPath, vector<Point2D>& points)
{
	std::ifstream inputFile(objptsPath);

	string line;
	std::getline(inputFile, line);
	int tmp;
	double x, y;
	while (std::getline(inputFile, line)) {
		if (line[0] == '#') {
			continue;  // 跳过以#开头的行
		}
		std::istringstream iss(line);
		if (iss >> tmp >> x >> y) {
			points.emplace_back(x, y);
		}
	}
	inputFile.close();
}


void IOTop::readEdges(std::string inFileName, vector<Edge2D>& edges)
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
			continue;  // 跳过以#开头的行
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

void IOTop::writeEdges(std::string outFileName,const std::vector<Edge2D>& edges)
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
	for (int i = 0; i < edges.size(); i ++)
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