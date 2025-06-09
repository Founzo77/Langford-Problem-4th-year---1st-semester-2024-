#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include <chrono>
#include <omp.h>

#include <Aruka/Tracker/DispatcherEvent.hpp>

namespace LRDF
{
	namespace Aruka
	{
		namespace OpenMP
		{
			namespace Tracker
			{
				class StandardDispatcherTracker
				{
				private:
					std::vector<Aruka::Tracker::DispatcherEvent<double>> events;
					double start;
					std::size_t numberIntput;
					std::vector<std::size_t> workersId;

				public:
					inline StandardDispatcherTracker() : events(),
						start(omp_get_wtime()), numberIntput(0), workersId()
					{

					}

					inline void registerWorker(std::size_t id) noexcept
					{
						workersId.push_back(id);
					}

					inline void tasksRequested(std::size_t numberRequested) noexcept
					{
						numberIntput = numberRequested;
						pushEvent(LRDF::Aruka::Tracker::DispatcherEventType::Value::WAIT);
					}
					inline void tasksReturned(std::size_t workerId, std::size_t numberReturned) noexcept
					{
						pushEvent(LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_REQUESTED, workerId, numberIntput, numberReturned);
					}

					inline void tasksAddedStart() noexcept
					{
						pushEvent(LRDF::Aruka::Tracker::DispatcherEventType::Value::WAIT);
					}
					inline void tasksAdded(std::size_t workerId, std::size_t numberAdded) noexcept
					{
						pushEvent(LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_ADDED,
							workerId, 0, numberAdded);
					}

					inline void exporte(nlohmann::json& output) const noexcept
					{
						nlohmann::json eventsJson = nlohmann::json::array();

						for (const LRDF::Aruka::Tracker::DispatcherEvent<double>& event : events)
						{
							if (event.getType() == LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_ADDED ||
								event.getType() == LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_REQUESTED)
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

						for (const auto& workerId : workersId)
						{
							workersIdJson.push_back(workerId);
						}

						output["workersId"] = workersIdJson;
					}
					inline void exporte(rapidjson::Document& output) const noexcept
					{
						rapidjson::Document::AllocatorType& allocator = output.GetAllocator();

						rapidjson::Value eventsJson(rapidjson::kArrayType);

						for (const LRDF::Aruka::Tracker::DispatcherEvent<double>& event : events)
						{
							rapidjson::Value eventJson(rapidjson::kObjectType);

							if (event.getType() == LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_ADDED ||
								event.getType() == LRDF::Aruka::Tracker::DispatcherEventType::Value::TASKS_REQUESTED)
							{
								eventJson.AddMember("duration", event.getDuration(), allocator);
								eventJson.AddMember("orderKey", event.getOrderKey(), allocator);
								eventJson.AddMember("workerId", event.getWorkerId(), allocator);
								eventJson.AddMember("numberInput", event.getNumberInput(), allocator);
								eventJson.AddMember("numberOutput", event.getNumberOutput(), allocator);

								rapidjson::Value typeName(event.getType().getName().c_str(), allocator);
								eventJson.AddMember("type", typeName, allocator);
							}
							else // WAIT
							{
								eventJson.AddMember("duration", event.getDuration(), allocator);
								eventJson.AddMember("orderKey", event.getOrderKey(), allocator);

								rapidjson::Value typeName(event.getType().getName().c_str(), allocator);
								eventJson.AddMember("type", typeName, allocator);
							}

							eventsJson.PushBack(eventJson, allocator);
						}

						output.AddMember("events", eventsJson, allocator);

						rapidjson::Value workersIdJson(rapidjson::kArrayType);

						for (const auto& workerId : workersId)
						{
							workersIdJson.PushBack(workerId, allocator);
						}

						output.AddMember("workersId", workersIdJson, allocator);
					}


				private:
					inline void pushEvent(LRDF::Aruka::Tracker::DispatcherEventType type)
					{
						double now = omp_get_wtime();

						double elapsed = now - start;

						double duration = elapsed;

						events.push_back({
							duration * 1000, (int)events.size(), type
						});

						start = omp_get_wtime();
					}
					inline void pushEvent(LRDF::Aruka::Tracker::DispatcherEventType type,
						std::size_t workerId, std::size_t numberInput, std::size_t numberOutput)
					{
						double now = omp_get_wtime();

						double elapsed = now - start;

						double duration = elapsed;

						events.push_back({
							duration * 1000, workerId, numberInput, numberOutput, (int)events.size(), type
						});

						start = omp_get_wtime();
					}
				};
			}
		}
	}
}