#include <iostream>       // basic input output streams
#include <fstream>        // input output file stream class
#include <cmath>          // librerie mathematique de base
#include <iomanip>        // input output manipulators
#include <valarray>       // valarray functions
#include <vector>       // vector functions
#include <chrono>
#include <future>
#include <direct.h>

#include "Noeud.cpp"
#include "Planete.cpp"
#include "ConfigFile.h"
using namespace std; 

using namespace std::chrono;



void writeCoord(vector<Planete*> systeme, string fichierName, int img){
	ofstream* fichier = new ofstream(fichierName + "/" + to_string(img) + ".out"); 
	fichier->precision(5);
	
	for(int i(0); i<systeme.size(); i++){
			*fichier << systeme[i]->X[0] << ", " << systeme[i]->X[1] << ", " << systeme[i]->X[2] << ", " << systeme[i]->getA() << "\n" ;
		}
	fichier->close();
		
	}


double random(double min, double max){
		  return((max-min)*abs(rand())/RAND_MAX) + min;
	}

void calcAcc(int start, int stop, vector<Planete*> systeme, Noeud* Eve){
		for(int i(start); i<stop; i++){
			systeme[i]->A = Eve->Acc(systeme[i]);
		}
	}

void newStep(int start, int stop, vector<Planete*> systeme, double dt){
		for(int i(start); i<stop; i++){
			systeme[i]->step(dt);
		}
	}

