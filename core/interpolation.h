#pragma once


namespace co{
		/*Linearly interpolates source into target  and saves it into storage
		nt: length of target
		ns: length of source
		cols: number of columns in source!*/
		template<class T>
		bool get_array_like(const std::vector<T>& targettimes, const std::vector<T>& sourcetimes, const std::vector<T>& source, const std::vector<T>& storage){
			int saved_rows=0;
			int j=0;
			int nt=targettimes.size();
			int ns=sourcetimes.size();
			int cols=source.size()/ns;

			if (nt>ns){
				std::cout<<"Less source data than target data\n";
			}
			int i=0;
			int substract=0;
			if (nt==ns){
			std::cout<<"ns==nt!";
			std::cin.get();
			double t1=sourcetimes[nt-2];
			double t2=sourcetimes[nt-1];
			double tc=targettimes[nt-1];
			double t=(tc-t1)/(-t1+t2);
			for (int k=0;k<cols;k++){
			//std::cout<<"Bis hier: j: "<<j<<" saved_rows: "<<saved_rows<<" k:"<<k<<"\n";
			storage[(nt-1)*cols+k]=source[(nt-1)*cols+k]*(1-t)+source[(nt-1)*cols+k+1]*t;
			}
			substract=cols;
			}
			if (sourcetimes[0]>targettimes[0]){
				sourcetimes[0]=targettimes[0];
			}
			else{
				targettimes[0]=sourcetimes[0];
			}
			//sourcetimes[0]=targettimes[0]; //remove later. dont do it
			std::cout<<"Bis hier!\n";
			while((saved_rows*cols)<(nt-substract)*cols){
					std::cout<<"sourcetime[j]: "<< sourcetimes[j]<<"sourcetime[j+1]"<<sourcetimes[j+1]<<"  targettimes[i]"<<targettimes[i]<<"\n";
					std::cout<<"nt-substract:"<<nt-substract<<"\n";
					if (sourcetimes[j]>sourcetimes[j+1]){
						std::cout<<"ERROR\n";
						std::cin.get();
						return false;
					}
					
					if ((sourcetimes[j]<=targettimes[i])&&(sourcetimes[j+1]>=targettimes[i])){
						double t1=sourcetimes[j];
						double t2=sourcetimes[j+1];
						double tc=targettimes[i];
						double t=(tc-t1)/(-t1+t2);
						for (int k=0;k<cols;k++){
						std::cout<<"Bis hier: j: "<<j<<" saved_rows: "<<saved_rows<<" k:"<<k<<"t: "<<t<<"\n";
						std::cout<<"n:"<<saved_rows*cols+k<<" value: "<<source[j*cols+k]*(1-t)+source[(j+1)*cols+k]*t<<"\n";
						storage[saved_rows*cols+k]=source[j*cols+k]*(1-t)+source[(j+1)*cols+k]*t;
						}
		
					saved_rows++;
					i++;
				}		
				else{
					j++;
				}
			}
		
				
			return true;
		}
}