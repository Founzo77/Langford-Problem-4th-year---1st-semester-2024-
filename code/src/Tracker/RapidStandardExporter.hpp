#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <stdexcept>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <Tracker/RapidExporter.hpp>

namespace LRDF
{
	namespace Tracker
	{
		class RapidStandardExporter : public RapidExporter
		{
		protected:
			std::unordered_map<std::string, rapidjson::Document> outputs;

		public:
			RapidStandardExporter(const std::string baseDirectoryPath) :
				RapidExporter(baseDirectoryPath), outputs()
			{

			}

			virtual void open()
			{

			}

			virtual void close()
			{
				std::vector<std::unordered_map<
					std::string, rapidjson::Document>::iterator> iterators;
				for (auto it = outputs.begin(); it != outputs.end(); ++it) {
					iterators.push_back(it);
				}

			#pragma omp parallel for
				for (int i = 0; i < (int)iterators.size(); i++)
				{
					if (!iterators[i]->second.IsNull() && !iterators[i]->second.ObjectEmpty())
					{
						std::string nameOutput(baseDirectoryPath + "/" + iterators[i]->first);
						std::ofstream file(nameOutput);

						if (file.is_open())
						{
							file.rdbuf()->pubsetbuf(0, 8192);

							rapidjson::StringBuffer buffer;
							rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
							iterators[i]->second.Accept(writer);

							file << buffer.GetString();
							file.close();
							std::cout << "Write for : " << nameOutput << std::endl;
						}
						else
						{
							std::cerr << "Failed to open file: " << nameOutput << std::endl;
						}
					}
				}
			}

			virtual void saveClear()
			{
				const size_t bufferSize = 542880000;
				char* buffer = new char[bufferSize];

				for (const auto& pair : outputs)
				{
					if (!pair.second.IsNull() && !pair.second.ObjectEmpty())
					{
						std::string nameOutput(baseDirectoryPath + "/" + pair.first);
						std::ofstream file(nameOutput, std::ios::out | std::ios::binary);

						if (file.is_open())
						{
							file.rdbuf()->pubsetbuf(buffer, bufferSize);

							rapidjson::OStreamWrapper osw(file);
							rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
							pair.second.Accept(writer);

							file.close();
							std::cout << "Write for : " << nameOutput << std::endl;
						}
						else
						{
							std::cerr << "Failed to open file: " << nameOutput << std::endl;
						}
					}
				}

				delete[] buffer;
				outputs.clear();
			}

			virtual rapidjson::Document& getFile(const std::string& fileName)
			{
				if (outputs.find(fileName) == outputs.end())
				{
					rapidjson::Document newDoc;
					newDoc.SetObject();
					outputs[fileName] = std::move(newDoc);
				}

				return outputs[fileName];
			}

			virtual rapidjson::Document& getExistingFile(const std::string& fileName)
			{
				if (outputs.find(fileName) == outputs.end())
					throw std::runtime_error("File does not already exist: '" + fileName + "'");

				return outputs[fileName];
			}

			virtual rapidjson::Document& getNonExistingFile(const std::string& fileName)
			{
				if (outputs.find(fileName) != outputs.end())
					throw std::runtime_error("File does already exist: '" + fileName + "'");

				rapidjson::Document newDoc;
				newDoc.SetObject();
				outputs[fileName] = std::move(newDoc);

				return outputs[fileName];
			}
		};
	}
}
