/** \file options.h
*The only purpose of this file is to define settings and error codes that are used throughout
*the plugin. Optimizers are configured and report errors according to the enums and classes
*defined in this header. 
*Noteworthy are the co::ConfigOutput and co::ConfigEvaluation enums, as they are problem dependent
*and might significantly alter the behavior of the optimizers.
*/

#pragma once

namespace co{
	
	/*! Usually used to indicate which optimizing algorithm should be used. Possible values are usually all optimizers supported by the plugin. */
	enum class ConfigMethod{
		Newton
	};

	/*! Usually used to indicate the computation method within an optimizers. Is part of the template definition
	of instances of evaluator.h classes. This affects how callbacks to UG4 or other systems are handled in 
	order to evaluate function expressions f(x) that are problem specific.
	The evaluations can be done locally, which means all calculations are done on the same computer. 
	Cluster evaluations on distributed systems are also possible, ergo not yet implemented (version 0.31).*/
	enum class ConfigComputation{
		Local, 
		Cluster
	};

	/*! This is usually part of the template definition of an optimizer function like newton.h. Indicates how function expressions f(x)
	should be evaluated. Usually all instances of Evaluation are values of this enum. See more in evaluations.h.*/
	enum class ConfigEvaluation{
		BiogasEvaluation
	};
	
	/*! This is usually part of the template definition of an optimizer function like newton.h. Indicates how derivatives should be calculated.
	*/
	enum class ConfigDerivatives{
		FiniteDifferences
	};

	/*! This is usually used in computation_modes.h to indicate what happens to results of function evaluations f(x).
	In some problem sets they are written to file which means they have to be parsed afterwards in order to be
	used in subsequent calculations.
	*/
	enum class ConfigOutput{
		File
	};

	/*! Usually indicates if an optimizer like newton.h is run in Debug or Release mode.
	Debug mode might create diagnostic information but it is not guaranteed that values of this enum have an effect.
	*/
	enum class ConfigMode{
		Debug
	};
	
	/*! Error enums to indicate failures or success in the program execution of this plugin.
	Used heavily for example in parse.h or parameter_estimation.h.
	*/
	enum class ErrorCode{
		NoError,
		PathError,
		ParseError,
		OptimizationError,
		ComputationError,
	};
	
	/*! This class collects different configuration enums. The configurations these enums represent are general
	and not problem specific.
	@see NewtonOptions Class collecting configurations specific to the Newton optimizer.*/*/
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
	
	/*! This class collects different configuration enums. In contrast to the more general Options class, the enums collected
	are specific to the Newton optimizer.
	@see Options Class collecting general configuration information.*/
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
		ConfigDerivatives get_config_derivatives() const {
			return config_derivatives;
		}
		ConfigEvaluation get_config_evaluation() const{
			return config_evaluation;
		}
		ConfigComputation get_config_computation() const{
			return config_computation;
		}
		
	};
	
	
	
}