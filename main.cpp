#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include "Automaton.h"
#include "Statenode.h"

#define GRIDW 1024
#define GRIDH 768
#define WHEEL_UP    3
#define WHEEL_DOWN  4

int FRAMESPEED = 1000;
int updateTick = 0;
int updatePeriod = 50;

using namespace std;

vector<Automaton *> sandbox;
int selector = 0;
GLfloat grid[GRIDH][GRIDW][3];

bool initGL();
void display();
void update();
void runMainLoop( int val );

void printGrid()
{
    for (int x = 0; x < GRIDW; x++)
    {
        for(int y=0; y<GRIDH; y++)
        {
            cout << grid[0][y][x]/255;
        }
        cout << endl;
    }
    cout << endl;
}

bool initGL()
{
    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    float aspect = (float)GRIDW / (float)GRIDH;
    // glOrtho(-aspect, aspect, -1, 1, -1, 1);
    glOrtho(0, aspect, aspect, 0, -1, 1);
    glRasterPos2f(0,0);
    glPixelZoom(1, -1);
    gluLookAt(0,0,1,0,0,0,0,1,0);

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        return false;
    }

    return true;
}

void update()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float aspect = (float)GRIDW / (float)GRIDH;
    glOrtho(0, aspect, aspect, 0, -1, 1);
    // glOrtho(-aspect, aspect, -1, 1, -1, 1);
    
    for(vector<Automaton *>::iterator it = sandbox.begin(); it != sandbox.end(); it+=1)
    {
        if((*it)->isDead())
            continue;
        if((*it)->posx>=0 && (*it)->posy>=0 && (*it)->posx < GRIDW && (*it)->posy < GRIDW)
        {
            GLfloat inputmode = grid[(*it)->posy][(*it)->posx][0];
            GLfloat paintcolor = (GLfloat)(*it)->getWColor(inputmode);
            grid[(*it)->posy][(*it)->posx][0] = paintcolor;
            grid[(*it)->posy][(*it)->posx][1] = paintcolor;
            (*it)->transition(inputmode);
        }
        else
        {
            (*it)->kill();
        }
    }
}

void runMainLoop( int val )
{
    //Frame logic
    update();
    if((updateTick++)%updatePeriod == 0)
        display();

    //Run frame one more time
    glutTimerFunc( 1000 / FRAMESPEED, runMainLoop, val );
}

void display()
{
    // glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT );
    
    glDrawPixels( GRIDW, GRIDH, GL_RGB, GL_FLOAT, grid );
    
    glutSwapBuffers();
}

void keybd(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'q':
            cout << "Terminating..." << endl;
            exit(0);
        break;
        case '+':
            // cout << "Speed up?" << endl;
            FRAMESPEED += 100;
            updatePeriod = FRAMESPEED / 20;
        break;
        case '-':
            if(FRAMESPEED > 100)
            {
                FRAMESPEED -= 100;
                updatePeriod = FRAMESPEED / 20;
            }
        break;
        default:
            cout << key << " was pressed!" << endl;
        break;
    }
}

void mouser(int button, int state, int x, int y)
{
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            if(state==GLUT_UP)
            {
                Automaton * spawn = new Automaton(*sandbox[selector]);
                spawn->posx = x;
                spawn->posy = y;
                spawn->name = to_string((long) spawn);
                cout << "Spawning in: " << spawn->name << " at " << x << " " << y << endl;
                spawn->raise();
                sandbox.push_back(spawn);
            }
        break;
        case WHEEL_UP:
            if(state == GLUT_UP)
            {
                selector = ((selector-1)%sandbox.size());
                cout << "Now selected: " << sandbox[selector]->name << endl;
            }
        break;
        case WHEEL_DOWN:
            if(state == GLUT_UP)
            {
                selector = ((selector+1)%sandbox.size());
                cout << "Now selected: " << sandbox[selector]->name << endl;
            }
        default:
        break;
    }
}

string strip(string str)
{
    size_t first = str.find_first_not_of(' ');
    if(string::npos == first)
        return str;
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}

int main(int argc, char *argv[])
{
    memset(grid, 0, 3*GRIDW*GRIDH* sizeof(bool));
    
    if(argc == 1)
    {
        cout << "Please provide an input automaton." << endl;
        return 1;
    }
    
    ifstream autofile(string(argv[1]), ifstream::in);
    string line;
    Automaton * addme;
    while(getline(autofile, line)) {
        line = strip(line);
        if(line[0] == '#' || line.size()==0)
            continue;
        if(line.find("[Automaton]") == 0) {
            addme = new Automaton();
            cout << "Constructing Automaton." << endl;
        }
        else if(line.find("[Name]") == 0) {
            string name = line.substr(line.find(" ")+2);
            name = name.substr(0, name.size()-1);
            cout << "Name found: " << name << endl;
            addme->name = name;
        }
        else if(line.find("[Start]") == 0){
            stringstream ss;
            ss.str(line);
            int x, y, state;
            string temp;
            string orientation;
            ss >> temp >> x >> y >> state >> orientation;
            addme->posx = x;
            addme->posy = y;
            addme->curState = state;
            addme->setDirFromString(orientation);
            cout << "Start info (x,y,state,orientation): " << x << " " << y << " " << state << " " << orientation << endl;
        }
        else if(line.find("[State]") == 0)
        {
            stringstream ss;
            ss.str(line);
            int nsOff, nsOn, wOff, wOn;
            string tOff, tOn;
            string temp;
            ss >> temp >> nsOff >> nsOn >> tOff >> tOn >> wOff >> wOn;
            addme->addState(nsOff, nsOn, addme->getTurnFromString(tOff), addme->getTurnFromString(tOn), wOff, wOn);
            cout << "Add state (nsOff, nsOn, tOff, tOn, wOff, wOn): " << nsOff << " " << nsOn << " " << tOff << " " << tOn << " " << wOff << " " << wOn << endl;
        }
        else if(line.find("[/Automaton]")== 0){
            sandbox.push_back(addme);
            cout << "Automaton constructed\n\n";
        }
        else
        {
            cout << "Bad line: " << line << endl;
            return 1;
        }
    }
    
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowSize( GRIDW, GRIDH );
    glutCreateWindow( "Turmite Simulator 2017" );
    glutKeyboardFunc( keybd );
    glutMouseFunc( mouser );
    
    //Do post window/context creation initialization
    if( !initGL() )
    {
        printf( "Unable to initialize graphics library!\n" );
        return 1;
    }
    
    glutDisplayFunc( display );
    glutTimerFunc( 1000 / FRAMESPEED, runMainLoop, 0 );
    //Set main loop
    glutMainLoop();

    return 0;
}