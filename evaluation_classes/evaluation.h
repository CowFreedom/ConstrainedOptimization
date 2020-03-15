/** \file evaluation.h
*This file defines the abstract base class Evaluation, which 
*is used by nearly every optimizer like newton.h. Classes inheriting from Evaluation
*define how data is loaded and the target function to be optimized is evaluated.
*Evaluation subclasses cannot always be reused. Sometimes every optimization problem requires its own Evaluation subclass.
*Prominent example is BiogasEvaluation, which has been tailored for the UG4 biogas_app problem, but can also be used
*for problems that behave "similar" (in the sense of data loading, interpolation and evaluation).
*As evaluation subclasses are not only dependent on the numeric type but also the computation mode ConfigComputation, sometimes
*several evaluation subclasses are created for the same problem.
@see BiogasEvaluation<T,ConfigComputation::Local> for a specific subclass of Evaluation.
*/

#pragma once
#include "../core/options.h"
#include <memory>

namespace co{

	/*! Abstract evaluation class. This class cannot be used directly but serves as a parent for problem specific subclasses like
	BiogasEvaluation. It is templatized by the numeric datatype (like EFloat64) used by the problem and the way the objective function f(x) is evaluated by the system.
	Before creating your own Evaluation child class, it should always be investigated if existing child classes like BiogasEvaluation represent the problem well enough.
	When an existing evaluation child class does not represent a problem well, a new class inheriting from Evaluation should be created.*/
	template<class T,ConfigComputation M>
	class Evaluation{
		public:
		
		Evaluation(){}
		
		/*Evaluators ought to not be copied (which disqualifies pass by value), as they sometimes contain resources (e.g. pointers) to themselves*/
		Evaluation(const Evaluation& rhs)=delete;
		
		const ConfigComputation computation_mode=M;
		
		/*! Abstract class that represents evaluations of the objective function f(t).
		Target is needed as the parameter because the output vector might be tailored towards target as seen in tailor_array in biogas_evaluation.h.
		The output is a vector of vectors as each EVarManager v may need multiple evaluations (as done in derivative.h).
		The template parameter T represents the numeric datatype (such as EFloat64) underlying the problem.
		 @param[in,out] v Vector containing variables managed by an EVarManager.
		 @param[in,out] target Vector containing values of the target. These are usually time values which are needed to tailor the time values of the simulation to experimental data.
		 \return Vector of vectors collecting evaluations for each variable pair in EVarManager.
		@see BiogasEvaluation<T,ConfigComputation::Local> For a specific implementation.		 
		*/
		virtual std::vector<std::vector<T>> eval(const std::vector<EVarManager<T>>& v, const std::vector<T>& target, ErrorCode& e, std::string message="")=0;
		
		/*! Abstract class that represents how problem data is loaded (like experimental data etc.). 
		The data is saved in two containers. The first represent time data (the t values in a problem of the form f(t)) and
		the second codomain data (the function values resulting from f(t)). If the problem also includes spatial domain data x (like f(t,x)) a new function should probably
		be created that also saves this spatial data. The template parameter T represents the numeric datatype (such as EFloat64) underlying the problem.
		 @param[in,out] t Vector storing the time values of the problem.
		 @param[in,out] d Vector representing 
		 \return Error enum indicating the success of the loading operation.
		@see BiogasEvaluation<T,ConfigComputation::Local> For a specific implementation. 
		*/
		virtual ErrorCode load_target(std::vector<T>& t,std::vector<T>& d)=0;
		

		
	};
}