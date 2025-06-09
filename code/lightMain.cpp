
#include <vector>
#include <iostream>
#include <mpi.h>

#include <Helper.hpp>

#include <Tracker/RapidStandardExporter.hpp>

constexpr uint8_t C = 2;

constexpr uint8_t F = 13; // A MODIFIER

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Erreur : Veuillez specifier le dossier de sortie." << std::endl;
		std::cerr << "Usage : " << argv[0] << " <dossier_de_sortie>" << std::endl;

		return 1;
	}
	
	std::string outputDirectory = argv[1];

	// A MODIFIER

	/* Mono Worker */
	//using Resolver = LRDF::Aruka::Mono::u_StandardLightTrackerResolver<C, F>;

	/* Multi Workers OpenMP */
	//using Resolver = LRDF::Aruka::OpenMP::u_ParallelLightTrackerResolver<C, F>;
	//using Resolver = LRDF::Aruka::OpenMP::u_SynchronizedLightTrackerResolver<C, F>;
	using Resolver = LRDF::Aruka::OpenMP::u_HybridStaticLightTrackerResolver<C, F>;
	
	/* Multi Workers MPI */
	//using Resolver = LRDF::Aruka::MPI::u_HybridStaticLightTrackerResolver<C, F>;

	// FIN A MODIFIER

	Resolver resolver;

	std::vector<Resolver::m_Solution> solutions;

	int isMPI = 0, idMPI = 0;
	MPI_Initialized(&isMPI);
	if (isMPI)
		MPI_Comm_rank(MPI_COMM_WORLD, &idMPI);
	int isMain = (isMPI == 0 || idMPI == 0);
	
	if (isMain)
	{
		std::cout << "Start Resolve for : L(" << std::to_string(C)
			<< ", " << std::to_string(F) << ")" << std::endl;
	}

	resolver.resolve(solutions);

	if (isMain)
	{
		std::cout << "Problem resolved" << std::endl;

		std::cout << solutions.size() << " solutions found" << std::endl;

		std::cout << "Starte Export" << std::endl;

		LRDF::Tracker::RapidStandardExporter s(outputDirectory);
		resolver.startExporte(s);

		std::cout << "End" << std::endl;
	}

	return 0;
}