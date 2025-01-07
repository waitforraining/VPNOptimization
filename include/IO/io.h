#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>


#include <gdal_priv.h>
#include <gdal.h>
#include <ogrsf_frmts.h>

#include "Shape/Edge2D.h"
#include "Shape/Point2D.h"


namespace ViewPointNetwork
{
	class IOBase
	{
	public:
		virtual ~IOBase() {}  

		virtual void readEdges(const std::string& inFileName, std::vector<Edge2D>& edges) = 0;
		virtual void writeEdges(const std::string& outFileName, const std::vector<Edge2D>& edges) = 0;
	};

	class IOTop :public IOBase
	{
	public:
		void readEdges(const std::string& inFileName, std::vector<Edge2D>& edges);
		void writeEdges(const std::string& outFileName, const std::vector<Edge2D>& edges);
	};
	
	class IODXF : public IOBase
	{
	public:
		void readEdges(const std::string& inFileName, std::vector<Edge2D>& edges);
		void writeEdges(const std::string& outFileName, const std::vector<Edge2D>& edges);
	};


	class IOFactory 
	{
	public:
		static std::unique_ptr<IOBase> createIO(const std::string& className) 
		{
			std::string upperStr = className;
			for (auto& c : upperStr)
			{
				c = std::toupper(c);
			}
			if (upperStr == "TOP")
				return std::make_unique<IOTop>();
			else if (upperStr == "DXF")
				return std::make_unique<IODXF>();
			return nullptr;
		}
	};

	void readEdges(const std::string& inFileName, std::vector<Edge2D>& edges, const std::string& className = "Top");
	void writeEdges(const std::string& outFileName, std::vector<Edge2D>& edges, const std::string& className = "Top");
}
