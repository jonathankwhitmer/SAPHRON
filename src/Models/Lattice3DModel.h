#pragma once

#include "BaseModel.h"
#include <functional>

namespace Models
{
	// 3D lattice model. It is an abstract class that can be overriden with a particular
	// Hamiltonian for the appropriate model.
	class Lattice3DModel : public BaseModel
	{
		private:

			// Interaction potential stored as a lower packed triangular matrix.
			// Ex. e11, e12, e13, e22, e23, e33.
			std::vector<double> _interactionParameter { 1.0 };

			int _xl, _yl, _zl;

		public:

			// Initializes Lattice3D with the specified lattice dimensions
			// and random number seed. The number of sites is the cube
			// of the size. So for a lattice size of 3x3x3, there are 27 sites.
			// The sites are initialized on a lattice including
			// positions, nearest neighbors and spins. The default BaseModel
			// parameters are used otherwise.
			Lattice3DModel(int xLength, int yLength, int zLength, int seed = 1);

			// Gets the interaction parameter for species i and j.
			double GetInteractionParameter(int i = 1, int j = 1);

			// Sets the interaction parameter between two species.
			double SetInteractionParameter(double e, int i, int j);
	};
}
