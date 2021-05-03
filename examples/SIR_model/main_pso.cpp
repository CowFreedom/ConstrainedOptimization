#include<iostream>
#include "../../core/parameter_estimation.h"
#include "../../core/parameters.h"
#include <string>
#include<filesystem>


int main(){

	//std::vector<std::string> names={"alpha","kappa","theta","qq","pp"};
	//std::vector<std::string> names={"alpha","theta", "qq"};
	std::vector<std::string> names={"alpha","tau","theta"};
	
	
	std::string dir= std::filesystem::current_path().string();

	std::vector<co::EFloat64> bounds={co::EFloat64(1e-8),co::EFloat64(6e-7),co::EFloat64(1e-13),co::EFloat64(0.5),co::EFloat64(0.00001),co::EFloat64(0.5)};


	co::PSOOptions options;
	options.set_max_iterations(20);
	options.set_n_particles(32);
	options.set_n_groups(4);
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::File> evaluator(dir,"subset_target.lua", "subset_sim.lua");
	co::ParticleSwarmOptimizer<co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::File>> pso(options,evaluator);
	co::EVarManager<co::EFloat64> estimated_parameters;
	pso.run(estimated_parameters,names,bounds);
}
