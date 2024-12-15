//#define _USE_MATH_DEFINES
//#include "Test_Overlap.h"
//
//#include <list>
//#include <math.h>
//#include <stdio.h>
//#include <fstream>
//#include <cstdlib>
//#include <cmath>
//#include <map>
//#include <vector>
//
////#include "../ModelClass.h"
//#include "../VPN/BSPNode.h"
//#include "../VPN/House.h"
//#include "../VPN/Station.h"
//#include "../VPN/StationNet.h"
//#include "../VPN/HeatMap.h"
//#include "../Skeleton/SkeletonGraph.h"
//#include "../Skeleton/extract_ridges.h"
//#include "../WallInit.h"
//#include "../Utils/LineTopUtils.h"
//#include "../VPNTypeDefinition.h"
//#include "../VPN/VpnOptimizerBySkeleton.h"
//
//#include <opencv2/opencv.hpp>
//
//using namespace ViewPointNetwork;
////using namespace WallInit;
//using namespace std;
//using namespace cv;
//namespace VPN = ViewPointNetwork;
//
//namespace ViewPointNetwork
//{
//	std::vector<Edge2D> ConstructRectangle()
//	{
//		//构造测试重叠度的数据
//		LineTopologies iniLineTops;
//		ObjectPoints iniObjPnts;
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0, 0, 0), Position3D(0, 10, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0, 10, 0), Position3D(10, 10, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(10, 10, 0), Position3D(10, 0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(10, 0, 0), Position3D(0, 0, 0), 0, 0, 1);
//
//		iniObjPnts.Write("rectangle.objpts");
//		iniLineTops.Write("rectangle.top");
//
//		LineSegments2D lineSeg2Ds(iniObjPnts, iniLineTops);
//		std::vector<Edge2D> vecEdges = ConvertLineSegment(lineSeg2Ds);
//		MoveMinPoint2Origin(vecEdges);
//		WriteEdges2LineTop(vecEdges, "rectangle_move_min.top");
//
//		return vecEdges;
//	}
//
//	std::vector<Edge2D> ConstructHShape()
//	{
//		vector<LineSegment2D> linestop;
//		LineTopologies iniLineTops;
//		ObjectPoints iniObjPnts;
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(7, 14, 0), Position3D(35, 14, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(7, 14, 0), Position3D(7, 21, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(7, 21, 0), Position3D(0, 21, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0, 21, 0), Position3D(0, 0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0, 0, 0), Position3D(7, 0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(7, 0, 0), Position3D(7, 7, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(7, 7, 0), Position3D(35, 7, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(35, 21, 0), Position3D(35, 14, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(35, 21, 0), Position3D(42, 21, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(42, 0, 0), Position3D(42, 21, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(42, 0, 0), Position3D(35, 0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(35, 7, 0), Position3D(35, 0, 0), 0, 0, 1);
//
//		iniObjPnts.Write("H_shape.objpts");
//		iniLineTops.Write("H_shape.top");
//
//		LineSegments2D lineSeg2Ds(iniObjPnts, iniLineTops);
//		std::vector<Edge2D> vecEdges = ConvertLineSegment(lineSeg2Ds);
//		MoveMinPoint2Origin(vecEdges);
//		WriteEdges2LineTop(vecEdges, "H_shape_move_min.top");
//
//		return vecEdges;
//	}
//
//	std::vector<Edge2D> ConstructFatHShape()
//	{
//		vector<LineSegment2D> linestop;
//		LineTopologies iniLineTops;
//		ObjectPoints iniObjPnts;
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0, 0, 0), Position3D(0, 10, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0, 10, 0), Position3D(6, 10, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(6, 10, 0), Position3D(6, 7, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(6, 7, 0), Position3D(8, 7, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(8, 7, 0), Position3D(8, 10, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(8, 10, 0), Position3D(200, 10, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(200, 10, 0), Position3D(200, 0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(200, 0, 0), Position3D(8, 0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(8, 0, 0), Position3D(8, 3, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(8, 3, 0), Position3D(6, 3, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(6, 3, 0), Position3D(6, 0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(6, 0, 0), Position3D(0, 0, 0), 0, 0, 1);
//
//		iniObjPnts.Write("Fat_H_shape.objpts");
//		iniLineTops.Write("Fat_H_shape.top");
//
//		LineSegments2D lineSeg2Ds(iniObjPnts, iniLineTops);
//		std::vector<Edge2D> vecEdges = ConvertLineSegment(lineSeg2Ds);
//		MoveMinPoint2Origin(vecEdges);
//		WriteEdges2LineTop(vecEdges, "Fat_H_shape_move_min.top");
//
//		return vecEdges;
//	}
//
//	std::vector<Edge2D> ConstructTShape()
//	{
//		vector<LineSegment2D> linestop;
//		LineTopologies iniLineTops;
//		ObjectPoints iniObjPnts;
//
//		//AddTopLine(iniObjPnts, iniLineTops, Position3D(7, 14, 0), Position3D(35, 14, 0), 0, 0, 1);
//		//AddTopLine(iniObjPnts, iniLineTops, Position3D(7, 14, 0), Position3D(7, 21, 0), 0, 0, 1);
//		
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0, 0, 0), Position3D(7, 0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(7, 0, 0), Position3D(7, 7, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(7, 7, 0), Position3D(35, 7, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(35, 7, 0), Position3D(35, 14, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(35, 14, 0), Position3D(7, 14, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(7, 14, 0), Position3D(7, 21, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(7, 21, 0), Position3D(0, 21, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0, 21, 0), Position3D(0, 0, 0), 0, 0, 1);
//		//AddTopLine(iniObjPnts, iniLineTops, Position3D(42, 0, 0), Position3D(42, 21, 0), 0, 0, 1);
//
//		//AddTopLine(iniObjPnts, iniLineTops, Position3D(42, 0, 0), Position3D(35, 0, 0), 0, 0, 1);
//		
//
//		iniObjPnts.Write("T_shape.objpts");
//		iniLineTops.Write("T_shape.top");
//
//		LineSegments2D lineSeg2Ds(iniObjPnts, iniLineTops);
//		std::vector<Edge2D> vecEdges = ConvertLineSegment(lineSeg2Ds);
//		//MoveMinPoint2Origin(vecEdges);
//		//WriteEdges2LineTop(vecEdges, "H_shape_move_min.top");
//
//		return vecEdges;
//	}
//
//	std::vector<Edge2D> ConstructOutdoorCrowsnest()
//	{
//		vector<LineSegment2D> linestop;
//		LineTopologies iniLineTops;
//		ObjectPoints iniObjPnts;
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0.0, 70.0, 0), Position3D(80.0, 70.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(80.0, 70.0, 0), Position3D(80.0, 52.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(80.0, 52.0, 0), Position3D(35.0, 52.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(35.0, 52.0, 0), Position3D(35.0, 46.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(35.0, 46.0, 0), Position3D(39.0, 46.0, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(39.0, 46.0, 0), Position3D(39.0, 40.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(39.0, 40.0, 0), Position3D(35.0, 40.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(35.0, 40.0, 0), Position3D(35.0, 28.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(35.0, 28.0, 0), Position3D(85.0, 28.0, 0), 0, 0, 1);
//		
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(85.0, 28.0, 0), Position3D(85.0, 12.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(85.0, 12.0, 0), Position3D(35.0, 12.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(35.0, 12.0, 0), Position3D(35.0, 5.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(35.0, 5.0, 0), Position3D(23.0, 5.0, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(23.0, 5.0, 0), Position3D(23.0, 12.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(23.0, 12.0, 0), Position3D(14.0, 12.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(14.0, 12.0, 0), Position3D(14.0, 28.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(14.0, 28.0, 0), Position3D(23.0, 28.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(23.0, 28.0, 0), Position3D(23.0, 52.0, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(23.0, 52.0, 0), Position3D(0.0, 52.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0.0, 52.0, 0), Position3D(0.0, 70.0, 0), 0, 0, 1);
//
//		/*AddTopLine(iniObjPnts, iniLineTops, Position3D(1.1, 11.5, 0), Position3D(12.0, 11.5, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(12.0, 11.5, 0), Position3D(12.0, 9.1, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(12.0, 9.1, 0), Position3D(5.7, 9.1, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.7, 9.1, 0), Position3D(5.7, 8.2, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.7, 8.2, 0), Position3D(6.8, 8.2, 0), 0, 0, 1);*/
//
//		/*AddTopLine(iniObjPnts, iniLineTops, Position3D(6.8, 8.2, 0), Position3D(6.8, 7.1, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(6.8, 7.1, 0), Position3D(5.7, 7.1, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.7, 7.1, 0), Position3D(5.7, 5.3, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.7, 5.3, 0), Position3D(13.1, 5.3, 0), 0, 0, 1);*/
//
//		/*AddTopLine(iniObjPnts, iniLineTops, Position3D(13.1, 5.3, 0), Position3D(13.1, 2.9, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(13.1, 2.9, 0), Position3D(5.7, 2.9, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.7, 2.9, 0), Position3D(5.7, 1.5, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.7, 1.5, 0), Position3D(3.5, 1.5, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(3.5, 1.5, 0), Position3D(3.5, 2.9, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(3.5, 2.9, 0), Position3D(2.2, 2.9, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.2, 2.9, 0), Position3D(2.2, 5.3, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.2, 5.3, 0), Position3D(3.5, 5.3, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(3.5, 5.3, 0), Position3D(3.5, 9.1, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(3.5, 9.1, 0), Position3D(1.1, 9.1, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(1.1, 9.1, 0), Position3D(1.1, 11.5, 0), 0, 0, 1);*/
//
//		iniObjPnts.Write("Outdoor_Crowsnest.objpts");
//		iniLineTops.Write("Outdoor_Crowsnest.top");
//
//		LineSegments2D lineSeg2Ds(iniObjPnts, iniLineTops);
//		std::vector<Edge2D> vecEdges = ConvertLineSegment(lineSeg2Ds);
//		MoveMinPoint2Origin(vecEdges);
//		WriteEdges2LineTop(vecEdges, "Outdoor_Crowsnest_move_min.top");
//
//		return vecEdges;
//	}
//
//	std::vector<Edge2D> ConstructOutdoorCCIT()
//	{
//		vector<LineSegment2D> linestop;
//		LineTopologies iniLineTops;
//		ObjectPoints iniObjPnts;
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0.0, 0.0, 0), Position3D(0.0, 45.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0.0, 45.0, 0), Position3D(5.0, 45.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.0, 45.0, 0), Position3D(5.0, 49.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.0, 49.0, 0), Position3D(2.0, 49.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.0, 49.0, 0), Position3D(2.0, 58.0, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.0, 58.0, 0), Position3D(5.0, 58.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.0, 58.0, 0), Position3D(5.0, 63.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.0, 63.0, 0), Position3D(8.0, 63.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(8.0, 63.0, 0), Position3D(8.0, 88.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(8.0, 88.0, 0), Position3D(29.0, 88.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(29.0, 88.0, 0), Position3D(29.0, 76.0, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(29.0, 76.0, 0), Position3D(44.0, 76.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(44.0, 76.0, 0), Position3D(44.0, 88.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(44.0, 88.0, 0), Position3D(71.0, 88.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(71.0, 88.0, 0), Position3D(71.0, 54.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(71.0, 54.0, 0), Position3D(29.0, 54.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(29.0, 54.0, 0), Position3D(29.0, 2.0, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(29.0, 2.0, 0), Position3D(8.0, 2.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(8.0, 2.0, 0), Position3D(8.0, 0.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(8.0, 0.0, 0), Position3D(0.0, 0.0, 0), 0, 0, 1);
//
//		/*AddTopLine(iniObjPnts, iniLineTops, Position3D(2.9, 17.3, 0), Position3D(6.8, 17.3, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(6.8, 17.3, 0), Position3D(6.8, 15.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(6.8, 15.0, 0), Position3D(9.9, 15.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(9.9, 15.0, 0), Position3D(9.9, 17.3, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(9.9, 17.3, 0), Position3D(15.2, 17.3, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(15.2, 17.3, 0), Position3D(15.2, 10.8, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(15.2, 10.8, 0), Position3D(6.1, 10.8, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(6.1, 10.8, 0), Position3D(6.1, 0.8, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(6.1, 0.8, 0), Position3D(2.1, 0.8, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.1, 0.8, 0), Position3D(2.1, 0.2, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.1, 0.2, 0), Position3D(1.1, 0.2, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(1.1, 0.2, 0), Position3D(1.1, 9.1, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(1.1, 9.1, 0), Position3D(2.4, 9.1, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.4, 9.1, 0), Position3D(2.4, 10.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.4, 10.0, 0), Position3D(1.7, 10.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(1.7, 10.0, 0), Position3D(1.7, 11.8, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(1.7, 11.8, 0), Position3D(2.4, 11.8, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.4, 11.8, 0), Position3D(2.4, 12.3, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.4, 12.3, 0), Position3D(2.9, 12.3, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.9, 12.3, 0), Position3D(2.9, 17.3, 0), 0, 0, 1);*/
//
//		iniObjPnts.Write("Outdoor_CCIT.objpts");
//		iniLineTops.Write("Outdoor_CCIT.top");
//
//		LineSegments2D lineSeg2Ds(iniObjPnts, iniLineTops);
//		std::vector<Edge2D> vecEdges = ConvertLineSegment(lineSeg2Ds);
//		MoveMinPoint2Origin(vecEdges);
//		WriteEdges2LineTop(vecEdges, "Outdoor_CCIT_move_min.top");
//
//		return vecEdges;
//	}
//
//	std::vector<Edge2D> ConstructPaperData()
//	{
//		vector<LineSegment2D> linestop;
//		LineTopologies iniLineTops;
//		ObjectPoints iniObjPnts;
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(3.0, 36.0, 0), Position3D(13.5, 36.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(13.5, 36.0, 0), Position3D(13.5, 33.6, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(13.5, 33.6, 0), Position3D(21.6, 33.6, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(21.6, 33.6, 0), Position3D(21.6, 28.9, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(21.6, 28.9, 0), Position3D(19.0, 28.9, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(19.0, 28.9, 0), Position3D(19.0, 22.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(19.0, 22.0, 0), Position3D(2.5, 22.0, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.5, 22.0, 0), Position3D(2.5, 26.4, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(2.5, 26.4, 0), Position3D(0.2, 26.4, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0.2, 26.4, 0), Position3D(0.2, 28.9, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(0.2, 28.9, 0), Position3D(3.0, 28.9, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(3.0, 28.9, 0), Position3D(3.0, 36.0, 0), 0, 0, 1);
//
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.7, 34.4, 0), Position3D(5.7, 24.8, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(5.7, 24.8, 0), Position3D(10.7, 24.8, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(10.7, 24.8, 0), Position3D(10.7, 34.4, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(10.7, 34.4, 0), Position3D(5.7, 34.4, 0), 0, 0, 1);
//		
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(13.1, 29.7, 0), Position3D(13.1, 26.5, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(13.1, 26.5, 0), Position3D(16.6, 26.5, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(16.6, 26.5, 0), Position3D(16.6, 29.7, 0), 0, 0, 1);
//		AddTopLine(iniObjPnts, iniLineTops, Position3D(16.6, 29.7, 0), Position3D(13.1, 29.7, 0), 0, 0, 1);
//
//		iniObjPnts.Write("paperData.objpts");
//		iniLineTops.Write("paperData.top");
//
//		LineSegments2D lineSeg2Ds(iniObjPnts, iniLineTops);
//		std::vector<Edge2D> vecEdges = ConvertLineSegment(lineSeg2Ds);
//		MoveMinPoint2Origin(vecEdges);
//		WriteEdges2LineTop(vecEdges, "paperData_move_min.top");
//
//		return vecEdges;
//	}
//
//	void test_heatmap_rectangle()
//	{
//		createAndChangeDirectory("heatmap_rectangle");
//		std::vector<Edge2D> vecEdges = ConstructRectangle();
//		computeHeatMap(vecEdges, "rectangle", 0.02, 3.5);
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//	void test_one_edge_heatmap_rectangle()
//	{
//		std::vector<Edge2D> vecEdges = ConstructRectangle();
//
//		House house(vecEdges, HT_Indoor);
//		double cell = 0.02;
//
//		cout << "构建热力图..." << endl;
//		HeatMap edgeMap(house, cell);
//		edgeMap.generateOneEdgeHeat(house, Edge2D(Point2D(0, 0), Point2D(10, 0)), 0.5, 3.5);
//		edgeMap.saveBGR_YellowOrange(house, "edge", false);
//	}
//	void test_one_edge_heatmap()
//	{
//		vector<Edge2D> houseEdges = ReadEdgefromTop("scene_00010");
//		House house(houseEdges, HT_Indoor);
//
//		double cell = 0.02;
//		cout << "构建热力图..." << endl;
//		HeatMap edgeMap(house, cell);
//		edgeMap.generateOneEdgeHeat(house, Edge2D(Point2D(5.5, 5.911), Point2D(9.85, 5.911)), 0.5, 1.2);
//		edgeMap.saveBGR_YellowOrange(house, "edge", false);
//	}
//
//	void test_heatmap_H_shape()
//	{
//		createAndChangeDirectory("heatmap_H_shape");
//
//		std::vector<Edge2D> vecEdges = ConstructHShape();
//		//computeHeatMap(vecEdges, "H_shape", 0.05, 3.0);
//		OptSkelParam param(HT_Indoor, 0.05, 3.0, 30.0, 0.4, 0.1);
//		VpnOptimizerBySkeleton optimizer(vecEdges, param);
//		optimizer.setHouseName("H_shape");
//		optimizer.doOptimize();
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//
//	void test_heatmap_Fat_H_shape()
//	{
//		createAndChangeDirectory("heatmap_Fat_H_shape");
//
//		std::vector<Edge2D> vecEdges = ConstructFatHShape();
//		//computeHeatMap(vecEdges, "H_shape", 0.05, 3.0);
//		OptSkelParam param(HT_Indoor, 0.05, 3.0, 30.0, 0.4, 0.1);
//		VpnOptimizerBySkeleton optimizer(vecEdges, param);
//		optimizer.setHouseName("Fat_H_shape");
//		optimizer.doOptimize();
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//
//	void test_overlap_Fat_H_shape()
//	{
//		createAndChangeDirectory("overlap_Fat_H_shape");
//
//		//////////////////////////////////////////////////////////////////////////
//		//prepare dataset
//		std::vector<Edge2D> vecEdges = ConstructFatHShape();
//		House house(vecEdges);
//
//		//////////////////////////////////////////////////////////////////////////
//		//construct station points along the ridge
//		//VPN::Point2D base(3.5, 10.5);
//		std::vector<VPN::Point2D> vecEndPnts;
//		vecEndPnts.emplace_back(0.0, 5.0);
//		vecEndPnts.emplace_back(20.0, 5.0);
//		//vecEndPnts.emplace_back(38.5, 10.5);
//		//vecEndPnts.emplace_back(38.5, 21);
//
//		vector<VPN::Point2D> vecRidgePnts, vecTemPnts;
//		Edge2D edge;
//		for (size_t i = 0; i < vecEndPnts.size() - 1; i++)
//		{
//			vecRidgePnts.push_back(vecEndPnts[i]);
//			edge = Edge2D(vecEndPnts[i], vecEndPnts[i + 1]);
//			vecTemPnts = edge.densify(0.5, false);
//			vecRidgePnts.insert(vecRidgePnts.end(), vecTemPnts.begin(), vecTemPnts.end());
//		}
//		vecRidgePnts.push_back(vecEndPnts[vecEndPnts.size() - 1]);
//
//		//////////////////////////////////////////////////////////////////////////
//		//
//		ofstream scoreFile("_Fat_H_ridges_overlap.csv");
//		scoreFile << "x,y,AVERAGE_LEN,MIN_LEN,UNION_LEN,INTERSECTSUM_ANG,INTERSECT_ANG,Len_s0,Ang_s0,Len_s1,Ang_s1,overlap_len,overlap_ang0,overlap_ang1\n";
//		VPN::Station s(3.0, 5.0, 0.8);
//		
//		s.scan(house.getBspRoot());
//		s.wirteScannedEdges();
//
//		for (size_t i = 0; i < vecRidgePnts.size(); i++)
//		{
//			Station tmpS(vecRidgePnts[i], 0.8);
//			tmpS.scan(house.getBspRoot());
//			writeOverlapPercentByLen(scoreFile, s, tmpS);
//			tmpS.wirteScannedEdges();
//		}
//
//		scoreFile.close();
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//
//	void test_heatmap_outdoor_Crowsnest()
//	{
//		createAndChangeDirectory("outdoor_Crowsnest");
//
//		std::vector<Edge2D> vecEdges = ConstructOutdoorCrowsnest();
//		//computeHeatMap(vecEdges, "H_shape", 0.05, 3.0);
//		OptSkelParam param(HT_Outdoor, 0.05, 1.2, 30.0, 0.4, 0.1);
//		VpnOptimizerBySkeleton optimizer(vecEdges, param);
//		optimizer.setHouseName("Crowsnest");
//		optimizer.doOptimize();
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//
//	void test_heatmap_outdoor_TShape()
//	{
//		createAndChangeDirectory("outdoor_TShape");
//
//		std::vector<Edge2D> vecEdges = ConstructTShape();
//		//computeHeatMap(vecEdges, "H_shape", 0.05, 3.0);
//		OptSkelParam param(HT_Outdoor, 0.1, 1.2, 30.0, 0.4, 0.1);
//		VpnOptimizerBySkeleton optimizer(vecEdges, param);
//		optimizer.setHouseName("TShape");
//		optimizer.doOptimize();
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//
//	void test_heatmap_outdoor_HShape()
//	{
//		createAndChangeDirectory("outdoor_HShape");
//
//		std::vector<Edge2D> vecEdges = ConstructHShape();
//		//computeHeatMap(vecEdges, "H_shape", 0.05, 3.0);
//		OptSkelParam param(HT_Outdoor, 0.1, 1.2, 30.0, 0.4, 0.1);
//		VpnOptimizerBySkeleton optimizer(vecEdges, param);
//		optimizer.setHouseName("HShape");
//		optimizer.doOptimize();
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//
//	void test_heatmap_outdoor_CCIT()
//	{
//		createAndChangeDirectory("outdoor_CCIT");
//
//		std::vector<Edge2D> vecEdges = ConstructOutdoorCCIT();
//		OptSkelParam param(HT_Outdoor, 0.1, 1.2, 30.0, 0.4, 0.1);
//		VpnOptimizerBySkeleton optimizer(vecEdges, param);
//		optimizer.setHouseName("CCIT");
//		optimizer.doOptimize();
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//
//	void test_heatmap_outdoor_rectangle()
//	{
//		createAndChangeDirectory("outdoor_rectangle");
//
//		std::vector<Edge2D> vecEdges = ConstructRectangle();
//		OptSkelParam param(HT_Outdoor, 0.1, 1.2, 30.0, 0.4, 0.1);
//		VpnOptimizerBySkeleton optimizer(vecEdges, param);
//		optimizer.setHouseName("out_rectangle");
//		optimizer.doOptimize();
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//
//	void test_paperData()
//	{
//		createAndChangeDirectory("paperData");
//
//		std::vector<Edge2D> vecEdges = ConstructPaperData();
//		OptSkelParam param(HT_Outdoor, 0.1, 0.6, 30.0, 0.4, 1.0);
//		VpnOptimizerBySkeleton optimizer(vecEdges, param);
//		optimizer.setHouseName("out_rectangle");
//		optimizer.doOptimize();
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//
//	/*
//	void test_overlap_H_shape()
//	//void getHeatMap1(const vector<WallInit::LineSegment2d>& lines, const string& outresultpath)
//	{
//		std::vector<Edge2D> vecEdges = ConstructHShape();
//		House house(vecEdges, 0.1);
//		Edge2D nearstEdge = house.getNearstEdge(house.getCentre());
//		BSPNode root(nearstEdge);
//		root.generateBSP(house.getEdges());//构建bsp树关系
//
//		cout << "构建热力图..." << endl;
//		//HeatMap heatMap(house, 0.1);
//		//heatMap.generate(house, root, 1.2);
//
//		//double scalar = 0.25;
//		//std::vector<cv::Vec3b> colors;
//		//colors.push_back(cv::Vec3b(3, 51, 153));
//		//colors.push_back(cv::Vec3b(13, 91, 217));
//		//colors.push_back(cv::Vec3b(41, 155, 255));
//		//colors.push_back(cv::Vec3b(142, 217, 255));
//		//colors.push_back(cv::Vec3b(212, 255, 255));
//		//reverse(colors.begin(), colors.end());
//		//heatMap.showBGR1(house, colors, name, scalar, true);
//		//heatMap.showGRAY(house, name);
//
//		Station b(3.5, 10, 1.2);
//		Station a(3.5, 17.5, 1.2);
//		Station c(38.5, 10, 1.2);
//		Station d(38.5, 17.5, 1.2);
//
//		house.addStation(b);
//
//
//		ofstream scoreFile("line0.csv");
//		scoreFile << "x,y,AVERAGE_LEN,MIN_LEN,UNION_LEN,INTERSECTSUM_ANG,INTERSECT_ANG,s0_len,s1_len,overlap_len,overlap_ang0,overlap_ang1,heatValue0,heatValue1\n";
//		int i = 0;
//		//auto v = heat.value;
//		//auto cell = heat.getCell();
//		double heatValue0, heatValue1;
//		for (double m = 17.5; m >= 10; m -= 0.25)
//		{
//			Station a(3.5, m);
//
//			//heatValue1 = v[a.getX() / cell][a.getY() / cell];
//			//heatValue0 = v[b.getX() / cell][b.getY() / cell];
//			//heatValue1 = heatMap.getValue(a);
//			//heatValue0 = heatMap.getValue(b);
//
//			house.addStation(a);
//			house.scan(root);
//			Station ha = house.getStations(0);
//			Station hb = house.getStations(1);
//
//			writeOverlapPercentByLen(scoreFile, ha, hb);
//			house.delStation(1);
//		}
//
//		for (double m = 3.5; m <= 38.5; m += 1.17)
//		{
//			Station a(m, 10);
//
//			house.addStation(a);
//			house.scan(root);
//			Station ha = house.getStations(0);
//			Station hb = house.getStations(1);
//			//heatValue1 = v[a.getX() / cell][a.getY() / cell];
//			//heatValue0 = v[b.getX() / cell][b.getY() / cell];
//			//heatValue1 = heatMap.getValue(a);
//			//heatValue0 = heatMap.getValue(b);
//			writeOverlapPercentByLen(scoreFile, ha, hb);
//
//			house.delStation(1);
//		}
//
//		for (double m = 10; m <= 17.5; m += 0.25)
//		{
//			Station a(38.5, m);
//
//			//heatValue1 = v[a.getX() / cell][a.getY() / cell];
//			//heatValue0 = v[b.getX() / cell][b.getY() / cell];
//			//heatValue1 = heatMap.getValue(a);
//			//heatValue0 = heatMap.getValue(b);
//			house.addStation(a);
//			house.scan(root);
//			Station ha = house.getStations(0);
//			Station hb = house.getStations(1);
//			writeOverlapPercentByLen(scoreFile, ha, hb);
//
//			house.delStation(1);
//		}
//
//		scoreFile.close();
//
//		//cv::Mat figure = cv::Mat(heat.shape[1]+10, heat.shape[0]+10, CV_8UC3,Scalar(255,255,255));
//		//showHouse(figure, house, heat.getCell());
//		//showScanArea(house, figure, heat.getCell());
//		//showOverlapScanArea(ha, hb, figure, heat.getCell());
//		//vector<int> allStationInd(2);
//		//iota(allStationInd.begin(), allStationInd.end(), 0);
//		//showStation(figure, house, allStationInd, heat.getCell(), cv::Scalar(125, 0, 125), 2);
//		//cv::imwrite("All.png", figure);
//	}*/
//
//
//	void test_overlap_H_shape()
//	{
//		createAndChangeDirectory("overlap_H_shape");
//
//		//////////////////////////////////////////////////////////////////////////
//		//prepare dataset
//		std::vector<Edge2D> vecEdges = ConstructHShape();
//		House house(vecEdges);
//
//		//////////////////////////////////////////////////////////////////////////
//		//construct station points along the ridge
//		//VPN::Point2D base(3.5, 10.5);
//		std::vector<VPN::Point2D> vecEndPnts;
//		vecEndPnts.emplace_back(3.5, 21);
//		vecEndPnts.emplace_back(3.5, 10.5);
//		vecEndPnts.emplace_back(38.5, 10.5);
//		vecEndPnts.emplace_back(38.5, 21);
//
//		vector<VPN::Point2D> vecRidgePnts, vecTemPnts;
//		Edge2D edge;
//		for (size_t i = 0; i < vecEndPnts.size() - 1; i++)
//		{
//			vecRidgePnts.push_back(vecEndPnts[i]);
//			edge = Edge2D(vecEndPnts[i], vecEndPnts[i + 1]);
//			vecTemPnts = edge.densify(0.5, false);
//			vecRidgePnts.insert(vecRidgePnts.end(), vecTemPnts.begin(), vecTemPnts.end());
//		}
//		vecRidgePnts.push_back(vecEndPnts[vecEndPnts.size() - 1]);
//
//		//////////////////////////////////////////////////////////////////////////
//		//
//		ofstream scoreFile("_H_ridges_overlap.csv");
//		scoreFile << "x,y,AVERAGE_LEN,MIN_LEN,UNION_LEN,INTERSECTSUM_ANG,INTERSECT_ANG,Len_s0,Ang_s0,Len_s1,Ang_s1,overlap_len,overlap_ang0,overlap_ang1\n";
//		VPN::Station s(21.0, 10.5, 1.2);
//		s.scan(house.getBspRoot());
//		s.wirteScannedEdges();
//
//		for (size_t i = 0; i < vecRidgePnts.size(); i++)
//		{
//			Station tmpS(vecRidgePnts[i], 1.2);
//			tmpS.scan(house.getBspRoot());
//			writeOverlapPercentByLen(scoreFile, s, tmpS);
//			tmpS.wirteScannedEdges();
//		}
//		scoreFile.close();
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//
//	void test_overlap_T_shape()
//	{
//		createAndChangeDirectory("overlap_T_shape");
//		//////////////////////////////////////////////////////////////////////////
//		//prepare dataset
//		std::vector<Edge2D> vecEdges = ConstructTShape();
//		House house(vecEdges);
//
//		//////////////////////////////////////////////////////////////////////////
//		//construct station points along the ridge
//		//VPN::Point2D base(3.5, 10.5);
//		std::vector<VPN::Point2D> vecEndPnts;
//		vecEndPnts.emplace_back(3.5, 21);
//		vecEndPnts.emplace_back(3.5, 10.5);
//		vecEndPnts.emplace_back(35.0, 10.5);
//
//		vector<VPN::Point2D> vecRidgePnts, vecTemPnts;
//		Edge2D edge;
//		for (size_t i = 0; i < vecEndPnts.size() - 1; i++)
//		{
//			vecRidgePnts.push_back(vecEndPnts[i]);
//			edge = Edge2D(vecEndPnts[i], vecEndPnts[i + 1]);
//			vecTemPnts = edge.densify(0.5, false);
//			vecRidgePnts.insert(vecRidgePnts.end(), vecTemPnts.begin(), vecTemPnts.end());
//		}
//		vecRidgePnts.push_back(vecEndPnts[vecEndPnts.size() - 1]);
//
//		//////////////////////////////////////////////////////////////////////////
//		//
//		ofstream scoreFile("_T_ridges_overlap.csv");
//		scoreFile << "x,y,AVERAGE_LEN,MIN_LEN,UNION_LEN,INTERSECTSUM_ANG,INTERSECT_ANG,Len_s0,Ang_s0,Len_s1,Ang_s1,overlap_len,overlap_ang0,overlap_ang1\n";
//
//		//house.addStation(VPN::Station(21.0, 10.5, 1.2));
//		VPN::Station s(3.5, 10.5, 1.2);
//		s.scan(house.getBspRoot());
//		s.wirteScannedEdges();
//
//		for (size_t i = 0; i < vecRidgePnts.size(); i++)
//		{
//			Station tmpS(vecRidgePnts[i], 1.2);
//			tmpS.scan(house.getBspRoot());
//			writeOverlapPercentByLen(scoreFile, s, tmpS);
//			tmpS.wirteScannedEdges();
//
//		}
//
//		scoreFile.close();
//
//		fs::current_path("..");//回到上一层路径
//		std::cout << "Current Path: " << fs::current_path().string() << std::endl;
//	}
//
//	void test_overlap_scene_009()
//	{
//		//////////////////////////////////////////////////////////////////////////
//		//prepare dataset
//		std::string houseName = "scene_00009";
//		std::vector<VPN::Edge2D> vecEdges = ReadEdgefromTop(houseName);
//
//		House house(vecEdges);
//		double cell = 0.01;
//		double r_min = 0.5;
//		double r_max = 30.0;
//		//////////////////////////////////////////////////////////////////////////
//		//heat map
//		cout << "构建热力图..." << endl;
//
//		HeatMap heatMap(house, cell);
//		heatMap.generate(house, r_min, r_max);
//
//		stringstream ss;
//		ss << std::fixed << setprecision(3);
//		ss << "_" << cell << "_" << r_min;
//		ss << "_to_" << r_max;
//		string name = houseName + ss.str();
//		heatMap.saveBGR_YellowOrange(house, name, false);
//		heatMap.saveGRAY(house, name);
//
//		//////////////////////////////////////////////////////////////////////////
//		//overlap
//		std::vector<VPN::Point2D> vecEndPnts;
//		vecEndPnts.emplace_back(5.82, 5.17);
//		vecEndPnts.emplace_back(5.83, 4.03);
//		vecEndPnts.emplace_back(6.61, 5.31);
//		vecEndPnts.emplace_back(7.21, 4.03);
//		vecEndPnts.emplace_back(4.84, 4.63);
//		vecEndPnts.emplace_back(3.47, 5.50);
//		vecEndPnts.emplace_back(2.40, 3.83);
//
//		//////////////////////////////////////////////////////////////////////////
//		//
//		vector<Station> stations;
//		for (size_t i = 0; i < vecEndPnts.size(); i++) {
//			Station tmpS(vecEndPnts[i], r_min, r_max);
//			tmpS.scan(house.getBspRoot());
//			tmpS.wirteScannedEdges();
//			stations.push_back(tmpS);
//		}
//		StationNet stationNet(stations);
//		stationNet.writeStationOverlapMatrix("_scene_00009_overlap.csv");
//	}
//
//	void test_overlap_house_shape()
//	{
//		//////////////////////////////////////////////////////////////////////////
//		//prepare dataset
//		std::vector<VPN::Edge2D> vecEdges = ReadEdgefromTop("scene_00001");
//		House house(vecEdges);
//
//
//		//////////////////////////////////////////////////////////////////////////
//		std::vector<VPN::Point2D> vecEndPnts;
//		vector<Station> stations;
//		stations.push_back(Station(5.46, 0.42, 0.0001));//21
//		stations.push_back(Station(6.00, 0.42, 0.0001));//22
//		stations.push_back(Station(6.20, 0.50, 0.0001));//19
//		stations.push_back(Station(6.80, 0.50, 0.0001));//20
//		
//		for (auto& station : stations)
//		{
//			station.scan(house.getBspRoot());
//		}
//		StationNet stationNet(stations);
//
//		stationNet.writeStationScannedEdges();
//		stationNet.writeStationOverlapMatrix("part_overlap.csv");
//
//		ofstream scoreFile("_H_ridges_overlap.csv");
//		scoreFile << "x,y,AVERAGE_LEN,MIN_LEN,UNION_LEN,INTERSECTSUM_ANG,INTERSECT_ANG,Len_s0,Ang_s0,Len_s1,Ang_s1,overlap_len,overlap_ang0,overlap_ang1\n";
//
//		Station ha = stations[0];
//
//		for (size_t i = 1; i < stations.size(); i++)
//		{
//			Station hb = stations[i];
//			writeOverlapPercentByLen(scoreFile, ha, hb);
//		}
//
//		scoreFile.close();
//	}
//
//	void test_weight_shortpath()
//	{
//		std::string houseName = "Chen_Outer1";
//		std::vector<VPN::Edge2D> vecEdges = ReadEdgefromTop(houseName);
//		House house(vecEdges);
//		ifstream is("theirStation.txt");
//		double x, y;
//
//		ObjectPoints objpts;
//		LineTopologies lineTops;
//		double offSet = 0.1;
//		vector<Station> stations;
//		while (is >> x >> y)
//		{
//			AddTopLine(objpts, lineTops, Position3D(x, y, 0.0),
//				Position3D(x + offSet, y, 0.0), 0, 0, 1);
//			AddTopLine(objpts, lineTops, Position3D(x, y, 0.0),
//				Position3D(x, y + offSet, 0.0), 0, 0, 1);
//
//			AddTopLine(objpts, lineTops, Position3D(x, y + offSet, 0.0),
//				Position3D(x + offSet, y + offSet, 0.0), 0, 0, 1);
//			AddTopLine(objpts, lineTops, Position3D(x + offSet, y, 0.0),
//				Position3D(x + offSet, y + offSet, 0.0), 0, 0, 1);
//			Station s(x, y, 1.2, 75);
//			s.scan(house.getBspRoot());
//			stations.push_back(s);
//		}
//		StationNet stationNet(stations);
//
//		stationNet.writeStationOverlapMatrix("their.csv");
//
//		/*house.show("./");*/
//		stationNet.writeStationWeightOverlap("shortPathPath.csv", 0.3);
//	}
//
//	void test_outdoor_Crowsnest();
//	void test_outdoor_CCIT();
//
//	//void optimizeVPNbySkeleton(const vector<WallInit::LineSegment2d> & lines, const string & outresultpath, const int& HouseType)
//	void optimizeVPNbySkeleton(const vector<Edge2D> & vecEdges, const string & houseName, HouseType houseType)
//	{
//		/*
//		double cell = 0.02;
//		double stationRasius = 0.6;
//
//		House house(vecEdges, 0.1, houseType);
//		Edge2D nearstEdge = house.getNearstEdge(house.getCentre());
//		BSPNode root(nearstEdge);
//		root.generateBSP(house.getEdges());//构建bsp树关系
//
//		HeatMap heatMap(house, cell);
//		heatMap.generate(house, root, stationRasius);
//		heatMap.saveBGR_YellowOrange(house, houseName, false);
//		heatMap.saveGRAY(house, houseName);
//
//		cv::Mat skeleton = creatRidge(house, heatMap, houseName + "_GRAYheat.png", houseName);
//		SkeletonGraph sGraph(skeleton, house, heatMap);
//		sGraph.getSkeletonGraph();
//		//double threshold = 5;
//		sGraph.addNewStations(5);
//		sGraph.initSg(house, root);
//		sGraph.drawWholeHouse(house, houseName, cell, JT_INITSTATIONS, RT_L2, 5);
//
//		//double scalar = 0.25;
//
//		//构建站点网络
//		StationNet sNet(house);
//		sNet.computeMat();
//		//重叠矩阵
//		
//#ifdef _DEBUG
//		//vector<vector<int>> skeletonConMat = sGraph.connectMat;
//		house.writeStationOverlapMatrix("overlap.csv");
//		house.writeStationScannedEdges();
//#endif
//
//		vector<vector<double>> stationOverlapMat = sNet.getOverlapMat();
//		//重叠度阈值
//		double overlapThre = 0.5;
//		sGraph.refine(stationOverlapMat, overlapThre);
//		house.setAllStations(sGraph.stations, sGraph.usedStationInds);
//		sGraph.drawWholeHouse(house, houseName, cell, JT_REFINEDSTATIONS, RT_L2, 5);*/
//	}
//
//	void test_bsp(const string& houseName)
//	{
//		std::vector<VPN::Edge2D> vecEdges = ReadEdgefromTop(houseName);
//		MoveMinPoint2Origin(vecEdges);
//		double minDist = std::numeric_limits<double>::max();
//		double d;
//		size_t ind = 0;
//
//		House house = House(vecEdges, HT_Indoor);
//
//		auto p = house.getCentre();
//		for (size_t i = 0; i < vecEdges.size(); i++)
//		{
//			d = vecEdges[i].distance(p);
//
//			if (d < minDist)
//			{
//				minDist = d;
//				ind = i;
//			}
//		}
//		double cell = 0.02;
//
//		HeatMap m_heatMap(house, cell);
//		Edge2D nearstEdge = house.getNearstEdge(house.getCentre());
//
//		std::shared_ptr<BSPNode> bspNode = buildBspTree(vecEdges, ind);
//
//		
//	}
//
//	void test_one_station_scan(const string& houseName)
//	{
//		std::vector<VPN::Edge2D> vecEdges = ReadEdgefromTop(houseName);
//		MoveMinPoint2Origin(vecEdges);
//		double minDist = std::numeric_limits<double>::max();
//		double d;
//		size_t ind = 0;
//
//		House house = House(vecEdges, HT_Indoor);
//
//		auto p = house.getCentre();
//		for (size_t i = 0; i < vecEdges.size(); i++)
//		{
//			d = vecEdges[i].distance(p);
//
//			if (d < minDist)
//			{
//				minDist = d;
//				ind = i;
//			}
//		}
//
//		Station s(5, 6.5);
//		s.scan(house.getBspRoot());
//		ObjectPoints a;
//		LineTopologies b;
//		Position3D p0,p1;
//		for (auto edge : s.getScanedEdges())
//		{
//			p0.X() = edge.second.getBegPoint().X();
//			p0.Y() = edge.second.getBegPoint().Y();
//			p1.X() = edge.second.getEndPoint().X();
//			p1.Y() = edge.second.getEndPoint().Y();
//			AddTopLine(a, b, p0, p1, 0, 0, 1);
//		}
//		a.Write("scan.objpts");
//		b.Write("scan.top");
//	}
//	void optimizeVPNbySkeleton(const string & houseName, HouseType houseType)
//	{
//		std::vector<VPN::Edge2D> vecEdges = ReadEdgefromTop(houseName);
//		optimizeVPNbySkeleton(vecEdges, houseName, houseType);
//	}
//
//
//	void test_read_DXF()
//	{
//		LineTopologies lineTops;
//		ObjectPoints objPnts;
//		//objPnts.ReadDXF("rectangle.dxf", lineTops, 0, 0, 0, 0, 0, false);
//
//// 		objPnts.ReadNewDXF("rectangle.dxf", lineTops);
//// 		objPnts.Write("rectangle.objpts");
//// 		lineTops.Write("rectangle.top");
//		objPnts.ReadNewDXF("Zeng_SceneA.dxf", lineTops);
//		for (auto& pnt : objPnts)
//		{
//			pnt.SetPos2D(Position2D(pnt.X() / 2.5378431034482758620689655172414,
//			pnt.Y() / 2.5378431034482758620689655172414));
//		}
//		objPnts.Write("Zeng_SceneA.objpts");
//		lineTops.Write("Zeng_SceneA.top");
//		ReadEdgefromTop("Zeng_SceneA");
//	}
//}