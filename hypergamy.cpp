#include <windows.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

float camYaw = 0.0f;
float camPitch = 0.1f;
float camDist = 26.0f;
bool mousePressed = false;
double lastMouseX = 0.0, lastMouseY = 0.0;
float g_time = 0.0f;

struct Person {
    float x, y, z;
    int level;
    int colorType;
};

struct Line {
    float x1, y1, z1;
    float x2, y2, z2;
    int colorType;
};

std::vector<Person> leftMen;
std::vector<Person> leftWomen;
std::vector<Person> rightMen;
std::vector<Person> rightWomen;
std::vector<Line> leftLines;
std::vector<Line> rightLines;

const int LEVELS = 10;
const float SPACING = 0.9f;
const float START_Y = -4.0f;

void buildScene() {
    leftMen.clear();
    leftWomen.clear();
    rightMen.clear();
    rightWomen.clear();
    leftLines.clear();
    rightLines.clear();
    srand((unsigned)time(NULL));

    float leftMenX = -14.0f;
    float leftWomenX = -9.0f;
    float rightMenX = 1.0f;
    float rightWomenX = 6.0f;

    for (int i = 0; i < LEVELS; ++i) {
        float y = START_Y + i * SPACING;

        Person lm;
        lm.x = leftMenX; lm.y = y; lm.z = 0.0f; lm.level = i; lm.colorType = 0;
        leftMen.push_back(lm);

        Person lw;
        lw.x = leftWomenX; lw.y = y; lw.z = 0.0f; lw.level = i; lw.colorType = 1;
        leftWomen.push_back(lw);

        Person rm;
        rm.x = rightMenX; rm.y = y; rm.z = 0.0f; rm.level = i; rm.colorType = 2;
        rightMen.push_back(rm);

        Person rw;
        rw.x = rightWomenX; rw.y = y; rw.z = 0.0f; rw.level = i; rw.colorType = 1;
        rightWomen.push_back(rw);
    }

    for (int i = 0; i < LEVELS; ++i) {
        int targets[3];
        int count = 0;
        targets[count++] = i;
        if (i + 1 < LEVELS) targets[count++] = i + 1;
        if (i - 1 >= 0) targets[count++] = i - 1;

        for (int k = 0; k < count; ++k) {
            Line l;
            l.x1 = leftMen[i].x;  l.y1 = leftMen[i].y;  l.z1 = leftMen[i].z;
            l.x2 = leftWomen[targets[k]].x; l.y2 = leftWomen[targets[k]].y; l.z2 = leftWomen[targets[k]].z;
            l.colorType = 0;
            leftLines.push_back(l);
        }
    }

    int topLevels[3] = { LEVELS - 1, LEVELS - 2, LEVELS - 3 };
    for (int i = 0; i < LEVELS; ++i) {
        for (int j = 0; j < 3; ++j) {
            int t = topLevels[j];
            Line l;
            l.x1 = rightWomen[i].x; l.y1 = rightWomen[i].y; l.z1 = rightWomen[i].z;
            l.x2 = rightMen[t].x;   l.y2 = rightMen[t].y;   l.z2 = rightMen[t].z;
            l.colorType = 2;
            rightLines.push_back(l);
        }
    }
}

void drawSphere(float x, float y, float z, float r, float R, float G, float B, bool glow) {
    glPushMatrix();
    glTranslatef(x, y, z);

    GLfloat diff[] = { R, G, B, 1.0f };
    GLfloat em[] = { 0, 0, 0, 1.0f };
    if (glow) { em[0] = R * 0.5f; em[1] = G * 0.5f; em[2] = B * 0.5f; }

    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_EMISSION, em);

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluSphere(q, r, 24, 24);
    gluDeleteQuadric(q);
    glPopMatrix();
}

void drawLine3D(const Line& l) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (l.colorType == 0) glColor4f(0.8f, 0.1f, 0.2f, 0.85f);
    else                  glColor4f(0.1f, 0.6f, 0.9f, 0.85f);

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(l.x1, l.y1, l.z1);
    glVertex3f(l.x2, l.y2, l.z2);
    glEnd();

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

