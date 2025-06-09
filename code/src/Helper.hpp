#pragma once

#include <stdint.h>

#include <LangFord.hpp>
#include <Solution.hpp>

#include <Tracker/NoMemoryTracker.hpp>

#include <Aruka/Task.hpp>
#include <Aruka/TrackerAllocator.hpp>

#include <Aruka/Tracker/NoWorkerTracker.hpp>
#include <Aruka/Tracker/NoResolverTracker.hpp>
#include <Aruka/Tracker/NoDispatcherTracker.hpp>

#include <Aruka/Mono/StandardResolver.hpp>
#include <Aruka/Mono/StandardWorker.hpp>
#include <Aruka/Mono/StandardQueueDispatcher.hpp>

#include <Aruka/Mono/Tracker/StandardDispatcherTracker.hpp>
#include <Aruka/Mono/Tracker/StandardResolverTracker.hpp>
#include <Aruka/Mono/Tracker/StandardWorkerTracker.hpp>

#include <Aruka/OpenMP/ConcurrentParallelQueueDispatcher.hpp>
#include <Aruka/OpenMP/ParallelResolver.hpp>
#include <Aruka/OpenMP/ParallelWorker.hpp>

#include <Aruka/OpenMP/MonoQueueDispatcher.hpp>
#include <Aruka/OpenMP/SynchronizedResolver.hpp>
#include <Aruka/OpenMP/SynchronizedWorker.hpp>

#include <Aruka/OpenMP/HybridStaticDispatcher.hpp>
#include <Aruka/OpenMP/HybridStaticResolver.hpp>
#include <Aruka/OpenMP/HybridStaticWorker.hpp>

#include <Aruka/OpenMP/Tracker/StandardDispatcherTracker.hpp>
#include <Aruka/OpenMP/Tracker/StandardResolverTracker.hpp>
#include <Aruka/OpenMP/Tracker/StandardWorkerTracker.hpp>

#include <Aruka/MPI/HybridStaticResolver.hpp>
#include <Aruka/MPI/HybridStaticWorker.hpp>

namespace LRDF
{
	template <uint8_t C, uint8_t F>
	using u_Solution = LRDF::Solution<C, F>;

	namespace Aruka
	{
		template <uint8_t C, uint8_t F>
		using u_Task = LRDF::Aruka::Task<C, F>;

		template <typename IArukaTask, typename IMemoryTracker>
		using u_Allocator = LRDF::Aruka::WrapperAllocator <IArukaTask, std::allocator<IArukaTask>, IMemoryTracker>;

		template <uint8_t C, uint8_t F,
			typename IMemoryTracker = LRDF::Tracker::NoMemoryTracker>
		using u_DynamicNoTrackerAllocator = LRDF::Aruka::u_Allocator<
			LRDF::Aruka::u_Task<C, F>,
			IMemoryTracker
		>;

		namespace Mono
		{
			template <uint8_t C, uint8_t F, 
				typename IAllocator = LRDF::Aruka::u_DynamicNoTrackerAllocator<C, F>,
				typename IDispatcherTracker = LRDF::Aruka::Tracker::NoDispatcherTracker>
			using u_DynamicQueueNoTrackerDispatcher = 
				LRDF::Aruka::Mono::StandardQueueDispatcher<
					C, F, 
					IAllocator, 
					IDispatcherTracker
				>;

			template <uint8_t C, uint8_t F,
				typename IAllocator = LRDF::Aruka::u_DynamicNoTrackerAllocator<C, F>,
				typename IDispatcherTracker = LRDF::Aruka::Mono::Tracker::StandardDispatcherTracker>
			using u_DynamicQueueTrackerDispatcher =
				LRDF::Aruka::Mono::StandardQueueDispatcher<
					C, F,
					IAllocator,
					IDispatcherTracker
				>;

			template <uint8_t C, uint8_t F, typename IDispatcher, 
				typename IWorkerTracker = LRDF::Aruka::Tracker::NoWorkerTracker>
			using u_NoTrackerStandardWorker = LRDF::Aruka::Mono::StandardWorker<
				C, F, 
				IDispatcher, 
				IWorkerTracker
			>;

