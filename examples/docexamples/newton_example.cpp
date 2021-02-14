#include<iostream>
#include "<your path to>/core/parameter_estimation.h"
#include "<your path to>/core/parameters.h"
#include <string>



int main(){
	co::EVar64Manager initial_vars;
	co::EVar64 theta1(co::EFloat64(1),co::EFloat64(0),co::EFloat64(30));
	co::EVar64 theta2(co::EFloat64(1),co::EFloat64(0),co::EFloat64(10));
	co::EVar64 theta3(co::EFloat64(1),co::EFloat64(0),co::EFloat64(30));

	initial_vars.add("v_theta1",theta1);
	initial_vars.add("v_theta2",theta2);
	initial_vars.add("v_theta3",theta3);

	co::NewtonOptions options;
	options.set_stepsize_alpha(1);
	std::string dir ="your path to your evaluation.lua file";
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local,co::ConfigOutput::File> evaluator(dir, "subset_target.lua","subset_sim.lua");
	co::EVarManager<co::EFloat64> estimated_vars;
	co::NewtonOptimizer<decltype(evaluator)> solver(options,evaluator);

	solver.run(initial_vars,estimated_vars);
}
