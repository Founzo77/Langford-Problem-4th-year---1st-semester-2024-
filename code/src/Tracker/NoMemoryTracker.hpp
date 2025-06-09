#pragma once

#include <cstddef>
#include <nlohmann/json.hpp>
#include <rapidjson/document.h>

namespace LRDF
{
	namespace Tracker
	{
		class NoMemoryTracker
		{
		public:
			inline NoMemoryTracker()
			{

			}

			inline void allocated(std::size_t size, const void* pointer) noexcept
			{

			}
			inline void freed(const void* pointer) noexcept
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