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
				typename ISynchronizedWorker,
				typename IMonoDispatcher,
				typename ISynchronizedResolverTracker>
			class SynchronizedResolver
			{
			public:
				using m_Worker = ISynchronizedWorker;
				using m_Dispatcher = IMonoDispatcher;
				using m_ResolverTracker = ISynchronizedResolverTracker;
				using m_Task = LRDF::Aruka::Task<C, F>;
				using m_Langford = LRDF::Langford<C, F>;
				using m_Solution = LRDF::Solution<C, F>;

			private:
				std::vector<m_Worker*> workers;
				m_ResolverTracker resolverTracker;
				m_Dispatcher dispatcher;
				std::size_t masterId;

			public:
				SynchronizedResolver() : 
					workers(), resolverTracker(), dispatcher(
						99999, 0), masterId(99999)
				{

				}

				~SynchronizedResolver()
				{
					for (auto& pWorker : workers)
					{
						delete pWorker;
					}
				}

				void resolve(std::vector<m_Solution>& solutions)
				{
					resolverTracker.initializationStarted();

					m_Task rootTask(m_Langford(), F, 1);
					this->dispatcher.addTask(0, rootTask);

					m_Dispatcher& dispatcher = this->dispatcher;
					std::vector<m_Worker*>& workers = this->workers;
					m_ResolverTracker& resolverTracker = this->resolverTracker;

					bool isFinished = false;
					std::size_t masterId = this->masterId;

					#pragma omp parallel shared(dispatcher, workers, solutions, resolverTracker, isFinished, masterId)
					{
						// Initialization
						std::size_t id = (std::size_t)omp_get_thread_num();
						m_Worker* worker = new m_Worker(id);

						#pragma omp barrier

						#pragma omp critical
						{
							workers.push_back(worker);
						}

						#pragma omp single
						{
							resolverTracker.initializationEnded();
							resolverTracker.workStarted();
						}

						// Work

						while (!isFinished)
						{
							#pragma omp master
							{

								std::size_t nbTasksBlocks =
									dispatcher.getNbTasks() / workers.size();

								std::size_t remainingTasks = 
									dispatcher.getNbTasks() % workers.size();

								for (std::size_t i = 0; i < workers.size(); i++)
								{
									std::size_t nbTasksForThisWorker = nbTasksBlocks;

									if (remainingTasks > 0)
									{
										remainingTasks--;
										nbTasksForThisWorker++;
									}

									dispatcher.getTasks(
										id, nbTasksForThisWorker, workers[i]->getT());
								}
								int k = 0;
							}

							#pragma omp barrier

							worker->work();

							#pragma omp barrier

							#pragma omp master
							{
								for (m_Worker* worker : workers)
								{
									worker->getD().getAllTasks(id, dispatcher);
								}

								isFinished = dispatcher.getNbTasks() == 0;
							}

							#pragma omp barrier
						}

						worker->finish();
						
						// Merge
						#pragma omp barrier

						#pragma omp single
						{
							resolverTracker.workEnded();
							resolverTracker.mergeStarted();
						}

						#pragma omp critical
						{
							solutions.insert(solutions.end(),
								worker->getSolutions().cbegin(), worker->getSolutions().cend());
						}
					}

					resolverTracker.mergeEnded();
				}

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
						std::string fileName("worker_" + std::to_string(worker->getId()) + ".json");

						worker->exporte(exporter.getNonExistingFile(fileName));

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
						output["nbDispatchers"] = workers.size() + 1;
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
						output.AddMember("nbDispatchers", workers.size() + 1, allocator);
						output.AddMember("C", C, allocator);
						output.AddMember("F", F, allocator);
						output.AddMember("metaName", "", allocator);
					}
				}
			};
		}
	}
}
