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

/* Modélisation d'un SAC : une liste de 0 ou 1 selon si l'objets est pris ou non et un vector
contenant la performance de cette solution
*/
struct Sac {
    vector<int>     listeObjets;
    vector<double>  utilites;
};

//----------------------------------------------------------------------//

                                //MAIN//

//----------------------------------------------------------------------//


int main(int argc, const char * argv[]) {

  // Signatures de ILS ET PLS ET fontions de lectures :
  vector<Sac> run(vector<int> &sacI, vector<vector<double>> &mat);
  Sac run(vector<int> &sacI, vector<vector<double>> &mat, vector<double> &poidsDecideur, int *nbQ, int *nbI);
  void split(const string &chaine, char delimiteur, vector<string> &elements);
  vector<string> split(const string &chaine, char delimiteur);

  while (argc != 5) {
      cout << "Entrer: Numéro Méthode    Taille de l'instance     Le nombre de critères    Le nom du fichier PF " <<endl;
      cout << "Exemple : ./executable 1 30 3 ../InstancesSAC/PF_30_3.txt " << endl;
      return 1;
  }
  int ind               = stoi(argv[1]);
  int nbInstances       = stoi(argv[2]);
  int nbCriteres        = stoi(argv[3]);
  string Nfichier       = argv[4];
  int nbMaxObejets      = nbInstances/2; // k
  int M                 = 100;
  int nbIterations      = 20;
  int nbFichier         = 1000;
  int nbFichierCritere  = 5;

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
    string ligne;//----------------------------------------------------------------------//

    int i = 0;
    while(getline(fichier, ligne)){
      pf.push_back(vector<double>(nbCriteres));
      vector<string> mots = split(ligne, ' ');
      for(int j(0); j < nbCriteres; j++)
	       pf[i][j] = stoi(mots[j]);
      sort(pf[i].begin(), pf[i].end(), less<>());
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
    poidsDecideur = listePoidsDecideur[i];
    sort(poidsDecideur.begin(), poidsDecideur.end(), greater<>());
    vector<int> tab(nbInstances);
    vector<int> sacInitial(nbInstances, 0);
    int indice(-1);

    for(int i(0); i < nbInstances; i++)
      tab[i] = i;

    for(int i(0); i < nbMaxObejets; i++){
      indice = (int)rand()%(tab.size());
      sacInitial[tab[indice]] = 1;
      tab.erase(tab.begin() + indice);
    }
    resultat[i] = vector<double>(3);

    if(ind == 1 ){

      high_resolution_clock::time_point  start1 = high_resolution_clock::now();
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
    fichier.open("../PF.txt");
    if (fichier.is_open()) {
      string ligne;
      int i = 0;
      while(getline(fichier, ligne)){
        alternatives.push_back(vector<double>(nbCriteres));
        vector<string> mots = split(ligne, ' ');
        for(int j(0); j < nbCriteres; j++)
  	       alternatives[i][j] = stoi(mots[j]);
        sort(alternatives[i].begin(), alternatives[i].end(), less<>());
        i++;
      }
    }
    else
      cout << "Erreur : Impossible d'ouvrir le fichier 3" << endl;
    fichier.close();
    CSS strategie;

    if(alternatives.size() > M){
      while(alternatives.size() > M)
        alternatives.erase(alternatives.begin() + ((int)rand()%(alternatives.size())) );
    }

    high_resolution_clock::time_point  start2 = high_resolution_clock::now();
    vector<double> res = strategie.css(alternatives, poidsDecideur);
    high_resolution_clock::time_point finish2 = high_resolution_clock::now();
    duration<double> e2 =(finish2 - start2);
    questionsCSS[i] = res[0];
    tempsCSS[i] = e2.count();

    sort(alternatives[(int)res[1]].begin(), alternatives[(int)res[1]].end(), less<>());


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

    qualiteCSS[i] = (opt - valUs) / opt * 100;

    }
    else if(ind == 2){
      int nbQ;
      int nbI;
      high_resolution_clock::time_point  start1 = high_resolution_clock::now();
      Sac solution = run(sacInitial, matriceCout, poidsDecideur, &nbQ, &nbI);
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

      qualiteCSS[i] = (opt - valUs) / opt * 100;
      questionsCSS[i] = nbQ;
      it[i] = nbI;

    }
    else{
      cout << "Pas de méthode avec cet indice" << endl;
    }
  }

  if(ind == 1){
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
  if(ind == 2){
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






//----------------------------------------------------------------------//

                                //OUTILS//

// Valeur d'un sac
// Plusieurs Pareto
// Fonction de voisinage
// ILS
// PLS

//----------------------------------------------------------------------//








vector<double> valeurSac(vector<int> &sac, vector<vector<double>> &mat){
  vector<double> val(mat[0].size(), 0);
  for(int i(0); i < sac.size(); i++){
    if(sac[i] == 1){
      for(int j(0); j < mat[0].size(); j++)
	     val[j] += mat[i][j];
    }
  }
  sort(val.begin(), val.end(), less<>());
  return val;
}

vector<double> valeurSacPLS(vector<int> &sac, vector<vector<double>> &mat){
  vector<double> val(mat[0].size(), 0);
  for(int i(0); i < sac.size(); i++){
    if(sac[i] == 1){
      for(int j(0); j < mat[0].size(); j++)
	     val[j] += mat[i][j];
    }
  }
  return val;
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
//  int m = sacs.size();
//  for(int i(0); i < m; i++){
  int v = paretoDomine(s1.utilites, s.utilites);
  if(v == 1)
    return true;
    //}
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
vector<Sac> run(vector<int> &sacI, vector<vector<double>> &mat){

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
        s.utilites     = valeurSacPLS(voisins[i], mat);
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
  dev2.clear();

  } while( resultat.size() != 0);
  dev = pareto_classique(dev);
  for(int i(0); i < dev.size(); i++)
    sort(dev[i].utilites.begin(), dev[i].utilites.end(), less<>());

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
Sac run(vector<int> &sacI, vector<vector<double>> &mat, vector<double> &poidsDecideur, int *nbQ, int *nbI){
  int nbCriteres  = mat[0].size();
  int nbInstances = mat.size();
  int nbIt        = 0;
  int nb_question = 0;

  bool                   test;
  CSS                    strategie;
  Sac                    courant;
  Sac                    oldCourant;
  vector<Sac>            sacsVoisins;
  vector<vector<int>>    voisins;
  vector<vector<double>> alternatives;
  Sac                    sacInitial;


  sacInitial.utilites     =  valeurSac(sacI, mat);
  sacInitial.listeObjets  = sacI;
  courant = sacInitial;
  do {

    nbIt++;
    voisins.clear();
    alternatives.clear();
    sacsVoisins.clear();
    voisins = voisinage(courant.listeObjets, voisins);

    sacsVoisins.push_back(courant);

    for(int i(0); i < voisins.size(); i++){
      Sac s;
      s.listeObjets  = voisins[i];
      s.utilites     = valeurSac(voisins[i], mat);
      sacsVoisins.push_back(s);
    }
    sacsVoisins = pareto_classique(sacsVoisins);
    for(int i(0); i < sacsVoisins.size(); i++)
      alternatives.push_back(sacsVoisins[i].utilites);
    vector<double> res = strategie.css(alternatives, poidsDecideur);
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
