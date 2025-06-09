#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <stdexcept>

#include <Tracker/Exporter.hpp>

namespace LRDF
{
	namespace Tracker
	{
		class StandardExporter : public Exporter
		{
		protected:
			std::unordered_map<std::string, nlohmann::json> outputs;

		public:
			StandardExporter(const std::string baseDirectoryPath) :
				Exporter(baseDirectoryPath), outputs()
			{

			}

			virtual void open()
			{
				
			}

			virtual void close()
			{
				for (const auto& pair : outputs)
				{
					if (!pair.second.empty())
					{
						std::string nameOutput(baseDirectoryPath + "/" + pair.first);
						std::ofstream file(nameOutput);

						if (file.is_open())
						{
							file.rdbuf()->pubsetbuf(0, 8192);
							file << pair.second.dump(4);
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
				for (const auto& pair : outputs)
				{
					if (!pair.second.empty())
					{
						std::string nameOutput(baseDirectoryPath + "/" + pair.first);
						std::ofstream file(nameOutput);

						if (file.is_open())
						{
							file.rdbuf()->pubsetbuf(0, 8192);
							file << pair.second.dump(4);
							file.close();
							std::cout << "Write for : " << nameOutput << std::endl;
						}
						else
						{
							std::cerr << "Failed to open file: " << nameOutput << std::endl;
						}
					}
				}
				outputs.clear();
			}

			virtual nlohmann::json& getFile(const std::string& fileName)
			{
				if (outputs.find(fileName) == outputs.end())
				{
					outputs[fileName] = nlohmann::json();
				}

				return outputs[fileName];
			}

			virtual nlohmann::json& getExistingFile(const std::string& fileName)
			{
				if (outputs.find(fileName) == outputs.end())
					throw std::runtime_error("File does not already exist : '" + fileName + "'");

				return outputs[fileName];
			}

			virtual nlohmann::json& getNonExistingFile(const std::string& fileName)
			{
				if (outputs.find(fileName) != outputs.end())
					throw std::runtime_error("File does already exist : '" + fileName + "'");

				return outputs[fileName];
			}
		};
	}
}