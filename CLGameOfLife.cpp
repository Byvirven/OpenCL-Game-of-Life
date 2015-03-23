#include "CLGameOfLife.hpp"

// constructeur de la classe OpenCL
CLGameOfLife::CLGameOfLife(int width, int height, int nbPoint) {
		// instancier une matrice pour l'image de taille cv::Size(width,height)
	// de type CV_8UC3 : 3 canaux pour RGB de la taille d'un octet
	// et remplir la matrice de brun (RGB = 70,141,189)
	img = cv::Mat(cv::Size(width,height), CV_8UC3,cv::Scalar(255,255,255));
	// générer les points initiaux
	CLGameOfLife::generateDot(nbPoint);
	// taille de la matrice
	matSize = new int[1];
	matSize[0] = img.total();
	// Récupérer les données concernant l'architecture du
	// système ou 'platform model' comme décrit dans la
	// documentation OpenCL au chapitre 3.1
	// Voir chapitre 2.1 de l'API OpenCL C++
	if (cl::Platform::get(&platforms) == CL_SUCCESS) {
		std::cout<<"Architecture compatible OpenCL."<<std::endl;
	} else {
		throw std::logic_error("Architecture non-compatible OpenCL");
	}
	
	// Définir le contexte OpenCL qui gérera les périphériques
	// OpenCL, les queues des commandes, la mémoire, les objets
	// ainsi que les kernel et leurs exécutions.
	// Voir chapitre 4.3 de la documentation OpenCL
	// et chapitre 2.3 de l'API OpenCL C++'
	// CL_DEVICE_TYPE_ALL pour générer un contexte
	// prenant en compte tous les périphériques compatibles OpenCL
	// CL_DEVICE_TYPE_GPU est le type générique pour les GPU
	// Voir chapitre 4.2 de la documentation OpenCL pour la 
	// liste des périphériques type
	context = cl::Context(CL_DEVICE_TYPE_ALL, NULL, NULL, NULL, &err);
	if (err == CL_SUCCESS) {
		std::cout<<"Création du contexte OpenCL réussie."<<std::endl;
	} else {
		throw std::logic_error("Création du contexte OpenCL échouée.");
	}

	// Récupérer la liste des propriété du contexte OpenCL
	// Voir chapitre 2.3 de l'API OpenCL C++
	context_properties = context.getInfo<CL_CONTEXT_PROPERTIES>();
	if (context_properties.size() != 0) {
		std::cout
		<<"Récupération des propriétés du contexte réussie."
		<<std::endl;
	} else {
		throw std::logic_error
		("Récupération des propriétés du contexte échouée.");
	}

	// Récupérer la liste du/des périphérique(s) compatible(s)
	// OpenCL disponible pour le contexte créé.
	// Voir chapitre 2.3 de l'API OpenCL C++
	context_devices = context.getInfo<CL_CONTEXT_DEVICES>();
	if (context_devices.size() != 0) {
		std::cout
		<<"Récupération de la liste des périphériques du contexte réussie."
		<<std::endl;
	} else {		
		throw std::logic_error
		("Récupération de la liste des périphériques du contexte échouée.");
	}
	// Récupérer l'identifiant attribué au premier GPU
	// découvert sur la plateforme
	for (int i=0;i<platforms.size();i++) {
		for (int j=0;j<context_devices.size();j++) {
			if (context_devices[j].getInfo<CL_DEVICE_TYPE>() == 4) {
				id_GPU = j;
				break;
			}
		}
	} 
	// Créer la queue d'exécution des commandes OpenCL
	// N'ayant qu'un seul périphérique sur la machine de test
	// le choix du périphériques est simplifié, donc on prend le 0
	// Tentative de gestion de l'identifiant du GPU sur une plateforme 
	// possédant plusieurs périphériques
	// Voir le chapitre 5.1 de la documentation OpenCL ainsi
	// que le chapitre 3.9 de l'API OpenCL C++
	queue = cl::CommandQueue(context, context_devices[id_GPU], 0, &err);
	if (err == CL_SUCCESS) {
		std::cout<<
		"Création de la queue d'exécution OpenCL réussie."
		<<std::endl;
	} else {
		throw std::logic_error
		("Création de la queue d'exécution OpenCL échouée.");
	}
	// créer les buffers mémoire
	inputImg = cl::Buffer(context, // contexte du buffer 
						CL_MEM_READ_ONLY, // mode
						img.total() * img.channels() * sizeof(uchar) // taille
						);
	outputImg = cl::Buffer(context, 
						CL_MEM_WRITE_ONLY, 
						img.total() * img.channels() * sizeof(uchar)
						);
	imgWidth = cl::Buffer(context, // contexte du buffer 
						CL_MEM_READ_ONLY, // mode
						sizeof(int) // taille
						);
	imgSize = cl::Buffer(context, // contexte du buffer 
						CL_MEM_READ_ONLY, // mode
						sizeof(int) // taille
						);
	// instancier la fenêtre d'affichage
	cv::namedWindow("Jeu de la vie",CV_WINDOW_KEEPRATIO );
	// afficher l'image dans la fenêtre
	cv::imshow("Jeu de la vie", img);
}

