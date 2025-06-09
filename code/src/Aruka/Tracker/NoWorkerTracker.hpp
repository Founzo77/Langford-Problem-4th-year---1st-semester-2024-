#pragma once

#include <cstddef>
#include <nlohmann/json.hpp>
#include <rapidjson/document.h>

namespace LRDF
{
	namespace Aruka
	{
		namespace Tracker
		{
			class NoWorkerTracker
			{
			public:
				inline NoWorkerTracker()
				{

				}

				inline void workStarted() const noexcept
				{

				}
				inline void workEnded() const noexcept
				{

				}

				inline void taskStarted() const noexcept
				{

				}
				inline void taskEnded() const noexcept
				{

				}

				inline void taskAddedStart() const noexcept
				{

				}

				inline void tasksAdded(std::size_t numberAdded) const noexcept
				{

				}

				inline void tasksRequested(std::size_t numberRequested) const noexcept
				{

				}
				inline void tasksReturned(std::size_t numberRequested) const noexcept
				{

				}

				inline void exporte(nlohmann::json& output) const noexcept
				{

				}
				inline void exporte(rapidjson::Document& output) const noexcept
				{

				}
			};
		}
	}
}