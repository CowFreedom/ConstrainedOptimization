#pragma once

namespace co{
	
	//Supported Optimizers
	enum class ConfigMethod{
		Newton
	};

	enum class ConfigComputation{
		Local,
		Cluster
	};

	enum class ConfigEvaluation{
		BiogasEvaluation
	};

	enum class ConfigDerivatives{
		FiniteDifferences
	};

	enum class ConfigOutput{
		File
	};

	enum class ConfigMode{
		Debug
	};
	
	
	class Options{
		private:
		ConfigMethod config_method;
		ConfigMode config_mode;
		ConfigOutput config_output;	
		
		public:
		
		void set_config_method(ConfigMethod cm){
			config_method=cm;
		}
		void set_config_mode(ConfigMode cm){
			config_mode=cm;
		}
		void set_config_output(ConfigOutput co){
			config_output=co;
		}
		ConfigMethod get_config_method() const{
			return config_method;
		}
		ConfigMode get_config_mode() const{
			return config_mode;
		}
		ConfigOutput get_config_output() const{
			return config_output;
		}				
	};
	
	class NewtonOptions:public Options{
		private:
		ConfigEvaluation config_evaluation;
		ConfigDerivatives config_derivatives;
		ConfigComputation config_computation;
		public:
		
		void set_config_evaluation(ConfigEvaluation ce){
			config_evaluation=ce;
		}
		
		void set_config_derivatives(ConfigDerivatives cd){
			config_derivatives=cd;
		}

		void set_config_computation(ConfigComputation cc){
			config_computation=cc;
		}		
		ConfigDerivatives get_config_derivatives(){
			return config_derivatives;
		}
		ConfigEvaluation get_config_evaluation(){
			return config_evaluation;
		}
		ConfigComputation get_config_computation(){
			return config_computation;
		}
		
	};
	
	
	
}