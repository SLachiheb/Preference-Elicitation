#include "CSS.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Sac {
    vector<int>     listeObjets;
    vector<double>  utilites;
};


bool comparer(pair<int, double> p1, pair<int, double> p2){
  return p1.second > p2.second;
}

vector<double> valeurSac(vector<int> sac, vector<vector<double>> mat){
  vector<double> val(mat[0].size(), 0);
  for(int i(0); i < sac.size(); i++){
    if(sac[i] == 1){
      for(int j(0); j < mat[0].size(); j++)
	     val[j] += mat[i][j];
    }
  }
  return val;
}


int minDist(vector<vector<double>> liste, vector<double> best){
  vector<double> distances(liste.size());
  int res(0);
  double valeurTmp(0);

  for (int i(0); i < liste.size(); i++){
    valeurTmp = 0;
    for(int k(0); k < liste[i].size(); k++){
      valeurTmp += abs((liste[i][k] - best[k]) * (liste[i][k] - best[k]));
    }
    distances[i] = valeurTmp;
  }
  for (int i(0); i < liste.size(); i++)
    if( distances[i] < distances[res] )
      res = i;

  return res;
}


vector<vector<int>> voisinage(vector<int> &tour, vector<vector<int>> &voisins){
  vector<int> tmp(tour.size());
  vector<int> tmp2(tour.size());

  for(int i(0); i < tour.size(); i++){
    if(tour[i] == 1){
      tmp = tour;
      tmp[i] = 0;
      for(int j(0); j < tour.size(); j++){
      	tmp2 = tmp;
      	if(tour[j] == 0){
      	  tmp2[j] = 1;
      	     voisins.push_back(tmp2);
      	}
      }
    }
  }
  return voisins;
}


int paretoDomine(vector<double> &x1, vector<double> &x2){
    int verif[x1.size()];
    int somme(0);
    for(int i(0); i < x1.size(); i++){
        if(abs(x1[i] - x2[i]) < 0.00001)
            verif[i] = 0;
        else if(x1[i] > x2[i])
            verif[i] = 1;
        else
            verif[i] = -1;
    }


    int nbNeg = 0;
    int nbSup = 0;
    for(int i(0); i < x1.size(); i++){
      if (verif[i] == -1 || verif[i] == 0)
        nbNeg++;
      if (verif[i] == 1 || verif[i] == 0)
        nbSup++;
    }

    if (nbSup == x1.size())
        return 1;
    else if (nbNeg == x1.size())
        return -1;
    return 0;
}




vector<Sac> pareto_classique(vector<Sac> &sacs){
  int m = sacs.size();
  for(int i(0); i < m; i++){
    for(int j(i+1); j < m ; j++){
      int v = paretoDomine((sacs[i]).utilites, (sacs[j]).utilites);
      if(v == 1){
        sacs.erase(sacs.begin() + j);
        j--;
        m--;
      }
      if(v == -1){
        sacs.erase(sacs.begin() + i);
        i--;
        m--;
        break;
      }
    }
  }
    return sacs;
}


bool pareto_classique(Sac &s1, Sac &s){
  int v = paretoDomine(s1.utilites, s.utilites);
  if(v == 1)
    return true;
  return false;
}

bool pareto_classique(vector<Sac> &sacs, Sac &s){
  int m = sacs.size();
  for(int i(0); i < m; i++){
      int v = paretoDomine(sacs[i].utilites, s.utilites);
      if(v == 1)
        return true;
  }
  return false;
}