			template <uint8_t C, uint8_t F, typename IDispatcher,
				typename IWorkerTracker = LRDF::Aruka::Mono::Tracker::StandardWorkerTracker>
			using u_TrackerStandardWorker = LRDF::Aruka::Mono::StandardWorker<
				C, F,
				IDispatcher,
				IWorkerTracker
			>;

			template <uint8_t C, uint8_t F, 
				typename IStandardWorker = LRDF::Aruka::Mono::u_NoTrackerStandardWorker<C, F, LRDF::Aruka::Mono::u_DynamicQueueNoTrackerDispatcher<C, F>>,
				typename IResolverTracker = LRDF::Aruka::Tracker::NoResolverTracker>
			using u_StandardNoTrackerResolver = LRDF::Aruka::Mono::StandardResolver<C, F,
				IStandardWorker,
				typename IStandardWorker::m_Dispatcher,
				IResolverTracker
			>;

			template <uint8_t C, uint8_t F,
				typename IStandardWorker = LRDF::Aruka::Mono::u_NoTrackerStandardWorker<C, F, LRDF::Aruka::Mono::u_DynamicQueueNoTrackerDispatcher<C, F>>,
				typename IResolverTracker = LRDF::Aruka::Mono::Tracker::StandardResolverTracker>
			using u_StandardLightTrackerResolver = LRDF::Aruka::Mono::StandardResolver<C, F,
				IStandardWorker,
				typename IStandardWorker::m_Dispatcher,
				IResolverTracker
			>;

			template <uint8_t C, uint8_t F,
				typename IStandardWorker = LRDF::Aruka::Mono::u_TrackerStandardWorker<C, F, LRDF::Aruka::Mono::u_DynamicQueueTrackerDispatcher<C, F>>,
				typename IResolverTracker = LRDF::Aruka::Mono::Tracker::StandardResolverTracker>
			using u_StandardTrackerResolver = LRDF::Aruka::Mono::StandardResolver<C, F,
				IStandardWorker,
				typename IStandardWorker::m_Dispatcher,
				IResolverTracker
			>;
		}

		namespace OpenMP
		{
			/* Dispatcher */

			// Parallel

			template <uint8_t C, uint8_t F,
				typename IAllocator = LRDF::Aruka::u_DynamicNoTrackerAllocator<C, F>,
				typename IDispatcherTracker = LRDF::Aruka::Tracker::NoDispatcherTracker>
			using u_ConcurrentParallelQueueNoTrackerDispatcher =
				LRDF::Aruka::OpenMP::ConcurrentParallelQueueDispatcher<
				C, F,
				IAllocator,
				IDispatcherTracker
			>;

			template <uint8_t C, uint8_t F,
				typename IAllocator = LRDF::Aruka::u_DynamicNoTrackerAllocator<C, F>,
				typename IDispatcherTracker = LRDF::Aruka::OpenMP::Tracker::StandardDispatcherTracker>
			using u_ConcurrentParallelQueueTrackerDispatcher =
				LRDF::Aruka::OpenMP::ConcurrentParallelQueueDispatcher<
				C, F,
				IAllocator,
				IDispatcherTracker
			>;

			// Synchronized

			template <uint8_t C, uint8_t F,
				typename IAllocator = LRDF::Aruka::u_DynamicNoTrackerAllocator<C, F>,
				typename IDispatcherTracker = LRDF::Aruka::Tracker::NoDispatcherTracker>
			using u_MonoQueueNoTrackerDispatcher =
				LRDF::Aruka::OpenMP::MonoQueueDispatcher<
				C, F,
				IAllocator,
				IDispatcherTracker
			>;

			template <uint8_t C, uint8_t F,
				typename IAllocator = LRDF::Aruka::u_DynamicNoTrackerAllocator<C, F>,
				typename IDispatcherTracker = LRDF::Aruka::OpenMP::Tracker::StandardDispatcherTracker>
			using u_MonoTrackerDispatcher =
				LRDF::Aruka::OpenMP::MonoQueueDispatcher<
				C, F,
				IAllocator,
				IDispatcherTracker
			>;