void drawLetterM(float x, float y, float z, float s) {
    glDisable(GL_LIGHTING);
    glColor3f(0, 0, 0);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(x, y, z);                glVertex3f(x, y + s, z);
    glVertex3f(x, y + s, z);            glVertex3f(x + s * 0.5f, y + s * 0.5f, z);
    glVertex3f(x + s * 0.5f, y + s * 0.5f, z); glVertex3f(x + s, y + s, z);
    glVertex3f(x + s, y + s, z);        glVertex3f(x + s, y, z);
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawLetterA(float x, float y, float z, float s) {
    glDisable(GL_LIGHTING);
    glColor3f(0, 0, 0);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(x, y, z);                         glVertex3f(x + s * 0.5f, y + s, z);
    glVertex3f(x + s * 0.5f, y + s, z);          glVertex3f(x + s, y, z);
    glVertex3f(x + s * 0.25f, y + s * 0.5f, z);  glVertex3f(x + s * 0.75f, y + s * 0.5f, z);
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawLetterN(float x, float y, float z, float s) {
    glDisable(GL_LIGHTING);
    glColor3f(0, 0, 0);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(x, y, z);         glVertex3f(x, y + s, z);
    glVertex3f(x, y + s, z);     glVertex3f(x + s, y, z);
    glVertex3f(x + s, y, z);     glVertex3f(x + s, y + s, z);
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawLetterW(float x, float y, float z, float s) {
    glDisable(GL_LIGHTING);
    glColor3f(0, 0, 0);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(x, y + s, z);                    glVertex3f(x + s * 0.25f, y, z);
    glVertex3f(x + s * 0.25f, y, z);            glVertex3f(x + s * 0.5f, y + s * 0.5f, z);
    glVertex3f(x + s * 0.5f, y + s * 0.5f, z);  glVertex3f(x + s * 0.75f, y, z);
    glVertex3f(x + s * 0.75f, y, z);            glVertex3f(x + s, y + s, z);
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawLetterO(float x, float y, float z, float s) {
    glDisable(GL_LIGHTING);
    glColor3f(0, 0, 0);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    for (int k = 0; k < 24; ++k) {
        float ang = k * 2.0f * 3.14159265f / 24.0f;
        glVertex3f(x + s * 0.5f + cos(ang) * s * 0.4f, y + s * 0.5f + sin(ang) * s * 0.5f, z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawWordMAN(float x, float y, float z, float s) {
    drawLetterM(x, y, z, s);
    drawLetterA(x + s * 1.2f, y, z, s);
    drawLetterN(x + s * 2.4f, y, z, s);
}

void drawWordWOMAN(float x, float y, float z, float s) {
    drawLetterW(x, y, z, s);
    drawLetterO(x + s * 1.2f, y, z, s);
    drawLetterM(x + s * 2.4f, y, z, s);
    drawLetterA(x + s * 3.6f, y, z, s);
    drawLetterN(x + s * 4.8f, y, z, s);
}

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    GLfloat l0pos[] = { 5.0f, 10.0f, 8.0f, 1.0f };
    GLfloat l0diff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, l0pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, l0diff);

    GLfloat l1pos[] = { -5.0f, -5.0f, 5.0f, 1.0f };
    GLfloat l1diff[] = { 0.3f, 0.4f, 0.6f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, l1pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, l1diff);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
        else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (mousePressed) {
        float dx = (float)(xpos - lastMouseX);
        float dy = (float)(ypos - lastMouseY);
        camYaw += dx * 0.005f;
        camPitch += dy * 0.005f;
        if (camPitch > 1.4f) camPitch = 1.4f;
        if (camPitch < -1.4f) camPitch = -1.4f;
        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camDist -= (float)yoffset * 1.0f;
    if (camDist < 12.0f) camDist = 12.0f;
    if (camDist > 50.0f) camDist = 50.0f;
}

void display(GLFWwindow* win) {
    int w, h;
    glfwGetFramebufferSize(win, &w, &h);
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;

    glViewport(0, 0, w, h);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.5, 150.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float camX = camDist * cos(camPitch) * sin(camYaw);
    float camY = camDist * sin(camPitch);
    float camZ = camDist * cos(camPitch) * cos(camYaw);

    gluLookAt(camX, camY, camZ,
        -3.5f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f);

    setupLighting();

    for (const auto& l : leftLines) drawLine3D(l);
    for (const auto& l : rightLines) drawLine3D(l);

    for (const auto& p : leftMen)   drawSphere(p.x, p.y, p.z, 0.28f, 0.85f, 0.10f, 0.15f, false);
    for (const auto& p : leftWomen) drawSphere(p.x, p.y, p.z, 0.28f, 0.05f, 0.05f, 0.05f, false);
    for (const auto& p : rightMen) {
        float pulse = 0.8f + 0.2f * sin(g_time * 4.0f);
        drawSphere(p.x, p.y, p.z, 0.30f, 0.10f, 0.55f * pulse, 0.95f, true);
    }
    for (const auto& p : rightWomen) drawSphere(p.x, p.y, p.z, 0.28f, 0.05f, 0.05f, 0.05f, false);

    float topY = START_Y + LEVELS * SPACING + 0.6f;
    float scale = 0.45f;

    drawWordMAN(-14.0f - scale * 1.5f, topY, 0.0f, scale);
    drawWordWOMAN(-9.0f - scale * 2.5f, topY, 0.0f, scale);

    drawWordMAN(1.0f - scale * 1.5f, topY, 0.0f, scale);
    drawWordWOMAN(6.0f - scale * 2.5f, topY, 0.0f, scale);

    glfwSwapBuffers(win);
}

int main() {
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* win = glfwCreateWindow(1400, 900, "hypergamy", nullptr, nullptr);
    if (!win) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    glfwSetMouseButtonCallback(win, mouseButtonCallback);
    glfwSetCursorPosCallback(win, cursorPosCallback);
    glfwSetScrollCallback(win, scrollCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    buildScene();

    double last = glfwGetTime();
    while (!glfwWindowShouldClose(win)) {
        double now = glfwGetTime();
        g_time += (float)(now - last);
        last = now;

        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(win, 1);
        if (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS) { buildScene(); glfwWaitEventsTimeout(0.3); }

        display(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
