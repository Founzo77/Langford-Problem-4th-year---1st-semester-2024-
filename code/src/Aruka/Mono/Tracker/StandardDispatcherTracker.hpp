#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include <chrono>

#include <Aruka/Tracker/DispatcherEvent.hpp>

#include <rapidjson/document.h>

namespace LRDF
{
	namespace Aruka
	{
		namespace Mono
		{
			namespace Tracker
			{
				class StandardDispatcherTracker
				{
				private:
					std::vector<Aruka::Tracker::DispatcherEvent<long long>> events;
					std::chrono::time_point<std::chrono::high_resolution_clock> 
						start;
					std::size_t numberIntput;

				public:
					inline StandardDispatcherTracker() : events(),
						start(std::chrono::high_resolution_clock::now()), numberIntput(0)
					{

					}

					inline void registerWorker(std::size_t) const noexcept
					{

					}

					inline void tasksRequested(std::size_t numberRequested) noexcept
					{
						numberIntput = numberRequested;
						pushEvent(LRDF::Aruka::Tracker::DispatcherEventType::Value::WAIT);
					}
					inline void tasksReturned(std::size_t numberReturned) noexcept
					{
						pushEvent(LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_REQUESTED, numberIntput, numberReturned);
					}

					inline void tasksAddedStart() noexcept
					{
						pushEvent(LRDF::Aruka::Tracker::DispatcherEventType::Value::WAIT);
					}
					inline void tasksAdded(std::size_t numberAdded) noexcept
					{
						pushEvent(LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_ADDED,
							0, numberAdded);
					}

					inline void exporte(nlohmann::json& output) const noexcept
					{
						nlohmann::json eventsJson = nlohmann::json::array();

						for (const LRDF::Aruka::Tracker::DispatcherEvent<long long>& event : events)
						{
							if (event.getType() == LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_ADDED ||
								event.getType() == LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_REQUESTED )
							{
								eventsJson.push_back({
									{"duration", event.getDuration()},
									{"orderKey", event.getOrderKey()},
									{"workerId", event.getWorkerId()},
									{"numberInput", event.getNumberInput()},
									{"numberOutput", event.getNumberOutput()},
									{"type", event.getType().getName()}
								});
							}
							else // WAIT
							{
								eventsJson.push_back({
									{"duration", event.getDuration()},
									{"orderKey", event.getOrderKey()},
									{"type", event.getType().getName()}
								});
							}
						}

						output["events"] = std::move(eventsJson);

						nlohmann::json workersIdJson = nlohmann::json::array();
						workersIdJson.push_back(0);
						output["workersId"] = workersIdJson;
					}
					inline void exporte(rapidjson::Document& output) const noexcept
					{
						rapidjson::Document::AllocatorType& allocator = 
							output.GetAllocator();

						rapidjson::Value eventsJson(rapidjson::kArrayType);

						for (const LRDF::Aruka::Tracker::DispatcherEvent<long long>& event : events)
						{
							rapidjson::Value eventJson(rapidjson::kObjectType);

							if (event.getType() == LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_ADDED ||
								event.getType() == LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_REQUESTED)
							{
								eventJson.AddMember("duration", static_cast<int64_t>(event.getDuration()), allocator);
								eventJson.AddMember("orderKey", event.getOrderKey(), allocator);
								eventJson.AddMember("workerId", event.getWorkerId(), allocator);
								eventJson.AddMember("numberInput", event.getNumberInput(), allocator);
								eventJson.AddMember("numberOutput", event.getNumberOutput(), allocator);

								rapidjson::Value typeName(event.getType().getName().c_str(), allocator);
								eventJson.AddMember("type", typeName, allocator);
							}
							else // WAIT
							{
								eventJson.AddMember("duration", static_cast<int64_t>(event.getDuration()), allocator);
								eventJson.AddMember("orderKey", event.getOrderKey(), allocator);

								rapidjson::Value typeName(event.getType().getName().c_str(), allocator);
								eventJson.AddMember("type", typeName, allocator);
							}

							eventsJson.PushBack(eventJson, allocator);
						}

						output.AddMember("events", eventsJson, allocator);

						rapidjson::Value workersIdJson(rapidjson::kArrayType);
						workersIdJson.PushBack(0, allocator);

						output.AddMember("workersId", workersIdJson, allocator);
					}


				private:
					inline void pushEvent(LRDF::Aruka::Tracker::DispatcherEventType type)
					{
						std::chrono::time_point<std::chrono::high_resolution_clock> now
							= std::chrono::high_resolution_clock::now();

						auto elapsed =
							std::chrono::duration_cast<std::chrono::milliseconds> (now - start);

						long long duration = elapsed.count();

						events.push_back(LRDF::Aruka::Tracker::DispatcherEvent<long long>(
							duration, (int)events.size(), type
						));

						start = std::chrono::high_resolution_clock::now();
					}
					inline void pushEvent(LRDF::Aruka::Tracker::DispatcherEventType type,
						std::size_t numberInput, std::size_t numberOutput)
					{
						std::chrono::time_point<std::chrono::high_resolution_clock> now
							= std::chrono::high_resolution_clock::now();

						auto elapsed =
							std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

						long long duration = elapsed.count();

						events.push_back(LRDF::Aruka::Tracker::DispatcherEvent<long long>(
							duration, 0, numberInput, numberOutput, (int)events.size(), type
						));

						start = std::chrono::high_resolution_clock::now();
					}
				};
			}
		}
	}
}