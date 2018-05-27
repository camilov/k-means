#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <utility>
#include <string.h>
#include <vector>
#include <tuple>
#include <math.h>
#include <cstdlib>
//#include "timer.hh"
#include <zmqpp/zmqpp.hpp>
#include <thread>

using namespace std;
using namespace zmqpp;

bool evalua(vector<double>&kresultados, vector<double>&pendientes ){

	
	double pendiente =0.0;
	double punto1    =0.0;
	double punto2    =0.0;



	return true;
}


int primero=1,ultimo=600,mitad=ultimo/2;
vector<int> elegidos;
vector<int> enviados;
vector<double> kresultados(ultimo+1,0);
vector<double> pendientes;
vector<double> valor;
vector<double> posicion;
pair<double,int> pareja;
int nuevoRangop;
int nuevoRangof;
double mayor=0;

//Hallando el K-Óptimo
void Optimo(){
	
  	context ctx;
	socket servidor(ctx,socket_type::rep);
	const string serverconexion = "tcp://*:5001";
	servidor.bind(serverconexion);

	elegidos.push_back(primero);
	elegidos.push_back(mitad);
	elegidos.push_back(ultimo);

	int i=0;
	int z=0;
	int op=1;
	int divi =8; 
	int banderafinal=0;
	bool seguir;
	int contar=0;
	double punto1 = 0.0;
	double punto2 = 0.0;
	double pendiente =0.0;
	double pendiendMayor = 0.0;
	
	while(true){

		string inicio;
		zmqpp::message saludo;
		servidor.receive(saludo);
		saludo >> inicio;
		cout << " cliente: " << inicio <<endl;
		string resultado, k;
				
		if(inicio == "pide"){
			if(i<elegidos.size()){
				int k=elegidos[i];
				string ki= to_string(k);
				zmqpp::message enviok;
				enviados.push_back(elegidos[i]);
				enviok << ki;
				servidor.send(enviok);
				i++;
			}
		}
		else{
			
			for(int t=0;t< inicio.size();t++){
    			if(inicio[t]=='-'){

    				k+= inicio.substr(t+1,inicio.size());
    				resultado+=  inicio.substr(0,t);
    				break;
    			}
    		}

    		pareja.first=atof(resultado.c_str());
    		pareja.second=atof(k.c_str());
			cout<<"resultado: "<<pareja.first<<endl;
			cout<<"k: "<<pareja.second<<endl;

			kresultados[pareja.second]=pareja.first;

			for(int i =0 ;i<kresultados.size();i++){
				if(kresultados[i] != 0){
					contar += 1;
					break;
				}

			}

			if(contar >=3){

				//bool a = evaluar(kresultados,pendientes);
				for(int i = 1; i<kresultados.size();i++){
					if(kresultados[i] != 0){
						pendiente += ((i-(i-1))/(kresultados[i]-kresultados[i-1]));
				    	pendientes.push_back(pendiente);
					}
				}


				for(int i = 0;i<pendientes.size();i++){
					if(mayor<pendientes[i]){				
						pendiendMayor = pendientes[i];
					}
				}
			}
			
			
		}	
	}
		
}


int main()
{	
	//srand(time(NULL)); 
	
    
    //Timer tTotal;    
    Optimo();
    //cout << "Tiempo: " << tTotal.elapsed() << endl;
}