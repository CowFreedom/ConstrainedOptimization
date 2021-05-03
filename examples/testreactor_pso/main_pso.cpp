#include<iostream>
#include "../../core/parameter_estimation.h"
#include "../../core/parameters.h"
#include <string>
#include <filesystem>

int main(int argc, char *argv[]){

	std::vector<std::string> names={"K_s_aceto_Methanogenesis","k_m_aceto_Methanogenesis","k_dec_MO_acetoM","k_dec_MO_hydroM","k_dec_MO_ButyricValeric","k_dec_MO_Propionic","k_dec_MO_AcidoMS","k_dec_MO_AcidoAA","k_dec_MO_AcidoLCFA"};
		
	//std::string dir="/home/devanshr/Programs/ug4/plugins/ConstrainedOptimization/examples/testreactor_pso/";
	std::string dir=std::filesystem::current_path().string();
	

	std::vector<co::EFloat64> bounds= {co::EFloat64(1e-6),co::EFloat64(0.5),co::EFloat64(0.1),co::EFloat64(0.5)};
	for(int i = 0; i<14;i+=2){
		bounds.push_back(co::EFloat64(1e-06));
		bounds.push_back(co::EFloat64(1e-01));
	}


	co::PSOOptions options;
	options.set_max_iterations(20);
	options.set_n_particles(8);
	options.set_n_groups(2);
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::File> evaluator(dir,"subset_target.lua", "subset_sim.lua");
	co::ParticleSwarmOptimizer<co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::File>> pso(options,evaluator);
	co::EVarManager<co::EFloat64> estimated_parameters;
	pso.run(estimated_parameters,names,bounds);
}
