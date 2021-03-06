/* ############################################################################
 * ############################################################################
 * #######################   DIFESA DELLA CITTADELLA   ########################
 * ############################################################################
 * ###########   Progetto Informatica Grafica AA 2009/2010   ##################
 * ############################################################################
 * ############################################################################
 * ####################### Stefano Bonetta - 584119 ###########################
 * ############################################################################
 * ############################################################################
 */

//MAIN FILE

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <GL/glut.h>
#include <vector>
#include <string.h>
#include <sstream>

#include "Obj.h"
#include "Sector.h"
#include "Building.h"
#include "Bomb.h"
#include "Cannon.h"
#include "Projectile.h"
#include "Explosion.h"

using namespace std;
//void CambiaDim(int, int);
//void DrawScene();
//void TastoPremuto(unsigned char, int, int);

/* ################################## GLOBALI ################################## */
//PiGreco
const float PI = 3.14159265;

//Risoluzione
int userW;
int userH;

//Stato Gioco avviato
bool gamesStarted;
//Stato Gioco finito
bool gamesEnded;
//Stato help visualizzato
bool helpShowed;
//Stato pausa
bool paused;
//Stato antialiasing
bool antialiasing;

//recoil del cannone
float recoil;
//tempo di ricarica
float recoilTime;
//vita globale della cittadella
float life;
//vita iniziale della cittadella
float initial;
//punteggio del giocatore
int score;
char stringscore[32];

//tempo di gioco
float gametime;
//coefficiente per determinare il tempo massimo di gioco
float timecoeff;

//La cittadella (ovvero un vettore di Settori)
vector<Sector*> myCitadel;
//Le bombe in volo
vector<Bomb*> myBombs;
//I proiettili in volo
vector<Projectile*> myAmmo;
//Le esplosioni correnti
vector<Explosion*>myExplosions;
//Il cannone
Cannon* myCannon;

//variabile per il calcolo del delta-t che intercorre tra la chiamata n e n+1 di GlutIdleFunction
long delta_t;

//Textures
vector<GLubyte*> Textures;

//Osservatore
float ossX, ossY, ossZ;
float ossA, ossB;

//Parametro per il frustum
float pNear=0.5;

/*#######################################################################################*/
/*##############################  Support Functions  ####################################*/

//metodo che abilita/disabilita Antialias (BETA)
void swapAntialias(){
	antialiasing=!antialiasing;
	if(antialiasing){
		//Antialiasing
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);

		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	}
	else {
		glDisable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
	}
}

//Reset del materiale
void clearMaterial(){
	//Variabili per definire materiali
	GLfloat ambiente[4] =  { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat direttiva[4] =  { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat brillante[4] = { 1.0f, 1.0f,1.0f, 1.0f };
	glMateriali(GL_FRONT, GL_SHININESS, 32);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambiente);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, direttiva);
	glMaterialfv(GL_FRONT, GL_SPECULAR, brillante);
}

//Disegna una scritta (BitmapMode)
void printBitmapString(void* font, char* s)
{
   if (s && strlen(s)) {
      while (*s) {
         glutBitmapCharacter(font, *s);
         s++;
      }
   }
}

//Disegna una scritta (StrokeMode)
void printStrokeString(void* font, char* s)
{
   if (s && strlen(s)) {
      while (*s) {
         glutStrokeCharacter(font, *s);
         s++;
      }
   }
}

//Metodo che aggiorna il punteggio
void updateScore(int delta){
	score += delta;
	sprintf(stringscore,"%d",score); // string result '65' is stored in char array cVal
}

//Display LOSE
void youLose(){
	gamesEnded = true;

	glPushMatrix();
		glTranslatef(-35,230,0);
		glColor3f(0.9f,0.1f,0.1f);
		glRasterPos2f(0,0);
		printBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,"YOU LOSE");
	glPopMatrix();

}

//Display WIN
void youWin(){
	gamesEnded = true;

	glPushMatrix();
		glTranslatef(-25,240,0);
		glColor3f(0.1f,0.9f,0.1f);
		glRasterPos2f(0,0);
		printBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,"YOU WIN");

		glTranslatef(-5,-40,0);
		glRasterPos2f(0,0);
		printBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,"Total score");
		glRasterPos2f(20,-20);
		printBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,stringscore);
	glPopMatrix();

}

//Disegna il menu di avvio
void drawMenu(){

	glPushMatrix();
	glTranslatef(-75,245,0);
	glColor3f(0.1f,0.9f,0.1f);
	glRasterPos2f(0,0);
	printBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,"Defense Of The Citadel");

	glTranslatef(30,-50,0);
	glRasterPos2f(0,0);
	printBitmapString(GLUT_BITMAP_HELVETICA_18,"Start game (s)");

	glTranslatef(0,-20,0);
	glRasterPos2f(0,0);
	printBitmapString(GLUT_BITMAP_HELVETICA_18,"Pause (p)    ");

	glTranslatef(0,-20,0);
	glRasterPos2f(0,0);
	printBitmapString(GLUT_BITMAP_HELVETICA_18,"Help me (h)   ");

	glTranslatef(0,-20,0);
	glRasterPos2f(0,0);
	printBitmapString(GLUT_BITMAP_HELVETICA_18,"Exit game (Esc)");

	glPopMatrix();
}

