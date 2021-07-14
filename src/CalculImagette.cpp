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

bool verif_border(int w, int h, int raw, int column)
{
	return raw >= 0 && column >= 0 && raw < h && column < w;
}

void parcoursPhotos_Moyenne()
{
	int w = 128;
	int h = 128;
	OCTET *imageIn;
	allocation_tableau(imageIn, OCTET, w*h);
	char file[250];
	char nomFichier[250];

	sprintf(nomFichier, "data/imagettes%d_moy.dat",w);
	//Pour �crire dans un fichier toutes les valeurs
	ofstream monFlux(nomFichier);

	if (monFlux)
	{
		printf("test\n");
		for (int i = 0; i < 10000; i++)
		{
			sprintf(file, "imagettes/imagettes%d/%d.pgm",w, i);
			lire_image_pgm(file, imageIn, w * h);
			printf("%d %s %d\n",i,file,calculMoy(imageIn, w, h));
			monFlux << i << " " << file << " " << calculMoy(imageIn, w, h) << endl; //L�, BAM, le calcul qui rentre dans le fichier, comme NeedBackup dans un miaou
		}
	}
	else cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
}



void testParcoursFichier_Moyenne()
{
	int w = 512 / 32;
	int h = 512 / 32;
	OCTET *imageIn;
	allocation_tableau(imageIn, OCTET, w*h);
	char file[250];
	string const nomFichierEcrit("test_lecture_imagettes_moy.dat");
	//Pour �crire dans un fichier toutes les valeurs
	ofstream monFluxEcrit(nomFichierEcrit.c_str());

	string const nomFichierLec("imagettes_moy.dat");
	//Pour �crire dans un fichier toutes les valeurs
	ifstream monFluxLec(nomFichierLec.c_str());
	
	char delim = ' ';
	if (monFluxLec && monFluxEcrit)
	{
	printf("test\n");
		//L'ouverture s'est bien pass�e, on peut donc lire

		string ligne; //Une variable pour stocker les lignes lues

		while (getline(monFluxLec, ligne)) //Tant qu'on n'est pas � la fin, on lit
		{
			//cout << ligne << endl;
			std::vector<string> recup_ligne;

			//C'est un split par espace ' '
			std::istringstream iss(ligne);
			std::copy(std::istream_iterator<std::string>(iss),
				std::istream_iterator<std::string>(),
				std::back_inserter(recup_ligne));

			monFluxEcrit << recup_ligne.back() << " " << recup_ligne.at(1) << endl;//L�, on r�cup�re la valeur calcul�e, et on peut avoir le nom de l'image
		}
	}
	else cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
}


int main(int argc, char* argv[])
{
	parcoursPhotos_Moyenne();
	printf("dqzdzd\n");
	testParcoursFichier_Moyenne();
	


	return 1;
}

