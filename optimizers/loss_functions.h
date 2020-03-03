#pragma once

namespace co{
	
	namespace ls{
		
	//Simple difference. Usually the residual function of the Newton Gauss algorithm
	template<class T>
	void err1(const std::vector<T>& x, const std::vector<T>& y, std::vector<T>& result,int stride){
		
		for (size_t i=0;i<x.size();i++){
			result[i+stride]=(y[i]-x[i]);
		}
	}
	
	/*Mean squared loss.Usually the target function of the Newton Gauss algorithm*/
	template<class T>
	T mse(const std::vector<T>& x, const std::vector<T>& y){
		T sum=T(0.0);
		for (size_t i=0;i<x.size();i++){
			sum+=(y[i]-x[i])*(y[i]-x[i]);
		}
		return sum;
	}
	

	}

}