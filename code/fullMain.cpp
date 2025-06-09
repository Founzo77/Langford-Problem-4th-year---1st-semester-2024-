
#include <vector>
#include <iostream>

#include <Helper.hpp>

#include <Tracker/StandardExporter.hpp>
#include <Tracker/RapidStandardExporter.hpp>

constexpr uint8_t C = 2;

constexpr uint8_t F = 13; // A MODIFIER

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Erreur : Veuillez sp�cifier le dossier de sortie." << std::endl;
		std::cerr << "Usage : " << argv[0] << " <dossier_de_sortie>" << std::endl;
		return 1;
	}

	std::string outputDirectory = argv[1];

	// A MODIFIER

	/* Mono Worker */
	//using Resolver = LRDF::Aruka::Mono::u_StandardTrackerResolver<C, F>;
	
	/* Multi Workers OpenMP */
	//using Resolver = LRDF::Aruka::OpenMP::u_ParallelTrackerResolver<C, F>;
	using Resolver = LRDF::Aruka::OpenMP::u_SynchronizedTrackerResolver<C, F>;
	//using Resolver = LRDF::Aruka::OpenMP::u_SynchronizedNoDispatcherTrackerResolver<C, F>; // Version alternative car plus rapide pour la sauvegarde, mais perd des infos sur le dispatcher
	//using Resolver = LRDF::Aruka::OpenMP::u_HybridStaticTrackerResolver<C, F>;

	// FIN A MODIFIER

	Resolver resolver;

	std::vector<Resolver::m_Solution> solutions;

	std::cout << "Start Resolve for : L(" << std::to_string(C)
		<< ", " << std::to_string(F) << ")" << std::endl;

	resolver.resolve(solutions);

	std::cout << "Problem resolved" << std::endl;

	std::cout << solutions.size() << " solutions found" << std::endl;

	std::cout << "Starte Export" << std::endl;

	//LRDF::Tracker::StandardExporter s(outputDirectory);
	LRDF::Tracker::RapidStandardExporter s(outputDirectory);
	resolver.startExporte(s);

	std::cout << "End" << std::endl;

	return 0;
}