#include <iostream>
#include "parameter_estimation.h"
#include "parameters.h"
#include <string>


int main(){

	std::vector<std::string> names={"theta"};
	std::string dir="D:/Documents/Programming/ug4/ug4/apps/parameter_estimation/nonuniform_transport_equation_pso";
	std::vector<co::EFloat64> bounds={co::EFloat64(0.001),co::EFloat64(40)};

	co::PSOOptions options;
	options.set_max_iterations(20);
	options.set_n_particles(12);
	options.set_n_groups(2);
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::File> evaluator(dir,"subset_target.lua", "subset_sim.lua");
	co::ParticleSwarmOptimizer<co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::File>> pso(options,evaluator);
	co::EVarManager<co::EFloat64> estimated_parameters;
	pso.run(estimated_parameters,names,bounds);


//	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::Direct> evaluator();

}