			// Hybrid Static

			template <uint8_t C, uint8_t F,
				typename IAllocator = LRDF::Aruka::u_DynamicNoTrackerAllocator<C, F>,
				typename IDispatcherTracker = LRDF::Aruka::Tracker::NoDispatcherTracker>
			using u_HybridStaticNoTrackerDispatcher =
				LRDF::Aruka::OpenMP::HybridStaticDispatcher<
				C, F,
				IAllocator,
				IDispatcherTracker
				>;

			template <uint8_t C, uint8_t F,
				typename IAllocator = LRDF::Aruka::u_DynamicNoTrackerAllocator<C, F>,
				typename IDispatcherTracker = LRDF::Aruka::OpenMP::Tracker::StandardDispatcherTracker>
			using u_HybridStaticTrackerDispatcher =
				LRDF::Aruka::OpenMP::HybridStaticDispatcher<
				C, F,
				IAllocator,
				IDispatcherTracker
				>;

			/* Worker */

			// Parallel

			template <uint8_t C, uint8_t F, typename IDispatcher,
				typename IWorkerTracker = LRDF::Aruka::Tracker::NoWorkerTracker>
			using u_NoTrackerParallelWorker = LRDF::Aruka::OpenMP::ParallelWorker<
				C, F,
				IDispatcher,
				IWorkerTracker
			>;

			template <uint8_t C, uint8_t F, typename IDispatcher,
				typename IWorkerTracker = LRDF::Aruka::OpenMP::Tracker::StandardWorkerTracker>
			using u_TrackerParallelWorker = LRDF::Aruka::OpenMP::ParallelWorker<
				C, F,
				IDispatcher,
				IWorkerTracker
			>;

			// Synchronized

			template <uint8_t C, uint8_t F, typename IDispatcher,
				typename IWorkerTracker = LRDF::Aruka::Tracker::NoWorkerTracker>
			using u_NoTrackerSynchronizedWorker = LRDF::Aruka::OpenMP::SynchronizedWorker<
				C, F,
				IDispatcher,
				IWorkerTracker
			>;

			template <uint8_t C, uint8_t F, typename IDispatcher,
				typename IWorkerTracker = LRDF::Aruka::OpenMP::Tracker::StandardWorkerTracker>
			using u_TrackerSynchronizedWorker = LRDF::Aruka::OpenMP::SynchronizedWorker<
				C, F,
				IDispatcher,
				IWorkerTracker
			>;

			// Hybrid Static

			template <uint8_t C, uint8_t F, typename IDispatcher,
				typename IWorkerTracker = LRDF::Aruka::Tracker::NoWorkerTracker>
			using u_NoTrackerHybridStaticWorker = LRDF::Aruka::OpenMP::HybridStaticWorker<
				C, F,
				IDispatcher,
				IWorkerTracker
			>;

			template <uint8_t C, uint8_t F, typename IDispatcher,
				typename IWorkerTracker = LRDF::Aruka::OpenMP::Tracker::StandardWorkerTracker>
			using u_TrackerHybridStaticWorker = LRDF::Aruka::OpenMP::HybridStaticWorker<
				C, F,
				IDispatcher,
				IWorkerTracker
			>;

			/* Resolver */

			// Parallel

			template <uint8_t C, uint8_t F,
				typename IStandardWorker = LRDF::Aruka::OpenMP::u_NoTrackerParallelWorker<
					C, F, 
					LRDF::Aruka::OpenMP::u_ConcurrentParallelQueueNoTrackerDispatcher<C, F>
				>,
				typename IResolverTracker = LRDF::Aruka::Tracker::NoResolverTracker>
			using u_ParallelNoTrackerResolver = LRDF::Aruka::OpenMP::ParallelResolver<C, F,
				IStandardWorker,
				typename IStandardWorker::m_Dispatcher,
				IResolverTracker
			>;

