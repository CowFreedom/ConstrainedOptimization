#pragma once

namespace co{
	
	namespace ls{
	template<class T>
	void err1(const std::vector<T>& x, const std::vector<T>& y, std::vector<T>& result,int stride){
		
		for (size_t i=0;i<x.size();i++){
			result[i+stride]=(y[i]-x[i]);
		}
	}
	}

}