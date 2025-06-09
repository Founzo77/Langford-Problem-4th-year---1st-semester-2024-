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
				typename IParallelWorker,
				typename IParallelDispatcher,
				typename IParallelResolverTracker>
			class ParallelResolver
			{
			public:
				using m_ParallelWorker = IParallelWorker;
				using m_ParallelDispatcher = IParallelDispatcher;
				using m_ResolverTracker = IParallelResolverTracker;
				using m_Task = LRDF::Aruka::Task<C, F>;
				using m_Langford = LRDF::Langford<C, F>;
				using m_Solution = LRDF::Solution<C, F>;

			private:
				std::vector<m_ParallelWorker*> workers;
				m_ResolverTracker resolverTracker;
				m_ParallelDispatcher dispatcher;

			public:
				ParallelResolver() : workers(), resolverTracker(), dispatcher()
				{

				}

				~ParallelResolver()
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
					dispatcher.addTask(0, rootTask);
					
					m_ParallelDispatcher& dispatcher = this->dispatcher;
					std::vector<m_ParallelWorker*>& workers = this->workers;
					m_ResolverTracker& resolverTracker = this->resolverTracker;

					#pragma omp parallel shared(dispatcher, workers, solutions, resolverTracker)
					{
						m_ParallelWorker* worker = new m_ParallelWorker((std::size_t)omp_get_thread_num(), dispatcher);

						dispatcher.registerWorker(worker->getId());

						#pragma omp barrier

						#pragma omp single
						{
							resolverTracker.initializationEnded();
							resolverTracker.workStarted();
						}

						worker->work();

						#pragma omp barrier

						#pragma omp single
						{
							resolverTracker.workEnded();
							resolverTracker.mergeStarted();
						}

						#pragma omp critical
						{
							workers.push_back(worker);

							solutions.insert(solutions.end(),
								worker->getSolutions().cbegin(), worker->getSolutions().cend());
						}
					}

					resolverTracker.mergeEnded();
				}

				inline const std::vector<m_ParallelWorker*>& getWorkers() const noexcept
				{
					return workers;
				}

				inline const m_ParallelDispatcher& getDispatcher() const noexcept
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
