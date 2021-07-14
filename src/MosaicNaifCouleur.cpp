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

#include "distance.h"

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
bool verif_border(int w, int h, int raw, int column)
{
	return raw >= 0 && column >= 0 && raw < h && column < w;
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


void autoMosaic(OCTET* imageIn, OCTET* imageMini, OCTET* imageOut, int taille_image, int qual)
{
	int qualite = qual * 2;
	int nb_bloc = taille_image / qualite;
	int taille_bloc = qualite * qualite;

	for (int i = 0; i < nb_bloc; i++)
	{
		for (int j = 0; j < nb_bloc; j++)
		{
			for (int k = 0; k < qualite; k++)
			{
				for (int g = 0; g < qualite; g++)
				{
					imageOut[((i * qualite) + k) * taille_image + ((j * qualite) + g)] = imageMini[k * qualite + g];
				}
			}
		}
	}
}

void copie_fichier()
{
	char file[250];

	string const nomFichierEcrit("temp_fichier.dat");
	//Pour �crire dans un fichier toutes les valeurs
	ofstream monFluxEcrit(nomFichierEcrit.c_str());

	string const nomFichierLec("imagettes_moy.dat");
	//Pour �crire dans un fichier toutes les valeurs
	ifstream monFluxLec(nomFichierLec.c_str());
	char delim = ' ';
	if (monFluxLec && monFluxEcrit)
	{
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

			monFluxEcrit << ligne << endl;//L�, on r�cup�re la valeur calcul�e, et on peut avoir le nom de l'image
		}
	}
	else cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
}



void ParcoursFichier_Moyenne(char* name_img, int val)
{
	char file[250];

	string const nomFichierLec("imagettes_moy.dat");
	//Pour �crire dans un fichier toutes les valeurs
	ifstream monFluxLec(nomFichierLec.c_str());
	char delim = ' ';
	if (monFluxLec)
	{
		//L'ouverture s'est bien pass�e, on peut donc lire

		string ligne; //Une variable pour stocker les lignes lues
		
		int verif = 0;
		while (getline(monFluxLec, ligne) && verif != val) //Tant qu'on n'est pas � la fin, on lit
		{
			//cout << ligne << endl;
			std::vector<string> recup_ligne;

			//C'est un split par espace ' '
			std::istringstream iss(ligne);
			std::copy(std::istream_iterator<std::string>(iss),
				std::istream_iterator<std::string>(),
				std::back_inserter(recup_ligne));

			//monFluxEcrit << recup_ligne.back() << " " << recup_ligne.at(1) << endl;//L�, on r�cup�re la valeur calcul�e, et on peut avoir le nom de l'image

			verif = stoi(recup_ligne.back());
			
			if (verif == val)
			{
				sprintf(name_img, recup_ligne[1].c_str());
				//doublon.push_back(name_img);

				auto indice = stoi(recup_ligne.at(0));
				
				if (find(doublon.begin(), doublon.end(), indice) != doublon.end())
				{
					cout << doublon.size() << endl;
					verif = 0;
				}
				else
				{
					cout << "YOOOOO" << endl;
					doublon.push_back(indice);
				}
			}

		}
	}
	else cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
}


void imagetteMosaic(OCTET* imageOut, int taille_image, int qual)
{
	int qualite = qual *2;
	int nb_bloc = taille_image / qualite;
	int taille_bloc = qualite * qualite;

	char* name_image = new char[250];

	int num = 0;
	int h = 512 / 16;
	int w = 512 / 16;

	OCTET* imagette;
	allocation_tableau(imagette, OCTET, h*w);


	for (int i = 0; i < nb_bloc; i++)
	{
		for (int j = 0; j < nb_bloc; j++)
		{
			sprintf(name_image, "imagettes/%d.pgm", num);
			num++;

			lire_image_pgm(name_image, imagette, h * w);



			for (int k = 0; k < qualite; k++)
			{
				for (int g = 0; g < qualite; g++)
				{
					imageOut[((i * qualite) + k) * taille_image + ((j * qualite) + g)] = imagette[k * qualite + g];
				}
			}
		}
	}
	free(imagette);
}


