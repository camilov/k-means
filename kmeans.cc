// g++ -std=c++11 -fopenmp -o kmeans kmeans.cc
// ./kmeans iris.data
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <unordered_map>
#include <vector>

using namespace std;

using Point = vector<double>;
ostream& operator<<(ostream& os, const Point& p) {
  os << "<";
  for (size_t i = 0; i < p.size(); i++) {
    os << " " << p[i];
  }
  os << ">";
}

/**
 * Reads the Iris dataset
 */
vector<Point> readIris(const string& fname) {
  vector<Point> ds;
  ifstream input(fname);
  double sl;
  double sw;
  double pl;
  double pw;
  string label;
  while (input >> sl >> sw >> pl >> pw >> label) {
    Point p{sl, sw, pl, pw};
    // cout << p << endl;
    ds.push_back(move(p));
  }
  cout << "Input has " << ds.size() << " points." << endl;
  return ds;
}

void printClustering(const vector<Point>& dataset,
                     const vector<size_t>& clustering, size_t k) {
/*
  for (size_t i = 0; i < dataset.size(); i++) {
    cout << dataset[i] << " -> " << clustering[i] << endl;
  }
  */
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

vector<Point> randomPoints(size_t k, const vector<Point>& ds) {
  random_device rd;
  mt19937 generator(rd());
  uniform_int_distribution<> unif(0, ds.size()-1);
  size_t dim = ds[0].size();
  vector<Point> c(k, Point(dim, 0.0));
  for (size_t i = 0; i < k; i++) {
    size_t r = unif(rd);
    //cout << "random point " << r << endl;
    c[i] = ds[r];
  }
  return c;
}


double sqdistance(const Point& p, const Point& q) {
  // cout << p << endl;
  // cout << q << endl;

  double d = 0.0;
  for (size_t i = 0; i < p.size(); i++) {
    d += pow(p[i] - q[i], 2);
  }
  return d;
}

tuple<size_t, double> closestCentroid(const Point& p,
                                      const vector<Point>& centroids) {
  double d = numeric_limits<double>::max();
  size_t c = 0;

  for (size_t i = 0; i < centroids.size(); i++) {
    double dt = sqdistance(p, centroids[i]);
    if (dt < d) {
      d = dt;
      c = i;
    }
  }

  return make_tuple(c,d);
}

pair<vector<size_t>, double> cluster(const vector<Point>& dataset,
                                     const vector<Point>& centroids) {
  size_t n = dataset.size();
  vector<size_t> clustering(n, 0);
  double ssd = 0.0;
  for (size_t i = 0; i < n; i++) {
    size_t c;
    double d;
    tie(c, d) = closestCentroid(dataset[i], centroids);
    clustering[i] = c;
    ssd += d;
  }
  return {clustering, ssd};
}

vector<Point> newCentroids(const vector<size_t>& clustering,
                           const vector<Point>& dataset,
                           vector<Point>& centroids) {
  size_t k = centroids.size();
  size_t dim = centroids[0].size();
  vector<Point> newCentroids(k, Point(dim, 0.0));
  vector<double> count(k, 0.0);
  for (size_t i = 0; i < dataset.size(); i++) {
    size_t ci = clustering[i];
    for (size_t j = 0; j < dim; j++) {
      newCentroids[ci][j] += dataset[i][j];
    }
    count[ci]++;
  }

  for (size_t i = 0; i < k; i++) {
    for (size_t j = 0; j < dim; j++) {
      newCentroids[i][j] /= count[i];
    }
  }
  return newCentroids;
}

vector<size_t> kmeans(const vector<Point>& dataset, size_t k, double epsilon,
                      size_t maxIter) {

  size_t dim = dataset[0].size();
  size_t n = dataset.size();

  vector<Point> centroids = randomPoints(k, dataset);
  vector<size_t> clustering(n, 0);
  double ssd = 0.0;
  double ssdPrev = 0.0;
  double d;
  size_t iter = 0;

  //while (iter < maxIter) {
  do {
    ssdPrev = ssd;
    cout << "Iteration " << iter << endl;
    tie(clustering, ssd) = cluster(dataset, centroids);
    cout << "SSD: " << ssd << endl;
    //printClustering(dataset, clustering);
    centroids = newCentroids(clustering, dataset, centroids);
    iter++;
    d = abs(ssdPrev - ssd);
    cout << "----> " << d << endl;
  } while(d > epsilon);

  return clustering;
}

int main(int argc, char** argv) {
  if (argc != 2)
    return -1;
  string fname(argv[1]);
  vector<Point> ds = readIris(fname);
  vector<size_t> clustering(150,0);
  clustering = kmeans(ds, 3, 0.001, 20);
  printClustering(ds, clustering, 3);
  return 0;
}
