/*
 *
 * Demonstrates how to load and display an Wavefront OBJ file. 
 * Using triangles and normals as static object. No texture mapping.
 *
 * OBJ files must be triangulated!!!
 * Non triangulated objects wont work!
 * You can use Blender to triangulate
 *
 Look into the main method to change the path to the moel file (shoud have no path) and the model should be at the
 same location of this file
 */
 
#include <windows.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <SOIL.h>
//#include "Tvector.h"
//#include "Tray.h"

#define KEY_ESCAPE 27
#define GL_CLAMP_TO_EDGE 0x812F

static GLuint texName;

GLuint _textureID;
using namespace std;
 
/************************************************************************
  Window
 ************************************************************************/
 
typedef struct {
    int width;
	int height;
	char* title;
 
	float field_of_view_angle;
	float z_near;
	float z_far;
} glutWindow;
 
//Reg: this code was copied from online; 
//Reg: http://stackoverflow.com/questions/12084889/how-can-i-use-a-texture-jpg-image-for-an-opengl-background-window-in-mac
 /*************************************************************************** 
  Texture Loading 
 ***************************************************************************/
//Reg: this section for texture is a hot mess...I'm going to try to figure it out later 4/12/15
 GLuint loadBMP_custom (const char * imagepath){ //Reg: takes in the file name, width, and height
	 
	 // Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath,"rb");
	if (!file)                              {printf("Image could not be opened\n"); return 0;}

	if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return false;
	}
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Not a correct BMP file\n");
		return 0;
	}

	// Read ints from the byte array
	// Reg: This reads the size of the file
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);
	 
	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];
 
	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);
 
	//Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
 
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
 
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
 
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S , GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return textureID;
}

void FreeTexture( GLuint texture ){
	glDeleteTextures( 1, &texture );
}


/*************************************************************************** 
  OBJ Loading 
 ***************************************************************************/

class Model_OBJ
{
  public: 
	Model_OBJ();			
    float* Model_OBJ::calculateNormal(float* coord1,float* coord2,float* coord3 );
    bool Model_OBJ::Load(char *filename);	// Loads the model
	void Model_OBJ::Draw();					// Draws the model on the screen
	void Model_OBJ::Release();				// Release the model; frees everything that was loaded
 
	float* normals;							// Stores the normals
    float* Faces_Triangles;					// Stores the triangles
	float* vertexBuffer;					// Stores the points which make the object
	long TotalConnectedPoints;				// Stores the total number of connected verteces
	long TotalConnectedTriangles;			// Stores the total number of connected triangles
 
};

//Reg: controls the player and makes player obj model
class player
{
public:
	//this will be used to control movement
	float x,y,z;
	float x_spd, z_spd;
	float x_accel, z_accel;
	double gravity;
	Model_OBJ obj;
	int timer;
	int jumping;
	float boost;

	
	void init()
	{
		x = z = 0;
		y = -1;
		gravity = .0098;
		x_spd = z_spd = 0.2;//for on button call
		x_spd = z_spd = .2; //for move() funct
		x_accel = z_accel = 0.0045;
		boost = 10;
		jumping = 0;
	}

	void move()
	{//Reg: this will make the ball go back and forth
			z += z_spd;
			reflect();//reflect
	}

	void reflect()
	{
		if (z < -6.5)
		{
			z_spd = -z_spd; //Reg: bounce back
			z = -6.4;
		}
		if (z > 6.5)
		{
			z_spd = -z_spd;
			z = 6.4;
		}
	}

	void update()
	{
		move();
		//activated from player.jump() function
		/*if (x_spd > .02)//for button call
		{
			x_spd -= 0.02;
		}
		if (z_spd > .02)
		{
			z_spd -= 0.02;
		}*/
		if (jumping == 2)
		{
			y = .01;
			boost = .25;
			jumping = 1;
			/*boost = 3;
			if ( y > 0)
			{
					y += boost;
					//boost -= gravity;
					//boost -= 0.1;
			}
			else
			{
			jumping = 0;
			}*/
		}

		if ( (y > -1) && (jumping == 0) )
		{
			/*if (jumping == 1)
			{
				y += boost;
				boost -= gravity;
				jumping = 0;
			}*/
			y -= gravity;
			gravity += .0098;

		}
		else if ((y > -1) && (jumping == 1))
		{
				y += boost;
				boost -= gravity;
				if (y<-1)
				{
					jumping = 0;
					y = -1;
				}
		}
		else 
		{
			gravity = .0098;
			jumping = 2;//Reg: Changing it to 0 will make the ball jump once...since we have it at 2 it will loop (bouncing)
			boost = 3;
		}

	}

