#pragma once

#include "options.h"
#include "../evaluation_classes/evaluation.h"

namespace co{

	template<class T>
	class ComputationMode{
		public:
		const ConfigComputation config_computation;
		const ConfigOutput config_output; //determines, if f will be invoked through cmd or function pointers directly
	};

	class ComputationMode<ConfigComputation::Local>{

	public:

	ComputationMode(ConfigOutput _config_output):config_computation(ConfigComputation::Local),config_output(_config_output){
	}

	};
	
}