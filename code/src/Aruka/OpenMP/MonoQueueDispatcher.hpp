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
				typename IMonoDispatcherTracker>
			class MonoQueueDispatcher
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
				using m_DispatcherTracker = IMonoDispatcherTracker;
				using m_TaskAllocator = ITaskAllocator;
				using m_MemoryTracker = typename m_TaskAllocator::m_MemoryTracker;
				using m_Queue = std::list<m_Task, m_TaskAllocator>;
				//using m_Queue = List<m_Task>; La version propre class pas ouf

			private:
				m_Queue queue;
				m_DispatcherTracker dispatcherTracker;
				bool isFinished;
				std::size_t id;

			public:
				MonoQueueDispatcher(std::size_t dispatcherId, std::size_t workerId) :
					queue(), dispatcherTracker(), isFinished(false)
				{
					dispatcherTracker.registerWorker(workerId);
				}

				std::size_t getTask(std::size_t workerId, m_Task* task)
				{
					dispatcherTracker.tasksRequested(1);

					if (!queue.empty())
					{
						*task = std::move(queue.front());
						queue.pop_front();

						dispatcherTracker.tasksReturned(workerId, 1);

						return 1;
					}
					else
					{
						dispatcherTracker.tasksReturned(workerId, 0);
						return 0;
					}
				}

				std::size_t getTasks(std::size_t workerId, std::size_t nbTasks, m_Task* tasks)
				{
					dispatcherTracker.tasksRequested(nbTasks);

					std::size_t nbTasksGiven = 0;

					while (nbTasksGiven < nbTasks && !queue.empty())
					{
						tasks[nbTasksGiven] = std::move(queue.front());
						queue.pop_front();
						nbTasksGiven++;
					}

					dispatcherTracker.tasksReturned(nbTasksGiven);

					return nbTasksGiven;
				}

				std::size_t getTasks(std::size_t workerId, std::size_t nbTasks, MonoQueueDispatcher& dispatcher)
				{
					if (nbTasks == 0)
						return 0;

					dispatcherTracker.tasksRequested(nbTasks);

					std::size_t nbTasksGiven = std::min(nbTasks, queue.size());

					auto itBegin = queue.begin();
					std::advance(itBegin, nbTasksGiven);

					dispatcher.queue.splice(dispatcher.queue.end(), queue, queue.begin(), itBegin);

					dispatcherTracker.tasksReturned(workerId, nbTasksGiven);

					return nbTasksGiven;
				}

				std::size_t getAllTasks(std::size_t workerId, m_Task* tasks)
				{
					dispatcherTracker.tasksRequested(0);

					std::size_t nbTasksGiven = 0;

					while (!queue.empty())
					{
						tasks[nbTasksGiven] = std::move(queue.front());
						queue.pop_front();
						nbTasksGiven++;
					}

					dispatcherTracker.tasksReturned(workerId, nbTasksGiven);

					return nbTasksGiven;
				}

				std::size_t getAllTasks(std::size_t workerId, MonoQueueDispatcher& dispatcher)
				{
					dispatcherTracker.tasksRequested(0);

					std::size_t nbTasksGiven = queue.size();

					dispatcher.queue.splice(dispatcher.queue.end(), queue);

					dispatcherTracker.tasksReturned(workerId, nbTasksGiven);

					return nbTasksGiven;
				}

				void addTask(std::size_t workerId, const m_Task& task)
				{
					dispatcherTracker.tasksAddedStart();

					queue.push_back(task);

					dispatcherTracker.tasksAdded(workerId, 1);
				}

				void addTasks(std::size_t workerId, std::size_t nbTasks, const m_Task* tasks)
				{
					dispatcherTracker.tasksAddedStart();

					for (std::size_t i = 0; i < nbTasks; i++)
						queue.push_back(tasks[i]);

					dispatcherTracker.tasksAdded(nbTasks);
				}

				inline std::size_t getNbTasks() const noexcept
				{
					return queue.size();
				}

				inline void setFinish() noexcept
				{
					isFinished = true;
				}

				inline bool isFinish() const noexcept
				{
					return isFinished;
				}

				const m_DispatcherTracker& getDispatcherTracker() const noexcept
				{
					return dispatcherTracker;
				}

				const m_MemoryTracker getMemoryTracker() const noexcept
				{
					return queue.get_allocator().getMemoryTracker();
					//return m_MemoryTracker();
				}

				inline void exporte(nlohmann::json& output) const noexcept
				{
					dispatcherTracker.exporte(output);
					if (!output.empty())
					{
						output["id"] = id;
					}
				}
				inline void exporte(rapidjson::Document& output) const noexcept
				{
					dispatcherTracker.exporte(output);

					if (!output.IsNull() && !output.ObjectEmpty())
					{
						rapidjson::Document::AllocatorType& allocator = output.GetAllocator();

						output.AddMember("id", id, allocator);
					}
				}
			};
		}
	}
}