/* Algorithme PLS pour sac à dos: une solution random que l'on developpe avec un voisinage 1-1 jusqu'à
ce qu'il n'y ait plus de solution à developper.
Ils sont écrit dans le fichier ../PF.txt

ARG : La solution initial et la matrice des couts pour tous les objets.
RTN : L'ensemble des solutions non dominées qui ont été selectionnées
*/
vector<Sac> run(vector<int> sacI, vector<vector<double>> mat){

  ofstream ofs;
  string nom_fichier = "../PF.txt";
  ofs.open(nom_fichier, ofstream::out);


  int nbCriteres  = mat[0].size();
  int nbInstances = mat.size();
  bool test;
  int nbIt = 0;
  Sac sacInitial;
  vector<Sac>          resultat;
  vector<Sac>          sacsVoisins;
  vector<Sac>          dev;
  vector<Sac>          dev2;

  vector<vector<int>>  voisins;

  sacInitial.utilites     =  valeurSac(sacI, mat);
  sacInitial.listeObjets  = sacI;
  resultat.push_back(sacInitial);

  do {
    nbIt++;
    voisins.clear();
    sacsVoisins.clear();
    dev2.clear();

    for(int i(0); i < resultat.size(); i++){
      voisins.clear();
      sacsVoisins.clear();

      voisins = voisinage(resultat[i].listeObjets, voisins);

      for(int i(0); i < voisins.size(); i++){
        Sac s;
        s.listeObjets  = voisins[i];
        s.utilites     = valeurSac(voisins[i], mat);
        sacsVoisins.push_back(s);
      }

      sacsVoisins = pareto_classique(sacsVoisins);

      for(int j(0); j < sacsVoisins.size(); j++)
          if( !pareto_classique(resultat[i], sacsVoisins[j]))
            if( !pareto_classique(dev, sacsVoisins[j]) ){
              dev.insert(dev.begin(), sacsVoisins[j]);
              dev2.insert(dev2.begin(), sacsVoisins[j]);
            }
    }
  resultat = pareto_classique(dev2);

  } while( resultat.size() != 0);
  dev = pareto_classique(dev);


  for(int i(0); i < dev.size(); i++){
    for(int j(0); j < dev[i].utilites.size() ; j++){
      ofs << dev[i].utilites[j] << " ";
    }
    ofs <<  endl;
  }

  return dev;
}




/* Algorithme ILS pour sac à dos: une solution random que l'on developpe avec un voisinage 1-1,
on choisit la solution préférée(vai une procédure d'élicitation incrémentale basée sur le regret)
du décideur fictif puis on la developpe avec le voisinage jusqu'à ce qu'à prendre la solution
courante une seconde fois.

ARG : La solution initial, la matrice des couts pour tous les objets, le jeu de poids du
décideur, le nombre de question, le nombre d'itérations.
RTN : La solution de minimax regret à recommander
*/
Sac run(vector<int> sacI, vector<vector<double>> mat, vector<double> poidsDecideur, int *nbQ, int *nbI){
  int nbCriteres  = mat[0].size();
  int nbInstances = mat.size();
  bool test;
  int nbIt = 0;
  CSS strategie;

  Sac sacInitial;
  sacInitial.utilites     =  valeurSac(sacI, mat);
  sacInitial.listeObjets  = sacI;

  Sac          courant;
  Sac          oldCourant;
  vector<Sac>   sacsVoisins;
  vector<vector<int>>  voisins;
  vector<vector<double>> alternatives;
  int nb_question(0);

  courant = sacInitial;
  do {

    nbIt++;
    voisins.clear();
    alternatives.clear();
    sacsVoisins.clear();
    voisins = voisinage(courant.listeObjets, voisins);

    for(int i(0); i < voisins.size(); i++){
      Sac s;
      s.listeObjets  = voisins[i];
      s.utilites     = valeurSac(voisins[i], mat);
      sacsVoisins.push_back(s);
    }
    sacsVoisins.push_back(courant);
    sacsVoisins = pareto_classique(sacsVoisins);

    for(int i(0); i < sacsVoisins.size(); i++)
      alternatives.push_back(sacsVoisins[i].utilites);
    vector<double> res = strategie.css_sigma(alternatives, poidsDecideur);
    nb_question += res[0];
    oldCourant = courant;
    courant = sacsVoisins[(int)res[1]];

  } while( courant.listeObjets != oldCourant.listeObjets);
  *nbI = nbIt;
  *nbQ = nb_question;
  return courant;
}






