
#include "Building.h"

Building::Building(float _X,float _Y,float _Z,float _rX,float _rY,float _rZ) : Obj(_X,_Y,_Z,0,0,0,_rX,_rY,_rZ), DistructableObj(_X,_Y,_Z,0,0,0,_rX,_rY,_rZ) {
	randomizeDimension();

}

void Building::randomizeDimension(){
	R = (rand()%150+15)/100.0f;
	cout << "R:" << R;
}

void Building::drawMe(){
glMatrixMode(GL_MODELVIEW);
glPushMatrix();

/*
GLfloat ambiente[4] = {0.8, 0.0, 0.8, 1};
GLfloat direttiva[4] = {0.8, 0.0, 0.8, 1};
GLfloat brillante[4] = {1.0, 1.0, 1.0, 1};
glMateriali(GL_FRONT, GL_SHININESS, 32 ); //brillantezza

glMaterialfv(GL_FRONT, GL_AMBIENT, ambiente); //faccia anteriore della primitiva (c'è quella posteriore anche...)
glMaterialfv(GL_FRONT, GL_DIFFUSE, direttiva);
glMaterialfv(GL_FRONT, GL_SPECULAR, brillante);
*/
glTranslatef(getX(),getY()+getL(),getZ());
glRotatef(getrX(), 1, 0, 0);
glRotatef(getrY(), 0, 1, 0);
glRotatef(getrZ(), 0, 0, 1);

glBegin(GL_QUADS);

	glColor3f(1, 0, 1);
	glVertex3f(R, getL(), R);
	glVertex3f(-R, getL(), R);
	glVertex3f(-R, -getL(), R);
	glVertex3f(R, -getL(), R);

	glColor3f(0, 0, 1);
	glVertex3f(-R, getL(), -R);
	glVertex3f(R, getL(), -R);
	glVertex3f(R, -getL(), -R);
	glVertex3f(-R, -getL(), -R);

	glColor3f(1, 1, 0);
	glVertex3f(R, getL(), -R);
	glVertex3f(R, getL(), R);
	glVertex3f(R, -getL(), R);
	glVertex3f(R, -getL(), -R);

	glColor3f(0.5f, 0, 1);
	glVertex3f(-R, getL(), R);
	glVertex3f(-R, getL(), -R);
	glVertex3f(-R, -getL(), -R);
	glVertex3f(-R, -getL(), R);

	glColor3f(0, 1, 1);
	glVertex3f(R, getL(), -R);
	glVertex3f(-R, getL(), -R);
	glVertex3f(-R, getL(), R);
	glVertex3f(R, getL(), R);

	glColor3f(1, 0, 0);
	glVertex3f(-R, -getL(), -R);
	glVertex3f(R, -getL(), -R);
	glVertex3f(R, -getL(), R);
	glVertex3f(-R, -getL(), R);

glEnd();

//Bordi neri
/*
glColor3f(0, 0, 0);
glBegin(GL_LINE_LOOP); //"prima faccia" del cubo
glVertex3f(-1,-1,1);
glVertex3f(1,-1,1);
glVertex3f(1,1,1);
glVertex3f(-1,1,1);
glEnd();

glBegin(GL_LINE_LOOP); //seconda faccia del cubo
glVertex3f(-1,-1,-1);
glVertex3f(1,-1,-1);
glVertex3f(1,1,-1);
glVertex3f(-1,1,-1);
glEnd();

glBegin(GL_LINES);//I 4 segmenti che collegano le facce
	glVertex3f(-1,-1,-1);
	glVertex3f(-1,-1,1);
	glVertex3f(1,-1,-1);
	glVertex3f(1,-1,1);
	glVertex3f(1,1,-1);
	glVertex3f(1,1,1);
	glVertex3f(-1,1,-1);
	glVertex3f(-1,1,1);
glEnd();
*/
glPopMatrix(); //Con push e pop disaccoppio il disegno corrente dal resto del contesto

}

Building::~Building() {
	// TODO Auto-generated destructor stub
}
