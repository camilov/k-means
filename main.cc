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

/*bool evaluar(vector <int> &elegidos,vector <int> &enviados,vector <double> &kresultados,
	int &primero,int &mitad,int &ultimo,int divi){
	double Var1= atan((kresultados[primero]-kresultados[mitad])/(mitad-primero)) * 180 / M_PI;
	double Var2= atan((kresultados[mitad]-kresultados[ultimo])/(ultimo-mitad)) * 180 / M_PI;
	double valor1;
	double valor2;
	int ult=ultimo;
	int pri=primero;
	int aux;
	if(Var1 > 45)
		valor1=Var1-45;
	else
		valor1=45-Var1;
	if(Var2 > 45)
		valor2=Var2-45;
	else
		valor2=45-Var2;
	if(valor1 <= valor2){
		ultimo=mitad;
		mitad=ultimo/2;
	}
	else{
		primero=mitad;
		mitad=ultimo-(mitad/2);
	}
	elegidos.clear();
	enviados.clear();
	aux=(ult-pri)/divi;
	int ban=aux;
	if (aux <= 1)
		return true;
	cout << "ultimo: "<<ultimo<<" ----- "<< "Mitad: " <<mitad <<"------"<< " aux: "<< aux <<endl;
	for(int i=0;primero + aux<ultimo;i++){
		//cout<< aux <<"  aux "<< endl;
		elegidos.push_back(aux);
		aux=aux+ban;

	}
	return false;
}

void Repartir(vector <int> &elegidos,vector <int> &enviados, int ultimo, int divi){
	int aux=ultimo/divi;
	int ban=aux;
	elegidos.clear();
	enviados.clear();
	for(int i=0;aux<ultimo;i++){
		//cout<< aux <<"  aux "<< endl;
		elegidos.push_back(aux);
		aux=aux+ban;
	}
}

bool verificar(vector <double> &kdos,int &ultimo){

	if(kdos[ultimo] != '\0'){
			return true;
		}
	
	return false;
}

bool verificar2(vector<int> &enviados,int &ultimo){

	for(int i=0;i< enviados.size();i++){
		if(enviados[i] == ultimo){
			return true;
		}
	}
	return false;
}*/
pair<int,int> evaluar(vector<double>&kresultados,vector<double>&pendientes,vector<double>&valor,vector<double>&posicion,
					double mayor, int nuevoRangop,int nuevoRangof){
    
    double pendiente =0;
    string txt = "pendientes.txt";
    ofstream fs(txt.c_str());
    fs << " " << endl;
    fs.close();
    

    nuevoRangop =0;
    nuevoRangof =0;

	for(int i =0; i<kresultados.size();i++){
		if(kresultados[i] != 0){
			posicion[i] = i;
		    valor[i] = kresultados[i]; 
		}
	}

	for(int z = 0;z<posicion.size();z++){
		for(int y =0;y<valor.size();y++){
			pendiente += ((posicion[z] - posicion[z+1])/(valor[y]-valor[y+1]));
			pendientes.push_back(pendiente);
			ofstream fs(txt.c_str(), ios::out);
        	fs << pendiente << endl;
			fs.close();			 
		}
	}

	for(int k =0;k<pendientes.size();k++){
		mayor+= pendientes[0];
		if(mayor<pendientes[k]){
			mayor+= pendientes[k]; 
		}
	}


	return {nuevoRangop,nuevoRangof};
}

void mostrarVectores(vector <double> &kresultados,vector<int> &enviados,vector<int> &elegidos){
	cout << "enviados"<<endl;
	for (int o=0;o < enviados.size();o++){
		if(enviados[o] != '\0' )
			cout << enviados[o] <<"-";
	}
	cout<<endl;
	cout << "elegidos"<<endl;
	for (int o=0;o < elegidos.size();o++){
		if(elegidos[o] != '\0' )
			cout << elegidos[o] <<"-";
	}
	cout<<endl;
	cout << "kresultados"<<endl;
	for (int o=0;o < kresultados.size();o++){
		if(kresultados[o] != '\0' )
			cout << o <<"-";
	}
	cout << endl;
}


int primero=1,ultimo=133,mitad=ultimo/2;
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
	
	
	while(true){

		//mostrarVectores(kresultados,enviados,elegidos);
		string inicio;
		zmqpp::message saludo;
		servidor.receive(saludo);
		saludo >> inicio;
		string resultado, k;
				
		if(inicio == "pide"){
			if(i<elegidos.size()){
				int k=elegidos[i];
				string ki= to_string(k);
				zmqpp::message enviok;
				enviados.push_back(elegidos[i]);
				elegidos.push_back(elegidos[i]);
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
			
			for(int i =0;i<kresultados.size();i++){
				if(kresultados[i]>=0){
					z+= 1;
					if(z>=10){
					  tie(nuevoRangop,nuevoRangof)= evaluar(kresultados,pendientes,valor,posicion,mayor, nuevoRangop,nuevoRangof);
					}else{
						for(int i=0; i<enviados.size();i++){
							for(int j=0;j<elegidos.size();j++){
								if(enviados[i]==elegidos[j]){
									if(i<elegidos.size()){
										int k=elegidos[i+1];
								    	string ki= to_string(k);
										zmqpp::message enviok;
										enviados.push_back(elegidos[i]);
										enviok << ki;
										servidor.send(enviok);
										i++;
									}	
								}
							}
								
							
						}
					}

				}

			}
}}}


int main()
{	
	//srand(time(NULL)); 
	
    
    //Timer tTotal;    
    Optimo();
    //mostrarVectores(kresultados,enviados,elegidos);
    //cout << "Tiempo: " << tTotal.elapsed() << endl;
}