void split(const string &chaine, char delimiteur, vector<string> &elements){
  stringstream ss(chaine);
  string sousChaine;
  while (getline(ss, sousChaine, delimiteur))
    elements.push_back(sousChaine);
}

vector<string> split(const string &chaine, char delimiteur){
  vector<string> mots;
  split(chaine, delimiteur, mots);
  return mots;
}


/*----------------------------------MAIN------------------------------------------*/


int main(int argc, const char * argv[]) {
  /*=============================== Initialisation des variables ======================================*/

  while (argc != 5) {
      cout << "Entrer: Numéro Méthode    Taille de l'instance     Le nombre de critères    Le nom du fichier PF " <<endl;
      cout << "Exemple : ./executable 1 30 3 ../InstancesSAC/PF_30_3.txt " << endl;
      return 1;
  }
  int indice       = stoi(argv[1]);
  int nbInstances  = stoi(argv[2]);
  int nbCriteres   = stoi(argv[3]);
  string Nfichier  = argv[4];

  int nbMaxObejets = nbInstances/2;
  int k = 1;

  int nbIterations = 20;

  int nbFichier = 1000;
  int nbFichierCritere = 5;

  vector<vector<double>> resultat(nbIterations);
  vector<vector<double>> matriceCout(nbInstances);
  for(int i = 0; i < nbInstances; i++)
    matriceCout[i] = vector<double>(nbCriteres);
  /*=============================== Lectures Fichiers et mise en force dans une matrice de distance pour chaque ======================================*/

  ifstream fichier;
  fichier.open("../InstancesSAC/data.txt");
  if (fichier.is_open()) {
    string ligne;
    for(int i(0); i < nbInstances; i++) {
      getline(fichier, ligne);
      vector<string> mots = split(ligne, ' ');
      for(int j(0); j < nbCriteres; j++)
	       matriceCout[i][j] = stoi(mots[j]);
    }
  }
  else
    cout << "Erreur : Impossible d'ouvrir le fichier " << endl;
  fichier.close();

  vector<vector<double>> pf;
  fichier.open(Nfichier);
  if (fichier.is_open()) {
    string ligne;
    int i = 0;
    while(getline(fichier, ligne)){
      pf.push_back(vector<double>(nbCriteres));
      vector<string> mots = split(ligne, ' ');
      for(int j(0); j < nbCriteres; j++)
	       pf[i][j] = stoi(mots[j]);
      i++;
    }
  }
  else
    cout << "Erreur : Impossible d'ouvrir le fichier 2" << endl;
  fichier.close();



  srand(3);
  vector<vector<double>> listePoidsDecideur(nbIterations);
  double somme(0);
  vector<double>poidsDecideur(nbCriteres);
  for(int j(0); j < nbIterations; j++){
    listePoidsDecideur[j] = vector<double>(nbCriteres);
    somme = 0;
    for(int i(0); i < nbCriteres; i++){
      listePoidsDecideur[j][i] = ((float)rand() / (float)RAND_MAX);
      somme += listePoidsDecideur[j][i];
    }
    for(int i(0); i < nbCriteres; i++)
      listePoidsDecideur[j][i] = listePoidsDecideur[j][i]/somme;

    }


  /*=============================== Boucle ======================================*/

  vector<double> tempsPLS(nbIterations,0);
  vector<double> pourcentagePLS(nbIterations,0);
  vector<double> tempsCSS(nbIterations,0);
  vector<double> questionsCSS(nbIterations,0);
  vector<double> qualiteCSS(nbIterations,0);
  vector<double> it(nbIterations,0);



  for(int i(0); i < nbIterations; i++){
    cout << "---------------  " << i + 1 << endl;
    poidsDecideur = listePoidsDecideur[i];

    if(indice == 1 ){
      vector<int> tab(nbInstances);
      vector<vector<int>> sacInitiaux;
      vector<int> sacInitial(nbInstances, 0);
      int indice(-1);
      for(int j(0); j < k; j++){
        tab.clear();
        tab.resize(nbInstances);
        for(int i(0); i < nbInstances; i++)
  	       tab[i] = i;

        sacInitial.clear();
        sacInitial.resize(nbInstances, 0);
        indice = -1;
        for(int i(0); i < nbMaxObejets; i++){
        	indice = (int)rand()%(tab.size());
        	sacInitial[tab[indice]] = 1;
        	tab.erase(tab.begin() + indice);
        }
      }
      resultat[i] = vector<double>(3);
      high_resolution_clock::time_point  start1 = high_resolution_clock::now();
    //  vector<Sac> paretoH;
      vector<Sac> paretoH = run(sacInitial, matriceCout);
      high_resolution_clock::time_point finish1 = high_resolution_clock::now();
      duration<double> e1 =(finish1 - start1);
      tempsPLS[i] = e1.count();

      double nb = 0;
      for(int i(0); i < pf.size(); i++){
        for(int j(0); j < paretoH.size(); j++){
          if(pf[i] == paretoH[j].utilites){
            nb++;
            break;
          }
        }
      }
      pourcentagePLS[i] = ((double)nb / pf.size()) * 100;


    vector<vector<double>> alternatives(nbInstances);
    for(int i = 0; i < nbInstances; i++)
      alternatives[i] = vector<double>(nbCriteres);

    ifstream fichier;
    //fichier.open("../PF.txt");
    fichier.open(Nfichier);
    if (fichier.is_open()) {
      string ligne;
      int i = 0;
      while(getline(fichier, ligne)){
        alternatives.push_back(vector<double>(nbCriteres));
        vector<string> mots = split(ligne, ' ');
        for(int j(0); j < nbCriteres; j++)
  	       alternatives[i][j] = stoi(mots[j]);
        i++;
      }
    }
    else
      cout << "Erreur : Impossible d'ouvrir le fichier 2" << endl;
    fichier.close();
    CSS strategie;

    if(alternatives.size() > 100){
      while(alternatives.size() > 100)
        alternatives.erase(alternatives.begin() + ((int)rand()%(alternatives.size())) );
    }


    high_resolution_clock::time_point  start2 = high_resolution_clock::now();
//vector<double> res(nbCriteres+1, 0);
    vector<double> res = strategie.css_sigma(alternatives, poidsDecideur);
    high_resolution_clock::time_point finish2 = high_resolution_clock::now();
    duration<double> e2 =(finish2 - start2);
    questionsCSS[i] = res[0];
    tempsCSS[i] = e2.count();

    double valUs(0);
    for(int j(0); j < alternatives[res[1]].size(); j++)
      valUs += alternatives[(int)res[1]][j] * poidsDecideur[j];

    double opt(-1);
    double val(0);
    for(int i(0); i < pf.size(); i++){
      val = 0;
      for(int j(0); j < pf[i].size(); j++)
        val += pf[i][j] * poidsDecideur[j];
      if(val > opt)
        opt = val ;
    }
    int ctp(0);
    for(int i(0); i < pf.size(); i++){
      val = 0;
      for(int j(0); j < pf[i].size(); j++)
        val += pf[i][j] * poidsDecideur[j];
      if(((opt - val) / opt * 100) < 1)
        ctp++;
    }

    qualiteCSS[i] = ((opt - valUs) / opt) * 100;

  //  cout << (double)ctp / pf.size() * 100 << endl;

    }
    else if(indice == 2){
      vector<int> tab(nbInstances);
      vector<vector<int>> sacInitiaux;
      vector<int> sacInitial(nbInstances, 0);
      int indice(-1);
      for(int j(0); j < k; j++){
        tab.clear();
        tab.resize(nbInstances);
        for(int i(0); i < nbInstances; i++)
  	       tab[i] = i;

        sacInitial.clear();
        sacInitial.resize(nbInstances, 0);
        indice = -1;
        for(int i(0); i < nbMaxObejets; i++){
        	indice = (int)rand()%(tab.size());
        	sacInitial[tab[indice]] = 1;
        	tab.erase(tab.begin() + indice);
        }
      }
      resultat[i] = vector<double>(3);
      high_resolution_clock::time_point  start1 = high_resolution_clock::now();
      int nbQ;
      int nbIt;
      Sac solution = run(sacInitial, matriceCout, poidsDecideur, &nbQ, &nbIt);
      high_resolution_clock::time_point finish1 = high_resolution_clock::now();
      duration<double> e1 =(finish1 - start1);
      tempsCSS[i] = e1.count();

      double valUs(0);
      for(int j(0); j < poidsDecideur.size(); j++)
        valUs += solution.utilites[j] * poidsDecideur[j];

      double opt(-1);
      double val(0);
      for(int i(0); i < pf.size(); i++){
        val = 0;
        for(int j(0); j < pf[i].size(); j++)
          val += pf[i][j] * poidsDecideur[j];
          if(val > opt)
            opt = val ;
        }

      qualiteCSS[i] = ((opt - valUs) / opt) * 100;
      questionsCSS[i] = nbQ;
      it[i] = nbIt;

    }
    else{
      cout << "Pas de méthode avec cet indice" << endl;
    }
  }

  if(indice == 1){
    cout << "      Les temps PLS : " << endl;
    for(int i(0); i < nbIterations; i++)
      cout << tempsPLS[i] << endl;
    cout << "Moyenne : "  ;
    cout << std::accumulate(tempsPLS.begin(), tempsPLS.end(), 0.0) / nbIterations << endl<< endl;

    cout << "      Les pourcentages de réussite de PLS : " << endl;
    for(int i(0); i < nbIterations; i++)
      cout << pourcentagePLS[i] << endl;
    cout << "Moyenne : "  ;
    cout << std::accumulate(pourcentagePLS.begin(), pourcentagePLS.end(), 0.0) / nbIterations << endl<< endl;

    cout << "      Les temps de la CSS : " << endl;
    for(int i(0); i < nbIterations; i++)
      cout << tempsCSS[i] << endl;
    cout << "Moyenne : " ;
    cout << std::accumulate(tempsCSS.begin(), tempsCSS.end(), 0.0) / nbIterations << endl<< endl;

    cout << "      Les nombres de questions de la CSS : " << endl;
    for(int i(0); i < nbIterations; i++)
      cout << questionsCSS[i] << endl;
    cout << "Moyenne : " ;
    cout << std::accumulate(questionsCSS.begin(), questionsCSS.end(), 0.0) / nbIterations << endl<< endl;

    cout << "      Les pourcentages d'erreur de CSS : " << endl;
    for(int i(0); i < nbIterations; i++)
      cout << qualiteCSS[i] << endl;
    cout << "Moyenne : " ;
    cout << std::accumulate(qualiteCSS.begin(), qualiteCSS.end(), 0.0) / nbIterations << endl<< endl;
  }
  if(indice == 2){
    cout << "      Les temps de la CSS : " << endl;
    for(int i(0); i < nbIterations; i++)
      cout << tempsCSS[i] << endl;
    cout << "Moyenne : " ;
    cout << std::accumulate(tempsCSS.begin(), tempsCSS.end(), 0.0) / nbIterations << endl<< endl;

    cout << "      Les nombres de questions de la CSS : " << endl;
    for(int i(0); i < nbIterations; i++)
      cout << questionsCSS[i] << endl;
    cout << "Moyenne : " ;
    cout << std::accumulate(questionsCSS.begin(), questionsCSS.end(), 0.0) / nbIterations << endl<< endl;

    cout << "      Les pourcentages d'erreur de CSS : " << endl;
    for(int i(0); i < nbIterations; i++)
      cout << qualiteCSS[i] << endl;
    cout << "Moyenne : " ;
    cout << std::accumulate(qualiteCSS.begin(), qualiteCSS.end(), 0.0) / nbIterations << endl<< endl;
    cout << "      Le nombre d'itérations : " << endl;
    for(int i(0); i < nbIterations; i++)
      cout << it[i] << endl;
    cout << "Moyenne : " ;
    cout << std::accumulate(it.begin(), it.end(), 0.0) / nbIterations << endl<< endl;

  }
  return 0;
}