	//Reg: this is to make the object jump
	void jump()
	{	
		jumping = 2;
	}

	//Reg: this is to make the object slide
	void rolling(int x_axis, int z_axis){
		if (x_axis == 1)
		{
			if(x < 6.5)
			{
				x += x_spd; //Reg: this will move the player to the right
				x_spd += x_accel;
			}
		}
		else if (x_axis == 2)
			{
			if(x > -6.5)
			{
				x -= x_spd; //Reg: this will move the player to the right
				x_spd += x_accel;
			}
		}

		if (z_axis == 1)
		{
			if(z < 6.5)
			{
				z += z_spd; //Reg: this will move the player to the right
				z_spd += z_accel;
			}
		}
		else if (z_axis == 2)
			{
			if(z > -6.5)
			{
				z -= z_spd; //Reg: this will move the player to the right
				z_spd += z_accel;
			}
		}
	}
	

};
 
 
#define POINTS_PER_VERTEX 3
#define TOTAL_FLOATS_IN_TRIANGLE 9
using namespace std;
 
Model_OBJ::Model_OBJ()
{
	this->TotalConnectedTriangles = 0; 
	this->TotalConnectedPoints = 0;
}
 
float* Model_OBJ::calculateNormal( float *coord1, float *coord2, float *coord3 )
{
   /* calculate Vector1 and Vector2 */
   float va[3], vb[3], vr[3], val;
   va[0] = coord1[0] - coord2[0];
   va[1] = coord1[1] - coord2[1];
   va[2] = coord1[2] - coord2[2];
 
   vb[0] = coord1[0] - coord3[0];
   vb[1] = coord1[1] - coord3[1];
   vb[2] = coord1[2] - coord3[2];
 
   /* cross product */
   vr[0] = va[1] * vb[2] - vb[1] * va[2];
   vr[1] = vb[0] * va[2] - va[0] * vb[2];
   vr[2] = va[0] * vb[1] - vb[0] * va[1];
 
   /* normalization factor */
   val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );
 
	float norm[3];
	norm[0] = vr[0]/val;
	norm[1] = vr[1]/val;
	norm[2] = vr[2]/val;
 
 
	return norm;
}
 
 
bool Model_OBJ::Load(char* filename)
{
	string line;
	ifstream objFile (filename);
	
	if(!objFile){
	  cout<<"\nFile NOT FOUND:"<<filename <<endl;
	  return false;
	}


	if (objFile.is_open())													// If obj file is open, continue
	{
		objFile.seekg (0, ios::end);										// Go to end of the file, 
		long fileSize = objFile.tellg();									// get file size
		objFile.seekg (0, ios::beg);										// we'll use this to register memory for our 3d model
 
		vertexBuffer = (float*) malloc (fileSize);							// Allocate memory for the verteces
		Faces_Triangles = (float*) malloc(fileSize*sizeof(float));			// Allocate memory for the triangles
		normals  = (float*) malloc(fileSize*sizeof(float));					// Allocate memory for the normals
 
		int triangle_index = 0;												// Set triangle index to zero
		int normal_index = 0;												// Set normal index to zero
 
		while (! objFile.eof() )											// Start reading file data
		{		
			getline (objFile,line);											// Get line from file
 
			if (line.c_str()[0] == 'v')									// The first character is a v: on this line is a vertex stored.
			{
				line[0] = ' ';											// Set first character to 0. This will allow us to use sscanf
 
				sscanf(line.c_str(),"%f %f %f ",							// Read floats from the line: v X Y Z
					&vertexBuffer[TotalConnectedPoints],
					&vertexBuffer[TotalConnectedPoints+1], 
					&vertexBuffer[TotalConnectedPoints+2]);
 
				TotalConnectedPoints += POINTS_PER_VERTEX;					// Add 3 to the total connected points
			}
			if (line.c_str()[0] == 'f')										// The first character is an 'f': on this line is a point stored
			{
		    	line[0] = ' ';												// Set first character to 0. This will allow us to use sscanf
 
				int vertexNumber[4] = { 0, 0, 0 };
                sscanf(line.c_str(),"%i%i%i",								// Read integers from the line:  f 1 2 3
					&vertexNumber[0],										// First point of our triangle. This is an 
					&vertexNumber[1],										// pointer to our vertexBuffer list
					&vertexNumber[2] );										// each point represents an X,Y,Z.
 
				vertexNumber[0] -= 1;										// OBJ file starts counting from 1
				vertexNumber[1] -= 1;										// OBJ file starts counting from 1
				vertexNumber[2] -= 1;										// OBJ file starts counting from 1
 
 
				/********************************************************************
				 * Create triangles (f 1 2 3) from points: (v X Y Z) (v X Y Z) (v X Y Z). 
				 * The vertexBuffer contains all verteces
				 * The triangles will be created using the verteces we read previously
				 */
 
				int tCounter = 0;
				for (int i = 0; i < POINTS_PER_VERTEX; i++)					
				{
					Faces_Triangles[triangle_index + tCounter   ] = vertexBuffer[3*vertexNumber[i] ];
					Faces_Triangles[triangle_index + tCounter +1 ] = vertexBuffer[3*vertexNumber[i]+1 ];
					Faces_Triangles[triangle_index + tCounter +2 ] = vertexBuffer[3*vertexNumber[i]+2 ];
					tCounter += POINTS_PER_VERTEX;
				}
 
				/*********************************************************************
				 * Calculate all normals, used for lighting
				 */ 
				float coord1[3] = { Faces_Triangles[triangle_index], Faces_Triangles[triangle_index+1],Faces_Triangles[triangle_index+2]};
				float coord2[3] = {Faces_Triangles[triangle_index+3],Faces_Triangles[triangle_index+4],Faces_Triangles[triangle_index+5]};
				float coord3[3] = {Faces_Triangles[triangle_index+6],Faces_Triangles[triangle_index+7],Faces_Triangles[triangle_index+8]};
				float *norm = this->calculateNormal( coord1, coord2, coord3 );
 
				tCounter = 0;
				for (int i = 0; i < POINTS_PER_VERTEX; i++)
				{
					normals[normal_index + tCounter ] = norm[0];
					normals[normal_index + tCounter +1] = norm[1];
					normals[normal_index + tCounter +2] = norm[2];
					tCounter += POINTS_PER_VERTEX;
				}
 
				triangle_index += TOTAL_FLOATS_IN_TRIANGLE;
				normal_index += TOTAL_FLOATS_IN_TRIANGLE;
				TotalConnectedTriangles += TOTAL_FLOATS_IN_TRIANGLE;			
			}	
		}
		objFile.close();	//Close OBJ file
		return true;
	}
	else 
	{
		cout << "Unable to open file";
	}
	return false;
}
 
