#pragma once

#include <stdint.h>
#include <cstddef>
#include <vector>
#include <string>

#include <Solution.hpp>

#include <Langford.hpp>

#include <Aruka/Task.hpp>
#include <Tracker/Exporter.hpp>
#include <Tracker/RapidExporter.hpp>

#include <rapidjson/document.h>

#include <omp.h>

namespace LRDF
{
	namespace Aruka
	{
		namespace OpenMP
		{
			template <uint8_t C, uint8_t F,
				typename IHybridStaticWorker,
				typename IHybridStaticDispatcher,
				typename IHybridStaticResolverTracker,
				std::size_t NB_BYTES = static_cast<std::size_t>(4e10),
				std::size_t ROOT_DEPTH = 1>
			class HybridStaticResolver
			{
			public:
				using m_Worker = IHybridStaticWorker;
				using m_Dispatcher = IHybridStaticDispatcher;
				using m_ResolverTracker = IHybridStaticResolverTracker;
				using m_Task = LRDF::Aruka::Task<C, F>;
				using m_Langford = LRDF::Langford<C, F>;
				using m_Solution = LRDF::Solution<C, F>;

			private:
				std::vector<m_Worker> workers;
				m_ResolverTracker resolverTracker;
				m_Dispatcher dispatcher;

			public:
				static std::size_t getDispatcherSizeTasksBlock() noexcept
				{
					return NB_BYTES / 2 / omp_get_max_threads() / sizeof(m_Task);
				}
				static std::size_t getWorkersSizeTasksBlock() noexcept
				{
					return NB_BYTES / 2 / omp_get_max_threads() / sizeof(m_Task);
				}

			public:
				HybridStaticResolver() :
					workers(), resolverTracker(), dispatcher(getDispatcherSizeTasksBlock())
				{

				}

				void resolve(std::vector<m_Solution>& solutions)
				{
					resolverTracker.initializationStarted();

					m_Dispatcher& dispatcher = this->dispatcher;
					std::vector<m_Worker>& workers = this->workers;

					#pragma omp parallel shared(dispatcher)
					{
						#pragma omp critical
						{
							std::size_t id = (std::size_t)omp_get_thread_num();
							workers.push_back({id, getWorkersSizeTasksBlock()});
							dispatcher.registerWorker(id);
						}
					}

					resolverTracker.initializationEnded();
					resolverTracker.workStarted();

					m_Task rootTask(m_Langford(), F, 1);
					
					resolveRoot(rootTask);

					resolverTracker.workEnded();
					resolverTracker.mergeStarted();

					for (auto& worker : workers)
					{
						solutions.insert(solutions.end(),
							worker.getSolutions().cbegin(), worker.getSolutions().cend());
					}

					resolverTracker.mergeEnded();
				}

			private:
				void resolveRoot(m_Task& task)
				{
					uint8_t currentFrequency = task.getCurrentFrequency();

					if (currentFrequency == F - ROOT_DEPTH)
					{
						resolveTree(task);
						return;
					}

					m_Langford state(task.getState());

					bool isPair = currentFrequency % 2 == 0;

					if (currentFrequency == 0)
					{
						//solutions.push_back(m_Solution(state));
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

								resolveRoot(newTask);
							}
						}
					}
				}

				void resolveTree(m_Task& rootTask)
				{
					m_Dispatcher& dispatcher = this->dispatcher;
					std::vector<m_Worker>& workers = this->workers;
					
					dispatcher.addRootTask(rootTask);

					#pragma omp parallel shared(dispatcher, workers)
					{
						std::size_t id = (std::size_t)omp_get_thread_num();
						std::size_t nbThreads = (std::size_t)omp_get_num_threads();

						m_Task task;

						workers[id].getWorkerTracker().workStarted();

						while (dispatcher.getNbTasks() != 0)
						{
							std::size_t totalTasks = dispatcher.getNbTasks();

							auto iterator(dispatcher.getIterator(id, nbThreads));

							workers[id].getWorkerTracker().taskStarted();

							for (std::size_t i = id; i < totalTasks; i += nbThreads)
							{
								//iterator.getTask(task);

								//workers[id].resolveTask(task);

								workers[id].resolveTask(iterator.getTask());

								iterator.next();
							}

							workers[id].getWorkerTracker().taskEnded();

							#pragma omp barrier

							#pragma omp single
							{
								for (std::size_t i = 0; i < workers.size(); i++)
								{
									workers[id].getWorkerTracker().taskAddedStart();
									workers[id].getWorkerTracker().
										tasksAdded(workers[i].getD().size());

									dispatcher.swapD(i, workers[i].getD());
								}
							}
						#pragma omp barrier
						}

						workers[id].getWorkerTracker().workEnded();
					}
				}

			public:

				inline const std::vector<m_Worker*>& getWorkers() const noexcept
				{
					return workers;
				}

				inline const m_Dispatcher& getDispatcher() const noexcept
				{
					return dispatcher;
				}

				inline const m_ResolverTracker& getResolverTracker() const noexcept
				{
					return resolverTracker;
				}

				void startExporte(LRDF::Tracker::Exporter& exporter)
				{
					exporter.open();

					exporte(exporter.getNonExistingFile("resolver.json"));

					exporter.saveClear();

					for (const auto& worker : workers)
					{
						std::string fileName("worker_" + std::to_string(worker->getId()) + ".json");

						worker->exporte(exporter.getNonExistingFile(fileName));

						exporter.saveClear();
					}

					dispatcher.exporte(exporter.getNonExistingFile("dispatcher_0.json"));

					exporter.saveClear();

					dispatcher.getMemoryTracker().exporte(exporter.getNonExistingFile("dispatcher_memory.json"));

					exporter.close();
				}
				void startExporte(LRDF::Tracker::RapidExporter& exporter)
				{
					exporter.open();

					exporte(exporter.getNonExistingFile("resolver.json"));

					//exporter.saveClear();

					for (const auto& worker : workers)
					{
						std::string fileName("worker_" + std::to_string(worker.getId()) + ".json");

						worker.exporte(exporter.getNonExistingFile(fileName));

						//exporter.saveClear();
					}

					dispatcher.exporte(exporter.getNonExistingFile("dispatcher_0.json"));

					//exporter.saveClear();

					dispatcher.getMemoryTracker().exporte(exporter.getNonExistingFile("dispatcher_memory.json"));

					exporter.close();
				}

				inline void exporte(nlohmann::json& output) const noexcept
				{
					resolverTracker.exporte(output);
					if (!output.empty())
					{
						output["nbWorkers"] = workers.size();
						output["nbDispatchers"] = 1;
						output["C"] = C;
						output["F"] = F;
						output["metaName"] = "";
					}
				}
				inline void exporte(rapidjson::Document& output) const noexcept
				{
					resolverTracker.exporte(output);

					if (!output.IsNull() && !output.ObjectEmpty())
					{
						rapidjson::Document::AllocatorType& allocator = output.GetAllocator();

						output.AddMember("nbWorkers", workers.size(), allocator);
						output.AddMember("nbDispatchers", 1, allocator);
						output.AddMember("C", C, allocator);
						output.AddMember("F", F, allocator);
						output.AddMember("metaName", "", allocator);
					}
				}
			};
		}
	}
}
