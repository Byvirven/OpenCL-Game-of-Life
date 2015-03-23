#ifndef CL_GAMEOFLIFE_CLASS_H
#define CL_GAMEOFLIFE_CLASS_H

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <time.h>
#include <stdexcept>
#include <cv.h>
#include <highgui.h>

class CLGameOfLife {
	public:
		// Constructeurs et destructeur
		// de la classe OpenCL
		CLGameOfLife(int width, int height, int nbPoint); // constructeur
		~CLGameOfLife(void); // destructeur
		// fonction
		void run(int ms);
		void generateDot(int nbPoint);
		void makeKernel(void);
		void writeMemory(void);
		void execKernel(void);
		void readMemory(void);
	private:
		// platform model
		std::vector<cl::Platform> platforms;
		// contexte d'exécution
		cl::Context context;
		// propriétés du contexte
		std::vector<cl_context_properties> context_properties; 
		// périphériques du contexte
		std::vector<cl::Device> context_devices;
		// identifiant du GPU
		cl_device_type id_GPU;
		// queue d'exécution
		cl::CommandQueue queue;
		// programme
		cl::Program program;
		// kernel
		cl::Kernel kernel;
		// buffer mémoire
		cl::Buffer inputImg;
		cl::Buffer imgWidth;
		cl::Buffer imgSize;
		cl::Buffer outputImg;
		// gestion d'événement
		cl::Event event;
		//gestion d'erreur
		cl_int err; 
		// Matrice de l'image
		cv::Mat img; 
		// Taille de la matrice
		int * matSize;
};

#endif
