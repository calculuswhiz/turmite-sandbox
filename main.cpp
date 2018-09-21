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

#define GRIDW 2048
#define GRIDH 1024
#define WHEEL_UP    3
#define WHEEL_DOWN  4

int FRAMESPEED = 60000;
int updateTick = 0;
int updatePeriod = FRAMESPEED / 20;

using namespace std;

vector<Automaton *> sandbox;
int selector = 0;
int maxselector = 0;
bool paused = 0;
int alive = 0;

// The 3 is for the colors
GLfloat grid[GRIDH][GRIDW][3];

void display();

void printGrid()
{
    for (int x = 0; x < GRIDW; x++)
    {
        for (int y = 0; y < GRIDH; y++)
        {
            cout << grid[0][y][x] / 255;
        }
        cout << endl;
    }
    cout << endl;
}

bool initGL()
{
    //Initialize Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //Initialize Modelview Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    float aspect = (float)GRIDW / (float)GRIDH;
    // glOrtho(-aspect, aspect, -1, 1, -1, 1);
    glOrtho(0, aspect, aspect, 0, -1, 1);
    glRasterPos2f(0, 0);
    glPixelZoom(1, -1);
    gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);

    //Initialize clear color
    glClearColor(0.f, 0.f, 0.f, 1.f);

    //Check for error
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
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
    
    if (paused)
    {
        return;
    }
        
    alive = 0;
    for(vector<Automaton *>::iterator it = sandbox.begin(); it != sandbox.end(); it += 1)
    {
        if ((*it)->isDead())
        {
            continue;
        }
        if ((*it)->posx >= 0 && (*it)->posy >= 0 && (*it)->posx < GRIDW && (*it)->posy < GRIDH)
        {
            GLfloat * cache = grid[(*it)->posy][(*it)->posx];
            GLfloat inputmode = cache[0] || cache[1] || cache[2];
            GLfloat shouldpaint = (GLfloat)(*it)->shouldWrite(inputmode);
            GLfloat * wcolor = (*it)->color;
            cache[0] = shouldpaint * wcolor[0];
            cache[1] = shouldpaint * wcolor[1];
            cache[2] = shouldpaint * wcolor[2];
            (*it)->transition(inputmode);
            alive += 1;
        }
        else
        {
            (*it)->kill();
        }
    }
    
    // Don't touch originally loaded.
    for (int i = sandbox.size() - 1; i >= maxselector; i--)
    {
        if (sandbox[i]->isDead())
        {
            delete (sandbox[i]);
            sandbox.erase(sandbox.begin() + i);
        }
    }
}

void runMainLoop(int val)
{
    //Frame logic
    update();
    if ((updateTick++) % updatePeriod == 0)
    {
        display();
    }

    //Run frame one more time
    glutTimerFunc( 1000 / FRAMESPEED, runMainLoop, val );
}

// Totally swiped this from StackOverflow:
void renderText(float x, float y, const char* text) 
{
    x /= GRIDW;
    y /= GRIDH;
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)text);
    glRasterPos2f(0, 0);
}

#define FONT_WIDTH  16
#define FONT_HEIGHT 20
void display()
{
    // glClearColor( 0, 0, 0, 1 );
    glClear(GL_COLOR_BUFFER_BIT);
    
    glDrawPixels(GRIDW, GRIDH, GL_RGB, GL_FLOAT, grid);
    
    // Text data
    char selectionbuf[255];
    char speedbuf[32];
    sprintf(selectionbuf, "%s", sandbox[selector]->name.c_str());
    sprintf(speedbuf, "Speed %d", FRAMESPEED);
    renderText(0, FONT_HEIGHT, "Click will spawn: ");
    glColor3f(sandbox[selector]->color[0], sandbox[selector]->color[1], sandbox[selector]->color[2]);
    renderText(FONT_WIDTH * 18, FONT_HEIGHT, selectionbuf);
    
    glColor3f(1, 1, 1);
    renderText(0, 2 * FONT_HEIGHT, speedbuf);
    char alivetext[4];
    sprintf(alivetext, "%d", alive);
    renderText(0, FONT_HEIGHT * 3, "Alive: ");
    renderText(FONT_WIDTH * 7, FONT_HEIGHT * 3, alivetext);
    
    if (paused)
    {
        char freezu[] = "Za Warudo! Toki yo tomare!";
        renderText(0, FONT_HEIGHT * 4, freezu);
    }
    
    glutSwapBuffers();
}

