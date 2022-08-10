#ifndef PLANETE
#define PLANETE

#include <iostream>       // basic input output streams
#include <fstream>        // input output file stream class     // input output manipulators
#include <valarray>       // valarray functions


	#include "Noeud.cpp" 
	class Noeud;
using namespace std;
class Planete{
	public:
	Planete(valarray<double>X0,valarray<double>V0={0,0,0},double masseSolaire=1. ,double dt=1.):X(X0),masse(masseSolaire){
		Xprec = X0 - V0*dt;	
	}

	
	double getMasse(){return masse;}
	valarray<double>X;
	valarray<double>Xprec;
	
	valarray<double>A;
	double masse;
	
	void writeX(ofstream *output){*output << X[0] << ", " << X[1] << endl;}
	void writeA(ofstream *output){*output << A[0] << ", " << A[1] << endl;}
	double getA(){return sqrt( (A*A).sum() );}
	double distOrigine(){return sqrt( (X*X).sum() ); }
	
	void step(double dt){
		valarray<double>C=X;
		X = 2.*X - Xprec + A*dt*dt;
		Xprec=C;
	}
	
	
};
#endif 
