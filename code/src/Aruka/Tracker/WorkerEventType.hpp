#pragma once

#include <string>

namespace LRDF
{
	namespace Aruka
	{
		namespace Tracker
		{
			class WorkerEventType
			{
			public:
				enum class Value
				{
					WAIT,
					TASKS_REQUESTED,
					TASKS_ADDED,
					TASK_RESOLVED
				};

			private:
				Value value;

			public:

				WorkerEventType(Value value) : value(value)
				{

				}
				WorkerEventType(const WorkerEventType& other) : value(other.value)
				{

				}

				bool operator==(const WorkerEventType& other) const
				{
					return value == other.value;
				}

				bool operator!=(const WorkerEventType& other) const
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
					case Value::TASK_RESOLVED:
						return std::string("task_resolved");
					default:
						return std::string("unknown");
					}
				}

			};
		}
	}
}