//Disegna gli aiuti contestuali
void drawHelp(){

	glPushMatrix();
		glTranslatef(20,-70,0);
		glColor3f(0.1f,0.9f,0.9f);
		glRasterPos2f(0,0);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"Move the cannon by MOVING your mouse.");

		glBegin(GL_LINES);
			glVertex2f(-2,-5);
			glVertex2f(-20,-60);
		glEnd();

		glTranslatef(0,-20,0);
		glRasterPos2f(0,0);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"Use LEFT mouse click to shoot.");

		glTranslatef(0,-20,0);
		glRasterPos2f(0,0);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"Press 'x' to toggle the targeting system.");
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-320,-180,0);
		glColor3f(0.1f,0.9f,0.9f);
		glRasterPos2f(0,0);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"Defend the citadel until time expires to win.");

		glBegin(GL_LINES);
			glVertex2f(15,-25);
			glVertex2f(-20,-50);
		glEnd();

		glTranslatef(0,-20,0);
		glRasterPos2f(0,0);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"Remember : don't open fire against buildings :)");
	glPopMatrix();


	glPushMatrix();
		glTranslatef(-320,180,0);
		glColor3f(0.1f,0.9f,0.9f);
		glRasterPos2f(0,0);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"Destroy the bombs dropped from the sky");
		glRasterPos2f(0,-20);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"to defend the citadel and get score.");

		glBegin(GL_LINES);
			glVertex2f(55,15);
			glVertex2f(80,50);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(230,-180,0);
		glColor3f(0.1f,0.9f,0.9f);
		glRasterPos2f(0,0);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"Watch at the cannon-monitor ");
		glRasterPos2f(0,-20);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"to adjust your shooting!");

		glBegin(GL_LINES);
			glVertex2f(55,-25);
			glVertex2f(25,-45);
		glEnd();
	glPopMatrix();

}

//disegna "pause"
void drawPause(){
	glPushMatrix();
		glTranslatef(-30,10,0);
		glColor3f(0.1f,0.9f,0.1f);
		glRasterPos2f(0,0);
		printBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,"PAUSED");
		glRasterPos2f(-20,-20);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"(press 'p' to continue)");
	glPopMatrix();
}

/*#######################################################################################*/
/*###########################  CALLBACK Functions #######################################*/


/*#################### CALLBACK gestione input da tastiera #####################*/
void KeyHandler(unsigned char t, int, int)
{

	//Esci dal gioco
	if(t == 27) exit(0);

	//inizia a giocare
	else if(t == 's' && !gamesStarted){
		gamesStarted=true;
		helpShowed=false;
		myCannon->swapTargetingEnabled();
	}

	//toggle help
	else if(t == 'h'){
		helpShowed = ! helpShowed;
	}

	//toggle pause
	else if(t == 'p'){
		paused = !paused;
	}

	//toggle antialias
	else if(t == 'a'){
		swapAntialias();
	}

	//Toggle del mirino
	else if(t == 'x') myCannon->swapTargetingEnabled();


	glutPostRedisplay();		//richiesta di ridisegnare la finestra
}

/*#################### CALLBACK resizing #####################*/
void CambiaDim(int w, int h)
{
	glViewport(0,0,w,h);
	//glViewport(0,100,w,h);
}