			template <uint8_t C, uint8_t F,
				typename IStandardWorker = LRDF::Aruka::OpenMP::u_NoTrackerParallelWorker<
				C, F,
				LRDF::Aruka::OpenMP::u_ConcurrentParallelQueueNoTrackerDispatcher<C, F>
				>,
				typename IResolverTracker = LRDF::Aruka::OpenMP::Tracker::StandardResolverTracker>
			using u_ParallelLightTrackerResolver = LRDF::Aruka::OpenMP::ParallelResolver<C, F,
				IStandardWorker,
				typename IStandardWorker::m_Dispatcher,
				IResolverTracker
			>;

			template <uint8_t C, uint8_t F,
				typename IStandardWorker = LRDF::Aruka::OpenMP::u_TrackerParallelWorker<
					C, F, 
					LRDF::Aruka::OpenMP::u_ConcurrentParallelQueueTrackerDispatcher<C, F>
				>,
				typename IResolverTracker = LRDF::Aruka::OpenMP::Tracker::StandardResolverTracker>
			using u_ParallelTrackerResolver = LRDF::Aruka::OpenMP::ParallelResolver<C, F,
				IStandardWorker,
				typename IStandardWorker::m_Dispatcher,
				IResolverTracker
			>;

			// Synchronized

			template <uint8_t C, uint8_t F,
				typename ISynchronizedWorker = LRDF::Aruka::OpenMP::u_NoTrackerSynchronizedWorker<
				C, F,
				LRDF::Aruka::OpenMP::u_MonoQueueNoTrackerDispatcher<C, F>
				>,
				typename IResolverTracker = LRDF::Aruka::Tracker::NoResolverTracker>
			using u_SynchronizedNoTrackerResolver = LRDF::Aruka::OpenMP::SynchronizedResolver<C, F,
				ISynchronizedWorker,
				typename ISynchronizedWorker::m_Dispatcher,
				IResolverTracker
			>;

			template <uint8_t C, uint8_t F,
				typename ISynchronizedWorker = LRDF::Aruka::OpenMP::u_NoTrackerSynchronizedWorker<
				C, F,
				LRDF::Aruka::OpenMP::u_MonoQueueNoTrackerDispatcher<C, F>
				>,
				typename IResolverTracker = LRDF::Aruka::OpenMP::Tracker::StandardResolverTracker>
			using u_SynchronizedLightTrackerResolver = LRDF::Aruka::OpenMP::SynchronizedResolver<C, F,
				ISynchronizedWorker,
				typename ISynchronizedWorker::m_Dispatcher,
				IResolverTracker
			>;

			template <uint8_t C, uint8_t F,
				typename ISynchronizedWorker = LRDF::Aruka::OpenMP::u_TrackerSynchronizedWorker<
				C, F,
				LRDF::Aruka::OpenMP::u_MonoQueueNoTrackerDispatcher<C, F>
				>,
				typename IResolverTracker = LRDF::Aruka::OpenMP::Tracker::StandardResolverTracker>
			using u_SynchronizedNoDispatcherTrackerResolver = LRDF::Aruka::OpenMP::SynchronizedResolver<C, F,
				ISynchronizedWorker,
				typename ISynchronizedWorker::m_Dispatcher,
				IResolverTracker
			>;

			template <uint8_t C, uint8_t F,
				typename ISynchronizedWorker = LRDF::Aruka::OpenMP::u_TrackerSynchronizedWorker<
				C, F,
				LRDF::Aruka::OpenMP::u_MonoTrackerDispatcher<C, F>
				>,
				typename IResolverTracker = LRDF::Aruka::OpenMP::Tracker::StandardResolverTracker>
			using u_SynchronizedTrackerResolver = LRDF::Aruka::OpenMP::SynchronizedResolver<C, F,
				ISynchronizedWorker,
				typename ISynchronizedWorker::m_Dispatcher,
				IResolverTracker
			>;

			// Hybrid Static

