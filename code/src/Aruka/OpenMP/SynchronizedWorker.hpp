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
				typename IMonoDispatcher, typename ISynchronizedWorkerTracker>
			class SynchronizedWorker
			{
			public:
				using m_Task = LRDF::Aruka::Task<C, F>;
				using m_Dispatcher = IMonoDispatcher;
				using m_WorkerTracker = ISynchronizedWorkerTracker;
				using m_Solution = LRDF::Solution<C, F>;
				using m_Langford = LRDF::Langford<C, F>;

			private:
				m_WorkerTracker workerTracker;
				m_Dispatcher t;
				m_Dispatcher d;
				std::vector<m_Solution> solutions;
				std::size_t id;

			public:
				SynchronizedWorker(std::size_t id) :
					workerTracker(), t(id, id), d(id, id), id(id)
				{
					workerTracker.workStarted();
				}

				void work()
				{
					workerTracker.taskStarted();

					m_Task task;

					while (t.getNbTasks() != 0)
					{
						t.getTask(id, &task);

						resolveTask(task);
					}
					
					workerTracker.taskEnded();
				}

				inline void finish() noexcept
				{
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

								d.addTask(id, newTask);
							}
						}
					}
				}

			public:
				inline m_Dispatcher& getT() noexcept
				{
					return t;
				}

				inline m_Dispatcher& getD() noexcept
				{
					return d;
				}

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