/*#################### CALLBACK progress del gioco #####################*/
void Progress(){
	//Check: gioco avviato? gioco in pausa?
	if(!gamesStarted || gamesEnded)return;
	else if(paused){
		delta_t=glutGet(GLUT_ELAPSED_TIME);
		return;
	}

	//Life check
	life=0;
	for(register unsigned int i=0; i<myCitadel.size();i++){
		life+=myCitadel.at(i)->getBuildingsLife();
	}
	if(life==0){gamesEnded=true;/* GAME OVER*/}


	/*--- Ammo collision ---*/
	register float impactDistance = 1.0f;
	register float distanceAB;
	register float distanceAC;
	register float distanceBC;
	register float semi;
	register float h;

	for(register unsigned int i=0;i<myAmmo.size();i++){
		myAmmo.at(i)->move(glutGet(GLUT_ELAPSED_TIME)-delta_t);
		register bool die=false;

		//-------->with bombs
		distanceAB =sqrt( pow(myAmmo.at(i)->getX() -myAmmo.at(i)->getOldX(),2) + pow(myAmmo.at(i)->getY() - myAmmo.at(i)->getOldY(),2) + pow(myAmmo.at(i)->getZ() - myAmmo.at(i)->getOldZ(),2) );
		for(register unsigned int j=0;j<myBombs.size();j++){
			//Si adotta un tipo di check-collision diverso a seconda della situazione, per risolvere il problema del FRAME-SKIP!

			//Metodo dell'altezza del triangolo (sfrutta Erone, è più pesante ma garantisce un check affidabile)
			if(myBombs.at(j)->getZ() >= myAmmo.at(i)->getZ() && myBombs.at(j)->getZ() <= myAmmo.at(i)->getOldZ() ){
				distanceAC = sqrt( pow(myAmmo.at(i)->getX() - myBombs.at(j)->getX(),2) + pow(myAmmo.at(i)->getY() - myBombs.at(j)->getY(),2) + pow(myAmmo.at(i)->getZ() - myBombs.at(j)->getZ(),2) );
				distanceBC = sqrt( pow(myAmmo.at(i)->getOldX() - myBombs.at(j)->getX(),2) + pow(myAmmo.at(i)->getOldY() - myBombs.at(j)->getY(),2) + pow(myAmmo.at(i)->getOldZ() - myBombs.at(j)->getZ(),2) );
				semi = (distanceAB+distanceAC+distanceBC)/2.0f;
				h = pow( (2*semi*(semi-distanceAB)*(semi-distanceAC)*(semi-distanceBC))/distanceAB , 2);
				//Lavoro volutamente coi quadrati, per risparmiare un controllo sotto e per evitare la sqrt nel caso base (sqrt è pesante)
			}
			else{//Metodo della distanza semplice
				h = pow(myAmmo.at(i)->getX() - myBombs.at(j)->getX(),2) + pow(myAmmo.at(i)->getY() - myBombs.at(j)->getY(),2) + pow(myAmmo.at(i)->getZ() - myBombs.at(j)->getZ(),2);
				//h = sqrt( pow(myAmmo.at(i)->getX() - myBombs.at(j)->getX(),2) + pow(myAmmo.at(i)->getY() - myBombs.at(j)->getY(),2) + pow(myAmmo.at(i)->getZ() - myBombs.at(j)->getZ(),2) );
				//Evito la sqrt per velocizzare e lavoro coi quadrati
			}

			if(h <=impactDistance){
				myExplosions.push_back(new Explosion(myAmmo.at(i)->getX(),myAmmo.at(i)->getY(),myAmmo.at(i)->getZ()));
				delete myBombs.at(j);
				delete myAmmo.at(i);
				myBombs.erase(myBombs.begin()+j);
				myAmmo.erase(myAmmo.begin()+i);
				j = myBombs.size();
				i--;
				die=true;
				updateScore(5);
			}
		}
		//---------->with buildings
		register int toDestroy=-1;
		register float nearest=-100;
		for(register unsigned int k=0;k<myCitadel.size()&& !die;k++){
			Sector* checkSect = myCitadel.at(k);
			for(register unsigned int m=0; m < checkSect->getBuildings().size(); m++){

				register float impactDistance = myAmmo.at(i)->getDimX() + checkSect->getBuildings().at(m)->getR()*sqrt(2);
				register float effectiveDistance = sqrt(
											  pow(myAmmo.at(i)->getX()-checkSect->getBuildings().at(m)->getX() ,2) +
											  //pow(this->getY()-others.at(i)->getY() ,2) +
											  pow(myAmmo.at(i)->getZ()-checkSect->getBuildings().at(m)->getZ() ,2));

				if(effectiveDistance <= impactDistance && myAmmo.at(i)->getY() <= checkSect->getBuildings().at(m)->getL()*2){
					if(checkSect->getBuildings().at(m)->getZ() + checkSect->getBuildings().at(m)->getR() >= nearest){
						nearest = checkSect->getBuildings().at(m)->getZ() + checkSect->getBuildings().at(m)->getR();
						toDestroy=m;
				}}
			}

			if(toDestroy != -1){
				checkSect->getBuildings().at(toDestroy)->setL(checkSect->getBuildings().at(toDestroy)->getL()-0.5f);
				myExplosions.push_back(new Explosion(myAmmo.at(i)->getX(),myAmmo.at(i)->getY()+myAmmo.at(i)->getDimY(),checkSect->getBuildings().at(toDestroy)->getZ() + checkSect->getBuildings().at(toDestroy)->getR()));
				delete myAmmo.at(i);
				myAmmo.erase(myAmmo.begin()+i);

				if(checkSect->getBuildings().at(toDestroy)->getL() <= 0.0f){
					cout << "checkCollisione,dentro if" << endl;
					checkSect->removeBuilding(checkSect->getBuildings().at(toDestroy));
				}
				//others.at(i)->addRefer();
				//m=checkSect->getBuildings().size();
				k=myCitadel.size();
				i--;
				die=true;
				updateScore(-5);
			}

		}

		if(!die && (myAmmo.at(i)->getZ() <= -110 || myAmmo.at(i)->getY()<=0)){
			delete myAmmo.at(i);
			myAmmo.erase(myAmmo.begin()+i);
			i--;
		}
	}


	/* Collision bombe->edifici */
	for(register unsigned int i=0;i<myBombs.size();i++){
		myBombs.at(i)->move(glutGet(GLUT_ELAPSED_TIME)-delta_t);
		if(bool coll=myBombs.at(i)->checkCollision() || myBombs.at(i)->getY() <= 0){
			Bomb* check = myBombs.at(i);
			myBombs.erase(myBombs.begin()+i);

			//Aggiorno vita
			if(coll)life-=0.5f;

			//Aggiungo explosione
			myExplosions.push_back(new Explosion(check->getX(),check->getY()+check->getDimY(),check->getZ()));

			check->clearRef();
			delete check;
			i--;
		}
	}

	/* Gestione esplosioni */
	for(register unsigned int i=0;i<myExplosions.size();i++){
		myExplosions.at(i)->processExplosion(glutGet(GLUT_ELAPSED_TIME)-delta_t);
		if(myExplosions.at(i)->isFinished()){
			delete myExplosions.at(i);
			myExplosions.erase(myExplosions.begin()+i);
			i--;
		}

	}

	/* Lancio di nuove bombe su edifici casuali */
	register unsigned int newbombs = rand() % 100;
	if(myBombs.size()<=5 && newbombs >=95){
		register int quarter = rand()%6;
		//register int randomBuilding =
		Sector* tmpSector = myCitadel.at(quarter);
		Building* tmpBuilding = tmpSector->getRandomBuilding();
		if(tmpBuilding){
			Bomb* newBomb = new Bomb(tmpBuilding->getX(),25,tmpBuilding->getZ(),  tmpSector,tmpBuilding);
			myBombs.push_back(newBomb);
		}
	}

	//Aggiorno il tempo
	delta_t = glutGet(GLUT_ELAPSED_TIME);

	glutPostRedisplay();
}