void Model_OBJ::Release()
{
	free(this->Faces_Triangles);
	free(this->normals);
	free(this->vertexBuffer);
}
 
void Model_OBJ::Draw()
{
 	glEnableClientState(GL_VERTEX_ARRAY);						// Enable vertex arrays
 	glEnableClientState(GL_NORMAL_ARRAY);						// Enable normal arrays
	glVertexPointer(3,GL_FLOAT,	0,Faces_Triangles);				// Vertex Pointer to triangle array
	glNormalPointer(GL_FLOAT, 0, normals);						// Normal pointer to normal array	
	glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);		// Draw the triangles
	glDisableClientState(GL_VERTEX_ARRAY);						// Disable vertex arrays
	glDisableClientState(GL_NORMAL_ARRAY);						// Disable normal arrays
}
 
/***************************************************************************
 * Program code
 ***************************************************************************/
 
Model_OBJ obj;
Model_OBJ table;
player Player;

float zoom_z = 15, zoom_y = 15, slide_x = 0;
float up_x = 0, up_y = 1, up_z = 0;
float camera_rot = 0;
float movementspeed, a_speed;

int move_x, move_z; 
int counter; // sam: variable for framerate...kinda

//Reg: our current rotation coordinate
float g_rotation;
//Reg: our rotation speed
float spinSpeed = 0.2;
glutWindow win;


