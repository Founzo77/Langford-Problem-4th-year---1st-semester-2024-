#pragma once

#include <cstddef>
#include <Aruka/Tracker/DispatcherEventType.hpp>

namespace LRDF
{
	namespace Aruka
	{
		namespace Tracker
		{
			template <typename TimeUnit>
			class DispatcherEvent
			{
			private:
				TimeUnit duration;
				std::size_t workerId;
				std::size_t numberInput;
				std::size_t numberOutput;
				int orderKey;
				LRDF::Aruka::Tracker::DispatcherEventType type;

			public:
				DispatcherEvent(TimeUnit duration, std::size_t workerId, std::size_t numberInput, std::size_t numberOutput,
					int orderKey, LRDF::Aruka::Tracker::DispatcherEventType type) :
					duration(duration), workerId(workerId), numberInput(numberInput), numberOutput(numberOutput), orderKey(orderKey),
					type(type)
				{

				}

				DispatcherEvent(TimeUnit duration,
					int orderKey, LRDF::Aruka::Tracker::DispatcherEventType type) :
					duration(duration), workerId(0), numberInput(0), numberOutput(0), orderKey(orderKey),
					type(type)
				{

				}

				inline TimeUnit getDuration() const noexcept
				{
					return duration;
				}
				inline std::size_t getWorkerId() const noexcept
				{
					return workerId;
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
				inline LRDF::Aruka::Tracker::DispatcherEventType getType() const noexcept
				{
					return type;
				}
			};
		}
	}
}