			template <uint8_t C, uint8_t F,
				typename IHybridStaticWorker = LRDF::Aruka::OpenMP::u_NoTrackerHybridStaticWorker<
				C, F,
				LRDF::Aruka::OpenMP::u_HybridStaticNoTrackerDispatcher<C, F>
				>,
				typename IResolverTracker = LRDF::Aruka::Tracker::NoResolverTracker>
			using u_HybridStaticNoTrackerResolver = LRDF::Aruka::OpenMP::HybridStaticResolver<C, F,
				IHybridStaticWorker,
				typename IHybridStaticWorker::m_Dispatcher,
				IResolverTracker
			>;

			template <uint8_t C, uint8_t F,
				typename IHybridStaticWorker = LRDF::Aruka::OpenMP::u_NoTrackerHybridStaticWorker<
				C, F,
				LRDF::Aruka::OpenMP::u_HybridStaticNoTrackerDispatcher<C, F>
				>,
				typename IResolverTracker = LRDF::Aruka::OpenMP::Tracker::StandardResolverTracker>
			using u_HybridStaticLightTrackerResolver = LRDF::Aruka::OpenMP::HybridStaticResolver<C, F,
				IHybridStaticWorker,
				typename IHybridStaticWorker::m_Dispatcher,
				IResolverTracker
			>;

			template <uint8_t C, uint8_t F,
				typename IHybridStaticWorker = LRDF::Aruka::OpenMP::u_TrackerHybridStaticWorker<
				C, F,
				LRDF::Aruka::OpenMP::u_HybridStaticTrackerDispatcher<C, F>
				>,
				typename IResolverTracker = LRDF::Aruka::OpenMP::Tracker::StandardResolverTracker>
			using u_HybridStaticTrackerResolver = LRDF::Aruka::OpenMP::HybridStaticResolver<C, F,
				IHybridStaticWorker,
				typename IHybridStaticWorker::m_Dispatcher,
				IResolverTracker
			>;
		}

		namespace MPI
		{
			/* Worker */

			// Hybrid Static

			template <uint8_t C, uint8_t F,
				typename IWorkerTracker = LRDF::Aruka::Tracker::NoWorkerTracker>
			using u_NoTrackerHybridStaticWorker = LRDF::Aruka::MPI::HybridStaticWorker<
				C, F,
				IWorkerTracker
			>;

			template <uint8_t C, uint8_t F, 
				typename IWorkerTracker = LRDF::Aruka::OpenMP::Tracker::StandardWorkerTracker>
			using u_TrackerHybridStaticWorker = LRDF::Aruka::MPI::HybridStaticWorker<
				C, F,
				IWorkerTracker
			>;

			/* Resolver */

			// Hybrid Static
			template <uint8_t C, uint8_t F,
				typename IHybridStaticWorker = LRDF::Aruka::MPI::u_NoTrackerHybridStaticWorker<
				C, F
				>,
				typename IResolverTracker = LRDF::Aruka::Tracker::NoResolverTracker>
			using u_HybridStaticNoTrackerResolver = LRDF::Aruka::MPI::HybridStaticResolver<C, F,
				IHybridStaticWorker,
				IResolverTracker
			>;

			template <uint8_t C, uint8_t F,
				typename IHybridStaticWorker = LRDF::Aruka::MPI::u_NoTrackerHybridStaticWorker<
				C, F
				>,
				typename IResolverTracker = LRDF::Aruka::OpenMP::Tracker::StandardResolverTracker>
			using u_HybridStaticLightTrackerResolver = LRDF::Aruka::MPI::HybridStaticResolver<C, F,
				IHybridStaticWorker,
				IResolverTracker
			>;

			template <uint8_t C, uint8_t F,
				typename IHybridStaticWorker = LRDF::Aruka::MPI::u_TrackerHybridStaticWorker<
				C, F
				>,
				typename IResolverTracker = LRDF::Aruka::OpenMP::Tracker::StandardResolverTracker>
			using u_HybridStaticTrackerResolver = LRDF::Aruka::MPI::HybridStaticResolver<C, F,
				IHybridStaticWorker,
				IResolverTracker
			>;
		}
	}
}