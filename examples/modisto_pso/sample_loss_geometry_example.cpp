#include<iostream>
#include "../../core/parameter_estimation.h"
#include "../../core/parameters.h"
#include <string>
#include <filesystem>



int main(){
	//Define evaluation type
	//std::string dir ="your path to evaluation.lua file";
	std::string dir=std::filesystem::current_path();
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local,co::ConfigOutput::File> evaluator(dir, "subset_target.lua","subset_sim.lua");
	
	//Set parameter names
	std::vector<std::string> names={"K_s_aceto_Methanogenesis","k_m_aceto_Methanogenesis"};
		
	//std::string dir="/home/devanshr/Programs/ug4/plugins/ConstrainedOptimization/examples/testreactor_pso/";
	
	

	std::vector<co::EFloat64> bounds= {co::EFloat64(1e-6),co::EFloat64(0.5),co::EFloat64(0.1),co::EFloat64(0.5)};
	
	
	int n=20; //number of samples
	
	//Call sampler (which writes results to file in the evaluations folder)
	co::sample_loss_geometry(evaluator,param_names,bounds, n);
}
