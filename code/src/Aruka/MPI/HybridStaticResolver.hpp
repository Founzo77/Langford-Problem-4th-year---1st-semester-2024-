#pragma once

#include <stdint.h>
#include <cstddef>
#include <vector>
#include <string>
#include <utility>

#include <Solution.hpp>

#include <Langford.hpp>

#include <Aruka/Task.hpp>
#include <Tracker/Exporter.hpp>
#include <Tracker/RapidExporter.hpp>

#include <rapidjson/document.h>

#include <mpi.h>

namespace LRDF
{
	namespace Aruka
	{
		namespace MPI
		{
			template <uint8_t C, uint8_t F,
				typename IHybridStaticWorker,
				typename IHybridStaticResolverTracker,
				std::size_t NB_BYTES = static_cast<std::size_t>(5e10),
				std::size_t ROOT_DEPTH = 4>
			class HybridStaticResolver
			{
			public:
				using m_Worker = IHybridStaticWorker;
				using m_ResolverTracker = IHybridStaticResolverTracker;
				using m_Task = LRDF::Aruka::Task<C, F>;
				using m_Langford = LRDF::Langford<C, F>;
				using m_Solution = LRDF::Solution<C, F>;

			private:
				m_Worker worker;
				m_ResolverTracker resolverTracker;
				int nbProcs, id;

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
					worker(0), resolverTracker(), nbProcs(0), id(0)
				{
					MPI_Init(NULL, NULL);
					MPI_Comm_size(MPI_COMM_WORLD, &nbProcs);
					MPI_Comm_rank(MPI_COMM_WORLD, &id);
				}
				
				~HybridStaticResolver()
				{
					MPI_Finalize();
				}

				void resolve(std::vector<m_Solution>& solutions)
				{
					if (id == 0)
					{
						resolverTracker.initializationStarted();
					}

					std::vector<m_Task> treeTasks;

					if (id == 0)
					{
						m_Task rootTask(m_Langford(), F, 1);
						std::vector<m_Task> globalTreeTasks;
						resolveRoot(rootTask, globalTreeTasks);

						std::cout << "nb trees: " << globalTreeTasks.size() << std::endl;

						std::size_t nbGlobalTasks = globalTreeTasks.size();
						int chunkSize = nbGlobalTasks / nbProcs;
						int remainder = nbGlobalTasks % nbProcs;
						std::size_t offset = 0;

						// remainder est partage equitablement entre les procs
						std::size_t nbTasks = chunkSize + (remainder > 0 ? 1 : 0);
						treeTasks.insert(treeTasks.end(), globalTreeTasks.begin(), globalTreeTasks.begin() + nbTasks);
						offset += nbTasks;

						for (int i = 1; i < nbProcs; i++)
						{
							std::size_t nbtoSend = chunkSize + (i < remainder ? 1 : 0);

							MPI_Send(&nbtoSend, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);
							MPI_Send(globalTreeTasks.data() + offset, nbtoSend * sizeof(m_Task),
								MPI_BYTE, i, 0, MPI_COMM_WORLD);

							offset += nbtoSend;
						}
					}
					else
					{
						std::size_t nbTasks;

						MPI_Recv(&nbTasks, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

						treeTasks.resize(nbTasks);

						int chunkSize = nbTasks / nbProcs;

						MPI_Recv(treeTasks.data(), nbTasks * sizeof(m_Task), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					}

					if (id == 0)
					{
						resolverTracker.initializationEnded();
						resolverTracker.workStarted();
					}

					for (auto& treeTask : treeTasks)
					{
						worker.resolveTree(treeTask);
					}

					if (id != 0)
					{
						const std::vector<m_Solution>& localSolutions = 
							worker.getSolutions();
						std::size_t nbSolutions = localSolutions.size();

						MPI_Send(&nbSolutions, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD);
						MPI_Send(localSolutions.data(), nbSolutions * sizeof(m_Solution), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
					}

					if (id == 0)
					{
						resolverTracker.workEnded();
						resolverTracker.mergeStarted();

						const std::vector<m_Solution>& localSolutions = 
							worker.getSolutions();
						solutions.insert(solutions.end(), localSolutions.begin(), localSolutions.end());

						for (int i = 1; i < nbProcs; i++)
						{
							std::size_t nbSolutions;
							MPI_Recv(&nbSolutions, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

							std::vector<m_Solution> localSolutions(nbSolutions);
							MPI_Recv(localSolutions.data(), nbSolutions * sizeof(m_Solution), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

							solutions.insert(solutions.end(), localSolutions.begin(), localSolutions.end());
						}

						resolverTracker.mergeEnded();
					}
				}

			private:
				void resolveRoot(m_Task& task, std::vector<m_Task>& treeTasks)
				{
					uint8_t currentFrequency = task.getCurrentFrequency();

					if (currentFrequency == F - ROOT_DEPTH)
					{
						treeTasks.push_back(task);
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

								resolveRoot(newTask, treeTasks);
							}
						}
					}
				}

			public:

				/*
				inline const m_Dispatcher& getDispatcher() const noexcept
				{
					return dispatcher;
				}
				*/

				inline const m_ResolverTracker& getResolverTracker() const noexcept
				{
					return resolverTracker;
				}

				void startExporte(LRDF::Tracker::Exporter& exporter)
				{
					if (id != 0)
						return;

					exporter.open();

					exporte(exporter.getNonExistingFile("resolver.json"));

					exporter.saveClear();

					/*

					for (const auto& worker : workers)
					{
						std::string fileName("worker_" + std::to_string(worker->getId()) + ".json");

						worker->exporte(exporter.getNonExistingFile(fileName));

						exporter.saveClear();
					}

					exporter.saveClear();
					*/

					exporter.close();
				}
				void startExporte(LRDF::Tracker::RapidExporter& exporter)
				{
					if (id != 0)
						return;

					exporter.open();

					exporte(exporter.getNonExistingFile("resolver.json"));
					exporter.saveClear();

					/*
					for (const auto& worker : workers)
					{
						std::string fileName("worker_" + std::to_string(worker.getId()) + ".json");

						worker.exporte(exporter.getNonExistingFile(fileName));

						//exporter.saveClear();
					}
					*/

					exporter.close();
				}

				inline void exporte(nlohmann::json& output) const noexcept
				{
					resolverTracker.exporte(output);
					if (!output.empty())
					{
						output["nbWorkers"] = nbProcs;
						output["nbDispatchers"] = 0;
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

						output.AddMember("nbWorkers", nbProcs, allocator);
						output.AddMember("nbDispatchers", 0, allocator);
						output.AddMember("C", C, allocator);
						output.AddMember("F", F, allocator);
						output.AddMember("metaName", "", allocator);
					}
				}
			};
		}
	}
}