/*################## CALLBACK di visualizzazione ##################*/
void DrawScene()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-0.4f, 0.4f, -0.25f, 0.25f,  pNear,2000);

	//Fonte di luce in alto a dx (simula la luna)
	GLfloat PosLite[4] = { 30.0f, 15.0f, -50.0f, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, PosLite);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Osservatore
	glRotatef(-ossB, 1, 0, 0); //imp per prima..
	glRotatef(-ossA, 0, 1, 0); //imp
	glTranslatef(-ossX,-ossY,-ossZ);

	//Variabili per definire materiali
	//GLfloat ambiente[4] = { 0.8f, 0.8f, 0.6f, 1 };
	//GLfloat direttiva[4] = { 0.7f, 0.7f, 0.7f, 1 };
	//GLfloat brillante[4] = { 0.8f, 0.8f, 0.8f, 1 };

	//cannone
	clearMaterial();
	myCannon->drawMe();

	//Pavimento..
	register float a,z,delta;
	delta=7.0f;
	register int c;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 3);

	//Terreno
	clearMaterial();
	GLfloat scuro[4] = { 0.0f, 0.0f,0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, scuro);

	for(a=-80; a<80; a+=delta)
	{
		glBegin(GL_QUAD_STRIP);
		for(c=0, z=5; z> -110; z-=delta,c++){

				if(c%2 == 0)glTexCoord2f(0,0);
				else glTexCoord2f(0,1);
				glNormal3f(0,1.0f,0);
				glVertex3f(a,0,z);

				if(c%2 == 0)glTexCoord2f(1,0);
				else glTexCoord2f(1,1);

				glNormal3f(0,1.0f,0);
				glVertex3f(a+delta,0,z);
		}
		glEnd();
	}

	//Cielo
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, 5);
	glBegin(GL_QUADS);
	 for(int i=0;i<3;i++){
			glNormal3f(0,0,0.0f);
			glTexCoord2f(0,1);
			glVertex3f(-120+i*80,0,-100);

			glNormal3f(0,0,0.0f);
			glTexCoord2f(1,1);
			glVertex3f(-120+(i+1)*80,0,-100);

			glNormal3f(0,0,0.0f);
			glTexCoord2f(1,0);
			glVertex3f(-120+(i+1)*80,40,-100);

			glNormal3f(0,0,0.0f);
			glTexCoord2f(0,0);
			glVertex3f(-120+i*80,40,-100);
	 }
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);


	//Disegno settori
	for(register unsigned int i=0; i< myCitadel.size();i++){
		if(i>2) myCitadel.at(i)->drawMe(1);
		else    myCitadel.at(i)->drawMe(0);
	}

	//Disegno cittadella
	glColor3f(1,0,1);
	for(register unsigned int i=0;i<myCitadel.size();i++){
		myCitadel.at(i)->drawBuildings();
	}

	//Disegna strade
	GLfloat ambiente[4] =  { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat direttiva[4] =  { 0.2f,0.2f, 0.2f, 1.0f };
	GLfloat brillante[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambiente);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, direttiva);
	glMaterialfv(GL_FRONT, GL_SPECULAR, brillante);

	int halfSect = myCitadel.size()/2;

	int roadA = 0;
	int roadB = 0;
	float roadY = 0.06f;

	for(int j=0;j<halfSect;j++){
		roadA +=  myCitadel.at(j)->getDimX();
		roadB +=  myCitadel.at(j+halfSect)->getDimX();
		if(j<halfSect-1){
			int interRoadUp   = max( myCitadel.at(j)->getDimZ(),myCitadel.at(j+1)->getDimZ() );
			int interRoadDown = max( myCitadel.at(j+halfSect)->getDimZ(),myCitadel.at(j+halfSect+1)->getDimZ() );

			glPushMatrix();
			glTranslatef(myCitadel.at(j+1)->getX()-0.25f,0,-40);
			glBegin(GL_QUADS);
				//Quartieri alti
				glColor3f(0, 0, 0);
				glVertex3f(0, roadY, 0);
				glVertex3f(0.5f, roadY, 0);
				glVertex3f(0.5f, roadY, -1*interRoadUp);
				glVertex3f(0, roadY, -1*interRoadUp);
			glEnd();
			glPopMatrix();
			glPushMatrix();
			glTranslatef(myCitadel.at(j+halfSect+1)->getX()-0.25f,0,-40);
			glBegin(GL_QUADS);
				//Quartieri bassi
				glColor3f(0, 0, 0);
				glVertex3f(0, roadY, 0);
				glVertex3f(0, roadY, interRoadDown);
				glVertex3f(0.5f, roadY, interRoadDown);
				glVertex3f(0.5f, roadY, 0);
			glEnd();
			glPopMatrix();
		}
	}
	int road = max(roadA,roadB);
	glPushMatrix();
	glTranslatef(-15,0,-39.5f);

	glBegin(GL_QUADS);
		glColor3f(0, 0, 0);
		glVertex3f(0, roadY, 0);
		glVertex3f(road, roadY, 0);
		glVertex3f(road, roadY, -0.5f);
		glVertex3f(0, roadY, -0.5f);
	glEnd();
	glPopMatrix();

	//Bombe
	for(register unsigned int i=0; i< myBombs.size(); i++)
		myBombs.at(i)->drawMe();

	//Proiettili
	for(register unsigned int i=0; i< myAmmo.size(); i++)
		myAmmo.at(i)->drawMe();

	//Esplosioni
	for(register unsigned int i=0; i< myExplosions.size(); i++){
		myExplosions.at(i)->drawMe();
	}


    /* ###############  2D WIDGET  ################# */
	//glPushMatrix();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-400,400, -300,300, -1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	register float posX  =-360.0f;
	register float posY  =-290.0f;
	register float size = 100.0f;

	//Contenitori
	clearMaterial();
	//Sx
	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glTranslatef (posX, posY, -0.1);
		glBindTexture(GL_TEXTURE_2D, 11);
		glBegin(GL_QUADS);
				glNormal3f(1,0,0);
				glTexCoord2f(0,1);
				glVertex2f(-45.0f, -20.0f);
				glTexCoord2f(0,0);
				glVertex2f(-45.0f, 85.0f);
				glTexCoord2f(0.7f,0);
				glVertex2f(size+55, 85.0f);
				glTexCoord2f(1,1);
				glVertex2f(size+155, -20.0f);

		glEnd();

		glBindTexture(GL_TEXTURE_2D, 10);
		glBegin(GL_QUADS);
				glNormal3f(1,0,0);
				glTexCoord2f(0,1);
				glVertex2f(-45.0f, -20.0f);
				glTexCoord2f(0,0);
				glVertex2f(-45.0f, 80.0f);
				glTexCoord2f(0.7f,0);
				glVertex2f(size+50, 80.0f);
				glTexCoord2f(1,1);
				glVertex2f(size+150, -20.0f);
		glEnd();
	glPopMatrix();

	//Dx
	posX=360.0f;
	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glTranslatef (posX, posY, -0.1);
		glBindTexture(GL_TEXTURE_2D, 11);
		glBegin(GL_QUADS);
				glNormal3f(-1,0,0);
				glTexCoord2f(0,1);
				glVertex2f(45.0f, -20.0f);
				glTexCoord2f(0,0);
				glVertex2f(45.0f, 85.0f);
				glTexCoord2f(0.7f,0);
				glVertex2f(-size-55, 85.0f);
				glTexCoord2f(1,1);
				glVertex2f(-size-155, -20.0f);

		glEnd();

		glBindTexture(GL_TEXTURE_2D, 10);
		glBegin(GL_QUADS);
				glNormal3f(-1,0,0);
				glTexCoord2f(0,1);
				glVertex2f(45.0f, -20.0f);
				glTexCoord2f(0,0);
				glVertex2f(45.0f, 80.0f);
				glTexCoord2f(0.7f,0);
				glVertex2f(-size-50, 80.0f);
				glTexCoord2f(1,1);
				glVertex2f(-size-150, -20.0f);
		glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);


	//Menu
	if(!gamesStarted) drawMenu();

	//End game
	if(gamesEnded){
		if(life <=0)youLose();
		else youWin();
	}
	//Help
	if(helpShowed) drawHelp();

	//Pause
	if(paused) drawPause();

	//Vita
	posX=-360.0f;
	size=80.0f;
	glPushMatrix();
		glTranslatef (posX, posY, 0.0);
		glBegin(GL_QUADS);
			glColor3f(0.1f,0.1f,0.1f);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(0.0f, 20.0f);
			glVertex2f(size, 20.0f);
			glVertex2f(size, 0.0f);

			glColor3f(0.1f,0.9f,0.1f);
			glVertex2f(5.0f, 3.0f);
			glVertex2f(5.0f, 17.0f);
			glVertex2f(5.0f+(size-10.0f)*life/initial, 17.0f);
			glVertex2f(5.0f+(size-10.0f)*life/initial, 3.0f);
		glEnd();

		glRasterPos2f(-30,5);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"LIFE");
	glPopMatrix();

	//Tempo
	posY = -250.0f;
	glPushMatrix();
		glTranslatef (posX, posY, 0.0);
		glBegin(GL_QUADS);

			glColor3f(0.1f,0.1f,0.1f);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(0.0f, 20.0f);
			glVertex2f(size, 20.0f);
			glVertex2f(size, 0.0f);

			glColor3f(0.2f,0.7f,0.7f);
			glVertex2f(5.0f, 3.0f);
			glVertex2f(5.0f, 17.0f);
			glVertex2f(5.0f+(size-10.0f)*gametime/(initial*timecoeff), 17.0f);
			glVertex2f(5.0f+(size-10.0f)*gametime/(initial*timecoeff), 3.0f);
		glEnd();

		glRasterPos2f(-32,5);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"TIME");
	glPopMatrix();

	//Score
	posX=-260.0f;
	glPushMatrix();
		glTranslatef (posX, posY, 0.0);
		glColor3f(0.7f,0.7f,0.1f);
		glRasterPos2f(0,5);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"SCORE");
		glRasterPos2f(6,-20);
		printBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,stringscore);
	glPopMatrix();

	//InfoCannone
	posX=260.0f;
	posY=-258.0f;
	size=35.0f;
	glPushMatrix();
		glTranslatef (posX, posY, 0.1);
		glColor3f(0.1f,0.1f,0.1f);
		glBegin(GL_QUADS);
			glVertex3f(-size,size,0);
			glVertex3f(size,size,0);
			glVertex3f(size,-size,0);
			glVertex3f(-size,-size,0);

			size=30.0f;
			glColor3f(0.1f,0.1f,0.2f);
			glVertex3f(-size,size,0);
			glVertex3f(size,size,0);
			glVertex3f(size,-size,0);
			glVertex3f(-size,-size,0);
		glEnd();

		glColor3f(0.0f,1.0f,0.0f);
		glBegin(GL_LINES);
			glVertex3f(0,size,0);
			glVertex3f(0,-size,0);

			glVertex3f(-size,0,0);
			glVertex3f(size,0,0);

			glColor3f(1,1,1);
			//glVertex3f(0,0,0);
			glVertex3f(cos((myCannon->getrZ()+90.0f)/180.0f*PI)*size*2,sin((myCannon->getrX()+90.0f)/180.0f*PI)*size*2,0);
			glVertex3f(cos((myCannon->getrZ()+90.0f)/180.0f*PI)*size*2,0,0);

			glVertex3f(cos((myCannon->getrZ()+90.0f)/180.0f*PI)*size*2,sin((myCannon->getrX()+90.0f)/180.0f*PI)*size*2,0);
			glVertex3f(0,sin((myCannon->getrX()+90.0f)/180.0f*PI)*size*2,0);
		glEnd();

		glColor3f(1,0,0);
		glTranslatef(cos((myCannon->getrZ()+90.0f)/180.0f*PI)*size*2,sin((myCannon->getrX()+90.0f)/180.0f*PI)*size*2,0.01f);
		glutSolidSphere(2.0f,10,10);

	glPopMatrix();

	//Recoil
	posX=320.0f;
	posY=-240.0f;
	size=50;
	glPushMatrix();
		glTranslatef (posX, posY, 0.1);
		glBegin(GL_QUADS);
			glColor3f(0.1f,0.1f,0.1f);
			glVertex2f(15.0f, 0.0f);
			glVertex2f(15.0f, -size);
			glVertex2f(45.0f, -size);
			glVertex2f(45.0f, 0.0f);

			//glColor3f(1/recoil,1-(1/recoil),0.1f);
			glColor3f(1/recoil,0.1f,0.1f);
			glVertex2f(20.0f, -5.0f);
			glVertex2f(20.0f, -size+5);
			glVertex2f(40.0f, -size+5);
			glVertex2f(40.0f, -5.0f);
		glEnd();

		glColor3f(0.8f,0.1f,0.1f);
		glRasterPos2f(10,13);
		printBitmapString(GLUT_BITMAP_HELVETICA_18,"RECOIL");

	glPopMatrix();

	//Luna
	glPushMatrix();
		glTranslatef(300,250,0.1f);

		glEnable(GL_BLEND);

		glColor3f(1,1,1);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 14);

		glBlendFunc(GL_DST_COLOR,GL_ZERO);

		//Applico la maschera per la trasparenza
		register float radius=50;
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(-radius,radius,0);
			glTexCoord2f(1,0);
			glVertex3f(radius,radius,0);
			glTexCoord2f(1,1);
			glVertex3f(radius,-radius,0);
			glTexCoord2f(0,1);
			glVertex3f(-radius,-radius,0);
		glEnd();

		// Disegno la sprite
		glTranslatef(0,0,0.1f);
		glBindTexture(GL_TEXTURE_2D, 15);
		glBlendFunc(GL_ONE,GL_ONE);

		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex3f(-radius,radius,0);
		glTexCoord2f(1,0);
		glVertex3f(radius,radius,0);
		glTexCoord2f(1,1);
		glVertex3f(radius,-radius,0);
		glTexCoord2f(0,1);
		glVertex3f(-radius,-radius,0);
		glEnd();

		glDisable(GL_BLEND);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glutSwapBuffers();
}


