#ifndef CSS_H
#define CSS_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

class CSS {

  private :
  float MR;
  float MMR;
  vector<vector<double>> prefDecideurConnu;
  vector<vector<double>> prefDecideurConnu2;

  public :
  CSS();
  static bool comparer(pair<int, double> , pair<int, double> );

  double pmr (vector<double>, vector<double>, vector<vector<double>>);
  vector<double> mr (vector<double>, vector<vector<double>>, vector<vector<double>>);
  vector<double> mmr(vector<vector<double>>, vector<vector<double>>);

  vector<double> css_sigma(vector<vector<double>>, vector<double>);
  vector<double> choix_decideur(vector<double>, vector<double>, vector<double>);
  double getMMR();

  void addPrefDecideurConnu(vector<double> pref);

  vector<vector<double>> getPrefDecideurConnu();


};
#endif
