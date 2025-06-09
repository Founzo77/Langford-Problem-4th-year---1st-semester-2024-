
#include <vector>
#include <iostream>

#include <Helper.hpp>

constexpr uint8_t C = 2;

constexpr uint8_t F = 15; // A MODIFIER

int main()
{
	// A MODIFIER

	/* Mono Worker */
	using Resolver = LRDF::Aruka::Mono::u_StandardNoTrackerResolver<C, F>;
	
	/* Multi Workers OpenMP */
	//using Resolver = LRDF::Aruka::OpenMP::u_ParallelNoTrackerResolver<C, F>;
	//using Resolver = LRDF::Aruka::OpenMP::u_SynchronizedNoTrackerResolver<C, F>;
	//using Resolver = LRDF::Aruka::OpenMP::u_HybridStaticNoTrackerResolver<C, F>;

	/* Multi Workers MPI */
	//using Resolver = LRDF::Aruka::MPI::u_HybridStaticNoTrackerResolver<C, F>;

	// FIN A MODIFIER

	Resolver resolver;

	std::vector<Resolver::m_Solution> solutions;

	resolver.resolve(solutions);

	return 0;
}