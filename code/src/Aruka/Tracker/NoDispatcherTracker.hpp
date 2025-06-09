#pragma once

#include <nlohmann/json.hpp>
#include <rapidjson/document.h>

#include <cstddef>


namespace LRDF
{
	namespace Aruka
	{
		namespace Tracker
		{
			class NoDispatcherTracker
			{
			public:
				inline NoDispatcherTracker()
				{

				}

				inline void registerWorker(std::size_t) const noexcept
				{

				}

				inline void taskRequested() const noexcept
				{

				}
				inline void taskReturned(std::size_t) const noexcept
				{

				}
				inline void taskReturned(std::size_t, std::size_t) const noexcept
				{

				}

				inline void tasksRequested(std::size_t) const noexcept
				{

				}
				inline void tasksReturned(std::size_t) const noexcept
				{

				}
				inline void tasksReturned(std::size_t, std::size_t) const noexcept
				{

				}

				inline void taskAddedStart() const noexcept
				{

				}
				inline void taskAdded() const noexcept
				{

				}

				inline void tasksAddedStart() const noexcept
				{

				}
				inline void tasksAdded(std::size_t) const noexcept
				{

				}
				inline void tasksAdded(std::size_t, std::size_t) const noexcept
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