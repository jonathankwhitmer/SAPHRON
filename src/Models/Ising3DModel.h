#pragma once

#include "BaseModel.h"

namespace Models
{
	// 3D Ising model. It is an n-by-n-by-n lattice where each site can have an
	// up-spin or down-spin. The z unit vector in BaseModel was arbitrarily chosen
	// to hold the spin data. The default exchange energy is 1 (ferromagnetic).
	class Ising3DModel : public BaseModel
	{
		private:

			// Size of since lattice dimension.
			int latticeSize;

			// Normalized "J", interaction parameter (J) such that a specified
			// temperature is reduced with a normalized kb from BaseModel.
			double interactionParameter = 1.0;
		public:

			// Initializes Ising3D with a given lattice size (the length of a
			// dimension), Temperature and random number seed. The number of
			// sites is the cube of the size. So for a lattice size of 3, there
			// are 9 sites. The sites are initialized on a lattice including
			// positions, nearest neighbors and spins. The default BaseModel
			// parameters are used otherwise.
			Ising3DModel(int latticeSize, double T, int seed = 1);

			// Evaluates the Ising Hamiltonian for a given site using the formula
			// H = -J*sum(si*sj) where si is the spin for the site at index. Since outside
			// the "sphere of influence" the interaction energy doesn't change (i.e. only
			// two body interactions), we only need to evaluate nearest neighbors.
			double EvaluateHamiltonian(int site);

			// Evaluates the Ising Hamiltonian for a given site using the formula
			// H = -J*sum(si*sj). Since outside the "sphere of influence" the interaction
			// energy doesn't change (i.e. only two body interactions), we only need to
			// evaluate nearest neighbors.
			double EvaluateHamiltonian(Site* site);


			// Evaluates the Boltzmann probability of the system transitioning from prevH
			// to currH via exp(-(currH-prevH)/kb*T).
			double AcceptanceProbability(double prevH, double currH);

			// Sets the interaction parameter in the Hamiltonian.
			double SetInteractionParameter(double j)
			{
				return this->interactionParameter = j;
			};

			// Gets the interaction parameter in the Hamiltonian.
			double GetInteractionParameter()
			{
				return this->interactionParameter;
			};

			// Gets the lattice size (length of a single dimension).
			int GetLatticeSize()
			{
				return this->latticeSize;
			}
	};
}