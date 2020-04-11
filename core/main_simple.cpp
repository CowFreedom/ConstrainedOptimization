#include <iostream>
#include "parameter_estimation.h"
#include "parameters.h"
#include <string>

int main(){

	co::EVarManager<co::EFloat64> vars;
	std::vector<std::string> names={"theta1","theta2","theta3"};
	std::string dir="D:/Documents/Programming/ug4/ug4/apps/parameter_estimation/menten_kinetics";
	std::vector<co::EFloat64> bounds={co::EFloat64(0.0),co::EFloat64(40.0),co::EFloat64(0.0000001),co::EFloat64(12.0),co::EFloat64(-10.001),co::EFloat64(40.0)};

	co::NewtonOptions options;
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local> evaluator(dir,"subset_target.lua", "subset_sim.lua");
	co::ParticleSwarmOptimizer<co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local>> pso(options,evaluator);
		
	pso.run<co::EFloat64>(names,bounds);


}