/*#################### CALLBACK gestione mouse click #####################*/
void processMouseAction (int button, int state, int x, int y){
	if(!gamesStarted || paused)return;
	//	buttons[button] = state;

	if(recoil >=recoilTime && button==0 && state ==0){//singolo mouseClickDown
		myCannon->setRecoil(-1.0f);
		recoil=0.0f;
		cout << "Sparato!!"<<endl;

		//Fattore di spostamento trasversale (vX)
		register float cosZ = -cos((myCannon->getrZ()+90.0f) /180.0f*PI);

		//Velocità di movimento lungo asse z (vZ)
		register float sinZ =  sin((myCannon->getrZ()+90.0f) /180.0f*PI);

		//Fattore di spostamento verticale (vY)
		register float sinX = sin((myCannon->getrX()+90.0f) /180.0f*PI);
		register float cosX = cos((myCannon->getrX()) /180.0f*PI);

		//cout << "Debug: angolo=" <<  myCannon->getrX()+90.0f << "  rad= " << ((myCannon->getrX()+90.0f) /180.0f*PI)<< "  sin=" << sinX << endl;
		//cout << "Projectile angle,cos:" <<myCannon->getrZ()+90.0f <<" "<< cosZ << endl;
		//cout << "Debug sinZ " << senZ << endl;
		register float vZ = (sinX>=0) ? sinX : -sinX; //max(sinX,-sinX);
		//cout << "Debug VZ " << vZ << "   asasd " << sinZ-vZ << endl;
		//cout << "Debug cosX " << vZ << "   cosX " << cosX << endl;
		myAmmo.push_back(new Projectile(myCannon->getX(),myCannon->getY(),myCannon->getZ(),  cosZ,-sinX, sinZ ));
		/* Volendo, si dovrebbe migliorare le precisione del calcolo della vZ*/
	}

}

