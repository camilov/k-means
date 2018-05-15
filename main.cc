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


void Optimo(){

  	context ctx;
	socket servidor(ctx,socket_type::rep);
	const string serverconexion = "tcp://*:5001";
	servidor.bind(serverconexion);


	int i=0;
	int op=1;
	int banMensaje=0;
	int divi =8; 
	int banderafinal=0;
	bool seguir;
	printf("iniciado");
	while(true){

		//mostrarVectores(kresultados,enviados,elegidos);
		string inicio;
		zmqpp::message saludo;
		servidor.receive(saludo);
		saludo >> inicio;
		string resultado, k;
				
		if(inicio == "oe"){
			
			int ksito= 4;
			string puntoya= to_string(ksito);
			zmqpp::message enviok;
			enviok << puntoya;
			servidor.send(enviok);
		}
	}
}


int main()
{	
	//srand(time(NULL)); 
	
    
    //Timer tTotal;    
    Optimo();
    //mostrarVectores(kresultados,enviados,elegidos);
    //cout << "Tiempo: " << tTotal.elapsed() << endl;
    return 0;
}