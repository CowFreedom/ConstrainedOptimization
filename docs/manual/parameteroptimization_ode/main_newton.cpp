#include <iostream>
#include "C:\Users\devan\ug4\plugins\ConstrainedOptimization\core\parameter_estimation.h"
#include "C:\Users\devan\ug4\plugins\ConstrainedOptimization\core\parameters.h"
#include <string>



int main(){
	co::EVar64Manager initial_vars;
	co::EVar64 alpha(co::EFloat64(0.00001),co::EFloat64(0),co::EFloat64(0.1));
	co::EVar64 kappa(co::EFloat64(0.0002),co::EFloat64(0),co::EFloat64(1));
	co::EVar64 theta(co::EFloat64(0.0002),co::EFloat64(0),co::EFloat64(1));
	co::EVar64 qq(co::EFloat64(14),co::EFloat64(0),co::EFloat64(50));
	co::EVar64 pp(co::EFloat64(7),co::EFloat64(4),co::EFloat64(10));

	//initial_vars.add("alpha",alpha); //scale factor for susceptible derivative
	//initial_vars.add("pp",pp); //sim19:59:52
	//initial_vars.add("kappa",kappa);
	initial_vars.add("qq",qq);
	//initial_vars.add("theta",theta);
	/* initial_vars.add("theta",theta); //scale factor for death derivative
	initial_vars.add("kappa",kappa); //scale factor for recovered derivative
	initial_vars.add("qq",qq); //scale factor for death derivative
	initial_vars.add("pp",pp); //scale factor for death derivative
	 */
	co::NewtonOptions options;
	options.set_stepsize_alpha(1);
	std::string dir ="C:\\Users\\devan\\ug4\\apps\\test";
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local,co::ConfigOutput::File> evaluator(dir, "subset_target.lua","subset_sim.lua");
	co::EVarManager<co::EFloat64> estimated_vars;
	co::NewtonOptimizer<decltype(evaluator)> solver(options,evaluator);

	solver.run(initial_vars,estimated_vars);
}
