#pragma once

#include <cstddef>
#include <nlohmann/json.hpp>
#include <vector>
#include <chrono>
#include <omp.h>

#include <Aruka/Tracker/WorkerEvent.hpp>
#include <Aruka/Tracker/WorkerEventType.hpp>

namespace LRDF
{
	namespace Aruka
	{
		namespace OpenMP
		{
			namespace Tracker
			{
				class StandardWorkerTracker
				{
				private:
					std::vector<LRDF::Aruka::Tracker::WorkerEvent<double>>
						workerEvents;
					double workDuration;
					double start;
					double startWork;
					std::size_t input;

				public:
					inline StandardWorkerTracker() : workerEvents(),
						workDuration(0), start(omp_get_wtime()),
						startWork(), input(0)
					{

					}

					inline void workStarted() noexcept
					{
						startWork = omp_get_wtime();
					}
					inline void workEnded() noexcept
					{
						double now = omp_get_wtime();

						double elapsed = now - startWork;

						workDuration = elapsed / 1000;
					}

					inline void taskStarted() noexcept
					{
						pushEvent(LRDF::Aruka::Tracker::WorkerEventType::Value::WAIT);
					}
					inline void taskEnded() noexcept
					{
						pushEvent(LRDF::Aruka::Tracker::WorkerEventType::Value::TASK_RESOLVED);
					}

					inline void tasksRequested(std::size_t numberRequested) noexcept
					{
						pushEvent(LRDF::Aruka::Tracker::WorkerEventType::Value::WAIT);
						input = numberRequested;
					}
					inline void tasksReturned(std::size_t numberReturned) noexcept
					{
						pushEvent(
							LRDF::Aruka::Tracker::WorkerEventType::Value::TASKS_REQUESTED,
							input, numberReturned);
					}

					inline void taskAddedStart() noexcept
					{
						pushEvent(LRDF::Aruka::Tracker::WorkerEventType::Value::WAIT);
					}
					inline void tasksAdded(std::size_t numberAdded) noexcept
					{
						pushEvent(
							LRDF::Aruka::Tracker::WorkerEventType::Value::TASKS_ADDED,
							numberAdded, numberAdded);
					}

					inline void exporte(nlohmann::json& output) const noexcept
					{
						nlohmann::json workJson;
						output["workDuration"] = workDuration;

						nlohmann::json eventsJson = nlohmann::json::array();

						for (const LRDF::Aruka::Tracker::WorkerEvent<double>& event : workerEvents)
						{
							if (event.getType() == LRDF::Aruka::Tracker::WorkerEventType::Value::TASKS_REQUESTED)
							{
								eventsJson.push_back({
									{"duration", event.getDuration()},
									{"orderKey", event.getOrderKey()},
									{"numberInput", event.getNumberInput()},
									{"numberOutput", event.getNumberOutput()},
									{"type", event.getType().getName()}
									});
							}
							else // WAIT | TASK_RESOLVED
							{
								eventsJson.push_back({
									{"duration", event.getDuration()},
									{"orderKey", event.getOrderKey()},
									{"type", event.getType().getName()}
									});
							}
						}

						output["events"] = std::move(eventsJson);
					}
					inline void exporte(rapidjson::Document& output) const noexcept
					{
						rapidjson::Document::AllocatorType& allocator = output.GetAllocator();

						output.AddMember("workDuration", workDuration, allocator);

						rapidjson::Value eventsJson(rapidjson::kArrayType);

						for (const LRDF::Aruka::Tracker::WorkerEvent<double>& event : workerEvents)
						{
							rapidjson::Value eventJson(rapidjson::kObjectType);

							if (event.getType() == LRDF::Aruka::Tracker::WorkerEventType::Value::TASKS_REQUESTED)
							{
								eventJson.AddMember("duration", event.getDuration(), allocator);
								eventJson.AddMember("orderKey", event.getOrderKey(), allocator);
								eventJson.AddMember("numberInput", event.getNumberInput(), allocator);
								eventJson.AddMember("numberOutput", event.getNumberOutput(), allocator);

								rapidjson::Value typeName(event.getType().getName().c_str(), allocator);
								eventJson.AddMember("type", typeName, allocator);
							}
							else // WAIT | TASK_RESOLVED
							{
								eventJson.AddMember("duration", event.getDuration(), allocator);
								eventJson.AddMember("orderKey", event.getOrderKey(), allocator);

								rapidjson::Value typeName(event.getType().getName().c_str(), allocator);
								eventJson.AddMember("type", typeName, allocator);
							}

							eventsJson.PushBack(eventJson, allocator);
						}

						output.AddMember("events", eventsJson, allocator);
					}


				private:
					inline void pushEvent(LRDF::Aruka::Tracker::WorkerEventType type)
					{
						double now = omp_get_wtime();

						double elapsed = now - start;

						double duration = elapsed;

						workerEvents.push_back({
							duration * 1000, (int)workerEvents.size(), type
							});

						start = omp_get_wtime();
					}
					inline void pushEvent(LRDF::Aruka::Tracker::WorkerEventType type,
						std::size_t numberInput, std::size_t numberOutput)
					{
						double now = omp_get_wtime();

						double elapsed = now - start;

						double duration = elapsed;

						workerEvents.push_back({
							duration * 1000, numberInput, numberOutput, (int)workerEvents.size(), type
						});

						start = omp_get_wtime();
					}
				};
			}
		}
	}
}