int main(int argc, char* argv[])
{
	
	int nombreCoeur(1);

    cout << "Nombre de Coeurs pour parallelisation ?" << endl;
    cin >> nombreCoeur;
    cout << endl;
    
	auto start = high_resolution_clock::now();

	
	
	
	//
	srand (time(NULL));						//CHIANT
	string inputPath("configuration.in");
		if(argc>1)
    inputPath = argv[1];
	ConfigFile configFile(inputPath);
		for(int i(2); i<argc; ++i)
    configFile.process(argv[i]);
    
    int nbEtoile(configFile.get<int>("nbEtoile"));
	double rayon(configFile.get<double>("rayon"));
	double epaisseur(configFile.get<double>("epaisseur"));
	double V(configFile.get<double>("V0"));
	double tfin(configFile.get<double>("tfin"));
	double dt(configFile.get<double>("dt"));
	int sampling(configFile.get<int>("sampling"));
	string fichierName(configFile.get<string>("output"));

	
    
    
    
	//INITIALISATION DU SYSTEME
	vector<Planete*> systeme;

	double X,Y,Z;
	double G=100;
	double pi =3.1415;
	double rho = nbEtoile*1/(4*pi*rayon*rayon*rayon*epaisseur/3);
	V = V * sqrt(4*G*pi*rho/3);
	cout << "v0= " << V <<endl;
	for(int i(0); i < nbEtoile; i++){
			double r = rayon * random(0,1);
			double phi =  random(0,2*3.1415);
			double theta =  random(0,3.1415);
			X = r*cos(phi)*sin(theta);
			Y= r*sin(phi)*sin(theta);
			Z = epaisseur * r* cos(theta);
			
			
			systeme.push_back(new Planete( valarray<double>{X,Y,Z}, valarray<double>{-V*r*sin(phi),V*r*cos(phi),0.},1,dt ));
	}
	//systeme.push_back(new Planete( valarray<double>{0,0,0}, valarray<double>{0,0,0},100000,dt ));
	//nbEtoile++;
		
	Noeud* Eve = new Noeud(systeme, -1.01*rayon,1.01*rayon,-1.01*rayon,1.01*rayon,-1.01*epaisseur,1.01*epaisseur);

	
	
	
	
	
	mkdir(fichierName.c_str());





		int startIndex,stopIndex;
		for(int i(0); i<nombreCoeur; i++){
				if(i<(nbEtoile%nombreCoeur)){
					startIndex =  i*(nbEtoile/nombreCoeur+1);
					stopIndex = (i+1) *(nbEtoile/nombreCoeur+1);
				}else{
					startIndex = (nbEtoile%nombreCoeur)*(nbEtoile/nombreCoeur+1) + (i - nbEtoile%nombreCoeur) * (nbEtoile/nombreCoeur) ;
					stopIndex = startIndex+nbEtoile/nombreCoeur;
				}
				cout << startIndex << " -> " << stopIndex << endl;
			}	




	
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	
	std::cout <<"Initialisation Finie, t = "  << duration.count() << " ms"<< std::endl;
	start = high_resolution_clock::now();
	
	
	//ECRITURE INITIALE
	writeCoord(systeme, fichierName, 0);
	
	//DEBUT DE LA BOUCLE
	int pourcent(0),actual(0), nbIter(0);
	for(double t(0); t < tfin; t+=dt){
		pourcent = trunc(100 * t/tfin);
		if(pourcent>actual){
				actual = pourcent;
				cout << pourcent << "%" << endl;
		}
		nbIter++;
		
		
		
		
		
		// CALCULER L'ACCELERATION
		vector<future<void>> taches;

		int startIndex,stopIndex;
		for(int i(0); i<nombreCoeur; i++){
				if(i<(nbEtoile%nombreCoeur)){
					startIndex =  i*(nbEtoile/nombreCoeur+1);
					stopIndex = (i+1) *(nbEtoile/nombreCoeur+1);
				}else{
					startIndex = (nbEtoile%nombreCoeur)*(nbEtoile/nombreCoeur+1) + (i - nbEtoile%nombreCoeur) * (nbEtoile/nombreCoeur) ;
					stopIndex = startIndex+nbEtoile/nombreCoeur;
				}				
				taches.push_back(async(std::launch::async,calcAcc,startIndex,stopIndex, systeme, Eve));
			}	
		
		for(auto & tache : taches){
			tache.get();
		}
		
		
		
		
		
		
		
		
		
		
		
		

		
		// CALCULER LA PROCHAINE STEP
		vector<future<void>> taches2;
		for(int i(0); i<nombreCoeur; i++){
				if(i<(nbEtoile%nombreCoeur)){
					startIndex =  i*(nbEtoile/nombreCoeur+1);
					stopIndex = (i+1) *(nbEtoile/nombreCoeur+1);
				}else{
					startIndex = (nbEtoile%nombreCoeur)*(nbEtoile/nombreCoeur+1) + (i - nbEtoile%nombreCoeur) * (nbEtoile/nombreCoeur) ;
					stopIndex = startIndex+nbEtoile/nombreCoeur;
				}
				taches2.push_back(async(std::launch::async,newStep,startIndex,stopIndex, systeme, dt));
			}	
		
		for(auto & tache : taches2){
			tache.get();
		}
		
		
		//ECRITURE DONNEES
		if(nbIter%sampling == 0)
		writeCoord(systeme, fichierName, nbIter/sampling);

		
		
		//CALCUL NOUVEAU GRAPH
		double maxX=0.,maxY=0.,maxZ=0.,minX=0.,minY=0.,minZ=0;
		for(int i(0); i<systeme.size(); i++){
			if(systeme[i]->X[0] > maxX)maxX = systeme[i]->X[0];
			if(systeme[i]->X[0] < minX)minX = systeme[i]->X[0];
			if(systeme[i]->X[1] > maxY)maxY = systeme[i]->X[1];
			if(systeme[i]->X[1] < minY)minY = systeme[i]->X[1];
			if(systeme[i]->X[2] > maxZ)maxZ = systeme[i]->X[2];
			if(systeme[i]->X[2] < minZ)minZ = systeme[i]->X[2];
		}
		delete Eve;
		Eve = new Noeud(systeme,1.01*minX,1.01*maxX,1.01*minY,1.01*maxY,1.01*minZ,1.01*maxZ);
		
		

		
		
	}
	
	delete Eve;
	stop = high_resolution_clock::now();
	duration = duration_cast<milliseconds>(stop - start);
	std::cout << "Simulation Finie, t = " << duration.count() << " ms"<< std::endl;
	
  return 0;
}
