#pragma once

#include <nlohmann/json.hpp>
#include <chrono>
#include <omp.h>

namespace LRDF
{
	namespace Aruka
	{
		namespace OpenMP
		{
			namespace Tracker
			{
				class StandardResolverTracker
				{
				private:
					double initializationDuration;
					double workDuration;
					double mergeDuration;
					double start;
				public:
					inline StandardResolverTracker() : initializationDuration(0),
						workDuration(0), mergeDuration(0)
					{

					}

					inline void initializationStarted() noexcept
					{
						start = omp_get_wtime();
					}
					inline void initializationEnded() noexcept
					{
						double now = omp_get_wtime();

						auto elapsed = now - start;

						initializationDuration = elapsed * 1000;
					}

					inline void workStarted() noexcept
					{
						start = omp_get_wtime();
					}
					inline void workEnded() noexcept
					{
						double now = omp_get_wtime();

						double elapsed = now - start;

						workDuration = elapsed * 1000;
					}

					inline void mergeStarted() noexcept
					{
						start = omp_get_wtime();
					}
					inline void mergeEnded() noexcept
					{
						double now = omp_get_wtime();

						double elapsed = now - start;

						mergeDuration = elapsed * 1000;
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

						output.AddMember("initializationDuration", initializationDuration, allocator);
						output.AddMember("workDuration", workDuration, allocator);
						output.AddMember("mergeDuration", mergeDuration, allocator);
					}
				};
			}
		}
	}
}