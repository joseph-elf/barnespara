#ifndef NOEUD
#define NOEUD
#include <iostream>       // basic input output streams
#include <fstream>        // input output file stream class     // input output manipulators
#include <valarray>       // valarray functions
#include <vector>       // vector functions


	#include "Planete.cpp" 
using namespace std;

double norme(valarray<double> vec){return sqrt((vec*vec).sum());}


class Planete;
class Noeud{
	public:
	
	Noeud(vector<Planete*> planete0, double x0, double x1, double y0, double y1 ,double z0,double z1):planetes(planete0),x0(x0),x1(x1),y0(y0),y1(y1),z0(z0),z1(z1){
		masse=0;
		X = valarray<double>{0,0,0};
		for(int i(0); i<planetes.size(); i++){
			masse+=planetes[i]->getMasse();
			X += planetes[i]->X * planetes[i]->getMasse();
		}
		X/=masse;
		
		
		if(planetes.size() >1){
		generateDaughter();
		}
		
	}
	
	~Noeud(){
		for(int i(0);i<daughter.size(); i++){
			delete daughter[i];
		}
		daughter.clear();
		planetes.clear();
		
	}
	
	

	
	
	double masse;
	vector<Planete*> planetes;
	double x0,x1,y0,y1,z0,z1;

	valarray<double>X;
	vector<Noeud*> daughter;
	
	
	double G =100;
	double fmax = 0.3;
	double theta = 0.7;
	
	double deltaX(){return x1-x0;}
	double deltaY(){return y1-y0;}
	double deltaZ(){return z1-z0;}
	
	
	
	void generateDaughter(){
		//if(planetes.size() <=1)return;
		
		vector<Planete*> planetes1,planetes2,planetes3,planetes4,planetes5,planetes6,planetes7,planetes8;
		
		for(int i(0); i<planetes.size(); i++){
			if( (planetes[i]->X[0] < 0.5*(x1+x0)) && (planetes[i]->X[1] < 0.5*(y1+y0))	&&	(planetes[i]->X[2] < 0.5*(z1+z0))	){
				planetes1.push_back(planetes[i]);
			}else if( (planetes[i]->X[0] > 0.5*(x1+x0)) && (planetes[i]->X[1] < 0.5*(y1+y0))	&&	(planetes[i]->X[2] < 0.5*(z1+z0))	){
				planetes2.push_back(planetes[i]);
			}else if( (planetes[i]->X[0] > 0.5*(x1+x0)) && (planetes[i]->X[1] > 0.5*(y1+y0))	&&	(planetes[i]->X[2] < 0.5*(z1+z0))	){
				planetes3.push_back(planetes[i]);
			}else if( (planetes[i]->X[0] < 0.5*(x1+x0)) && (planetes[i]->X[1] > 0.5*(y1+y0))	&&	(planetes[i]->X[2] < 0.5*(z1+z0))	){
				planetes4.push_back(planetes[i]);
			}else if( (planetes[i]->X[0] < 0.5*(x1+x0)) && (planetes[i]->X[1] < 0.5*(y1+y0))	&&	(planetes[i]->X[2] > 0.5*(z1+z0))	){
				planetes5.push_back(planetes[i]);
			}else if( (planetes[i]->X[0] > 0.5*(x1+x0)) && (planetes[i]->X[1] < 0.5*(y1+y0))	&&	(planetes[i]->X[2] > 0.5*(z1+z0))	){
				planetes6.push_back(planetes[i]);
			}else if( (planetes[i]->X[0] > 0.5*(x1+x0)) && (planetes[i]->X[1] > 0.5*(y1+y0))	&&	(planetes[i]->X[2] > 0.5*(z1+z0))	){
				planetes7.push_back(planetes[i]);
			}else if( (planetes[i]->X[0] < 0.5*(x1+x0)) && (planetes[i]->X[1] > 0.5*(y1+y0))	&&	(planetes[i]->X[2] > 0.5*(z1+z0))	){
				planetes8.push_back(planetes[i]);
			}
			
			
			
		}
		
		if(planetes1.size()>=1)daughter.push_back(new Noeud(planetes1,	x0,0.5*(x1+x0),y0,0.5*(y1+y0),z0,0.5*(z1+z0))	);
		if(planetes2.size()>=1)daughter.push_back(new Noeud(planetes2,	0.5*(x1+x0),x1,y0,0.5*(y1+y0),z0,0.5*(z1+z0))	);
		if(planetes3.size()>=1)daughter.push_back(new Noeud(planetes3,	0.5*(x1+x0),x1,0.5*(y1+y0),y1,z0,0.5*(z1+z0))	);
		if(planetes4.size()>=1)daughter.push_back(new Noeud(planetes4,	x0,0.5*(x1+x0),0.5*(y1+y0),y1,z0,0.5*(z1+z0))	);
		if(planetes5.size()>=1)daughter.push_back(new Noeud(planetes5,	x0,0.5*(x1+x0),y0,0.5*(y1+y0),0.5*(z1+z0),z1)	);
		if(planetes6.size()>=1)daughter.push_back(new Noeud(planetes6,	0.5*(x1+x0),x1,y0,0.5*(y1+y0),0.5*(z1+z0),z1)	);
		if(planetes7.size()>=1)daughter.push_back(new Noeud(planetes7,	0.5*(x1+x0),x1,0.5*(y1+y0),y1,0.5*(z1+z0),z1)	);
		if(planetes8.size()>=1)daughter.push_back(new Noeud(planetes8,	x0,0.5*(x1+x0),0.5*(y1+y0),y1,0.5*(z1+z0),z1)	);
		
	}
	
	
	valarray<double> Acc(Planete* currentPlanet){
		
		if(planetes.size() == 1 ){
			double d = norme(X-currentPlanet->X);
			if(d<0.001)return valarray<double>{0,0,0};
			return G*masse*(X-currentPlanet->X)/(pow(d,3) +d*fmax);
		}else{
			double d = norme(X-currentPlanet->X);
			double D = (valarray<double>{deltaX(),deltaY(),deltaZ()}).max();//sqrt(pow(deltaX(),2) + pow(deltaY(),2) + pow(deltaZ(),2));
			if(D/d	<	theta){
				return G*masse*(X-currentPlanet->X)/(pow(d,3) +d*fmax);
			}else{
				valarray<double>A = valarray<double>{0,0,0};
				
				for(int i(0); i < daughter.size(); i++ )
				A+=daughter[i]->Acc(currentPlanet);
				
				return A;
			
			}
			
			
			
		}
		
	}
	
	
};


#endif 

