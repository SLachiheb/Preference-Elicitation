#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <ilcplex/ilocplex.h>
#include "CSS.h"


CSS::CSS() {
}

bool CSS::comparer(pair<int, double> p1, pair<int, double> p2){
	return p1.second > p2.second;
}

double CSS::pmr(vector<double> x, vector<double> y, vector<vector<double>> prefDecideur){
	int nbCriteres = x.size();

  ILOSTLBEGIN

  IloEnv env;
  IloModel mod ( env );
  IloNumVarArray w_(env);

// Création des varioables de décisions :
  for (int i(0); i < nbCriteres; i++)
  	w_.add(IloNumVar(env, 0.0, 1.0 , ILOFLOAT));//, myname.str().c_str()));


// Contraintes sur les poids du décideurs qui somments à 1 :
  IloExpr contraint(env);
  for(int i(0); i < nbCriteres; i++){
    contraint += w_[i];
  }
    mod.add ( contraint == 1 );


// Contraintes sur les poids du décideurs par rapporte à ces préférences :
  if (prefDecideur.size() > 0) {
    for(int i(0); i < prefDecideur.size(); i++){
      IloExpr valA(env);
      IloExpr valB(env);
      for(int j(0); j < nbCriteres; j++){
        valA += w_[j] * prefDecideur[i][j];
        valB += w_[j] * prefDecideur[i][j + nbCriteres];
      }
      mod.add (valA >= valB);
    }
  }


  // Fonction objectif à maximiser :
  IloExpr expr(env);
  for(int i(0); i < x.size(); i++){
    expr += (w_[i] * y[i] - w_[i] * x[i]);
  }

  mod.add(IloMaximize(env, expr));

  IloCplex cplex(mod);
  cplex.setOut(env.getNullStream());
  cplex.solve();

  double objval = cplex.getObjValue();
  env.end();
  return objval;
}


vector<double> CSS::mr(vector<double> x, vector<vector<double>> alternatives, vector<vector<double>> prefDecideur) {
  vector<vector<double>> mr_y;
	int nbCriteres = x.size();

  for(int i(0); i < alternatives.size(); i++){
    vector<double> tmp(nbCriteres + 1, 0);
    tmp[0] = pmr(x, alternatives[i], prefDecideur);
    for (int j(1); j < nbCriteres + 1; j++)
      tmp[j] = alternatives[i][j-1];

    mr_y.push_back(tmp);
    if (tmp[0] > this->MR) {
      this->MR = tmp[0];
      if(this->MR > this->MMR)
        break;
    }
  }
  int max(0);
  for (int i(0); i < mr_y.size(); i++)
   if( mr_y[i][0] > mr_y[max][0] )
     max = i;

  return mr_y[max];
}


vector<double> CSS::mmr(vector<vector<double>> alternatives, vector<vector<double>> prefDecideur){
  vector<vector<double>> mmr_x_y(alternatives.size());
	int nbCriteres = alternatives[0].size();
  vector<double> tmp(nbCriteres + 1);

  double infMax = std::numeric_limits<double>::max();
  this->MMR = infMax;
  for(int i(0); i < alternatives.size(); i++){
    double infMin = std::numeric_limits<double>::min();
    this->MR = infMin;
    tmp = mr(alternatives[i], alternatives, prefDecideur);
    if (this->MR < this->MMR)
      this->MMR = this->MR;

    mmr_x_y[i] = vector<double>(nbCriteres * 2 + 1);
    mmr_x_y[i][0] = tmp[0];

    for (int j(1); j < nbCriteres + 1; j++)
      mmr_x_y[i][j] = alternatives[i][j-1];

    for (int j(nbCriteres + 1); j < nbCriteres * 2 + 1; j++)
      mmr_x_y[i][j] = tmp[j - nbCriteres];

  }

  int min(0);
  for (int i(0); i < mmr_x_y.size(); i++)
   if(mmr_x_y[i][0] < mmr_x_y[min][0])
     min = i;
  return mmr_x_y[min];
}



vector<double> CSS::css_sigma(vector<vector<double>> alternatives, vector<double> poidsDecideur) {
	int nbCriteres = poidsDecideur.size();
  double mmrVal;
  int nb_question = 0;
  vector <double> a(nbCriteres);
  vector <double> b(nbCriteres);
	double seuil = 1e-12;

	vector<double> mmr_a_b = mmr(alternatives, this->prefDecideurConnu);
	mmrVal = mmr_a_b[0];


	if (mmrVal <= seuil){
		for (int i(1); i < nbCriteres + 1; i++ )
			a[i - 1] = mmr_a_b[i];

		for (int i(nbCriteres + 1); i < nbCriteres * 2 + 1; i++ )
			b[i - nbCriteres - 1] = mmr_a_b[i];
		this->prefDecideurConnu.push_back(choix_decideur(a, b, poidsDecideur));
	}

	while (mmrVal > seuil){
		for (int i(1); i < nbCriteres + 1; i++ )
      a[i - 1] = mmr_a_b[i];

    for (int i(nbCriteres + 1); i < nbCriteres * 2 + 1; i++ )
      b[i - nbCriteres - 1] = mmr_a_b[i];

		this->prefDecideurConnu.push_back(choix_decideur(a, b, poidsDecideur));
		nb_question++;
		mmr_a_b = mmr(alternatives, this->prefDecideurConnu);
		mmrVal = mmr_a_b[0];
		if (mmrVal < 1e-12){
			for (int i(1); i < nbCriteres + 1; i++ )
				a[i - 1] = mmr_a_b[i];

			for (int i(nbCriteres + 1); i < nbCriteres * 2 + 1; i++ )
				b[i - nbCriteres - 1] = mmr_a_b[i];
			this->prefDecideurConnu.push_back(choix_decideur(a, b, poidsDecideur));
			mmrVal = 0;
			nb_question--;
		}
	}

	for(int i(0); i < a.size(); i++)
		a[i] = this->prefDecideurConnu[this->prefDecideurConnu.size()-1][i];

  int indiceOpt(-1);
  for (int i(0); i < alternatives.size(); i++){
		if (alternatives[i] == a){
			indiceOpt = i;
			break;
		}
  }

  return vector<double>{(double)nb_question, indiceOpt};
}



vector<double> CSS::choix_decideur(vector<double> a, vector<double> b, vector<double> poidsDecideur){
  float valA(0);
  float valB(0);
  vector<double> pref(a.size() * 2);

  for(int i(0); i < a.size(); i++){
      valA += poidsDecideur[i] * a[i];
      valB += poidsDecideur[i] * b[i];
  }

  if (valA >= valB)
    for(int i(0); i < a.size(); i++){
      pref[i] = a[i];
      pref[i + a.size()] = b[i];
    }

  else
    for(int i(0); i < a.size(); i++){
      pref[i] = b[i];
      pref[i + b.size()] = a[i];
    }

  return pref;
}

double CSS::getMMR(){
  return this->MMR;
}

void CSS::addPrefDecideurConnu(vector<double> pref){
  this->prefDecideurConnu.push_back(pref);
}

vector<vector<double>> CSS::getPrefDecideurConnu() {
    return this->prefDecideurConnu;
}
