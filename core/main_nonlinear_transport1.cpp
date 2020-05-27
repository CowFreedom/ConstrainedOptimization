#include <iostream>
#include "parameter_estimation.h"
#include "parameters.h"
#include <string>

int main(){

	co::EVarManager<co::EFloat64> vars;
	std::vector<std::string> names={"theta"};
	std::string dir="D:/Documents/Programming/ug4/ug4/apps/parameter_estimation/nonuniform_transport_equation_pso";
	std::vector<co::EFloat64> bounds={co::EFloat64(0.001),co::EFloat64(40)};

	co::NewtonOptions options;
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local> evaluator(dir,"subset_target.lua", "subset_sim.lua");
	co::ParticleSwarmOptimizer<co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local>> pso(options,evaluator);
		
	pso.run<co::EFloat64>(names,bounds);


}