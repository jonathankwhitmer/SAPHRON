#pragma once

#include "../src/Loggers/Logger.h"
#include "../src/Models/BaseModel.h"
#include <iostream>

using namespace Loggers;

class MockLogger : public Logger
{
	public:
		void LogThermalProperties(BaseModel& model)
		{
			for(auto &prop : this->ThermalProps)
				prop.second(model);
		}

		void FlushRunningAverages(int count)
		{
			for(double& avg : this->ThermalAverages)
				avg /= count;
		}
};