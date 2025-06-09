#pragma once

#include <stdint.h>
#include <cstddef>
#include <queue>
#include <thread>

#include <vector>

#include <Aruka/Task.hpp>

namespace LRDF
{
	namespace Aruka
	{
		namespace OpenMP
		{
			template <uint8_t C, uint8_t F,
				typename ITaskAllocator, typename IParallelDispatcherTracker>
			class ConcurrentParallelQueueDispatcher
			{
			public:
				template<typename _Ty, class _Container = std::deque<_Ty>>
				class AccessibleQueue : public std::queue<_Ty, _Container> {
				public:
					const typename std::queue<_Ty, _Container>::container_type& getContainer() const noexcept {
						return this->c;
					}
				};

			public:
				using m_Task = LRDF::Aruka::Task<C, F>;
				using m_DispatcherTracker = IParallelDispatcherTracker;
				using m_TaskAllocator = ITaskAllocator;
				using m_MemoryTracker = typename m_TaskAllocator::m_MemoryTracker;
				using m_queue = AccessibleQueue<m_Task, std::deque<m_Task, m_TaskAllocator>>;

			private:
				m_queue queue;
				m_DispatcherTracker dispatcherTracker;
				std::size_t nbWorkers;
				std::size_t nbWorkersIsFinished;

			public:
				ConcurrentParallelQueueDispatcher() : queue(), dispatcherTracker(),
					nbWorkersIsFinished(0), nbWorkers(0)
				{

				}

				void registerWorker(std::size_t workerId)
				{
					#pragma omp critical(write)
					{
						nbWorkers++;
						dispatcherTracker.registerWorker(workerId);
					}				
				}


				std::size_t getTask(std::size_t workerId, m_Task* task)
				{
					std::size_t nbTasksReturned;

					#pragma omp critical(write)
					{
						dispatcherTracker.tasksRequested(1);

						if (!queue.empty())
						{
							*task = std::move(queue.front());
							queue.pop();

							dispatcherTracker.tasksReturned(workerId, 1);

							nbTasksReturned = 1;
						}
						else
						{
							dispatcherTracker.tasksReturned(workerId, 0);
							nbTasksReturned =  0;
						}
					}

					return nbTasksReturned;
				}

				std::size_t getTasks(std::size_t workerId, std::size_t nbTasks, m_Task* tasks)
				{
					std::size_t nbTasksReturned = 0;

					#pragma omp critical(write)
					{
						dispatcherTracker.tasksRequested(nbTasks);

						while (nbTasksReturned < nbTasks && !queue.empty())
						{
							tasks[nbTasksReturned] = queue.front();
							queue.pop();
							nbTasksReturned++;
						}

						dispatcherTracker.tasksReturned(workerId, nbTasksReturned);
					}

					return nbTasksReturned;
				}

				std::size_t getTasks(std::size_t workerId, std::size_t nbTasks, std::vector<m_Task>& tasks)
				{
					std::size_t nbTasksReturned = 0;

					#pragma omp critical(write)
					{
						dispatcherTracker.tasksRequested(nbTasks);

						while (nbTasksReturned < nbTasks && !queue.empty())
						{
							tasks.push_back(queue.front());
							queue.pop();
							nbTasksReturned++;
						}

						dispatcherTracker.tasksReturned(workerId, nbTasksReturned);
					}

					return nbTasksReturned;
				}

				void addTask(std::size_t workerId, const m_Task& task)
				{
					#pragma omp critical(write)
					{
						dispatcherTracker.tasksAddedStart();

						queue.push(task);

						dispatcherTracker.tasksAdded(workerId, 1);
					}
				}

				void addTasks(std::size_t workerId, std::size_t nbTasks, const m_Task* tasks)
				{
					#pragma omp critical(write)
					{
						dispatcherTracker.tasksAddedStart();

						for (std::size_t i = 0; i < nbTasks; i++)
						{
							queue.push(tasks[i]);
						}

						dispatcherTracker.tasksAdded(workerId, nbTasks);
					}
				}

