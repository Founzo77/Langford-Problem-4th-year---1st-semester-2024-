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
		namespace Mono
		{
			template <uint8_t C, uint8_t F,
				typename IDispatcher, typename IWorkerTracker>
			class StandardWorker
			{
			public:
				using m_Task = LRDF::Aruka::Task<C, F>;
				using m_Dispatcher = IDispatcher;
				using m_WorkerTracker = IWorkerTracker;
				using m_Solution = LRDF::Solution<C, F>;
				using m_Langford = LRDF::Langford<C, F>;

			private:
				m_Dispatcher& dispatcher;
				m_WorkerTracker workerTracker;
				std::vector<m_Solution> solutions;

			public:
				StandardWorker(m_Dispatcher& dispatcher) : dispatcher(dispatcher),
					workerTracker(), solutions()
				{

				}

				void work()
				{
					workerTracker.workStarted();

					m_Task task;

					while (!dispatcher.isFinish())
					{
						workerTracker.tasksRequested(1);

						dispatcher.getTask(&task);

						workerTracker.tasksReturned(1);
						
						workerTracker.taskStarted();
						
						resolveTask(task);

						workerTracker.taskEnded();
					}

					workerTracker.workEnded();
				}

			private:

				void resolveTask(const m_Task& task)
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
								dispatcher.addTask(newTask);
							}
						}
					}
				}

			public:
				inline const std::vector<m_Solution> getSolutions() const
				{
					return solutions;
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
						output["id"] = 0;
					}
				}
				inline void exporte(rapidjson::Document& output) const noexcept
				{
					workerTracker.exporte(output);

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