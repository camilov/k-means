//g++ -std=c++11 -fopenmp -o netflix  netflix.cc
// ./netflix combined_data_1.txt
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <random>
#include <tuple>


using namespace std;
using SPoint = vector<pair<size_t, double>>;
using Rates = map<pair<string, string>, double>; // (user,movie) -> rate
using rate = vector<double>;


Rates readNetflix(const string& fname) {
  ifstream input(fname);
  string line;
  size_t lines = 0;
  Rates rates;

  string currMovie;

  while (getline(input, line)) {
    if (line.back() == ':') {
        line.pop_back();
        currMovie = line;
        cout << "Movie: " << currMovie << endl;
    } else {
        size_t endUser = line.find_first_of(",");
        string currUser = line.substr(0, endUser);
        line.erase(0, endUser + 1);
        size_t endRate = line.find_first_of(",");
        string currRate = line.substr(0, endRate);
        rates[{currUser, currMovie}] = stoi(currRate);
      }
        lines++;
    }
      return rates;
}

vector<SPoint> createPoints(const Rates& rates) {
  map<string, size_t> normUsers;
  map<string, size_t> normMovies;
  for (const auto& e : rates) {
      const auto& user = e.first.first;
      const auto& movie = e.first.second;

      if (normUsers.count(user) == 0)
          normUsers[user] = normUsers.size();
          if (normMovies.count(movie) == 0)
              normMovies[movie] = normMovies.size();
          }
          cout << "End of normalization " << normUsers.size() << " "
               << normMovies.size() << endl;

      vector<SPoint> users(normUsers.size(),SPoint());
      for (const auto& e : rates) {
          size_t user = normUsers[e.first.first];
          size_t movie = normMovies[e.first.second];
          double rate = e.second;
          pair<size_t,double> p= make_pair(movie, rate);
          users[user].push_back(p);
      }
  return users;
}


void printClustering(const vector<SPoint>& dataset, const vector<size_t>& clustering, size_t k) {
  size_t n = dataset.size();
  vector<size_t> count(k, 0);
  for (size_t i = 0; i < n; i++) {
       size_t ci = clustering[i];
       count[ci]++;
  }

  for(size_t i = 0; i < k; i++) {
      cout << " cluster " << i << ": " << count[i] << endl;
  }
}

vector<rate> randomcentroids(size_t k, size_t num_movies) {
  random_device rd;
  mt19937 generator(rd());
  uniform_int_distribution<> unif(0, 5);
  vector<rate> c(k, rate(num_movies+1, 0.0));

  for (size_t i = 0; i < k; i++) {
      for (size_t j= 1;j <= num_movies;j++){
           size_t r = unif(rd);
           c[i][j] = r;
      }    
  }
          
  for(size_t i=0; i < 3;i++){
     cout<< "fila"<<i<<endl;
     for(size_t j=0;j<=num_movies;j++){
        cout << " "<< c[i][j] << " ";
      }
  }
  return c;
}
        
vector<double> normuser(const vector<SPoint>& dataset){
  vector<double> norma(dataset.size(),0);
  for (size_t i=0; i< dataset.size();i++ ){
      for(size_t j=0;j < dataset[i].size();j++){
          norma[i]+= pow(dataset[i][j].second,2);
      }
  }
  
  for (size_t i=0;i < norma.size();i++){
      norma[i]= sqrt(norma[i]);
  }
  return norma;
}

vector<double> normcentroid(const vector<rate>& centroids){
  vector<double> normacentros(centroids.size(),0);
  for(size_t i=0;i< centroids.size();i++){
      for(size_t j=1;j<=centroids[0].size();j++){
          normacentros[i]+=pow(centroids[i][j],2);
      }
  }
              
  for(size_t i=0; i<normacentros.size();i++){
      normacentros[i]=sqrt(normacentros[i]);
  }
  return normacentros;
}

/*
        double angle( const SPoint& p, const SPoint& q) {
         
          double p_punto=0;
          double norma_A=0;
          double norma_B=0;
          #pragma omp parallel for
          for (size_t i = 0; i < p.size(); i++) {
            p_punto=(p[i].first*q[i].first)+(p[i].second*q[i].second);
            norma_A=(sqrt(pow(p[i].first,2)+pow(p[i].second,2)));
            norma_B=(sqrt(pow(q[i].first,2)+pow(q[i].second,2)));
          }

          double A_Punto_B=norma_A*norma_B;
          double division=0;
            if (A_Punto_B != 0) {
               division=p_punto/A_Punto_B;
             }

          double arc_cos=acos(division);
          double rad_grados=(arc_cos*180)/3.1415926535;

          return rad_grados;
        }
*/

