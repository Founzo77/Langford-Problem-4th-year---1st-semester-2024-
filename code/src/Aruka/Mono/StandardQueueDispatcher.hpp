#pragma once

#include <stdint.h>
#include <cstddef>
#include <queue>

#include <nlohmann/json.hpp>

#include <Aruka/Task.hpp>

namespace LRDF
{
	namespace Aruka
	{
		namespace Mono
		{
			template <uint8_t C, uint8_t F,
				typename ITaskAllocator,
				typename IDispatcherTracker>
			class StandardQueueDispatcher
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
				using m_DispatcherTracker = IDispatcherTracker;
				using m_TaskAllocator = ITaskAllocator;
				using m_MemoryTracker = typename m_TaskAllocator::m_MemoryTracker;
				using m_queue = AccessibleQueue<m_Task, std::deque<m_Task, ITaskAllocator>>;

			private:
				m_queue queue;
				m_DispatcherTracker dispatcherTracker;

			public:
				StandardQueueDispatcher() : queue(), dispatcherTracker()
				{
					
				}

				std::size_t getTask(m_Task* task)
				{
					dispatcherTracker.tasksRequested(1);

					if (!queue.empty())
					{
						*task = std::move(queue.front());
						queue.pop();

						dispatcherTracker.tasksReturned(1);

						return 1;
					}
					else
					{
						dispatcherTracker.tasksReturned(0);
						return 0;
					}
				}

				std::size_t getTasks(std::size_t nbTasks, m_Task* tasks)
				{
					dispatcherTracker.tasksRequested(nbTasks);

					std::size_t nbTasksGiven = 0;

					while (nbTasksGiven < nbTasks && !queue.empty())
					{

						tasks[nbTasksGiven] = queue.front();
						queue.pop();
						nbTasksGiven++;
					}

					dispatcherTracker.tasksReturned(nbTasksGiven);

					return nbTasksGiven;
				}

				void addTask(const m_Task& task)
				{
					dispatcherTracker.tasksAddedStart();

					queue.push(task);

					dispatcherTracker.tasksAdded(1);
				}

				void addTasks(std::size_t nbTasks, const m_Task* tasks)
				{
					dispatcherTracker.tasksAddedStart();

					for (std::size_t i = 0; i < nbTasks; i++)
						queue.push(tasks[nbTasks]);

					dispatcherTracker.tasksAdded(nbTasks);
				}

				inline bool isFinish() const
				{
					return queue.empty();
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