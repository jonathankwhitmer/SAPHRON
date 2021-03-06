// This example is a demonstration of Expanded Density of States sampling (EXEDOS)
// of a Lebwhol-Lasher binary mixture

// Include header files
#include "../src/Ensembles/EXEDOSEnsemble.h"
#include "../src/Loggers/CSVLogger.h"
#include "../src/Loggers/ConsoleLogger.h"
#include "../src/Models/LebwohlLasherModel.h"
#include "../src/Moves/SpeciesSwapMove.h"
#include "../src/Moves/SphereUnitVectorMove.h"

// Include for parsing using stringstream
#include <iostream>
#include <sstream>

// Function definition of our basic input parser.
// A very basic input parser.
int parse_input(char const* args[], int& latticeSize,
                double& temperature,
                int& iterations,
                double& minX,
                double& maxX,
                int& binCount,
                std::string& modelFile,
                std::string& SitesFile,
                std::string& vecsFile
                );

// The main program expects a user to input the lattice size, number of EXEDOS
// iterations, minimum and maximum mole fractions, number of bins for density-of-states
// histogram and model, sites and vector file outputs.
int main(int argc, char const* argv[])
{
	int latticeSize, iterations, binCount;
	double minX, maxX, temperature;
	std::string modelFile, sitesFile, vecsFile;

	if(argc != 10)
	{
		std::cerr << "Program syntax:" << argv[0] <<
		" lattice-size temperature iterations min-X max-X bin-count model-outputfile site-outputfile DOS-outputfile"
		          << std::endl;
		return 0;
	}

	if(parse_input(argv, latticeSize, temperature, iterations, minX, maxX, binCount, modelFile,
	               sitesFile,
	               vecsFile) != 0)
		return -1;

	// Initialize the Lebwohl-Lasher model.
	Models::LebwohlLasherModel model(latticeSize, latticeSize, latticeSize);

	// Initialize the moves we would like to perform on the model. This is the basic
	// "unit vector on sphere" move and a species swap move.
	Moves::SphereUnitVectorMove move1;
	Moves::SpeciesSwapMove move2(2);

	// Since all descendants of Lattice3D model by default initialize all spins up, we
	// need to randomize the initial configurations of the sites (spins).
	for(int i = 0; i < 3*model.GetSiteCount(); i++)
	{
		auto* site = model.DrawSample();
		move1.Perform(*site);
	}

	// Configure our mixture to initialize in between our min and max mole fractions.
	double avgX = (minX+maxX)/2.0;
	model.ConfigureMixture(2, {avgX, 1-avgX});

	// Define interaction parameters
	double gaa = 1.0;
	double gbb = 1.0;
	double ebb = 1.0;
	double eaa = 0.4;

	// Anisotropic interactions.
	model.SetInteractionParameter(eaa, 1, 1);
	model.SetInteractionParameter(sqrt(eaa*ebb), 1, 2);
	model.SetInteractionParameter(ebb, 2, 2);

	// Isotropic interactions.
	model.SetIsotropicParameter(gaa, 1, 1);
	model.SetIsotropicParameter(sqrt(gaa*gbb), 1, 2);
	model.SetIsotropicParameter(gbb, 2, 2);

	// Initialize CSV logger and console logger for output. We want to log our
	// density of states to a CSV file, and just monitor flatness in the console.
	Loggers::CSVLogger csvlogger(modelFile, sitesFile, vecsFile, 5000);
	Loggers::ConsoleLogger consolelogger(1000);

	// Monitor flatness
	auto flatness = [] (BaseModel&, const EnsembleProperty &eprops) {
		return *eprops.at("Flatness");
	};

	// Monitor scale factor
	auto scale = [] (BaseModel&, const EnsembleProperty &eprops) {
		return *eprops.at("ScaleFactor");
	};

	// Log density of states.
	auto dos = [] (BaseModel&, const EnsembleVector &evecs) {
		return *evecs.at("DOS");
	};

	// Show lower outlier count.
	auto lo = [] (BaseModel &, const EnsembleProperty &ep){
		return *ep.at("LowerOutliers");
	};

	// Show upper outlier count.
	auto uo = [] (BaseModel &, const EnsembleProperty &ep){
		return *ep.at("UpperOutliers");
	};

	// Monitor average energy.
	auto n1count = [] (BaseModel &, const EnsembleProperty &ep){
		return *ep.at("SpeciesCount");
	};

	// Register callbacks with loggers.
	csvlogger.AddVectorProperty("DOS", dos);
	consolelogger.AddModelProperty("SpeciesCount", n1count);
	consolelogger.AddModelProperty("Flatness", flatness);
	consolelogger.AddModelProperty("ScaleFactor", scale);
	consolelogger.AddModelProperty("LowerOutliers", lo);
	consolelogger.AddModelProperty("UpperOutliers", uo);

	// Initialize Wang-Landau sampler.
	Ensembles::EXEDOSEnsemble<Site> ensemble(model, temperature, minX, maxX, binCount);

	// Register loggers and moves with the ensemble.
	ensemble.AddLogger(csvlogger);
	ensemble.AddLogger(consolelogger);
	ensemble.AddMove(move1);
	ensemble.AddMove(move2);

	csvlogger.WriteHeaders();

	// Run WL sampling.
	ensemble.SetTargetFlatness(0.85);
	ensemble.Run(iterations);
}

// A very basic input parser.
int parse_input(char const* args[], int& latticeSize,
                double& temperature,
                int& iterations,
                double& minE,
                double& maxE,
                int& binCount,
                std::string& modelFile,
                std::string& SitesFile,
                std::string& vecsFile
                )
{
	std::stringstream ss;

	// Parse input
	ss.clear();
	ss.str(args[1]);
	if(!(ss >> latticeSize))
	{
		std::cerr << "Invalid lattice size. Must be an integer." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[2]);
	if(!(ss >> temperature))
	{
		std::cerr << "Invalid temperature. Must be a double." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[3]);
	if(!(ss >> iterations))
	{
		std::cerr << "Invalid iterations. Must be an integer." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[4]);
	if(!(ss >> minE))
	{
		std::cerr << "Invalid minimum energy. Must be a double." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[5]);
	if(!(ss >> maxE))
	{
		std::cerr << "Invalid maximum energy. Must be a double." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[6]);
	if(!(ss >> binCount))
	{
		std::cerr << "Invalid bin count. Must be an integer." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[7]);
	if(!(ss >> modelFile))
	{
		std::cerr << "Invalid output file. Must be a string." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[8]);
	if(!(ss >> SitesFile))
	{
		std::cerr << "Invalid output file. Must be a string." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[9]);
	if(!(ss >> vecsFile))
	{
		std::cerr << "Invalid output file. Must be a string." << std::endl;
		return -1;
	}

	return 0;
}
