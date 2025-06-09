#pragma once

#include <cstddef>
#include <Aruka/Tracker/WorkerEventType.hpp>

namespace LRDF
{
	namespace Aruka
	{
		namespace Tracker
		{
			template <typename TimeUnit>
			class WorkerEvent
			{
			private:
				TimeUnit duration;
				std::size_t numberInput;
				std::size_t numberOutput;
				int orderKey;
				LRDF::Aruka::Tracker::WorkerEventType type;

			public:
				WorkerEvent(TimeUnit duration, std::size_t numberInput, std::size_t numberOutput,
					int orderKey, LRDF::Aruka::Tracker::WorkerEventType type) :
					duration(duration), numberInput(numberInput), numberOutput(numberOutput), orderKey(orderKey),
					type(type)
				{

				}

				WorkerEvent(TimeUnit duration,
					int orderKey, LRDF::Aruka::Tracker::WorkerEventType type) :
					duration(duration), numberInput(0), numberOutput(0), orderKey(orderKey),
					type(type)
				{

				}

				inline TimeUnit getDuration() const noexcept
				{
					return duration;
				}
				inline std::size_t getNumberInput() const noexcept
				{
					return numberInput;
				}
				inline std::size_t getNumberOutput() const noexcept
				{
					return numberOutput;
				}
				inline int getOrderKey() const noexcept
				{
					return orderKey;
				}
				inline LRDF::Aruka::Tracker::WorkerEventType getType() const noexcept
				{
					return type;
				}
			};
		}
	}
}