void keybd(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'p':
            paused ^= true;
            break;
        case 'q':
            cout << "Terminating..." << endl;
            exit(0);
            break;
        case '+':
            // cout << "Speed up?" << endl;
            FRAMESPEED *= 1.1;
            updatePeriod = FRAMESPEED / 20;
            break;
        case '-':
            if (FRAMESPEED > 100)
            {
                FRAMESPEED /= 1.1;
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
            if (state == GLUT_UP)
            {
                Automaton * spawn = new Automaton(*sandbox[selector]);
                spawn->posx = x;
                spawn->posy = y;
                spawn->name = sandbox[selector]->name + "_" + to_string((long) spawn);
                cout << "Spawning in: " << spawn->name << " at " << x << " " << y << endl;
                spawn->raise();
                sandbox.push_back(spawn);
            }
            break;
        case WHEEL_UP:
            if (state == GLUT_UP)
            {
                selector = (selector - 1);
                if (selector < 0)
                {
                    selector = maxselector - 1;
                }
                cout << "Now selected: " << sandbox[selector]->name << endl;
            }
            break;
        case WHEEL_DOWN:
            if (state == GLUT_UP)
            {
                selector = ((selector+1)%maxselector);
                cout << "Now selected: " << sandbox[selector]->name << endl;
            }
            break;
        default:
            break;
    }
}

string strip(string str)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        cout << "Need file args. 1st is the automaton file, 2nd is the grid config file." << endl;
        return 1;
    }
    
    if (argc >= 2)
    {
        // Parse Automaton file
        ifstream automatonFileStream(string(argv[1]), ifstream::in);
        string line;
        Automaton * addme;
        while (getline(automatonFileStream, line)) 
        {
            line = strip(line);
            if (line[0] == '#' || line.size() == 0)
            {
                continue;
            }
            if (line.find("[Automaton]") == 0) 
            {
                addme = new Automaton();
                cout << "Constructing Automaton." << endl;
            }
            else if (line.find("[Name]") == 0) 
            {
                string name = line.substr(line.find(" ") + 2);
                name = name.substr(0, name.size() - 1);
                cout << "Name found: " << name << endl;
                addme->name = name;
            }
            else if (line.find("[Color]") == 0)
            {
                stringstream ss;
                ss.str(line);
                string temp;
                float r, g, b;
                ss >> temp >> r >> g >> b;
                addme->color[0] = r;
                addme->color[1] = g;
                addme->color[2] = b;
                cout << "Float Color (r,g,b): " << r << " " << g << " " << b << endl;
            }
            else if (line.find("[Start]") == 0)
            {
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
                addme->initialOrientation = addme->orientation;
                cout << "Start info (x,y,state,orientation): " << x << " " << y << " " << state << " " << orientation << endl;
            }
            else if (line.find("[State]") == 0)
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
            else if (line.find("[Debug]") == 0)
            {
                addme->silent = false;
                cout << "State debug mode on\n";
            }
            else if (line.find("[/Automaton]") == 0)
            {
                sandbox.push_back(addme);
                cout << "Automaton constructed\n\n";
            }
            else
            {
                cout << "Bad line: " << line << endl;
                return 1;
            }
        }
    }
    
    if (argc >= 3)
    {
        // Clear grid
        memset(grid, 0, 3 * GRIDW * GRIDH * sizeof(bool));
        
        cout << "Parsing Grid file...\n";
        
        // Parse grid configuration file
        ifstream gridConfigFileStream(string(argv[2]), ifstream::in);
        string line;
        float red = 1, green = 1, blue = 1;
        while (getline(gridConfigFileStream, line)) 
        {
            line = strip(line);
            if (line[0] == '#' || line.size() == 0)
            {
                continue;
            }
            else if (line.find("[Color]") == 0)
            {
                stringstream ss;
                ss.str(line);
                string temp;
                ss >> temp >> red >> green >> blue;
                cout << "Float Color (r,g,b): " << red << " " << green << " " << blue << endl;
            }
            else if (line.find("[Rect]") == 0)
            {
                stringstream ss;
                ss.str(line);
                string temp;
                int x, y, w, h;
                ss >> temp >> x >> y >> w >> h;
                
                for (int i = 0; i < w; i++)
                {
                    for (int j = 0; j < h; j++)
                    {
                        grid[y + j][x + i][0] = red;
                        grid[y + j][x + i][1] = green;
                        grid[y + j][x + i][2] = blue;
                        
                        cout << "Setting Cell: " << (x + i) << " " << (y + j) << endl;
                    }
                }
            }
            else
            {
                cout << "Bad line: " << line << endl;
                return 1;
            }
        }
    }
    
    maxselector = sandbox.size();
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(GRIDW, GRIDH);
    glutCreateWindow("Turmite Simulator 2017 (2018 edition)");
    glutKeyboardFunc(keybd);
    glutMouseFunc(mouser);
    
    //Do post window/context creation initialization
    if (!initGL())
    {
        printf("Unable to initialize graphics library!\n");
        return 1;
    }
    
    glutDisplayFunc(display);
    glutTimerFunc(1000 / FRAMESPEED, runMainLoop, 0);
    //Set main loop
    glutMainLoop();

    return 0;
}
