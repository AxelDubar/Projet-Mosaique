// test_couleur.cpp : Seuille une image en niveau de gris

#include <iostream>
#include <fstream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <string.h>
#include <math.h>
#include "image_ppm.h"
#include <vector>
#include <sstream>
#include <iterator>

using namespace std;


double euclidianDistance(OCTET *Img1,OCTET *Img2,int nH,int nW){
	double result=0.0;
	for (int i=0; i < nH; i++){
		for (int j=0; j < nW; j++){
			int p1=Img1[i*nW+j];
			int p2=Img2[i*nW+j];

			result+=pow((p2-p1),2);
		}
	}
	result=sqrt(result);
	return result;
}



int main(int argc, char* argv[])
{
	char cNomImg1[250], cNomImg2[250];
	int nH, nW, nTaille;
  
	if (argc != 3) 
	{
		printf("Usage: Image1.pgm Image2.pgm \n"); 
		exit (1) ;
	}
	
	sscanf (argv[1],"%s",cNomImg1) ;
	sscanf (argv[2],"%s",cNomImg2);
		
	OCTET *Img1, *Img2;
	
	lire_nb_lignes_colonnes_image_pgm(cNomImg1, &nH, &nW);
	nTaille = nH * nW;
	
	allocation_tableau(Img1, OCTET, nTaille);
	allocation_tableau(Img2, OCTET, nTaille);
	lire_image_pgm(cNomImg2, Img1, nH * nW);
	lire_image_pgm(cNomImg1, Img2, nH * nW);


	printf("%lf\n",euclidianDistance(Img1,Img2,nH,nW));

	return 1;
}

