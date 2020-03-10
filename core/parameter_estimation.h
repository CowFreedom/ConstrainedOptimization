#pragma once
#include<string>
#include "options.h"
#include "parameters.h"
#include "../optimizers/newton.h"
#include "efloat.h"
#include "../evaluation_classes/biogas_evaluation.h"
//#include <filesystem>

extern const float version_number=0.3;

namespace co{
	
	/*dir: directory where configuration files are
	shell_command: command that shall be executed (e.g. ugshell -ex biogas_app/biogas.lua -p Test.lua)
	E: Evaluation type
	*/
	bool run_parameter_estimator(std::string dir, std::string shell_command, const EVar64Manager& parameters){
		
		NewtonOptions options;
		options.set_config_method(co::ConfigMethod::Newton);
		options.set_config_mode(co::ConfigMode::Debug);
		options.set_config_output(co::ConfigOutput::File);
		options.set_config_evaluation(co::ConfigEvaluation::BiogasEvaluation);
		options.set_config_computation(co::ConfigComputation::Local);	
		
		BiogasEvaluation<EFloat64,ConfigComputation::Local> evaluator(dir,"subset_target.lua", "subset_sim.lua");
		NewtonOptimizer<BiogasEvaluation<EFloat64,ConfigComputation::Local>> optimizer(options,evaluator);
		
		std::vector<EVar64Manager> params;
		params.push_back(parameters);
		optimizer.run(params);
		return true;
		
	}
	
}