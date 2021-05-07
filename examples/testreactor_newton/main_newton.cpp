#include<iostream>
#include "../../core/parameter_estimation.h"
#include "../../core/parameters.h"
#include <string>
#include <filesystem>


int main(int argc, char *argv[]){
	co::EVar64Manager initial_vars;
	co::EVar64 v1(co::EFloat64(0.4268),co::EFloat64(1e-6),co::EFloat64(1.0);
	co::EVar64 v2(co::EFloat64(8.3E-04),co::EFloat64(1e-8),co::EFloat64(0.7));
	co::EVar64 v3(co::EFloat64(0.141),co::EFloat64(1e-8),co::EFloat64(0.7));

	initial_vars.add("k_m_aceto_Methanogenesis",v1); 
	initial_vars.add("k_dec",v2); 
	initial_vars.add("K_s_aceto_Methanogenesis",v3);

	co::NewtonOptions options;
	options.set_stepsize_alpha(1);
	std::string dir = std::string dir="/home/devanshr/Programs/ug4/plugins/ConstrainedOptimization/examples/testreactor_newton/";;
	//std::string dir=std::filesystem::current_path();
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local,co::ConfigOutput::File> evaluator(dir, "subset_target.lua","subset_sim.lua");
	co::EVarManager<co::EFloat64> estimated_vars;
	co::NewtonOptimizer<decltype(evaluator)> solver(options,evaluator);

	solver.run(initial_vars,estimated_vars);
}
