#pragma once

#include <nlohmann/json.hpp>
#include <chrono>

namespace LRDF
{
	namespace Aruka
	{
		namespace Mono
		{
			namespace Tracker
			{
				class StandardResolverTracker
				{
				private:
					long long initializationDuration;
					long long workDuration;
					long long mergeDuration;
					std::chrono::time_point<std::chrono::high_resolution_clock>
						start;
				public:
					inline StandardResolverTracker() : initializationDuration(0),
						workDuration(0), mergeDuration(0)
					{

					}

					inline void initializationStarted() noexcept
					{
						start = std::chrono::high_resolution_clock::now();
					}
					inline void initializationEnded() noexcept
					{
						std::chrono::time_point<std::chrono::high_resolution_clock> now
							= std::chrono::high_resolution_clock::now();

						auto elapsed =
							std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

						initializationDuration = elapsed.count();
					}

					inline void workStarted() noexcept
					{
						start = std::chrono::high_resolution_clock::now();
					}
					inline void workEnded() noexcept
					{
						std::chrono::time_point<std::chrono::high_resolution_clock> now
							= std::chrono::high_resolution_clock::now();

						auto elapsed =
							std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

						workDuration = elapsed.count();
					}

					inline void mergeStarted() noexcept
					{
						start = std::chrono::high_resolution_clock::now();
					}
					inline void mergeEnded() noexcept
					{
						std::chrono::time_point<std::chrono::high_resolution_clock> now
							= std::chrono::high_resolution_clock::now();

						auto elapsed =
							std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

						mergeDuration = elapsed.count();
					}

					inline void exporte(nlohmann::json& output) const noexcept
					{
						output["initializationDuration"] = initializationDuration;
						output["workDuration"] = workDuration;
						output["mergeDuration"] = mergeDuration;
					}
					inline void exporte(rapidjson::Document& output) const noexcept
					{
						rapidjson::Document::AllocatorType& allocator = output.GetAllocator();

						output.AddMember("initializationDuration", static_cast<int64_t>(initializationDuration), allocator);
						output.AddMember("workDuration", static_cast<int64_t>(workDuration), allocator);
						output.AddMember("mergeDuration", static_cast<int64_t>(mergeDuration), allocator);
					}
				};
			}
		}
	}
}