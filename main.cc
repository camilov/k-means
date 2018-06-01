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

bool evaluar(vector <int> &elegidos,vector <int> &enviados,vector <double> &kresultados,
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
}



int primero=1,ultimo=10,mitad=ultimo/2;
vector<int> elegidos;
vector<int> enviados;
vector<double> kresultados(ultimo+1,0);
vector<double> pendientes;
vector<double> valor;
vector<double> posicion;
pair<double,int> pareja;
int contar=0;



void kOptimo(){
	
  	context ctx;
	socket servidor(ctx,socket_type::rep);
	const string serverconexion = "tcp://*:5001";
	servidor.bind(serverconexion);

	elegidos.push_back(primero);
	elegidos.push_back(mitad);
	elegidos.push_back(ultimo);
 
	int i=0;
	double pendiente1 =0.0;
	double pendiente2 =0.0;
	int op=1;
	int banMensaje=0;
	int divi =2; 
	int banderafinal=0;
	bool seguir;

	while(true){
		string iniciando = "Esperando solicitud....";
		cout << " " <<iniciando <<endl;
		

		string inicio;
		zmqpp::message saludo;
		servidor.receive(saludo);
		saludo >> inicio;
		cout << "Recibiendo: " << inicio <<endl;
		string resultado, k;
				
		if(inicio == "k"){
			if(i<elegidos.size()){
				int k=elegidos[i];
				string ki= to_string(k);
				zmqpp::message enviok;
				enviados.push_back(elegidos[i]);
				enviok << ki;
				cout <<"Enviando: " << k<<endl;
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

			cout <<" k: " << kresultados[i] <<endl;
			
			if(kresultados[ultimo] != '\0'){
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
				//if (aux <= 1)
				//	{return true;}	
				cout << "ultimo: "<<ultimo<<" ----- "<< "Mitad: " <<mitad <<"------"<< " aux: "<< aux <<endl;
				for(int i=0;primero + aux<ultimo;i++){
					//cout<< aux <<"  aux "<< endl;
					elegidos.push_back(aux);
					aux=aux+ban;

				}
			}else{
			  string  enviar= "terminado";
			  zmqpp::message enviando;
			  enviando << enviar;
			  servidor.send(enviando);
			  cout << "Enviando : " << enviar <<endl;
			}
			  
		}

			
	}		
}


int main()
{	
	//srand(time(NULL)); 
	
    
    //Timer tTotal;    
    kOptimo();
    //cout << "Tiempo: " << tTotal.elapsed() << endl;
}