/*############# CALLBACK gestione eventi strettamente temporali #################*/
void processTimedOperation(int i=0){
	if(gametime<=0.0f)gamesEnded=true;
	if(!paused && !gamesEnded){
		//gametime -= 10.1f;
		gametime -= 0.1f;
		myCannon->targetingAnimation();

		//Rinculo del cannone
		if(recoil < recoilTime){
			myCannon->setRecoil(-(1-recoil/recoilTime));
			recoil += 1.0f;
		}
	}
	glutTimerFunc(30,processTimedOperation,1);
}


/*############# CALLBACK gestione movimenti del mouse #################*/
void processMouseActiveMotion(int x, int y){
	if(!gamesStarted)return;

	//Muove il cannone
	//cout<< "Mouse pos (x,y):"<< x << "," <<y <<endl;
	register float halfScreen = userW/2;//640.0f;
	register float myVal = ((x-halfScreen)/halfScreen);
	register float angle = acos(myVal)*180.0f/PI - 90.0f;
	cout << "ANGLE = " << angle << endl;

	if(angle>28.0f)angle=28.0f;
	else if(angle <-28.0f)angle=-28.0f;
	//cout << "Mouse acos in gradi = " << angle << endl;
	myCannon->setrZ(angle);

	halfScreen = userH/2;//400.0f;
	myVal = ((y-halfScreen)/halfScreen);
	angle = acos(myVal)*180.0f/PI - 180.0f;

	if(angle<-112.0f)angle=-112.0f;
	else if(angle >-75.0f)angle=-75.0f;

	//cout << "Mouse acos in gradi = " << angle << endl;
	myCannon->setrX(angle);
}