void imagetteMosaic_moy(OCTET* imageIn, OCTET* imageOut, int nH, int nW, int qualite)
{
	int taille_image=nH*nW;
	int nb_bloc = nH / qualite;
	int taille_bloc = qualite * qualite;
	int nH_bloc=qualite;
	int nW_bloc=qualite;
	char* name_image = new char[250];

	int num = 0;

	OCTET* currentImagette;
	OCTET* bestImagette;
	OCTET* bloc;
	allocation_tableau(currentImagette, OCTET, nH_bloc*nW_bloc);
	allocation_tableau(bestImagette, OCTET, nH_bloc*nW_bloc);
	allocation_tableau(bloc, OCTET, nH_bloc*nW_bloc);
	for (int i = 0; i < nb_bloc; i++)
	{
		for (int j = 0; j < nb_bloc; j++)
		{
			printf("%d/%d %d/%d\n",i,nb_bloc,j,nb_bloc);
			for (int k = 0; k < nH_bloc; k++)
			{
				for (int g = 0; g < nW_bloc; g++)
				{
					//score += imageIn[((i * qualite) + k) * taille_image + ((j * qualite) + g)];
					bloc[k*nW_bloc+g]=imageIn[((i * nW_bloc) + k) * nW + ((j * qualite) + g)];
				}
			}
			

			double score = 0.0, bestScore=9999999.0;
			int indiceBestImagette=0;

			char name_img[250];
			for(int numImagette=0;numImagette<10000;numImagette++){
				if(find(doublon.begin(), doublon.end(), numImagette)!=doublon.end()){
					continue;
				}
				sprintf(name_img, "imagettes%d/%d.pgm",qualite,numImagette);
				lire_image_pgm(name_img, currentImagette, nH_bloc * nW_bloc);
				score=euclidianDistance(bloc,currentImagette,nH_bloc,nW_bloc);
				if(score<bestScore){
					bestScore=score;
					indiceBestImagette=numImagette;
				}
			}
			doublon.push_back(indiceBestImagette);
			//ParcoursFichier_Moyenne(name_img, moy);
			sprintf(name_img, "imagettes%d/%d.pgm",qualite,indiceBestImagette);
			cout << name_img << " " << i << " " << j << " " << bestScore << endl;
			lire_image_pgm(name_img, bestImagette, nH_bloc * nW_bloc);

			for (int k = 0; k < nH_bloc; k++)
			{
				for (int g = 0; g < nW_bloc; g++)
				{
					imageOut[((i * nW_bloc) + k) * nW + ((j * qualite) + g)] = bestImagette[k * qualite + g];
				}
			}
			
		}
	}
	cout << ("------------------------------------------------------") << endl;
	for (int i = 0; i < doublon.size(); i++)
	{
		cout << doublon.at(i) << endl;
	}

	free(bestImagette);
	free(bloc);
	free(currentImagette);
}


int main(int argc, char* argv[])
{
	char cNomImgLue[250], cNomImgEcrite[250];
	int nH, nW, nTaille;

	if (argc != 3)
	{
		printf("Usage: ImageIn.ppm ImageOut.ppm\n");
		exit(1);
	}

	sscanf(argv[1], "%s", cNomImgLue);
	sscanf(argv[2], "%s", cNomImgEcrite);

	OCTET *ImgIn, *ImgOut, *ImgTemp;

	lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &nH, &nW);
	nTaille = nH * nW;


	allocation_tableau(ImgIn, OCTET, nTaille);
	lire_image_pgm(cNomImgLue, ImgIn, nH * nW);
	//allocation_tableau(ImgTemp, OCTET, nTaille/(Q * Q));
	allocation_tableau(ImgOut, OCTET, nH*nW);
	char name_img[250];
	std::cout << nW << " " << nH << endl;
	
	imagetteMosaic_moy(ImgIn, ImgOut,nH,nW, 8);
	
	printf("Copie finie\n");
	ecrire_image_pgm(cNomImgEcrite, ImgOut, nW, nH);
	free(ImgIn);
	return 1;
}

