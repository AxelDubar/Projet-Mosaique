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
	int nbDoublon;
	int *doublon;
}Assignement;

double globalBestScore;

Assignement *copieAssign(Assignement *a){
	Assignement* newA=(Assignement*)malloc(sizeof(Assignement));
	
	newA->score=a->score;
	
	newA->nbDoublon=a->nbDoublon;
	newA->doublon=(int*)malloc(sizeof(int)*a->nbDoublon);
	for(int i=0;i<a->nbDoublon;i++){
		newA->doublon[i]=a->doublon[i];
	}

	allocation_tableau(newA->image, OCTET, 512*512);
	for(int i=0;i<512;i++){
		for(int j=0;j<512;j++){
			newA->image[i*512+j]=a->image[i*512+j];
			newA->matriceColorie[i][j]=a->matriceColorie[i][j];
		}
	}
	return newA;
}

Assignement* newAssign(){
	Assignement *newA=(Assignement*)malloc(sizeof(Assignement));
	newA->score=0.0;
	allocation_tableau(newA->image, OCTET, 512*512);
	for(int i=0;i<512;i++){
		for(int j=0;j<512;j++){
			newA->image[i*512+j]=0;
			newA->matriceColorie[i][j]=false;
		}
	}
	newA->nbDoublon=0;
	return newA;
}