void display() 
{
	//Player.init(); //Reg: for now this sets up the x,y coord of the Player...check Player class for more info
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	//Reg: look what I can do :D:D:D:D:D this part binds the texture to the obj
	//Reg: you need to enable and disable your texture
	//GLuint image = loadBMP_custom("fsjal.bmp"); //load the texture
   glEnable( GL_TEXTURE_2D ); //enable 2D texturing
	gluLookAt( slide_x,zoom_y,zoom_z, 0,0,0, up_x,up_y,up_z); //Reg: you can adjust this in order to zoom out
	
	
	//Reg: we are drawing the stage with this first object...I will worry about skybox later
	glPushMatrix();
		glTranslatef(0.0,-2.0,-2.0); //Reg: we moved the obj a little bit down and a little bit back
		glRotatef(camera_rot,0,1,0);
		/*if (g_rotation >= 180){//Reg: this is sample code to get the object to rotate back and forth
			spinSpeed = -spinSpeed; //Reg: it rotates at .2
			g_rotation = 178;
		}
		else if (g_rotation <= 0) {
			spinSpeed = -spinSpeed;
			g_rotation = 2;
		}*/
		glRotatef(180,0,1,0);
		//glRotatef(180,1,0,0);
		//g_rotation += spinSpeed; //Reg: it rotates at .2
		glPushAttrib( GL_CURRENT_BIT );
		glColor3f(0,1.0,1.0);
		obj.Draw();
		glPopAttrib();
	glPopMatrix();

	//Reg: draw character...whatever it will be...
	glPushMatrix();
		glTranslatef(Player.x, Player.y, Player.z); //Reg: we moved the obj a little bit up in relation to the stage
		glRotatef(g_rotation,0,1,0);
		glRotatef(0,0,1,0);
		glRotatef(180,1,0,0);
		g_rotation += spinSpeed; //Reg: it rotates at .2
		/*if (move_b == 1){
			Player.x += 0.002;
		//Player.rolling(Player, move_b, movementspeed);//Reg: this is to get the ball to move
		}*/
		Player.rolling(move_x, move_z);
		if ((counter % 5) == 0) // Sam: every 60 runs through the processor
			Player.update();
		glPushAttrib( GL_CURRENT_BIT );
		
		
		
		//glColor3f(1,0,0);
		Player.obj.Draw();
		glPopAttrib();
	glPopMatrix();
	
	GLUquadric *quad;
	//gluQuadricDrawStyle(sphere, GLU_FILL);
	//gluQuadricTexture(sphere, GL_TRUE);
	//gluQuadricNormals(sphere, GLU_SMOOTH);
	quad = gluNewQuadric();
	glPushAttrib( GL_CURRENT_BIT );
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glPopAttrib();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricTexture(quad, GL_TRUE);
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluSphere(quad,25,10,10);

	glPushMatrix();
		glTranslatef(Player.x, Player.y,-2.0); //Reg: we moved the obj a little bit up in relation to the stage
		glRotatef(g_rotation,0,1,0);
		glRotatef(0,0,1,0);
		glPushAttrib( GL_CURRENT_BIT );
		glColor3f(0,0,1);
		table.Draw();
		glPopAttrib();
	glPopMatrix();
	glutSwapBuffers();
	//FreeTexture( image ); //Reg: this will disable GL_TEXTURE2D

	counter ++; // Sam: this is for the framerate...kinda
}
 
 
void initialize () 
{
	Player.init();
	GLuint image =  loadBMP_custom("bird.bmp");
	_textureID = image;
    glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, win.width, win.height);
	GLfloat aspect = (GLfloat) win.width / win.height;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);
    glMatrixMode(GL_MODELVIEW);
    glShadeModel( GL_SMOOTH );
    glClearColor( 0.0f, 0.1f, 0.0f, 0.5f );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
 
    GLfloat amb_light[] = { 0.1, 0.1, 0.1, 1.0 };
    //GLfloat diffuse[] = { 0.6, 0.6, 0.6, 1 };
    //GLfloat specular[] = { 0.7, 0.7, 0.3, 1 };
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );
    //glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    //glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
    glEnable( GL_LIGHT0 );
    glEnable( GL_COLOR_MATERIAL );
    glShadeModel( GL_SMOOTH );
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	//glEnable(GL_TEXTURE_2D);
    //glEnable(GL_DEPTH_TEST);
}
//Reg: in order to detect motion, first we need to "mousemove" function to know that the button has been pressed
int left_button_state;
//Reg: these will save the last coordinate of the mouse
float old_x, old_y;

	void mousemove(int x, int y)
	{
		if (left_button_state)
		{
			Player.x = (float)(old_x - x);
			Player.y = (float)(old_y - y);
		} 
		old_x = x;
		old_y = y;
	}
 
	void mouse(int button, int state, int x, int y)
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			//Reg: if the button is pressed, we change left button state to true
			old_x = x;
			old_y = y;
			left_button_state = 1;
			//Player.x = x;
			//Player.y = y;
		}
		else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			//Reg: if the button is released, then we can change the state is false
			left_button_state = 0;
		}
	}

	void keyboard ( unsigned char key, int x, int y ) 
	{
	  switch ( key ) {
		case KEY_ESCAPE:        
		  exit ( 0 );   
		  break;
		case 'a':
			slide_x += 1;
			break;
		case 'd':
			slide_x -= 1;
			break;
		case 's':
			zoom_z += 1;
			break;
		case 'w':
			zoom_z -= 1;
			break;
		case 'r':
			zoom_y += 1;
			break;
		case 't':
			zoom_y -= 1;
			break;
		//Reg: I want to figure out what up does for "lookAt"
		case 'z':
			up_x += 1;
			break;
		case 'x':
			up_y += 1;
			break;
		case 'c':
			up_z += 1;
			break;
		//Reg: negatives for X,Y, and Z
		case 'v':
			up_x -= 1;
			break;
		case 'b':
			up_y -= 1;
			break;
		case 'n':
			up_z -= 1;
			break;
		default:      
		  break;
	
	  } 
	}
	
	void specialkey ( int key, int x, int y)
	  {
		switch ( key ) {
			case GLUT_KEY_LEFT:
			//left key
				move_x = 2;
			break;
			case GLUT_KEY_RIGHT:
			//right key
				move_x= 1;
			break;
			case GLUT_KEY_UP:
			//move away from camera
			//spinSpeed+=0.2;
				move_z = 2;
			break;
			case GLUT_KEY_DOWN:
			//move towards camera
				move_z = 1;
			break;
			case GLUT_KEY_INSERT:
			//jump key
				Player.jump();
			break;
		default:
			break;
		}
}

	void specialkeyup ( int key, int x, int y)
	  {
		switch ( key ) {
			case GLUT_KEY_LEFT:
				move_x = 0; //deactivates movement 
			break;
			case GLUT_KEY_RIGHT:
			//right key
				move_x = 0; //deactivates movement
				break;
			case GLUT_KEY_UP:
				move_z = 0;
			break;
			case GLUT_KEY_DOWN:
				move_z = 0;
			break;
		default:
			break;
		}
}
 
