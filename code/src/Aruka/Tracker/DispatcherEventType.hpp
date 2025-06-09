#pragma once

#include <string>

namespace LRDF
{
	namespace Aruka
	{
		namespace Tracker
		{
			class DispatcherEventType
			{
			public:
				enum class Value
				{
					WAIT,
					TASKS_REQUESTED,
					TASKS_ADDED
				};

			private:
				Value value;

			public:

				DispatcherEventType(Value value) : value(value)
				{

				}
				DispatcherEventType(const DispatcherEventType& other) : value(other.value)
				{

				}

				bool operator==(const DispatcherEventType& other) const
				{
					return value == other.value;
				}

				bool operator!=(const DispatcherEventType& other) const
				{
					return value != other.value;
				}

				std::string getName() const
				{
					switch (value)
					{
					case Value::WAIT:
						return std::string("wait");
					case Value::TASKS_REQUESTED:
						return std::string("tasks_requested");
					case Value::TASKS_ADDED:
						return std::string("tasks_added");
					default:
						return std::string("unknown");
					}
				}

			};
		}
	}
}