void detruireAssign(Assignement *assign){


	free(assign->doublon);
	free(assign->image);
	free(assign);
}

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
	//result=sqrt(result);
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
			if(abs(verif-val)<=1){
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

bool findDoublon(int *doublon,int nbDoublon,int nbfichier){
	for(int i=0;i<nbDoublon;i++){
		if(doublon[i]==nbfichier)
			return true;
	}
	return false;
}

int findBestImagette(OCTET *bloc,OCTET *currentImagette,int nH_bloc,int nW_bloc,int precision,Assignement *assign,double *returnScore){
	
	double score = 0.0, bestScore=9999999.0;
	int indiceBestImagette=-1;
	int grisMoyen=calculMoy(bloc,nW_bloc,nH_bloc);
	vector<int> candidats=ParcoursFichier_Moyenne(grisMoyen,nH_bloc,precision);
	char name_img[250];
	int numImagette=-1;
	for(int i=0;i<candidats.size();i++){
		numImagette=candidats.at(i);
		if(findDoublon(assign->doublon,assign->nbDoublon, numImagette)){
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
		return findBestImagette(bloc,currentImagette,nH_bloc,nW_bloc,precision+1,assign,returnScore);
	}
	//printf("precision=%d %lf\n",precision,bestScore);
	*returnScore=bestScore;
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

void blocToImage(Assignement *assign,OCTET* bloc,int ligneDep,int colDep,int nW,int nH_bloc,int nW_bloc){

	for (int k = 0; k < nH_bloc; k++)
	{
		for (int g = 0; g < nW_bloc; g++)
		{
			assign->image[(ligneDep+k)*nW   +   colDep+g] = bloc[k * nH_bloc + g];
			assign->matriceColorie[(ligneDep+k)][colDep+g]=true;
		}
	}
}

void nextToDraw(int *nexti,int *nextj,Assignement *assign){
	for(int i=0;i<512;i++){
		for(int j=0;j<512;j++){
			if(!assign->matriceColorie[i][j]){
				*nexti=i;
				*nextj=j;
				return;
			}
		}
	}
	*nexti=-1;
	*nextj=-1;
}

bool placeSuffisante(int i,int j,int nH_bloc,Assignement *assign){
	if(i+nH_bloc>512 || j+nH_bloc>512){
		return false;
	}
	for(int k=i;k<i+nH_bloc;k++){
		for(int l=j;l<j+nH_bloc;l++){
			if(assign->matriceColorie[k][l]){
				return false;
			}
		}
	}
	return true;
}

void undoChange(Assignement *assign,int ligneDep,int colDep,int nW,int nH_bloc,int nW_bloc){
	assign->nbDoublon-=1;
	assign->doublon=(int*)realloc(assign->doublon,sizeof(int)*assign->nbDoublon);
	for (int k = 0; k < nH_bloc; k++)
	{
		for (int g = 0; g < nW_bloc; g++)
		{
			assign->image[(ligneDep+k)*nW   +   colDep+g] = 0;
			assign->matriceColorie[(ligneDep+k)][colDep+g]=false;
		}
	}
}

Assignement* imagetteMosaic_moy(OCTET* imageIn, int nH, int nW,Assignement *currentAssign)
{
	Assignement* bestAssign=copieAssign(currentAssign);
	bestAssign->score=std::numeric_limits<double>::max();
	int taille_image=nH*nW;
	int nb_bloc = nH / 32;
	int taille_bloc = 32 * 32;
	int nH_bloc=32;
	int nW_bloc=32;
	char* name_image = new char[250];
	double score;
	int num = 0;

	OCTET* currentImagette;
	OCTET* bestImagette;
	OCTET* bloc;
	int n=0;
	char name_img[250];
	int i=0,j=0;


	for(int n=0;n<2;n++){
		nextToDraw(&i,&j,currentAssign);
		nH_bloc=32/pow(2,n);
		nW_bloc=32/pow(2,n);
		if(n==1 && currentAssign->score<1.0){
			printf("50\n");
		}
		if(!placeSuffisante(i,j,nH_bloc,currentAssign)){
			if(n==1)
				bestAssign->score=currentAssign->score;
			//printf("place insu\n");
			continue;
		}
		//printf("Ca calc\n");
		allocation_tableau(currentImagette, OCTET, nH_bloc*nW_bloc);
		allocation_tableau(bestImagette, OCTET, nH_bloc*nW_bloc);
		allocation_tableau(bloc, OCTET, nH_bloc*nW_bloc);
		//printf("%d/%d %d/%d t=%d\n",i,512,j,512,nH_bloc);
		imageToBloc(imageIn,bloc,i,j,nW,nH_bloc,nW_bloc);
		int indiceBestImagette=findBestImagette(bloc,currentImagette,nH_bloc,nW_bloc,0,currentAssign,&score);
		
		currentAssign->score+=score;
		//printf("global=%lf\n",globalBestScore);
		if(currentAssign->score<globalBestScore){

			currentAssign->nbDoublon+=1;
			currentAssign->doublon=(int*)realloc(currentAssign->doublon,sizeof(int)*currentAssign->nbDoublon);
			currentAssign->doublon[currentAssign->nbDoublon-1]=indiceBestImagette;




			sprintf(name_img, "imagettes/imagettes%d/%d.pgm",nH_bloc,indiceBestImagette);
			cout << name_img << " " << i << " " << j << " " << endl;
			lire_image_pgm(name_img, bestImagette, nH_bloc * nW_bloc);

			blocToImage(currentAssign,bestImagette,i,j,nW,nH_bloc,nW_bloc);

			Assignement* possibleBestAssign=imagetteMosaic_moy(imageIn,nH,nW,copieAssign(currentAssign));

			if(bestAssign->score>possibleBestAssign->score){
				detruireAssign(bestAssign);
				bestAssign=possibleBestAssign;
				if(bestAssign->score<globalBestScore)
					globalBestScore=bestAssign->score;
			}
			else{
				detruireAssign(possibleBestAssign);
			}
		}
		
		currentAssign->score-=score;
		undoChange(currentAssign,i,j,nW,nH_bloc,nW_bloc);
		free(currentImagette);
		free(bestImagette);
		free(bloc);
	}
	
	
	detruireAssign(currentAssign);
	return bestAssign;
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
	
	allocation_tableau(ImgOut, OCTET, nH*nW);
	char name_img[250];
	std::cout << nW << " " << nH << endl;
	/*
	Assignement *init=newAssign();
	for(int i=32*3;i<512;i++){
		for(int j=0;j<512;j++){
			init->matriceColorie[i][j]=true;
		}
	}
	for(int i=0;i<512;i++){
		for(int j=32*3;j<512;j++){
			init->matriceColorie[i][j]=true;
		}
	} 
	globalBestScore=std::numeric_limits<double>::max();
	Assignement *assign=imagetteMosaic_moy(ImgIn,nH,nW,init);
	
	printf("Copie finie %lf\n",sqrt(globalBestScore));
	ecrire_image_pgm(cNomImgEcrite, assign->image, nW, nH);


	free(ImgIn);*/
	OCTET *bloc;
	allocation_tableau(bloc,OCTET,16*16);
	for(int i=0;i<32;i++){
		for(int j=0;j<32;j++){
			imageToBloc(ImgIn,bloc,i*16,j*16,nW,16,16);
			
			printf("%lf ",ecartType(bloc,16,16));
		}
		printf("\n");
	}


	return 1;
}

