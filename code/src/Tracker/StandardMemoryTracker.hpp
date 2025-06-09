#pragma once

#include <nlohmann/json.hpp>
#include <rapidjson/document.h>

namespace LRDF
{
	namespace Tracker
	{
		class StandardMemoryTracker
		{
		public:
			inline void exporte(nlohmann::json& output) noexcept
			{

			}
			inline void exporte(rapidjson::Document& output) const noexcept
			{

			}
		};
	}
}