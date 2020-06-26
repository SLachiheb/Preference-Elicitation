##Elicitation incrémentale et recherche locale pour le problème de sélection multi-objectifs


### Introduction :
Dans ce projet, nous allons explorer la notion d’apprentissage actif dans le domainedu multicritère sur le problème de sélection multi-objectifs. La procédure appliquéedurant ce travail est l’apprentissage par élicitation incrémentale basée sur le regret. Apartir de cette procédure, on souhaite restreindre l’espace des poids du décideur et ainsiréduire l’espace des solutions potentielles du décideur jusqu’à cerner suffisamment sonvecteur poids pour résoudre le système de recommandation du problème de sélectionmulti-objectifs.



### Modèle de compilation :

Pour complier il faut faire un make dans la console puis lancer l'exécutable avec 
"Numéro Méthode    Taille de l'instance     Le nombre de critères    Le nom du fichier PF "

Par exemple : ./executable 1 30 3 ../InstancesSAC/PF_30_3.txt 

Si vous le souhaitez vous pouvez changer le nombre M à sélectionner après PLS à la ligne 305 pour la somme pondérée et 47 pour OWA soit : 
  int M = 100 
