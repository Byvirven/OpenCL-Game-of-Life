//////////////////////////////////////////////////////////////////
//                                                              //
// Auteur      : Dos Santos Oliveira Marco                      //
// Date        : 16 mars 2012                                   //
// Sujet       : Projet de semestre OpenCL                      //
// Contenu     : Automate cellulaire : jeu de la vie            //
// Commentaire : Fichier principal                              //
// Enseignant  : Dr. Paul Albuquerque                           //
//                                                              //
//////////////////////////////////////////////////////////////////

#define __CL_ENABLE_EXCEPTIONS
#include "CLGameOfLife.hpp"
#include <iostream>
#include <cstring>


std::string usage(std::string appName) {
	return	"\nusage : "+appName+" largeur hauteur nbPoints millisec\n";
}

std::string man(void) {
	return	"\n* largeur  : largeur en pixel de la fenêtre\n"
			"* hauteur  : hauteur en pixel de la fenêtre\n"
			"* nbPoints : nombre de points à afficher\n"
			"* millisec : taux de rafraîchissement en milliseconde\n";
}

std::string error(int err) {
	switch(err) {
		case 1 :
			return "\nerreur : la largeur doit être un nombre entier\n";
		case 2 :
			return "\nerreur : la longueur doit être un nombre entier\n";
		case 3 :
			return 	"\nerreur : le nombre de points doit "
					"être un nombre entier\n";
		case 4 :
			return 	"\nerreur : le nombre de millisecondes doit "
					"être un nombre entier\n";
		default:
			return "\nerreur : le nombre d'arguments est incorrects\n";
	}
}

int main(int argc, char *argv[]){
	// vérifier si on a le bon nombre d'arguments
	if (argc != 5) {
		std::cerr<<error(0)<<usage(argv[0])<<man()<<std::endl;
		return 1;
	}
	// vérifier que les arguments ont le bon type
	for(int i=1;i<argc;i++) {
		for(int j=0;j<strlen(argv[i]);j++) {
			if (!isdigit(argv[i][j])) {
				std::cerr<<error(i)<<usage(argv[0])<<man()<<std::endl;
				return 1;
			}
		}
	}
	// démarrage
	try {
		std::cout<<"Welcome..."<<std::endl;
		CLGameOfLife GoLife(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]));
		GoLife.makeKernel();
		GoLife.run(atoi(argv[4]));
	} catch (cl::Error err) {
		std::cerr << "ERROR: " << err.what() << "(" 
		<< err.err() << ")" << std::endl;
	}
	return 0;
}
