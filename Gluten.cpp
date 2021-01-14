/*
Computer Graphics
Team #4
Final Project - OpenGL

Antonio Tarín Contreras, A01175875
Jorge Limón Cabrera, A01280734
Carlos Serret Heredia, A01281072
*/


// Libraries & Dependencies
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <string>
#include <math.h>
#include "Dir.h"
#include <stack>
#include <fstream>

using namespace std;

// Pointer to root node of the hierarchy
Dir *root;
int contY = 0;

// Define offset value for each printed node.
float offsetY = -2;
float offsetX = 6;

// XY position of the camera
float camX=0.0f, camY=1.0f;

// Object color definitions
float colorR, colorG, colorB;

// Name of input file
string File;

// Window properties
static void resize( int width, int height)
{
  const float ar = ( float ) width / ( float ) height;

  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Viewport definition
  glFrustum(-ar, ar, -1.0, 1.5, 1, 1000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity() ;
}


// Allow movement with arrow keys
void KeyboardResponse(int key, int xx, int yy)
{

  switch (key) {
    case GLUT_KEY_LEFT :
    camX -= 0.8;
    break;
    case GLUT_KEY_RIGHT :
    camX += 0.8;
    break;
    case GLUT_KEY_UP :
    camY += 0.8;
    break;
    case GLUT_KEY_DOWN :
    camY -= 0.8;
    break;
  }
}

// Prints a string to the screen
void Sprint(float x, float y, char *text) {
  int l,i;
  l=strlen(text); // see how many characters are in text string.
  glRasterPos2i(x, y); // location to start printing text

  for( i=0; i < l; i++) // loop until i is greater then l
  {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]); // Print a character on the screen
  }

}

// Creates a box
  void createBox(float locX, float locY, char *name, float colorR, float colorG, float colorB, double a) {

  glPushMatrix();
  glTranslated(locX,locY,-10); // Location parameters: (x,y,z) in units
  Sprint(1.2,-1,name);
  glRotated(90,1,0,0);
  glRotated(a,0,0,1);
  glColor3d(colorR,colorG,colorB); // Color parameters: (red,green,blue) from 0.00 to 1.00
  glutSolidCube(1); // Box occupies a single unit
  glPopMatrix();

}

// Creates a sphere
void createSphere(float locX, float locY, char *name, float colorR, float colorG, float colorB, double a) {
  glPushMatrix();
  glTranslated(locX,locY,-10); // Location parameters: (x,y,z) in units
  Sprint(1.2,-1,name);
  glRotated(90,1,0,0);
  glRotated(a,0,0,1);
  glColor3d(colorR,colorG,colorB); // Color parameters: (red,green,blue) from 0.00 to 1.00
  glutSolidSphere(0.6,16.0,16.0); // Parameters: (radius, slices, stacks)
  glPopMatrix();
}

// Creates a line
void createLine(float startX, float startY, float finalX, float finalY) {
  glLineWidth(2.5);
  glBegin(GL_LINES);
  glVertex3f(startX,startY,-10);  // Start location: Same as parent box
  glVertex3f(finalX,finalY,-10); // End location: Same as child box
  glColor3f(0.0,0.0,0.0);
  glEnd();
}

// Reads and parses text file, populates data structure.
void fillRoot() {
  string line;
  ifstream myfile (File);
  int level = 0;
  root = new Dir();
  root->level = 0;
  root->name = File.substr(0, File.find(".txt"));
  Dir *child;
  stack<Dir*> parent;

  parent.push(root);

  if (myfile.is_open())
  {

    while (getline(myfile,line)){

      // Get the level
      level = 1;
      while(line[0] == '>') {
        level++;
        line.erase(0,1);
      }

      // Create new child
      child = new Dir();
      child->level = level;
      child->name = line;

      // Remove leaf nodes from parent stack
      while(parent.top()->level >= level) {
        parent.pop();
      }

      // Add it to the structure
      parent.top()->children.push_back(child);
      parent.push(child);
    }
    myfile.close();
  }
}

// Calls object creation functions recursively, according to data structure contents.
void paintHierarchy(Dir* dir, double a){

  if (dir != NULL) {
    char charArr[dir->name.length() + 1];
    strcpy(charArr, dir->name.c_str());
    int thisY = contY;

    //Paint this directory
    createBox(dir->level * offsetX, contY * offsetY, charArr, colorR,colorG,colorB, a);

    for(int i = 0; i < dir->children.size(); i++) {
      createLine(dir->level * offsetX, thisY * offsetY, dir->children[i]->level * offsetX, contY * offsetY);
      paintHierarchy(dir->children[i], a);
      contY++;
    }
  }
}

// Main display function, objects are drawn here.
static void display( void )
{
  const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
  const double a = t*60.0;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  // Camera location, values change by KeyboardResponse function. (perspective x,y,z; move x,y,z; upVector x,y,z)
  gluLookAt(camX, camY, 5.0f,   camX, camY, 4.0f,   0.0f, 1.0f, 0.0f);
  //gluLookAt(0.0f, 1.0f, 5.0f,   camX, camY, 4.0f,   0.0f, 1.0f, 0.0f);

  // Universally defines the color of the objects, until there's a newer color definition.
  glColor3d(0.5,0.5,0.5);

  fillRoot();

  contY = 0;

  paintHierarchy(root, a);

  glutSwapBuffers();
}

// Lightning and shadow properties.
static void idle(void)
{
  glutPostRedisplay();
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

// Main application
int main( int argc, char *argv[])
{

  cout << "Dame el nombre del archivo: ";
  cin >> File;

  // Color definition
  colorR=(double)rand() / (double)RAND_MAX;
  colorG=(double)rand() / (double)RAND_MAX;
  colorB=(double)rand() / (double)RAND_MAX;

  glutInit(&argc, argv);
  glutInitWindowSize(800,800);
  glutInitWindowPosition(10,10);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  glutCreateWindow( "GLUTEN" );

  glutReshapeFunc(resize);
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutSpecialFunc(KeyboardResponse);

  glClearColor(1,1,1,1);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);

  glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

  glutMainLoop();

  return EXIT_SUCCESS;
}
