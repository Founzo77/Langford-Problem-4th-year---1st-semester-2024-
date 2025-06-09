#pragma once

#include <stdint.h>
#include <queue>

#include <nlohmann/json.hpp>

#include <Aruka/Task.hpp>
#include <Langford.hpp>
#include <Solution.hpp>

namespace LRDF
{
	namespace Aruka
	{
		namespace MPI
		{
			template <uint8_t C, uint8_t F,
				typename IHybridStaticWorkerTracker>
			class HybridStaticWorker
			{
			public:
				using m_Task = LRDF::Aruka::Task<C, F>;
				using m_WorkerTracker = IHybridStaticWorkerTracker;
				using m_Solution = LRDF::Solution<C, F>;
				using m_Langford = LRDF::Langford<C, F>;

			private:
				m_WorkerTracker workerTracker;
				std::queue<m_Task> d;
				std::vector<m_Solution> solutions;
				std::size_t id;

			public:
				HybridStaticWorker(std::size_t id) :
					workerTracker(), d(), solutions(), id(id)
				{
					//d.resize(sizeTasksBlock, {});
					//d.clear();
					//d.reserve(sizeTasksBlock);
					solutions.reserve(100000000);
				}

			public:
				void resolveTree(m_Task& rootTask)
				{
					d.push(rootTask);

					while (d.empty() == false)
					{
						m_Task tast = d.front();
						d.pop();

						resolveTask(tast);
					}
				}

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

								d.push(newTask);
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

				inline std::vector<m_Task>& getD() noexcept
				{
					return d;
				}

				inline m_WorkerTracker& getWorkerTracker() noexcept
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