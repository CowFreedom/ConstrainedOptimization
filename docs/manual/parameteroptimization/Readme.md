# Directory Structure:

## Epidemics:

/config/

/geometry/

/scripts/ 

	These folders contain required files for the model of which the parameters are being optimized. 
	The rest of the folders and files are part of the ConstrainedOptimization procedure.

## ConstrianedOptimization:

/CollectedData/ 

	This folder contains the empirical data/experimental data. 	The simulation output will be fitted against this data.

subset_sim.lua
	
	This file specifies the the data from the Epidemics model, 	which would be used in the optimization procedure. 

subset_target.lua

	This file specifies the respective columns and data that are required from the files in the CollectedData folder. 

	Note: The order of the columns in the subset_sim.lua and 	subset_target.lua should match. For example, if the first column in subset_target.lua is for Hannover_infected then the first column in subset_sim.lua should also contian the Hannove_infected data.

evaluate.lua
	
	This script contains the logic for the model. Once the optimization procedure has calculated new values for the parameters, they are loaded into evalute.lua and the model is executed with the new values. 

	Note: The entire logic for the model can reside in evaluate.lua file or alternatively the script for the model can be called.


main_\*.lua & main_*.cpp
	
	These files are the starting points for the Optimization procedure.
	These files define which parameters are to be optimized, their bounds and the settings for the solver being used.
	There are 2 ways to start the Optimization. 
	-C++
		g++ -O3 -pthread filename.cpp
	-Lua
		ugshell -ex filename.lua
		
### Loss Geometry

loss_geom.cpp

	This file allows the user to investigate the geometry of the objective function for a parameter region. 

## Output of ConstrainedOptimization
A folder called 'evaluations' is automatically generated in the working directory and the output is saved to file here.
	