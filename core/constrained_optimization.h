#pragma once
#include<limits>
#include <cstring>
#include<thread>

namespace co{
	
	const unsigned NTHREADS_SUPPORTED = std::thread::hardware_concurrency();
	constexpr double max_double=std::numeric_limits<double>::max();
	constexpr double infinity=std::numeric_limits<double>::infinity();
	constexpr double machine_epsilon=std::numeric_limits<double>::epsilon()*0.5;
	
	/*Converts a double number to its bits. Useful for numerical
	analysis in estimating floating point error*/
	inline uint64_t double_to_bits(double f){
		uint64_t ui;
		std::memcpy(&ui, &f, sizeof(double));
		return ui;
	}
	
	inline double bits_to_double(uint64_t ui){
		double f;
		std::memcpy(&f, &ui, sizeof(uint64_t));
		return f;
	}
	
	inline double next_double_up(double f){
		if(std::isinf(f)&&f>double(0.0)){
			return f;
		}
		else if (f==double(-0.0)){
			f=double(0.0);
		}
		uint64_t ui=double_to_bits(f);
		if (f>=double(0.0)){
			ui++;
		}
		else{
			ui--;
		}
		return bits_to_double(ui);
	}
	
	//Returns the next double a+epsilon
	inline double next_double_down(double f){
		if(std::isinf(f)&&f<double(0.0)){
			return f;
		}
		else if (f==double(0.0)){
			f=double(-0.0);
		}
		uint64_t ui=double_to_bits(f);
		if (f>double(0.0)){
			ui--;
		}
		else{
			ui++;
		}
		return bits_to_double(ui);
	}
	
	//Bounding of error terms in floating point arithmetics, see Higham 2002, Section 3.1
	inline constexpr double gamma(int n){
		return (n*machine_epsilon)/(1-n*machine_epsilon);
		
	}
	
	enum class Result{
		Ok,
		Err
	};


}