/*#######################################################################################*/
/*##############################  Sturtup Functions  ####################################*/

//Carica le texture
int loadTextures(){

	{
			GLubyte Texture1[256 * 256 * 3];
			FILE *fHan = fopen("img/metal4.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture1, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture2[256 * 256 * 3];
			fHan = fopen("img/sector2.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture2, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture3[256 * 256 * 3];
			fHan = fopen("img/rock.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture3, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture4[256 * 256 * 3];
			fHan = fopen("img/sky1.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture4, 256 * 256, 3, fHan);
			fclose(fHan);


			GLubyte Texture5[1024 * 512 * 3];
			fHan = fopen("img/univ3.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture5, 1024 * 512, 3, fHan);
			fclose(fHan);

			GLubyte Texture6[256 * 256 * 3];
			fHan = fopen("img/wall4.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture6, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture7[256 * 256 * 3];
			fHan = fopen("img/wall8.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture7, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture8[256 * 256 * 3];
			fHan = fopen("img/wall7.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture8, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture9[256 * 256 * 3];
			fHan = fopen("img/wall6.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture9, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture10[256 * 256 * 3];
			fHan = fopen("img/metal3.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture10, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture11[256 * 256 * 3];
			fHan = fopen("img/metal.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture11, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture12[256 * 256 * 3];
			fHan = fopen("img/sprite.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture12, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture13[256 * 256 * 3];
			fHan = fopen("img/spritemask.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture13, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture14[256 * 256 * 3];
			fHan = fopen("img/moonmask.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture14, 256 * 256, 3, fHan);
			fclose(fHan);

			GLubyte Texture15[256 * 256 * 3];
			fHan = fopen("img/moon.raw", "rb");
			if(fHan == NULL) return(0);
			fread(Texture15, 256 * 256, 3, fHan);
			fclose(fHan);

			Textures.push_back(Texture1);
			Textures.push_back(Texture2);
			Textures.push_back(Texture3);
			Textures.push_back(Texture4);
			Textures.push_back(Texture5);
			Textures.push_back(Texture6);
			Textures.push_back(Texture7);
			Textures.push_back(Texture8);
			Textures.push_back(Texture9);
			Textures.push_back(Texture10);
			Textures.push_back(Texture11);
			Textures.push_back(Texture12);
			Textures.push_back(Texture13);
			Textures.push_back(Texture14);
			Textures.push_back(Texture15);
	}

	for(unsigned int i=0; i<Textures.size(); i++){
		glBindTexture(GL_TEXTURE_2D, i+1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE); //GL_REPLACE//GL_MODULATE se vogliamo le luci

		if(i==4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, Textures.at(i));
		else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, Textures.at(i));
	}

	return 1;
}

//Inizializzazione del gioco
void initializeGame(){
	/* Settori della città
	 *
	 * 1  2  3
	 * 4  5  6
	 *
	 */
	//coord x da cui partire a generare i settori
	int startX = -15;
	//asse centrale della citta
	int middle = -40;
	//estensione della città
	int citysize = 30;
	//max altezza settore
	int sectorH = 25;

	int sectorPerLine = 3;

	int xIterator = startX;
	int yIterator = citysize;

	//Globals initialization
	gamesStarted=false;
	gamesEnded=false;
	helpShowed=false;
	paused=false;
	antialiasing=false;
	life=0;
	score=0;
	updateScore(0);
	gametime=0;
	timecoeff=20.0f;
	recoilTime=20.0f;
	recoil=recoilTime;

	/*
	 * Costruisco i quartieri con coordinate assolute, questo mi facilita la gestione di eventi
	 * che si verificano lungo i confini tra i vari quartieri e all'interno di essi
     */
	cout<< "## Building city... "<<endl;

	for(int j=0; j<sectorPerLine*2; j++){

		if(j==sectorPerLine){
			xIterator = startX;
			yIterator = citysize;
		}

		int myWidth;
		if(j==sectorPerLine-1 || j==sectorPerLine*2 -1)
			{
				myWidth  =  citysize/2 - xIterator;
				if(myWidth<=0)myWidth=7;
			}
		else
			 myWidth  =  min(11,max(rand() % max((yIterator-7),1),7));
			 //myWidth  =  min(10,max(rand() % max((yIterator-5) + 5,1),5));


		int myHeight =  max(rand() % max((sectorH-7),1),7);

		//Creo il settore
		Sector* newSector = new Sector(xIterator,0,middle, myWidth,0,myHeight);

		//Creo degli edifici nel settore (un numero casuale, scalato in base alle dimensioni del settore)
		int limiter = (myWidth * myHeight)/10;
		int toBuild = rand()%limiter + 1;

		int meno=1;
		if(j>2)
			meno=-1;

		for(int k=0; k < toBuild; k++){
			int relPosX = (rand() % max((myWidth-6),1))+3 + newSector->getX();
			int relPosZ = meno*((rand() % max((myHeight-6),1))+3) - newSector->getZ();
			int myrY = rand() % 360;

			Building* newBuilding = new Building(relPosX,0,-1*relPosZ, 0,myrY,0);
			life+=newBuilding->getL();
			newSector->addBuilding(newBuilding);
			//myObjCitadel.push_back(*newBuilding);
		}
		cout<< "\n Limiter   for sector " << j << " : " << limiter << endl;
		cout<< " Effective for sector " << j << " : " << toBuild << endl;

		//Aggiungo settore alla cittadella
		myCitadel.push_back(newSector);

		xIterator += myWidth;
		yIterator -= myWidth;
	}

	//Imposto parametri di gioco, in base alla dimensione della citta appena creata
	initial=life;
	gametime=life*timecoeff;

	//Creo cannone
	myCannon = new Cannon();

	//Posizione iniziale dell'osservatore
	ossY=13.3f;
	ossZ=1;
	ossX=0;
	ossB=-17;

	//Inizializzo il tempo
	delta_t= glutGet(GLUT_ELAPSED_TIME);

}


/*#######################################################################################*/
/*#################################   Main   ############################################*/

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	/*Avvio in fullscreen con risoluzion impostata da linea di comando (Default 1280x800)*/
	char* w="1280";
	char* h="800";

	if(argc==3){
		w=argv[1];
		h=argv[2];
		cout << "New resoltion " << w << "x" << h << endl;
	}
	else{
		cout << endl << "Game started with default resolution (1280x800)." << endl <<  "Use the two first line parameters to specify a different resolution (w x h)" << endl;
	}

	userW = atoi(w);
	userH = atof(h);

	//Costruisco la "gamestring" da passare alla callback per la modalità fullscreen-gamemode
	char myResolution[20];
	strcpy (myResolution,"");
	strcat(myResolution,w);
	strcat(myResolution,"x");
	strcat(myResolution,h);
	strcat(myResolution,":16");

	glutGameModeString(myResolution);
	glutEnterGameMode();

	/* Callback binding */
	glutReshapeFunc(CambiaDim);
	glutDisplayFunc(DrawScene);
	glutKeyboardFunc(KeyHandler);

	//(Cattura movimenti mouse)
	glutMouseFunc(processMouseAction);
	glutMotionFunc(processMouseActiveMotion);
	glutPassiveMotionFunc(processMouseActiveMotion);

	//Timed function
	glutTimerFunc(50,processTimedOperation,1);
	glutIdleFunc(Progress);

	/* end callback binding */

	// Zbuffer ed smartdrawing
	glEnable(GL_DEPTH_TEST); //abilita zbuffer
	glEnable(GL_CULL_FACE);

	//Lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	GLfloat black[4] = { 0.0f, 0.0f, 0.0f, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	//Luce
	GLfloat aLite[4] = { 0.3f, 0.3f, 0.3f, 1 };
	GLfloat dLite[4] = { 1.0f, 1.0f, 1.0f, 1 };
	GLfloat sLite[4] = { 1.0f, 1.0f, 1.0f, 1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, aLite);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dLite);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sLite);


	//Disabilita il cursore del mouse
	glutSetCursor(GLUT_CURSOR_NONE);

	//inizializza random seed
	srand ( time(NULL) );

	//Carica textures
	if(!loadTextures()){
		cout << "Problemi con il caricamento delle textures." << endl;
		return(0);
	}

	//Inizializza il gioco
	initializeGame();

	//GLUT mainloop
	glutMainLoop();
	return(0);
}
