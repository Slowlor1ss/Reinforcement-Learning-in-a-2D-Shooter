#include "stdafx.h"
#include "Utils_General.h"
#include "ConsoleColors.h"

#include <iostream>
#include <algorithm>
#include <chrono>
#include <string>
#include <utility>
#include <memory>
#include <fstream>
#include "framework/EliteMath/EVector2.h"

Timer::Timer(float interval, std::function<void()> func, bool tOnce)
	: isActivated(true), tOnce{ tOnce }, interval{ interval }, func{std::move(func)}
{}

void Timer::Update(float deltaTime)
{
	if (this->finished) return;
	if (!this->isActivated) return;

	this->accumulatedTime += deltaTime;
	if (this->accumulatedTime >= this->interval)
	{
		//our timer ticked 
		this->func();

		if (this->tOnce) this->finished = true;
		else  this->accumulatedTime -= this->interval;
	}
}


void Log(const std::string& message, const std::string& file, int line, const LogVerbosity verbosity)
{
	//const auto t = std::time(nullptr);
	//
	//tm tm{ localtime_s(&tm,&t) };
	//
	// std::cout << "[" << tm.tm_year + 1900 << "-" << StringPP(std::to_string(tm.tm_mon + 1)).PrefixFill(2, '0') << "-"
	//	 << StringPP(std::to_string(tm.tm_mday)).PrefixFill(2, '0') << " " << tm.tm_hour << ":" << tm.tm_min << ":" <<
	//	 tm.tm_sec << "] [" << file << "(" << std::to_string(line) << ")" << "] ";

	std::cout << "[" << file << "(" << std::to_string(line) << ")" << "] ";

	switch (verbosity)
	{
	case LogVerbosity::def:
		std::cout << FCYN("Log");
		break;
	case LogVerbosity::warning:
		std::cout << FYEL("Warning");
		break;
	case LogVerbosity::error:
		std::cout << FRED("Error");
		break;
	default:
		std::cout << FCYN("Log");
		break;
	}
	std::cout << ": " << message << std::endl;
}

void parseFile(const std::string& filePath, std::vector<std::vector<float>>& vertices, std::vector<uint32_t>& indices)
{
#ifdef _DEBUG
	__LOG__("Obj file parsing started...");
#endif

	vertices.clear();
	indices.clear();

	std::ifstream file{};
	file.open(filePath);
	if (file)
	{
		if (file.is_open())
		{
			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					float x, y, z;
					file >> x >> y >> z;
					vertices.emplace_back(std::vector<float>{ x, y, z});

				}
				else if (sCommand == "f")
				{
					size_t iPosition;
					for (size_t iFace = 0; iFace < 3; iFace++)
					{
						file >> iPosition;
						indices.push_back(static_cast<uint32_t>(iPosition));
					}
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');
			}
#ifdef _DEBUG
			__LOG__("Obj file parsing ended");
#endif
			file.close();
		}
#ifdef _DEBUG
		else
		{
			__LOGV__(std::string(strrchr(filePath.c_str(), '/') ? strrchr(filePath.c_str(), '/') + 1 : filePath.c_str()) +
				" failed to open maybe its open in another program?", LogVerbosity::warning);
		}
#endif
	}
#ifdef _DEBUG
	else
	{
		__LOGV__(std::string(strrchr(filePath.c_str(), '/') ? strrchr(filePath.c_str(), '/') + 1 : filePath.c_str()) +
			" not found", LogVerbosity::warning);
	}
#endif
}

void MakeFile(const std::string& filePath, std::vector<std::vector<float>>& vertices, std::vector<uint32_t>& indices, std::ios_base::openmode openMode)
{
	//Checking if file already exists
#ifdef _DEBUG
//https://stackoverflow.com/questions/2324658/how-to-determine-the-version-of-the-c-standard-used-by-the-compiler
#if __cplusplus > 201103L //If using c++17 or higher
	{
		std::filesystem::path f{ "file.txt" };
		if (!std::filesystem::exists(f)){
			__LOGV__(std::string(strrchr(filePath.c_str(), '/') ? strrchr(filePath.c_str(), '/') + 1 : filePath.c_str()) +
				" already exists", LogVerbosity::warning);
		}
	}
#else
	{
		std::ifstream file(filePath);
		if (file.is_open()) {
			__LOGV__(std::string(strrchr(filePath.c_str(), '/') ? strrchr(filePath.c_str(), '/') + 1 : filePath.c_str()) +
				" already exists", LogVerbosity::warning);
		}
	}
#endif
#endif

	std::ofstream file{};
	file.open(filePath, openMode);

	if (file)
	{
		if (file.is_open())
		{
			std::string toWrite{};
			toWrite += "# OBJ file format with ext .obj\n";
			toWrite += "# vertex count = " + std::to_string(vertices.size()) + '\n';
			toWrite += "# Face count = " + std::to_string((indices.size())/3) + '\n';

			for (const auto& v : vertices)
			{
				toWrite += "v ";
				for (auto value : v)
				{
					toWrite += std::to_string(value) + " ";
				}
				toWrite += '\n';

			}

			toWrite += "f ";
			int counter{0};
			for (auto i : indices)
			{
				if (counter % 3 == 0 && counter != 0)
				{
					toWrite += "\nf ";
				}
				toWrite += std::to_string(i) + " ";
				++counter;
			}
			file.write(toWrite.c_str(), toWrite.size());
			file.close();
		}
#ifdef _DEBUG
		else
		{
			__LOGV__(std::string(strrchr(filePath.c_str(), '/') ? strrchr(filePath.c_str(), '/') + 1 : filePath.c_str()) +
				" failed to open maybe its open in another program?", LogVerbosity::warning);
		}
#endif
	}
#ifdef _DEBUG
	else
	{
		__LOGV__(std::string(strrchr(filePath.c_str(), '/') ? strrchr(filePath.c_str(), '/') + 1 : filePath.c_str()) +
			" not found", LogVerbosity::warning);
	}
#endif
}

Elite::Vector2 GetRowCol(const size_t idx, const size_t nrOfCols)
{
	const size_t j = idx / nrOfCols;
	const size_t i = idx - (j * nrOfCols);
	return { static_cast<float>(i), static_cast<float>(j) };
}

size_t GetRow(size_t idx, size_t nrOfCols)
{
	const size_t j = idx / nrOfCols;
	const size_t i = idx - (j * nrOfCols);
	return i;
}

size_t GetCol(size_t idx, size_t nrOfCols)
{
	const size_t j = idx / nrOfCols;
	return j;
}

size_t GetIndex(const Elite::Vector2 rowCol, const size_t nrOfCols)
{
	return GetIndex(static_cast<size_t>(rowCol.x), static_cast<size_t>(rowCol.y), nrOfCols);
}
size_t GetIndex(size_t row, size_t col, size_t nrOfCols)
{
	return (col * nrOfCols) + row;
}
