#pragma once

#include <stdint.h>
#include <vector>

#include <nlohmann/json.hpp>

#include <Aruka/Task.hpp>
#include <Langford.hpp>
#include <Solution.hpp>

namespace LRDF
{
	namespace Aruka
	{
		namespace OpenMP
		{
			template <uint8_t C, uint8_t F,
				typename IParallelDispatcher, typename IParallelWorkerTracker>
			class ParallelWorker
			{
			public:
				using m_Task = LRDF::Aruka::Task<C, F>;
				using m_Dispatcher = IParallelDispatcher;
				using m_WorkerTracker = IParallelWorkerTracker;
				using m_Solution = LRDF::Solution<C, F>;
				using m_Langford = LRDF::Langford<C, F>;

			private:
				m_WorkerTracker workerTracker;
				m_Dispatcher& dispatcher;
				std::vector<m_Solution> solutions;
				std::size_t id;

			public:
				constexpr static std::size_t TASKS_BLOCK = 5000;

			public:
				ParallelWorker(std::size_t id, m_Dispatcher& dispatcher) :
					workerTracker(), dispatcher(dispatcher), solutions(), id(id)
				{

				}

				void work()
				{
					workerTracker.workStarted();

					//m_Task tasks[TASKS_BLOCK]; 
					std::vector<m_Task> nextTasks;
					nextTasks.reserve(1000000);
					std::vector<m_Task> tasks;
					tasks.reserve(10000000);

					bool isFinished = false;

					while (!isFinished)
					{
						workerTracker.tasksRequested(0);

						std::size_t nbTask = dispatcher.transfertRecommended(
							id, nextTasks, tasks);

						workerTracker.tasksReturned(nbTask);

						if (nbTask > 0)
						{
							for (std::size_t i = 0; i < nbTask; i++)
							{
								workerTracker.taskStarted();

								resolveTask(tasks[i], nextTasks);

								workerTracker.taskEnded();
							}

							tasks.clear();

							if (nextTasks.size() > 0)
							{
								workerTracker.taskAddedStart();

								dispatcher.addTasks(
									id, nextTasks.size(), nextTasks.data());

								workerTracker.tasksAdded(nextTasks.size());

								nextTasks.clear();
							}
						}
						else
						{
							isFinished = dispatcher.isFinish();
						}
					}

					workerTracker.workEnded();
				}
				
			private:
				void resolveTask(const m_Task& task, std::vector<m_Task>& nextTask)
				{
					uint8_t currentFrequency = task.getCurrentFrequency();
					m_Langford state(task.getState());

					bool isPair = currentFrequency % 2 == 0;

					if (currentFrequency == 0)
					{
						solutions.push_back(m_Solution(state));
						return;
					}

					for (uint16_t i = 0; i < state.getLength(); i++)
					{
						if (task.getIsSymmetryVerification() &&
							(
								(!isPair && i + 1 > state.getLength() - (i + currentFrequency + 2)) ||
								(isPair && i + 1 > state.getLength() - (i + currentFrequency + 1))
								)
							)
						{
							break;
						}

						bool isNextVerification = false;

						if (task.getIsSymmetryVerification() && isPair &&
							i == state.getLength() - (i + currentFrequency + 2))
						{
							isNextVerification = true;
						}

						if (state[i] == 0)
						{
							m_Langford newState(state);
							uint16_t count = 0;
							uint16_t j = i;

							while (j < state.getLength() && count < state.getCount())
							{
								if (newState[j] == 0)
								{
									newState[j] = currentFrequency;
									count++;
								}
								else
								{
									break;
									count = 0;
								}

								j += currentFrequency + 1;
							}

							if (count == state.getCount())
							{
								m_Task newTask(newState, currentFrequency - 1, isNextVerification);
								nextTask.push_back(newTask);
							}
						}
					}
				}

			public:
				inline const std::vector<m_Solution>& getSolutions() const noexcept
				{
					return solutions;
				}

				inline std::size_t getId() const noexcept
				{
					return id;
				}

				inline const m_WorkerTracker& getWorkerTracker() const
				{
					return workerTracker;
				}

				inline void exporte(nlohmann::json& output) const noexcept
				{
					workerTracker.exporte(output);

					if (!output.empty())
					{
						output["id"] = id;
					}
				}
				inline void exporte(rapidjson::Document& output) const noexcept
				{
					workerTracker.exporte(output);

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