				void addTasks(std::size_t workerId, const std::vector<m_Task>& tasks)
				{
					#pragma omp critical(write)
					{
						dispatcherTracker.tasksAddedStart();

						for (const auto& task : tasks)
						{
							queue.push(task);
						}

						dispatcherTracker.tasksAdded(workerId, tasks.size());
					}
				}

				inline std::size_t transfert(std::size_t workerId,
					std::size_t nbTasksIn, const m_Task* tasksIn,
					std::size_t nbTasksOut, m_Task* tasksOut) noexcept
				{
					std::size_t nbTasksReturned = 0;

					#pragma omp critical(write)
					{
						dispatcherTracker.tasksAddedStart();

						for (std::size_t i = 0; i < nbTasksIn; i++)
						{
							queue.push(tasksIn[i]);
						}

						dispatcherTracker.tasksAdded(workerId, nbTasksIn);

						dispatcherTracker.tasksRequested(nbTasksOut);

						while (nbTasksReturned < nbTasksOut && !queue.empty())
						{
							tasksOut[nbTasksReturned] = queue.front();
							queue.pop();
							nbTasksReturned++;
						}

						dispatcherTracker.tasksReturned(workerId, nbTasksReturned);
					}

					return nbTasksReturned;
				}

				inline std::size_t transfert(std::size_t workerId,
					const std::vector<m_Task>& tasksIn, std::size_t nbTasksOut,
					std::vector<m_Task>& tasksOut) noexcept
				{
					std::size_t nbTasksReturned = 0;

					#pragma omp critical(write)
					{
						dispatcherTracker.tasksAddedStart();

						for (const auto& task : tasksIn)
						{
							queue.push(task);
						}

						dispatcherTracker.tasksAdded(workerId, tasksIn.size());

						dispatcherTracker.tasksRequested(nbTasksOut);

						while (nbTasksReturned < nbTasksOut && !queue.empty())
						{
							tasksOut.push_back(queue.front());
							queue.pop();
							nbTasksReturned++;
						}

						dispatcherTracker.tasksReturned(workerId, nbTasksReturned);
					}

					return nbTasksReturned;
				}

				inline std::size_t transfertRecommended(std::size_t workerId,
					const std::vector<m_Task>& tasksIn,
					std::vector<m_Task>& tasksOut) noexcept
				{
					std::size_t nbTasksReturned = 0;

					#pragma omp critical(write)
					{
						dispatcherTracker.tasksAddedStart();

						for (const auto& task : tasksIn)
						{
							queue.push(task);
						}

						dispatcherTracker.tasksAdded(workerId, tasksIn.size());

						std::size_t nbTasksOut = static_cast<std::size_t>(std::ceil(static_cast<double>(queue.size()) / nbWorkers));

						dispatcherTracker.tasksRequested(nbTasksOut);

						while (nbTasksReturned < nbTasksOut && !queue.empty())
						{
							tasksOut.push_back(queue.front());
							queue.pop();
							nbTasksReturned++;
						}

						dispatcherTracker.tasksReturned(workerId, nbTasksReturned);
					}

					return nbTasksReturned;
				}

				inline std::size_t getNbTasks() const noexcept
				{
					std::size_t size;

					#pragma omp critical(write)
					{
						size = queue.size();
					}

					return size;
				}

				inline bool isFinish()
				{
					bool result = false;
					bool leave = false;
					bool isFirst = true;

					while (!leave)
					{
						#pragma omp critical(write)
						{
							

							if (isFirst)
							{
								isFirst = false;
								nbWorkersIsFinished++;
							}

							if (!queue.empty())
							{
								leave = true;
								result = false;
								nbWorkersIsFinished--;
							}
							else
							{
								if (nbWorkers == nbWorkersIsFinished)
								{
									leave = true;
									result = true;
								}
							}
						}

						if(!leave)
						{
							std::this_thread::sleep_for(std::chrono::nanoseconds(1));
						}
					}

					return result;
				}

				const m_DispatcherTracker& getDispatcherTracker() const
				{
					return dispatcherTracker;
				}

				const m_MemoryTracker& getMemoryTracker() const
				{
					return queue.getContainer().get_allocator().getMemoryTracker();
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