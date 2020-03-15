#pragma once
#include "../core/options.h"
#include <memory>

namespace co{

	/*M: Lokal, Cluster; T:EFloat64*/
	template<class T,ConfigComputation M>
	class Evaluation{
		public:
		
		Evaluation(){}
		
		/*Evaluators ought to not be copied (which disqualifies pass by value), as they sometimes contain resources (e.g. pointers) to themselves*/
		Evaluation(const Evaluation& rhs)=delete;
		const ConfigComputation computation_mode=M;
		
		/*Target is needed as the output vector might be tailored towards target*/
		virtual std::vector<std::vector<T>> eval(const std::vector<EVarManager<T>>& v, const std::vector<T>& target, ErrorCode& e, std::string message="")=0;
		
		//t: target times, d= target data
		virtual ErrorCode load_target(std::vector<T>& t,std::vector<T>& d)=0;
		

		
	};
}