#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <cmath>
#include <Windows.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <list>
#include <utility>
#include <algorithm>
#include <thread>

#define X 10
#define Y 10
#define alpha 0.3
#define gamma 0.9
#define recLength 50
#define MAX_STEPS 200


using namespace std;
float STATES[X][Y][4];
list<pair<int, int>> dead = {pair(5,6),pair(6,5), pair(7,6)};
list<pair<int, int>> good = { pair(6,6)};
pair<int, int> starting_point = pair(0, 0);
int windowWidth = 500;
int windowHeight = 500;
list<pair<int, int>> rectangle;

int Step = 1;
float success = 0;
float learning_rate = 0.1;



void init() {
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < 4; k++) {
                STATES[i][j][k] = 10;
            }
        }
    }
}

float random(float a) {
    return (float) rand() / RAND_MAX * a;
}

int soft_choose(int* d, int N) {
    float a, sum, t;
    sum = 0;
    t = 0;
    for (int i = 0; i < N; i++) {
        sum += d[i];
    }
    a = random(sum);
    for (int i = 0; i < N; i++) {
        t += d[i];
        if (a <= t) {
            return i;
        }
    }
    //Problematic?
}


int hard_choose(int x, int y) {
    float value = STATES[x][y][0];
    int r = 0;
    for (int k = 1; k < 4; k++) {
        if (STATES[x][y][k] > value) {
            r = k;
            value = STATES[x][y][k];
        }
    }
    return r;
}


int choice(int i, int j) {
    //int F[4];
    //for (int k = 0; k < 4; k += 1) {
     //       F[k] = A[i][j][k]+alpha;
    //}
    

    //return soft_choose(F, 4);

    int t = hard_choose(i, j);
    int r = rand() % 4;

    if ((float)rand() / RAND_MAX >= alpha)
        return t;
    else {

        while (r == t) {
            r = rand() % 4;
        }
        return r;
    }

}

bool contains(list<pair<int, int>>* set, int x, int y) {
    for (auto t : *set) {
        if (t.first == x && t.second == y)
            return true;
    }
    return false;
}

void traceback(list<pair<int, int>>* path, int* choices, int cnt, float R, int N) {
    if (path->size() != cnt)
        throw invalid_argument("Length of path and cnt must be equal");

    //reverse the path
    path->reverse();
    cnt -= 1;   //computer array indices starts from 0
    R *= gamma;

    int i, j, t;
    float previous;
    for (auto point : *path) {
        i = point.first;
        j = point.second;
        t = choices[cnt];
        cnt -= 1;
        if (i<0||i>9 ||j<0||j>9||t<0||t>9)
            cout << i << "  " << j << " " << t << endl;
        try {
            previous = STATES[i][j][t];
        }
        catch(exception& ex){
            cout << i << "  " << j << " " << t << endl;
        }
        STATES[i][j][t] += (R-previous)*learning_rate;
        R *= gamma;
    }
}


void mc_process(int n) {
    list<float> reward;
    list<pair<int, int> > path;
    int choices[MAX_STEPS];
    int cnt=0;

    path.push_back(starting_point);
    int x = starting_point.first;
    int y = starting_point.second;
    int t;
    float R;
    while (true) {
        t = choice(x, y);
        if (t < 0 || t>3)
            cout << t;
        choices[cnt]=t;
        cnt += 1;
        //cnt := number of elements in choices[]

        if (t == 0)
            x -= 1;
        if (t == 1)
            x += 1;
        if (t == 2)
            y -= 1;
        if (t == 3)
            y += 1;

        
        if (x < 0 || y < 0 || x>=X || y>=Y||contains(&dead, x, y)||path.size()>MAX_STEPS-1) {
            //do dead
            R = 0;
            break;
        }
        if (contains(&good, x, y)) {
            //do good
            R = 50;   
            success += 1;
            break;
        }

        path.push_back(pair(x, y));
    }

    //MC traceback
    traceback(&path, choices, cnt, R, n);
}







int main_console() {
    srand(time(NULL));
    init();
    //above initialize
    cout << endl << "N:  ";
    int n;
    cin >> n;
    for (int i = 1; i < n; i++) {
        mc_process(i);
    }
    for (int i = 0; i < X; i++) {
        cout << "Line " << i << endl;
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < 4; k++) {
                cout << STATES[i][j][k] << "\t";
            }
            cout << endl;
        }
        cout << endl;
    }
    cout << endl;
    for (auto t : STATES[3][2])
        cout << t << "\t";
    return 0;
}








void drawPosition(int x, int y, float R=0, float G=0, float B=1) {
    x *= recLength;
    y *= recLength;
    glColor3f(R, G, B);
    glRecti(x+10, y+10, x + recLength-10, y +recLength-10);
}

void illustrate() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    for (auto p : good) {
        drawPosition(p.first, p.second, 0, 1, 0);
    }
    for (auto p : dead) {
        drawPosition(p.first, p.second, 1, 0, 0);
    }
    for (auto p : rectangle) {
        drawPosition(p.first, p.second, 1, 1, 1);
    }
    glutSwapBuffers();
}


void mc_illustrate(bool soft=true) {
    rectangle.clear();
    int x = starting_point.first;
    int y = starting_point.second;

    rectangle.push_back(pair(x, y));
    int t;
    while (true) {
        if (soft)
            t = choice(x, y);
        else
            t = hard_choose(x, y);
        if (t == 0)
            x -= 1;
        if (t == 1)
            x += 1;
        if (t == 2)
            y -= 1;
        if (t == 3)
            y += 1;

        rectangle.push_back(pair(x, y));
        if (rectangle.size() > 100)
            return;
        if (x < 0 || y < 0 || x >= X || y >= Y || contains(&dead, x, y)) {
            break;
        }
        if (contains(&good, x, y)) {
            break;
        }
    }
}







void timer(int value) {
    for (int i = 0; i < 100; i++) {
        mc_process(Step);
        Step += 1;
    }
    mc_illustrate(false);


    glutPostRedisplay();
    // Call the timer callback again after 200ms
    glutTimerFunc(200, timer, 0);
}

int main_graph(int argc, char** argv) {
    srand(time(NULL));
    init();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("MC-RL");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowWidth, 0, windowHeight, -1, 1);

    glMatrixMode(GL_MODELVIEW);

    glutDisplayFunc(illustrate);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}

int main(int argc, char** argv) {
    int a;
    cout << "1. Graphical\n2. Print Result\n";
    cin >> a;
    if (a == 1)
        main_graph(argc, argv);
    else {
        main_console();
        cin >> a;
    }

}


