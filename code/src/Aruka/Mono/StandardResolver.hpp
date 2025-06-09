#pragma once

#include <stdint.h>
#include <cstddef>
#include <vector>
#include <nlohmann/json.hpp>

#include <Solution.hpp>

#include <Langford.hpp>

#include <Aruka/Task.hpp>
#include <Tracker/Exporter.hpp>
#include <Tracker/RapidExporter.hpp>

#include <rapidjson/document.h>

namespace LRDF
{
	namespace Aruka
	{
		namespace Mono
		{
			template <uint8_t C, uint8_t F,
				typename IStandardWorker, 
				typename IStandardDispatcher,
				typename IResolverTracker>
			class StandardResolver
			{
			public:
				using m_StandardWorker = IStandardWorker;
				using m_StandardDispatcher = IStandardDispatcher;
				using m_ResolverTracker = IResolverTracker;
				using m_Task = LRDF::Aruka::Task<C, F>;
				using m_Langford = LRDF::Langford<C, F>;
				using m_Solution = LRDF::Solution<C, F>;

			private:
				m_StandardWorker* worker;
				m_StandardDispatcher dispatcher;
				m_ResolverTracker resolverTracker;

			public:
				StandardResolver() : worker(nullptr), dispatcher(), resolverTracker()
				{

				}

				~StandardResolver()
				{
					if (worker)
					{
						delete worker;
						worker = nullptr;
					}
				}

				void resolve(std::vector<m_Solution>& solutions)
				{
					resolverTracker.initializationStarted();

					m_Task rootTask(m_Langford(), F, 1);
					dispatcher.addTask(rootTask);

					worker = new m_StandardWorker(dispatcher);

					resolverTracker.initializationEnded();

					resolverTracker.workStarted();

					worker->work();

					resolverTracker.workEnded();

					resolverTracker.mergeStarted();

					solutions = worker->getSolutions();

					resolverTracker.mergeEnded();
				}

				inline const m_StandardWorker& getWorker() const noexcept
				{
					return *worker;
				}

				inline const m_StandardDispatcher& getDispatcher() const noexcept
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

					worker->exporte(
						exporter.getNonExistingFile("worker_0.json"));

					dispatcher.exporte(exporter.getNonExistingFile("dispatcher_0.json"));

					dispatcher.getMemoryTracker().exporte(exporter.getNonExistingFile("dispatcher_memory.json"));

					exporter.close();
				}
				void startExporte(LRDF::Tracker::RapidExporter& exporter)
				{
					exporter.open();

					exporte(exporter.getNonExistingFile("resolver.json"));

					exporter.saveClear();

					worker->exporte(
						exporter.getNonExistingFile("worker_0.json"));

					exporter.saveClear();

					dispatcher.exporte(exporter.getNonExistingFile("dispatcher_0.json"));

					dispatcher.getMemoryTracker().exporte(exporter.getNonExistingFile("dispatcher_memory.json"));

					exporter.close();
				}

				inline void exporte(nlohmann::json& output) const noexcept
				{
					resolverTracker.exporte(output);
					if (!output.empty())
					{
						output["nbWorkers"] = 1;
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

						output.AddMember("nbWorkers", 1, allocator);
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