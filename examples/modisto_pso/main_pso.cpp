#include<iostream>
#include "../core/parameter_estimation.h"
#include "../core/parameters.h"
#include <string>


int main(int argc, char *argv[]){

	std::vector<std::string> names={"k_m_aceto_Methanogenesis","k_dec","K_s_aceto_Methanogenesis"};
		
	std::string dir=argv[0];

	std::vector<co::EFloat64> bounds={co::EFloat64(1e-6),co::EFloat64(0.5),co::EFloat64(4e-8),co::EFloat64(8e-4),co::EFloat64(0.1),co::EFloat64(0.5)};


	co::PSOOptions options;
	options.set_max_iterations(20);
	options.set_n_particles(32);
	options.set_n_groups(4);
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::File> evaluator(dir,"subset_target.lua", "subset_sim.lua");
	co::ParticleSwarmOptimizer<co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::File>> pso(options,evaluator);
	co::EVarManager<co::EFloat64> estimated_parameters;
	pso.run(estimated_parameters,names,bounds);
}