double Coseno_similarity(const SPoint& user, const rate& rates_centroid_actual,const double normauser,const  double normacentroids) {

  double d = 0.0;
          
  for (size_t i = 0; i < user.size(); i++) {
       size_t indice_pelicula_actual = user[i].first;
       d += user[i].second * rates_centroid_actual[indice_pelicula_actual]; 
  }
       d= acos(d / (normauser*normacentroids));
   return d;
  }

          
    
size_t closestCentroid(const SPoint& user, const vector<rate>& centroids,const double normauser,const  vector<double>& normacentroids) 
      {
  double d = numeric_limits<double>::max();
  size_t c = 0;      
                
  for (size_t i = 0; i < centroids.size(); i++) {
      double dt = Coseno_similarity(user, centroids[i],normauser, normacentroids[i]);
      if (dt < d) {
          d = dt;
          c = i;
      }
  }   
  return c;
}

vector<size_t> cluster(const vector<SPoint>& dataset,const vector<rate>& centroids,const vector<double>& normauser,const vector<double>& normacentroids) {

  size_t n = dataset.size(); 
  vector<size_t> clustering(n, 0);
       
  for (size_t i = 0; i < n; i++) {
       size_t c;
       c = closestCentroid(dataset[i], centroids,normauser[i],normacentroids);
       clustering[i] = c;
  }
  return clustering;
}

vector<rate> newCentroids(const vector<size_t>& clustering, const vector<SPoint>& dataset, vector<rate>& centroids){

  size_t k = centroids.size();
  size_t dim = centroids[0].size();
  vector<rate> newCentroids(k, rate(dim+1, 0.0)); 
  vector<double> count(k, 0.0);
  size_t indice_centroide;
  size_t indice_peli;
  for (size_t i = 0; i < dataset.size(); i++) {
      indice_centroide = clustering[i];
      for (size_t j = 0; j < dataset[i].size(); j++) {
          indice_peli = dataset[i][j].first;
          newCentroids[indice_centroide][indice_peli] += dataset[i][j].second;
      }
     count[indice_centroide]++;
  }

  #pragma omp parallel for 
    for (size_t i = 0; i < k; i++) {
         for (size_t j = 0; j <= dim; j++) {
             newCentroids[i][j] /= count[i];
          }
    }
    return newCentroids; 
}

double ssd_centroides(const vector<rate>& centroids,const vector<rate>& new_centroids){
  double ssd= 0.0;
  for (size_t i=0; i < centroids.size();i++){
       for(size_t j=1 ; j <= centroids[i].size();j++ ){
          ssd+=abs(centroids[i][j]-new_centroids[i][j]);
        }
  }
  return ssd;
}

double sacar_ssd(const vector<double>& angleusers,const vector<double>& angleusers_prev){
  double ssd = 0.0;
  for(size_t i=0; i< angleusers.size();i++){
      ssd+= abs(angleusers[i]-angleusers_prev[i]);
  }
  cout << "SSD: " << ssd<< endl;
  return ssd;
}
        

vector<size_t> kmeans(const vector<SPoint>& dataset, size_t k,size_t dim, double epsilon) {
  size_t n = dataset.size();
  vector<rate> centroids = randomcentroids(k, dim);
  vector<double> normauser= normuser(dataset);
  vector<size_t> clustering(n, 0);
  vector<rate> new_centroids;
  double ssd = 0.0;
  double ssdPrev = 0.0;
  double d;
  size_t iter = 0;
  vector<double> normacentroides(k,0);
  do{
     ssdPrev = ssd;
     cout << "Iteration " << iter << endl;
     normacentroides=normcentroid(centroids);
     clustering = cluster(dataset, centroids,normauser,normacentroides);
     printClustering(dataset,clustering,k);
     new_centroids = newCentroids(clustering, dataset, centroids);
     ssd = ssd_centroides(new_centroids,centroids);
     centroids=new_centroids;
     cout << "SSD: " << ssd << endl;
     iter++;
     d = abs(ssdPrev - ssd);
     cout << "----> " << d << endl;

  }while(d>epsilon);

  return clustering;
}

int main(int argc, char** argv) {
  if (argc != 2)
      return -1;
  string fname(argv[1]);
  Rates rates = readNetflix(fname);
  vector<SPoint> ds = createPoints(rates);
  vector<rate> centroids = randomcentroids(5, 12);
  vector<size_t> clustering(ds.size(),0);
  clustering = kmeans(ds,5,12,0.1);
  return 0;
}
