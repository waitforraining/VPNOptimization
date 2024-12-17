#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>

#include "Edge2D.h"
#include "Point2D.h"

namespace ViewPointNetwork
{
	class IOBase
	{
	public:
		virtual ~IOBase() {}  

		virtual void readEdges(std::string inFileName, std::vector<Edge2D>& edges) = 0;
		virtual void writeEdges(std::string outFileName, const std::vector<Edge2D>& edges) = 0;
	};

	class IOTop :public IOBase
	{
	public:
		void readEdges(std::string inFileName, std::vector<Edge2D>& edges);
		void writeEdges(std::string outFileName, const std::vector<Edge2D>& edges);
	};
	
	class IOFactory 
	{
	public:
		static std::unique_ptr<IOBase> createIO(const std::string& className) 
		{
			if (className == "Top") {
				return std::make_unique<IOTop>();
			}
			return nullptr;
		}
	};

	static void sReadEdges(std::string inFileName, std::vector<Edge2D>& edges,const std::string& className = "Top")
	{
		auto io = IOFactory::createIO(className);  
		if (io) {
			io->readEdges(inFileName, edges);
		}
		else {
			std::cerr << "Invalid class name for I/O!" << std::endl;
		}
	}

	static void sWriteEdges(std::string outFileName, std::vector<Edge2D>& edges, const std::string& className = "Top")
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
