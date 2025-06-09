#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace LRDF
{
	namespace Tracker
	{
		class Exporter
		{
		protected:
			std::string baseDirectoryPath;

		public:
			Exporter(const std::string baseDirectoryPath) : 
				baseDirectoryPath(baseDirectoryPath)
			{

			}

			virtual void open() = 0;
			virtual void close() = 0;
			virtual void saveClear() = 0;

			virtual nlohmann::json& getFile(const std::string& fileName) = 0;
			virtual nlohmann::json& getExistingFile(const std::string& fileName) = 0;
			virtual nlohmann::json& getNonExistingFile(const std::string& fileName) = 0;
		};
	}
}