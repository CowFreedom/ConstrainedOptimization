/*
 * Copyright (c) 2011-2016:  G-CSC, Goethe University Frankfurt
 * Author: Sebastian Reiter
 * 
 * This file is part of UG4.
 * 
 * UG4 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 3 (as published by the
 * Free Software Foundation) with the following additional attribution
 * requirements (according to LGPL/GPL v3 §7):
 * 
 * (1) The following notice must be displayed in the Appropriate Legal Notices
 * of covered and combined works: "Based on UG4 (www.ug4.org/license)".
 * 
 * (2) The following notice must be displayed at a prominent place in the
 * terminal output of covered works: "Based on UG4 (www.ug4.org/license)".
 * 
 * (3) The following bibliography is recommended for citation and must be
 * preserved in all covered files:
 * "Reiter, S., Vogel, A., Heppner, I., Rupp, M., and Wittum, G. A massively
 *   parallel geometric multigrid solver on hierarchically distributed grids.
 *   Computing and visualization in science 16, 4 (2013), 151-164"
 * "Vogel, A., Reiter, S., Rupp, M., Nägel, A., and Wittum, G. UG4 -- a novel
 *   flexible software system for simulating pde based models on high performance
 *   computers. Computing and visualization in science 16, 4 (2013), 165-179"
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 */

#include "bridge/util.h"

// if possible, replace this with util_domain_dependent.h or
// util_algebra_dependent.h to speed up compilation time
#include "bridge/util_domain_algebra_dependent.h"

#include "core/parameter_estimation.h"
#include "core/parameters.h"
#include "core/efloat.h"
#include "evaluation_classes/evaluation.h"
#include "evaluation_classes/biogas_evaluation.h"
#include<string>


using namespace std;
using namespace ug::bridge;

namespace ug{
namespace ConstrainedOptimization{


//	'ParameterEstimatorSampleClass' and 'ParameterEstimatorSampleFunction' serve as examples. Please
//	delete them if you use this file as a template for your own plugin.
//
//	If you want to test the registry with those classes, please uncomment the
//	commented sections in 'Functionality::DomainAlgebra' and 'Functionality::Common'.


///	a sample function that is used to show how a simple function can be registered
void RunParameterEstimator() {
	UG_LOG("RunParameterEstimator executed.\n");
}

class TestClass{
	public:
	TestClass(std::string a,double b){
		UG_LOG(a);
	}
	
};


struct Functionality
{

/**
 * Function called for the registration of Domain and Algebra independent parts.
 * All Functions and Classes not depending on Domain and Algebra
 * are to be placed here when registering.
 *
 * @param reg				registrySSSS
 * @param parentGroup		group for sorting of functionality
 */
static void Common(Registry& reg, string grp)
{
//	The code below shows how a simple function can be registered
/*	 reg.add_function("RunParameterEstimator", &RunParameterEstimator, "call Function",
	 				 "", "", "Runs the Parameter Estimator. See manual for details.");
					 
//	The code below shows how a simple function can be registered
	 reg.add_function("RunParameterEstimator2", &co::run_parameter_estimator, "call Function",
	 				 "dir#shell_command#parameters", "Runs the Parameter Estimator. See manual for details.");	
	*/				 
//	The code below shows how a simple function can be registered
	 reg.add_function("RunNewtonGauss_BiogasEval", co::RunNewtonGauss_BiogasEval,"call Function",
	 				 "dir#EVarManager#estimated_parameters#stepsize_alpha", "Runs the Newton-Gauss algorithm with BiogasEvaluation<EFloat64, ConfigComputation::Local, ConfigOutput::File>. See manual for details.");	

//	The code below shows how a simple function can be registered
	 reg.add_function("RunPSO_BiogasEval", co::RunPSO_BiogasEval,"call Function",
	 				 "dir#var_descriptor#estimated_parameters#n_particles#n_groups#max_iterations", "Runs the Particle Swarm Optimization algorithm with BiogasEvaluation<EFloat64, ConfigComputation::Local, ConfigOutput::File>. See manual for details.");	

//Registration of a class	

	reg.add_class_<co::EFloat64>("EFloat64", grp)
				.add_constructor<void(*)(double)>()
				.add_method("print",&co::EFloat64::print);	

	reg.add_class_<co::EVar64>("EVar64", grp)
				.add_constructor<void(*)(co::EFloat64&)>()
				.add_constructor<void(*)(co::EFloat64&,co::EFloat64&,co::EFloat64&)>()
				.add_method("print",&co::EVar64::print)
				.add_method("get_value_as_double",&co::EVar64::get_value_as_double);	

	reg.add_class_<co::EVar64Manager>("EVar64Manager", grp)
				.add_constructor()
				.add_method("add",&co::EVar64Manager::add,"name#EVar")
				.add_method("get_name",&co::EVar64Manager::get_name,"index")
				.add_method("get_param",&co::EVar64Manager::get_param,"index")
				.add_method("len",&co::EVar64Manager::len,"");	
				
	reg.add_class_<co::VarDescriptor64>("VarDescriptor64", grp)
				.add_constructor()
				.add_method("add",&co::VarDescriptor64::add,"name#low#high");	
				
	

/*
	reg.add_class_<co::Evar<double>>("Evar_double", grp)
				.add_constructor()
				.add_method("add", &co::EvarManager<double>::add, "add", "name" ,"adds variable");				 
*/	/*
	reg.add_class_<co::EvarManager<double>>("EvarManager_double", grp)
				.add_constructor<void(*)(std::string)>()
				.add_method("add", &co::EvarManager<double>::add, "add", "name" ,"adds variable");
		*/		
}

// end group plugin_template
/// \}

};

} // end namespace ConstrainedOptimization


/**
 * This function is called when the plugin is loaded.
 */
extern "C" void
InitUGPlugin_ConstrainedOptimization(Registry* reg, string grp)
{
	grp.append("ConstrainedOptimization");
	typedef ConstrainedOptimization::Functionality Functionality;

	try{
		RegisterCommon<Functionality>(*reg,grp);
		//RegisterDimensionDependent<Functionality>(*reg,grp);
		//RegisterDomainDependent<Functionality>(*reg,grp);
		//RegisterAlgebraDependent<Functionality>(*reg,grp);
		//RegisterDomainAlgebraDependent<Functionality>(*reg,grp);
	}
	UG_REGISTRY_CATCH_THROW(grp);
}

}// namespace ug