int main(int argc, char **argv) 
{

	// set window values
	win.width = 1200;
	win.height = 800;
	win.title = "OpenGL/GLUT OBJ Loader.";
	win.field_of_view_angle = 45;
	win.z_near = 1.0f;
	win.z_far = 500.0f;

	// initialize and run program
	glutInit(&argc, argv);										// GLUT initialization
	
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(win.width,win.height);					// set window size
	glutCreateWindow(win.title);								// create Window
	glutDisplayFunc(display);									// register Display Function
	glutIdleFunc( display );									// register Idle Function
    glutKeyboardFunc( keyboard );								// register Keyboard Handler
	glutSpecialFunc(specialkey);								// Reg: this is for your directional keys
	glutSpecialUpFunc(specialkeyup);							// Reg: this is to handle if the button is released
	//Reg: this is to get the object to move by using a mouse
	glutMouseFunc(mouse);
	glutMotionFunc(mousemove);
	initialize();

	//prompt for a file to load
	char filename[200] = "plane.obj";
	//char tablefile[200] = "table.obj";
	char filename2[200] = "sphere.obj";
	//cout<<"\nEnter object file name:";
	//cin>>filename;

	obj.Load(filename);
	//table.Load(tablefile);
	Player.obj.Load(filename2);
	//glutTimerFunc(600, timer(2),1);
	glutMainLoop();
	/*if(obj.Load(filename)){
		glutMainLoop();												// run GLUT mainloop
	}*/

	/*if(player.Load(filename2)){
		glutMainLoop();
	}
	return 0;*/
}
