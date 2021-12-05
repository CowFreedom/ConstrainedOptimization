#pragma once
#include <string>
#include <utility>
#include <algorithm>
#include <iostream>
#include<cmath>
#include "constrained_optimization.h"

namespace co{


	/*Extended float which tracks estimates of floating point errors*/
	class EFloat64{
		private:
			double val;
			double low;
			double high;	
		public:
		
			EFloat64():val(0.0),low(0.0),high(0.0){
				
			}
			EFloat64(double _val):val(_val),low(_val),high(_val){
				
			}
			
			EFloat64(double _val,double err):val(_val){
				if (err==double(0.0)){
					low=val;
					high=val;
				}
				else{
					low=next_double_down(val-err);
					high=next_double_up(val+err);
				}
			}
			
			//copy constructor
			EFloat64(const EFloat64& rhs){
				val=rhs.val;
				low=rhs.low;
				high=rhs.high;
			}
			
			EFloat64(double _val,double _low, double _high):val(_val){
				if (_high<_low){
					std::cerr<<"EFloat64 error: low>high in class initialization! high:"<<_high<<" low: "<<_low<<"\n";
				
					std::cin.get();
				}
				else{
					low=_low;
					high=_high;
				}
			}			

		//Addition. Uses interval arithmetic for bounds
		EFloat64 operator+(EFloat64 rhs) const{
		
			double r_val=val+rhs.get_v();
			double r_low=next_double_down(low+rhs.lower_bound());
			double r_high=next_double_up(high+rhs.upper_bound());
			return EFloat64(r_val,r_low,r_high);
		}
		
		EFloat64 operator-(EFloat64 rhs) const{
		
			double r_val=val-rhs.get_v();
			double r_low=next_double_down(low-rhs.upper_bound());
			double r_high=next_double_up(high-rhs.lower_bound());
			return EFloat64(r_val,r_low,r_high);
		}
		
		EFloat64 operator-() const{
		
			double r_val=-get_v();
			double r_low=-upper_bound();
			double r_high=-lower_bound();
			return EFloat64(r_val,r_low,r_high);
		}
		
		EFloat64 operator*(EFloat64 rhs) const{
			double r_val=val*rhs.get_v();
			double prod[4]={
			low*rhs.lower_bound(),high*rhs.lower_bound(),
			low*rhs.upper_bound(),high*rhs.upper_bound()};
			double r_low=next_double_down(std::min(prod[0],std::min(prod[1],std::min(prod[2],prod[3]))));
			double r_high=next_double_up(std::max(prod[0],std::max(prod[1],std::max(prod[2],prod[3]))));
			return EFloat64(r_val,r_low,r_high);
		}
		
		EFloat64 testmult(EFloat64 rhs) const{
			std::cout<<"Operator* start:\n";
			double r_val=val*rhs.get_v();
			double prod[4]={
			low*rhs.lower_bound(),high*rhs.lower_bound(),
			low*rhs.upper_bound(),high*rhs.upper_bound()};
			double r_low=next_double_down(std::min(prod[0],std::min(prod[1],std::min(prod[2],prod[3]))));
			double r_high=next_double_up(std::max(prod[0],std::max(prod[1],std::max(prod[2],prod[3]))));
			std::cout<<"r_low:"<<r_low<<"  r_high:"<<r_high<<"\n";
			std::cout<<"Operator* end:\n";
			return EFloat64(r_val,r_low,r_high);
		}
		
		EFloat64 operator/(EFloat64 rhs) const{
			/*If the interval includes zero, we have the possibility that the true value
			is x/0. We define 1/0 as infinity instead of rejecting the result
			*/
			if (rhs.upper_bound()>0.0 && rhs.lower_bound()<0.0){
				return EFloat64(val/rhs.val,-infinity,+infinity);
			}
			else{
			double r_val=val/rhs.val;
			double div[4]={low/rhs.lower_bound(),low/rhs.upper_bound(),high/rhs.lower_bound(),high/rhs.upper_bound()};
			double r_low=next_double_down(std::min(div[0],std::min(div[1],std::min(div[2],div[3]))));
			double r_high=next_double_up(std::max(div[0],std::max(div[1],std::max(div[2],div[3]))));
			
			return EFloat64(r_val,r_low,r_high);
			}
		}
		
		bool operator==(const EFloat64 rhs) const{
			if ((val==rhs.get_v() )&& (rhs.upper_bound()==high)&&(rhs.lower_bound()==low)){
				return true;
			}
			else if ((val!=rhs.get_v() )&& (rhs.upper_bound()!=high)&&(rhs.lower_bound()!=low)){
				return false;
			}
			else{
				std::cerr<<"Error: Cannot compare floats\n";
				return false;
			}
		}
		
		bool operator>(const EFloat64 rhs) const{
			if ((val>rhs.get_v() )&& (high>rhs.upper_bound())&&(low>rhs.lower_bound())){
				return true;
			}
			else if ((val<rhs.get_v() )&& (high<rhs.upper_bound())&&(low<rhs.lower_bound())){
				return false;
			}
			else{
				return false; //TODO: Change into result
				std::cerr<<"Error: Cannot compare floats\n";
			}
		}
		
