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

typedef struct Assignement{
	OCTET *image;
	bool matriceColorie[512][512];
	double score;
	vector<int> doublon;
}Assignement;


Assignement copieAssign(Assignement a){
	Assignement newA;
	newA.score=a.score;
	for(int i=0;i<a.doublon.size;i++){
		newA.doublon.push_back(a.doublon.at(i));
	}
	for(int i=0;i<512;i++){
		for(int j=0;j<512;j++){
			newA.image[i*512+j]=a.image[i*512+j];
			newA.matriceColorie[i][j]=a.matriceColorie[i][j];
		}
	}
	return newA;
}

Assignement newAssign(){
	Assignement newA;
	newA.score=0.0;
	for(int i=0;i<512;i++){
		for(int j=0;j<512;j++){
			newA.image[i*512+j]=0;
			newA.matriceColorie[i][j]=false;
		}
	}
	return newA;
}

bool matriceColorie[512][512];

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
			if(abs(verif-val)<=40){
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
		sprintf(name_img, "imagettes/imagettes%d/%d.pgm",nH_bloc,numImagette);
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


void imageToBloc(OCTET* imageIn ,OCTET* bloc ,int ligneDep,int colDep,int nW,int nH_bloc, int nW_bloc){

	for (int k = 0; k < nH_bloc; k++)
	{
		for (int g = 0; g < nW_bloc; g++)
		{
			bloc[k*nW_bloc+g]=imageIn[(ligneDep+k)*nW   +   colDep+g];
		}
	}
}

void blocToImage(OCTET* imageOut,OCTET* bloc,int ligneDep,int colDep,int nW,int nH_bloc,int nW_bloc){

	for (int k = 0; k < nH_bloc; k++)
	{
		for (int g = 0; g < nW_bloc; g++)
		{
			imageOut[(ligneDep+k)*nW   +   colDep+g] = bloc[k * nH_bloc + g];
			matriceColorie[(ligneDep+k)][colDep+g]=true;
		}
	}
}

void nextToDraw(int *nexti,int *nextj){
	for(int i=0;i<512;i++){
		for(int j=0;j<512;j++){
			if(!matriceColorie[i][j]){
				*nexti=i;
				*nextj=j;
				return;
			}
		}
	}
	*nexti=-1;
	*nextj=-1;
}

bool placeSuffisante(int i,int j,int nH_bloc){
	if(i+nH_bloc>512 || j+nH_bloc>512)
		return false;
	for(int k=i;k<i+nH_bloc;k++){
		for(int l=j;l<j+nH_bloc;l++){
			if(matriceColorie[k][l])
				return false;
		}
	}
	return true;
}

void imagetteMosaic_moy(OCTET* imageIn, OCTET* imageOut, int nH, int nW)
{
	int taille_image=nH*nW;
	int nb_bloc = nH / 32;
	int taille_bloc = 32 * 32;
	int nH_bloc=32;
	int nW_bloc=32;
	char* name_image = new char[250];

	int num = 0;

	OCTET* currentImagette;
	OCTET* bestImagette;
	OCTET* bloc;
	allocation_tableau(currentImagette, OCTET, nH_bloc*nW_bloc);
	allocation_tableau(bestImagette, OCTET, nH_bloc*nW_bloc);
	allocation_tableau(bloc, OCTET, nH_bloc*nW_bloc);
	int n=0;
	char name_img[250];
	int i=0,j=0;
	while(i!=-1 && j!=-1){
		n=(n+1)%3;
		nH_bloc=32/pow(2,n);
		nW_bloc=32/pow(2,n);
		if(!placeSuffisante(i,j,nH_bloc)){
			printf("Depassement\n");
			continue;
		}
		allocation_tableau(currentImagette, OCTET, nH_bloc*nW_bloc);
		allocation_tableau(bestImagette, OCTET, nH_bloc*nW_bloc);
		allocation_tableau(bloc, OCTET, nH_bloc*nW_bloc);
		printf("%d/%d %d/%d\n",i,nH_bloc,j,nH_bloc);

		imageToBloc(imageIn,bloc,i,j,nW,nH_bloc,nW_bloc);
		
		int indiceBestImagette=findBestImagette(bloc,currentImagette,nH_bloc,nW_bloc,0);
		
		doublon.push_back(indiceBestImagette);
		sprintf(name_img, "imagettes/imagettes%d/%d.pgm",nH_bloc,indiceBestImagette);
		cout << name_img << " " << i << " " << j << " " << endl;
		lire_image_pgm(name_img, bestImagette, nH_bloc * nW_bloc);

		blocToImage(imageOut,bestImagette,i,j,nW,nH_bloc,nW_bloc);
		
		nextToDraw(&i,&j);
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
	

	Assignement assign=newAssign();

	imagetteMosaic_moy(ImgIn, ImgOut,nH,nW);
	
	printf("Copie finie\n");
	ecrire_image_pgm(cNomImgEcrite, ImgOut, nW, nH);
	free(ImgIn);
	return 1;
}

