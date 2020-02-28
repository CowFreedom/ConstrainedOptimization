#pragma once

#include <limits>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include "EFloat.h"

namespace co{
	


	//I number of intervals (for piecewise interval)
	template<class T>
	class EVar{
		public:
			const T val;
			const T min; //minimum value allowed
			const T max; //maximum value allowed. 
		

			EVar(const T& _val, const T& _min, const T& _max):val(_val),min(_min),max(_max){
				//if debug then test if _min<_max
				
			}
			
		//Addition. Uses interval arithmetic for bounds
		EVar<T> operator+(T rhs) const{
			T res=val+rhs;
			if ((res<max)&&(res>min)){
				return EVar(res,min,max);
			}
			else {
				std::cerr<<"Error adding an EVar with an EFloat\n";
				std::cin.get();
			}
		}
			
		friend std::ostream& operator<<(std::ostream& os, const EVar<T>& v){
			os<<"EVar(value: "<<v.val<<",min: "<<v.min<<",max: "<<v.max<<")";
				return os;
			}

	};
	
	
	template<>
	class EVar<EFloat64>{
		
		public:
		
			EFloat64 val;
			EFloat64 min; //minimum value allowed
			EFloat64 max; //maximum value allowed. 
		

			EVar(const EFloat64& _val, const EFloat64& _min, const EFloat64& _max):val(_val),min(_min),max(_max){
				//test if _min<_max
				
				
			}
			
			
			EVar(const EFloat64& _val):val(_val),min(EFloat64(-std::numeric_limits<double>::infinity())),max(EFloat64(std::numeric_limits<double>::infinity())){}
			std::string print(){
				return val.print().append(" min:").append(min.print()).append(" max: ").append(max.print());
			}
			
			
			EVar<EFloat64> operator+(EFloat64 rhs) const{
				EFloat64 res=val+rhs;
				if ((res<=max)&&(res>=min)){
					return EVar(res,min,max);
				}
				else {
					std::cerr<<"Error adding an EVar with an EFloat\n";
					std::cin.get();
					return EVar(EFloat64(0.0),EFloat64(0.0),EFloat64(0.0)); //TODO CHANGE
				}
			}
			
			EVar<EFloat64> operator-(EFloat64 rhs) const{
				EFloat64 res=val-rhs;
				if ((res<=max)&&(res>=min)){
					return EVar(res,min,max);
				}
				else {
					std::cerr<<"Error substracting an EVar with an EFloat\n";
					std::cin.get();
					return EVar(EFloat64(0.0),EFloat64(0.0),EFloat64(0.0)); //TODO CHANGE 
				}
			}
				
			friend std::ostream& operator<<(std::ostream& os, const EVar<EFloat64>& v){
			os<<"EVar64(value: "<<v.val<<",min: "<<double(v.min)<<",max: "<<double(v.max)<<")";
			//os<<"EVar64("<<v.val<<","<<v.min<<","<<v.max<<")";
				return os;
			}
			
			
			std::string to_lua_string() const{
				std::ostringstream res;
				/*
				std::string res="EVar64(EFloat64(";
				res.append(std::to_string(val.get_v()));
				res.append("), EFloat64(");
				res.append(std::to_string(min.get_v()));
				res.append("),EFloat64(");
				res.append(std::to_string(max.get_v()));
				res.append("))");
				*/
				res<<"EVar64(EFloat64(";
				res<<val.get_v();
				res<<"), EFloat64(";
				res<<min.get_v();
				res<<"),EFloat64(";
				res<<max.get_v();
				res<<"))";
				return res.str();			
			}
			
			double upper_bound() const{
				return val.upper_bound();
			}
			
			double lower_bound() const{
				return val.lower_bound();
			}
			
			double get_min() const{
				return min.get_v();
			}
			double get_max() const{
				return max.get_v();
			}

			
			double get_value_as_double() const{
				return double(val);
			}

	};
	
	

	template<class T>
	class EVarManager{
			private:
			std::vector<EVar<T>> params;
			std::vector<std::string> names;
			
			public:
			EVarManager(){
				
			}
			
			EVarManager(std::vector<EVar<T>> _params, std::vector<std::string> _names){
				if (_params.size()!=_names.size()){
					std::cerr<<"EVarManager error: Parameter names are not the same length as parameter vector\n";
					std::cin.get();
				}
				params=_params;
				names=_names;
			}
			
			bool update_parameters(std::vector<T> new_params){
				if (new_params.size()!=params.size()){
					return false;
				}
				else{
					std::vector<EVar<T>> temp;
					for (size_t i=0;i<new_params.size();i++){
						if ((new_params[i].lower_bound()>=params[i].min.get_v())&&(new_params[i].upper_bound()<=params[i].max.get_v())){
							temp.push_back(EVar<T>(new_params[i],params[i].min,params[i].max));
						}
						else{
							std::cerr<<"Error: Cannot update parameters\n";
							std::cin.get();
							return false;
						}
						
					}
					params=temp;
					return true;
				}
			}
			
			bool update_parameters_cut(std::vector<T> new_params){
				if (new_params.size()!=params.size()){
					return false;
				}
				else{
					std::vector<EVar<T>> temp;
					for (size_t i=0;i<new_params.size();i++){
						if ((new_params[i].lower_bound()>=params[i].min.get_v())&&(new_params[i].upper_bound()<=params[i].max.get_v())){
							temp.push_back(EVar<T>(new_params[i],params[i].min,params[i].max));
						}
						else{
							
							if((new_params[i].lower_bound()<params[i].min.get_v()) &&(new_params[i].lower_bound()!=-infinity) ){
								temp.push_back(EVar<T>(params[i].min,params[i].min,params[i].max));
							}
							else if((new_params[i].upper_bound()>params[i].max.get_v())&&(new_params[i].upper_bound()!=+infinity)){
								temp.push_back(EVar<T>(params[i].max,params[i].min,params[i].max));
							}
							else{
								std::cerr<<"Error: Cannot update parameters\n";
								std::cin.get();
								return false;
							}
							
						}
						
					}
					std::cout<<"New parameters:\n";
					for(auto& x: temp){
						std::cout<<x <<"  ";
					}
					
					params=temp;
					return true;
				}
			}
			
			
			void add(std::string name, const EVar<T>& ev){
				params.push_back(ev);
				names.push_back(name);
			}
			size_t len() const{
				return params.size();
			}
			const std::vector<EVar<T>>& get_params() const{
				return params;
			}
			const std::vector<std::string>& get_names() const{
				return names;
			}
	};
	
	using EVar64=EVar<EFloat64>;
	using EVar64Manager=EVarManager<EFloat64>;


}