		bool operator<=(const EFloat64 rhs) const{
			if ((val<=rhs.get_v() )&& (high<=rhs.upper_bound())&&(low<=rhs.lower_bound())){
				return true;
			}
			else if ((val>rhs.get_v() )&& (high>rhs.upper_bound())&&(low>rhs.lower_bound())){
				return false;
			}
			else{
				return false;//TODO: Change into result
				std::cerr<<"Error: Cannot compare floats\n";
			}
		}	
		
		bool operator>=(const EFloat64 rhs) const{
			if ((val>=rhs.get_v() )&& (high>=rhs.upper_bound())&&(low>=rhs.lower_bound())){
				return true;
			}
			else if ((val<rhs.get_v() )&& (high<rhs.upper_bound())&&(low<rhs.lower_bound())){
				return false;
			}
			else{
				return false;//TODO: Change into result
				std::cerr<<"Error: Cannot compare floats\n";
			}
		}		
		bool operator!=(EFloat64 rhs) const{
			if ((val!=rhs.get_v() )||(rhs.upper_bound()!=high)||(rhs.lower_bound()!=low)){
				return true;
			}
			else{
				return false;
			}
		}

		EFloat64& operator=(const EFloat64 &rhs) {
			if (&rhs != this){
				val=rhs.val;
				low=rhs.low;
				high=rhs.high;
			}
			return *this;
		}


		EFloat64& operator+=(const EFloat64 &rhs) {
			val+=rhs.get_v();
			low=next_double_down(low+rhs.lower_bound());
			high=next_double_up(high+rhs.upper_bound());
			return *this;
		}
		
		
		EFloat64& operator-=(const EFloat64 &rhs) {
			val-=rhs.get_v();
			low=next_double_down(low-rhs.upper_bound());
			high=next_double_up(high-rhs.lower_bound());
			return *this;
		}

		EFloat64& operator*=(const EFloat64 &rhs) {
			
			val*=rhs.get_v();
			double prod[4]={
			low*rhs.lower_bound(),high*rhs.lower_bound(),
			low*rhs.upper_bound(),high*rhs.upper_bound()};
			low=next_double_down(std::min(prod[0],std::min(prod[1],std::min(prod[2],prod[3]))));
			high=next_double_up(std::max(prod[0],std::max(prod[1],std::max(prod[2],prod[3]))));
			return *this;
		}


		EFloat64& operator/=(const EFloat64 &rhs) {
			/*If the interval includes zero, we have the possibility that the true value
			is x/0. We define 1/0 as infinity instead of rejecting the result
			*/
			if (rhs.upper_bound()>0.0 && rhs.lower_bound()<0.0){
				val/=rhs.val;
				low=-infinity;
				high=+infinity;
				return *this;
			}
			else{
			val/=rhs.val;
			double div[4]={low/rhs.lower_bound(),low/rhs.upper_bound(),high/rhs.lower_bound(),high/rhs.upper_bound()};
			low=next_double_down(std::min(div[0],std::min(div[1],std::min(div[2],div[3]))));
			high=next_double_up(std::max(div[0],std::max(div[1],std::max(div[2],div[3]))));
			
			return *this;
			}
		}		

		EFloat64 abs() const{
			
			double abs_low=std::fabs(low);
			double abs_high=std::fabs(high);
			
			double new_min=std::min(abs_low,abs_high);
			double new_max=std::max(abs_low,abs_high);
			
			return EFloat64(std::fabs(val),new_min,new_max);
		}
		
		double get_absolute_error() const{
			return next_double_up(std::max(std::abs(high-val),std::abs(val-low)));
			
		}
		
		double upper_bound() const{
			return high;
		}
		
		double lower_bound() const{
			return low;
		}
		
		std::pair<Result,bool> less(EFloat64 rhs){
			if ((val<rhs.val)&&(low<rhs.low)&&(high<rhs.high)){
				return std::make_pair<Result,bool>(Result::Ok,true);
			}
			else if ((val>rhs.val)&&(low>rhs.low)&&(high>rhs.high)){
				return std::make_pair<Result,bool>(Result::Ok,false);
			}
			else{
				return std::make_pair<Result,bool>(Result::Err,false);
			}
			
		}
		
		EFloat64 sqrt() const{
			return EFloat64(std::sqrt(val),std::sqrt(low),std::sqrt(high));
		}
		
		double get_v() const{
			return val;
		}
		
		std::string print() const{
			return std::to_string(val);
		}
		
		friend std::ostream& operator<<(std::ostream& os, const EFloat64& v){
			os<<"EFloat64(value: "<<v.val<<",low: "<<v.low<<",high: "<<v.high<<")";
				return os;
			}
		
		
		explicit operator double() const{
				return val;	
			}
	
		
	};
	
	EFloat64 sqrt(EFloat64 v) {
			return v.sqrt();
	}

}