void CLGameOfLife::run(int ms) {
	int key;
	while(true) {
		key = cvWaitKey(ms);
		if (key == 'q' or key == 1048689 or key == 1048603) break;
		//std::cout<<key<<std::endl;
		writeMemory();
		execKernel();
		readMemory();
	}
	
}

void CLGameOfLife::generateDot(int nbPoint) {
	// graine
	srand((time(NULL)+rand())%(img.total()+rand()));
	//génération des points
	for (int i=0;i<nbPoint;i++) {
		int position = (rand()%img.total())*img.channels();
		img.data[position] = 0xFE;
		img.data[position+1] = 0;
		img.data[position+2] = 0;
	}
}


void CLGameOfLife::makeKernel(void) {
	// ouvrir le fichier source OpenCL
	std::ifstream sourceFile("golKernel.cl");
	// lire fichier source
	std::string sourceCode(
						std::istreambuf_iterator<char>(sourceFile), //start
						(std::istreambuf_iterator<char>()) //eof
						);
	// Créer un objet OpenCL depuis la source -> Chapitre 3.5 C++
	cl::Program::Sources programObj(
							1,std::make_pair(sourceCode.c_str(),
							sourceCode.length()+1)
							);
	// associer la source au contexte -> Chapitre 3.5 C++
	program = cl::Program(context, programObj);
	// Compiler le kernel online (JIT) associé au contexte
	// pour le périphérique OpenCL prédéfini
	program.build(context_devices);
	// Créer le kernel
	kernel = cl::Kernel(program, "goLife");
}

void CLGameOfLife::writeMemory(void) {
	// écrire le contenu de la matrice dans le buffer mémoire
	// et insérer ce dernier la queue d'exécution
	queue.enqueueWriteBuffer(inputImg,  // buffer de copie
						CL_TRUE, // bloquer en écriture
						0, // offset de départ
						img.total() * img.channels() * sizeof(uchar), // taille 
						img.data, // contenu du buffer
						NULL, // vecteur d'événement, par défaut null
						&event // variable d'événement
						);
	event.wait();
	queue.enqueueWriteBuffer(imgWidth,  // buffer de copie
						CL_TRUE, // bloquer en écriture
						0, // offset de départ
						sizeof(int), // taille 
						&img.cols, // contenu du buffer
						NULL, // vecteur d'événement, par défaut null
						&event // variable d'événement
						);
	// attendre la fin du chargement du buffer dans la queue
	event.wait();
	queue.enqueueWriteBuffer(imgSize,  // buffer de copie
						CL_TRUE, // bloquer en écriture
						0, // offset de départ
						sizeof(int), // taille 
						matSize, // contenu du buffer
						NULL, // vecteur d'événement, par défaut null
						&event // variable d'événement
						);
	// attendre la fin du chargement du buffer dans la queue
	event.wait();
	// Passer au kernel les buffers qui sont
	// arguments de ce dernier	
	kernel.setArg(0, inputImg);
	kernel.setArg(1, imgWidth);
	kernel.setArg(2, imgSize);
	kernel.setArg(3, outputImg);
}

void CLGameOfLife::readMemory(void) {
	queue.enqueueReadBuffer(outputImg, CL_TRUE, 0, img.total() * img.channels() * sizeof(uchar), img.data);
	cv::imshow("Jeu de la vie", img);
}

void CLGameOfLife::execKernel(void) {
	// exécuter le kernel
	queue.enqueueNDRangeKernel(kernel, // kernel associé à la queue
						cl::NullRange, // offset de départ (0,0,0)
						cl::NDRange(img.total()), //nb de work-item
						cl::NDRange(1), // nb work-item par work-group
						NULL, // vecteur d'événement, par défaut null
						&event // variable d'événement
						);
	// attendre la fin de l'exécution du kernel
	event.wait();
}

CLGameOfLife::~CLGameOfLife(void) {
	queue.finish();
	queue.flush();
	cv::destroyWindow("Jeu de la vie");
	std::cout<< "byebye life"<<std::endl;
}

