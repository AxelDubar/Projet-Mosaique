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




void permuter(vector<int>vect , int i,int j){
	int tmp=vect.at(i);
	vect.at(i)=vect.at(j);
	vect.at(j)=tmp;
}

void trier(vector<int> vect){
	for(int i=0;i<vect.size();i++){
		for(int j=0;j<i;j++){
			if(vect.at(j+1)<vect.at(j))
				permuter(vect,j,j+1);
		}
	}
}


int calculVariance(OCTET* image, int w, int h)
{
	int nv_gris = 0;
	vector<int> values;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			nv_gris += image[i*w + j];
			values.push_back(image[i*w + j]);
		}
	}
	int avg=nv_gris / (h *w);
	nv_gris=0;
	for(int i=0;i<values.size();i++){
		nv_gris+=pow((values.at(i)-avg),2);
	}
	return nv_gris;

}

double ecartType(OCTET *image,int w,int h){
	return sqrt((double)calculVariance(image,w,h) / (double)(w*h) );
}

int calculMedianne(OCTET* image, int w, int h)
{
	int nv_gris = 0;
	vector<int> valeurs;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			valeurs.push_back(image[i*w + j]);
		}
	}
	trier(valeurs);

	return valeurs.at(valeurs.size()/2);
}
int calculMode(OCTET* image, int w, int h)
{
	int val[256];
	for(int i=0;i<256;i++){
		val[i]=0;
	}

	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			val[image[i*w + j]]++;
		}
	}
	int mode=0;
	int nbOccur=0;
	for(int i=0;i<256;i++){
		if(nbOccur<val[i]){
			mode=i;
			nbOccur=val[i];
		}
	}
	return mode;
}

int calculMoy(OCTET* image, int w, int h)
{
	int nv_gris = 0;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			nv_gris += image[i*w + j];
		}
	}
	return nv_gris / (h *w);
}



int main(int argc, char* argv[])
{
	char cNomImg1[250];
	int nH, nW, nTaille;
  
	if (argc != 2) 
	{
		printf("Usage: Image.pgm \n"); 
		exit (1) ;
	}
	
	sscanf (argv[1],"%s",cNomImg1) ;
		
	OCTET *Img1;
	
	lire_nb_lignes_colonnes_image_pgm(cNomImg1, &nH, &nW);
	nTaille = nH * nW;
	
	allocation_tableau(Img1, OCTET, nTaille);
	lire_image_pgm(cNomImg1, Img1, nH * nW);

	int ecarttype=(int)ecartType(Img1,nW,nH);
	printf("%d %d\n",calculMoy(Img1,nW,nH)-ecarttype,calculMoy(Img1,nW,nH)+ecarttype);

	return 1;
}

