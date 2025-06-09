#pragma once

#include <nlohmann/json.hpp>
#include <rapidjson/document.h>

namespace LRDF
{
	namespace Aruka
	{
		namespace Tracker
		{
			class NoResolverTracker
			{
			public:
				inline NoResolverTracker()
				{

				}

				inline void initializationStarted() const noexcept
				{

				}
				inline void initializationEnded() const noexcept
				{

				}

				inline void workStarted() const noexcept
				{

				}
				inline void workEnded() const noexcept
				{

				}

				inline void mergeStarted() const noexcept
				{

				}
				inline void mergeEnded() const noexcept
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