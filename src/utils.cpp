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
#include <algorithm>
#include <map> 

using namespace std;

vector<int> doublon;

int calculMoy(OCTET* image, int w, int h);

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
bool verif_border(int w, int h, int row, int column)
{
	return row >= 0 && column >= 0 && row < h && column < w;
}

void parcoursPhotos(int* tab_photos)
{
	int w = 512;
	int h = 512;
	OCTET* image;
	allocation_tableau(image, OCTET, w*h);
	char file[250];
	for (int i = 0; i < 10000; i++)
	{
		sprintf(file, "photos/%d.pgm", i);
		lire_image_pgm(file, image, w * h);
		tab_photos[i] = calculMode(image, w, h);
	}
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



void pixelate(OCTET* imageIn, OCTET* imageOut, int taille_image, int qualite)
{
	int taille_bloc = taille_image / qualite;

	for (int i = 0; i < taille_bloc; i++)
	{
		for (int j = 0; j < taille_bloc; j++)
		{
			int nv_gris = 0;
			for (int k = 0; k < qualite; k++)
			{
				for (int g = 0; g < qualite; g++)
				{
					nv_gris += imageIn[((i * qualite) + k) * taille_image + ((j * qualite) + g)];
				}
			}
			int moy = nv_gris / (qualite * qualite);
			
			for (int k = 0; k < qualite; k++)
			{
				for (int g = 0; g < qualite; g++)
				{
					imageOut[((i * qualite) + k) * taille_image + ((j * qualite) + g)] = moy;
				}
			}
		}
	}
}



void miniat(OCTET* imageIn, OCTET* imageOut, int nH,int nW, int nH_bloc)
{
	int nb_bloc = nH / nH_bloc;
	for (int i = 0; i < nb_bloc; i++)
	{
		for (int j = 0; j < nb_bloc; j++)
		{
			int nv_gris = 0;
			for (int k = 0; k < nH_bloc; k++)
			{
				for (int g = 0; g < nH_bloc; g++)
				{
					nv_gris += imageIn[((i * nH_bloc) + k) * nW + ((j * nH_bloc) + g)];
				}
			}
			int moy = nv_gris / (nH_bloc * nH_bloc);

			imageOut[i*nb_bloc + j] = moy;
		}
	}
	
}



int main(int argc, char* argv[])
{
	char cNomImgLue[250], cNomImgEcrite[250];
	int nH=512, nW=512, nTaille;


	OCTET *ImgIn, *ImgOut, *ImgTemp;

	nTaille = nH * nW;


	allocation_tableau(ImgIn, OCTET, 8*8);
	allocation_tableau(ImgOut, OCTET, 8*8);
	char name_img[250];
	std::cout << nW << " " << nH << endl;
	
	
	for(int i=0;i<10000;i++){
		sprintf(name_img,"imagettes/imagettes8/%d.pgm",i);
		lire_image_pgm(name_img,ImgIn,8*8);
		if(i%100==0)
			printf("i=%d\n",i);
		for(int j=0;j<10000;j++){
			if(i==j)
				continue;
			sprintf(name_img,"imagettes/imagettes8/%d.pgm",j);
			lire_image_pgm(name_img,ImgOut,8*8);
			if(euclidianDistance(ImgIn,ImgOut,8,8)<0.5){
				printf("Similaire trouve: %d %d: %lf\n",i,j,euclidianDistance(ImgIn,ImgOut,8,8));
				//ecrire_image_pgm("Similaire.pgm",ImgOut,8,8);
			}
		}

	}


	free(ImgIn);
	return 1;
}

