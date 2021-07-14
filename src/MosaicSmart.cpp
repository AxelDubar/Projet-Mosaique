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

struct Assignement{
	OCTET *image;
	bool matriceColorie[512][512];
}



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





vector<int> ParcoursFichier_Moyenne(int val,int blocSize,int precision)
{
	vector<int> candidats;
	char file[250];
	char filename[250];
	sprintf(filename,"data/imagettes%d_moy.dat",blocSize);
	//printf("%s\n",filename);
	//Pour �crire dans un fichier toutes les valeurs
	ifstream monFluxLec(filename);
	char delim = ' ';
	if (monFluxLec)
	{
		//L'ouverture s'est bien pass�e, on peut donc lire

		string ligne; //Une variable pour stocker les lignes lues
		
		int verif = 0;
		int i=0;
		while (getline(monFluxLec, ligne)) //Tant qu'on n'est pas � la fin, on lit
		{
			//cout << ligne << endl;
			std::vector<string> recup_ligne;

			//C'est un split par espace ' '
			std::istringstream iss(ligne);
			std::copy(std::istream_iterator<std::string>(iss),
				std::istream_iterator<std::string>(),
				std::back_inserter(recup_ligne));

			//cout << recup_ligne.back() << " " << i <<endl;//L�, on r�cup�re la valeur calcul�e, et on peut avoir le nom de l'image
			
			verif = stoi(recup_ligne.back());
			if(abs(verif-val)<=80){
				candidats.push_back(i);
			}
			i++;
		}
	}
	else cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
	/*for(int i=0;i<candidats.size();i++){
		printf("%d\n",candidats.at(i));
	}*/
	return candidats;
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


int findBestImagette(OCTET *bloc,OCTET *currentImagette,int nH_bloc,int nW_bloc,int precision){
	
	double score = 0.0, bestScore=9999999.0;
	int indiceBestImagette=-1;
	int grisMoyen=calculMoy(bloc,nW_bloc,nH_bloc);
	vector<int> candidats=ParcoursFichier_Moyenne(grisMoyen,nH_bloc,precision);
	char name_img[250];
	int numImagette=-1;
	for(int i=0;i<candidats.size();i++){
		numImagette=candidats.at(i);
		if(find(doublon.begin(), doublon.end(), numImagette)!=doublon.end()){
			continue;
		}
		sprintf(name_img, "imagettes%d/%d.pgm",nH_bloc,numImagette);
		lire_image_pgm(name_img, currentImagette, nH_bloc * nW_bloc);
		score=euclidianDistance(bloc,currentImagette,nH_bloc,nW_bloc);
		if(score<bestScore){
			bestScore=score;
			indiceBestImagette=numImagette;
		}
	}
	if(indiceBestImagette==-1){
		return findBestImagette(bloc,currentImagette,nH_bloc,nW_bloc,precision+1);
	}
	printf("precision=%d\n",precision);
	return indiceBestImagette;
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
	
	char name_img[250];
	for (int i = 0; i < nb_bloc; i++)
	{
		for (int j = 0; j < nb_bloc; j++)
		{
			printf("%d/%d %d/%d\n",i,nb_bloc,j,nb_bloc);
			for (int k = 0; k < nH_bloc; k++)
			{
				for (int g = 0; g < nW_bloc; g++)
				{
					bloc[k*nW_bloc+g]=imageIn[((i * nW_bloc) + k) * nW + ((j * qualite) + g)];
				}
			}
			
			int indiceBestImagette=findBestImagette(bloc,currentImagette,nH_bloc,nW_bloc,0);
			
			doublon.push_back(indiceBestImagette);
			//ParcoursFichier_Moyenne(name_img, moy);
			sprintf(name_img, "imagettes%d/%d.pgm",qualite,indiceBestImagette);
			cout << name_img << " " << i << " " << j << " " << endl;
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
		printf("Usage: ImageIn.pgm ImageOut.pgm\n");
		exit(1);
	}

	sscanf(argv[1], "%s", cNomImgLue);
	sscanf(argv[2], "%s", cNomImgEcrite);

	OCTET *ImgIn, *ImgOut, *ImgTemp;

	lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &nH, &nW);
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

