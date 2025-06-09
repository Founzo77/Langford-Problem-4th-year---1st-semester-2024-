#pragma once

#include <stdint.h>
#include <cstddef>
#include <queue>
#include <list>
#include <cstdlib>

#include <nlohmann/json.hpp>
#include <rapidjson/document.h>

#include <Aruka/Task.hpp>

namespace LRDF
{
	namespace Aruka
	{
		namespace OpenMP
		{
			template <uint8_t C, uint8_t F,
				typename ITaskAllocator,
				typename IHybridStaticDispatcherTracker>
			class HybridStaticDispatcher
			{		
			public:
				using m_Task = LRDF::Aruka::Task<C, F>;
				using m_DispatcherTracker = IHybridStaticDispatcherTracker;
				using m_TaskAllocator = ITaskAllocator;
				using m_MemoryTracker = typename m_TaskAllocator::m_MemoryTracker;
				using m_Dispatcher = std::vector<std::vector<m_Task>>;

			public:
				class Iterator
				{
				private:
					HybridStaticDispatcher& dispatcher;
					std::size_t currentBlockId;
					std::size_t internBlockId;
					std::size_t step;

				public:
					Iterator(HybridStaticDispatcher& dispatcher, 
						std::size_t idStart, std::size_t step) : dispatcher(dispatcher),
						currentBlockId(0), internBlockId(0), step(step)
					{
						std::size_t taskCount = 0;

						for (std::size_t i = 0; i < dispatcher.tasks.size(); ++i) {
							std::size_t blockSize = dispatcher.tasks[i].size();

							if (taskCount + blockSize > idStart) {
								currentBlockId = i;
								internBlockId = idStart - taskCount;

								break;
							}

							taskCount += blockSize;
						}
					}

					Iterator(const Iterator& other) : dispatcher(other.dispatcher),
						currentBlockId(other.currentBlockId),
						internBlockId(other.internBlockId), step(other.step)
					{
						
					}

					Iterator& operator=(const Iterator& other) noexcept
					{
						dispatcher = other.dispatcher;
						currentBlockId = other.currentBlockId;
						internBlockId = other.internBlockId;
						step = other.step;
					}

					inline void getTask(m_Task& task) noexcept
					{
						task = dispatcher.tasks[currentBlockId][internBlockId];
					}

					inline m_Task& getTask() noexcept
					{
						return dispatcher.tasks[currentBlockId][internBlockId];
					}

					inline void next() noexcept
					{
						internBlockId += step;

						while (currentBlockId < dispatcher.tasks.size()
							&&
							internBlockId >= dispatcher.tasks[currentBlockId].size())
						{
							internBlockId -= dispatcher.tasks[currentBlockId].size();
							currentBlockId++;
						}
					}
				};

			private:
				m_Dispatcher tasks;
				m_DispatcherTracker dispatcherTracker;
				std::size_t nbTasks;
				std::size_t sizeBlock;

			public:
				HybridStaticDispatcher(std::size_t sizeBlock) :
					tasks(), dispatcherTracker(), nbTasks(0), sizeBlock(sizeBlock)
				{

				}

				void registerWorker(std::size_t workerId)
				{
					dispatcherTracker.registerWorker(workerId);
					std::vector<m_Task> block;
					block.reserve(sizeBlock);
					//block.resize(sizeBlock, {});
					//block.clear();
					tasks.push_back(std::move(block));
				}

				std::size_t getTask(std::size_t workerId, std::size_t id, m_Task& task)
				{
					dispatcherTracker.tasksRequested(1);

					if (id < nbTasks)
					{
						std::size_t current = 0;
						for (auto& block : tasks)
						{
							if (current + block.size() > id)
							{
								current = id - current;
								task = std::move(block[current]);
							}
							else
								current += block.size();
						}

						dispatcherTracker.tasksReturned(workerId, 1);

						return 1;
					}
					else
					{
						dispatcherTracker.tasksReturned(workerId, 0);

						return 0;
					}
				}

				void addRootTask(m_Task& task)
				{
					dispatcherTracker.tasksAddedStart();

					tasks[0].push_back(task);
					nbTasks++;

					dispatcherTracker.tasksAdded(0, 1);
				}

				void swapD(std::size_t workerIdOldTasks, 
					std::vector<m_Task>& newTasks) noexcept
				{
					dispatcherTracker.tasksAddedStart();

					nbTasks -= tasks[workerIdOldTasks].size();

					tasks[workerIdOldTasks].clear();
					tasks[workerIdOldTasks].swap(newTasks);

					nbTasks += tasks[workerIdOldTasks].size();

					dispatcherTracker.tasksAdded(workerIdOldTasks, tasks[workerIdOldTasks].size());
				}

				inline Iterator getIterator(std::size_t idStart, std::size_t step) noexcept
				{
					return Iterator(*this, idStart, step);
				}

				inline std::size_t getNbTasks() const noexcept
				{
					return nbTasks;
				}

				inline bool isFinish() const noexcept
				{
					return nbTasks == 0;
				}

				const m_DispatcherTracker& getDispatcherTracker() const noexcept
				{
					return dispatcherTracker;
				}

				const m_MemoryTracker getMemoryTracker() const noexcept
				{
					//return queue.get_allocator().getMemoryTracker();
					return m_MemoryTracker();
				}

				inline void exporte(nlohmann::json& output) const noexcept
				{
					dispatcherTracker.exporte(output);
					if (!output.empty())
					{
						output["id"] = 0;
					}
				}
				inline void exporte(rapidjson::Document& output) const noexcept
				{
					dispatcherTracker.exporte(output);

					if (!output.IsNull() && !output.ObjectEmpty())
					{
						rapidjson::Document::AllocatorType& allocator = output.GetAllocator();

						output.AddMember("id", 0, allocator);
					}
				}
			};
		}
	}
}