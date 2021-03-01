#include<iostream>
#include "../../core/parameter_estimation.h"
#include "../../core/parameters.h"
#include <string>



int main(){
	co::EVar64Manager initial_vars;
	co::EVar64 alpha(co::EFloat64(1.47e-7),co::EFloat64(1e-8),co::EFloat64(2));
	co::EVar64 tau(co::EFloat64(0.089),co::EFloat64(1e-15),co::EFloat64(5));
	co::EVar64 theta(co::EFloat64(0.00155),co::EFloat64(1e-15),co::EFloat64(5));

	initial_vars.add("alpha",alpha); //scale factor for susceptible derivative
	initial_vars.add("tau",tau); //scale factor for recovered derivative
	initial_vars.add("theta",theta); //scale factor for death derivative

	co::NewtonOptions options;
	options.set_stepsize_alpha(1);
	std::string dir ="/home/server1/Programs/ug4/ug4/plugins/ConstrainedOptimization/examples/SIR_model/";
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local,co::ConfigOutput::File> evaluator(dir, "subset_target.lua","subset_sim.lua");
	co::EVarManager<co::EFloat64> estimated_vars;
	co::NewtonOptimizer<decltype(evaluator)> solver(options,evaluator);

	solver.run(initial_vars,estimated_vars);
}
