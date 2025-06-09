#pragma once

#include <string>

#include <rapidjson/document.h>

namespace LRDF
{
	namespace Tracker
	{
		class RapidExporter
		{
		protected:
			std::string baseDirectoryPath;

		public:
			RapidExporter(const std::string baseDirectoryPath) :
				baseDirectoryPath(baseDirectoryPath)
			{

			}

			virtual void open() = 0;
			virtual void close() = 0;
			virtual void saveClear() = 0;

			virtual rapidjson::Document& getFile(const std::string& fileName) = 0;
			virtual rapidjson::Document& getExistingFile(const std::string& fileName) = 0;
			virtual rapidjson::Document& getNonExistingFile(const std::string& fileName) = 0;
		};
	}
}