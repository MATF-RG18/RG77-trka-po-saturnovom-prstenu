#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <GL/glut.h>

#define PI 3.14159265359

void on_reshape(int width, int height);
void on_display(void);
void on_keyboard(unsigned char key, int x, int y);
void on_timer(int value);

void draw_axes();
void draw_ring(float in_radius, float out_radius, int precision);

void draw_Saturn_ring();
void draw_Saturn_sphere();

void draw_cube();
void draw_cube1();

void draw_Spaceman();

static int rotation_angle = 0;
static float translation_left_right = 0;
static int animation_ongoing = 0;

int main(int argc, char** argv) {
    
    /* Inicijalizacija GLUT-a. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    
    /* Kreiranje prozora. */
    glutInitWindowSize(1000, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    

    /* Registracija funkcija za obradu događaja. */
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
    
    /* OpenGL Inicijalizacija. */
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    /* Ulazi se u glavnu petlju. */
    glutMainLoop();
    
    return 0;
}

/* Pomoćna funkcija za iscrtavanje koordinatnih osa. */
void draw_axes()
{
glBegin( GL_LINES );
glColor3f( 1.0, 0.0, 0.0);
glVertex3f( -50.0, 0.0, 0.0 );
glVertex3f( 50.0, 0.0, 0.0 );
glEnd();

glBegin( GL_LINES );
glColor3f( 0.0, 0.0, 1.0 );
glVertex3f( 0.0, -50.0, 0.0);
glVertex3f( 0.0, 50.0, 0.0);
glEnd();

glBegin( GL_LINES );
glColor3f( 0.0, 1.0, 0.0);
glVertex3f( 0.0, 0.0, -50.0);
glVertex3f( 0.0, 0.0, 50.0);
glEnd();
}

/* Obrada događaja on on_display. */
void on_display(void) {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);
        
    glPushMatrix();
        
        glRotatef(10,1,0,0);
        glTranslatef(-0.70,-0.1,0.33);
        
        glPushMatrix();
        draw_Saturn_sphere();
        glPopMatrix();
        
        glPushMatrix();
        glScalef(1,1,2.5);
        draw_Saturn_ring();
        glPopMatrix();
        
        
        glPushMatrix();
        glScalef(1,1.2,1);
        glScalef(1,1,2.5);
        draw_cube();
        glPopMatrix();
        
        
        glPushMatrix();
        glScalef(1,1.2,1);
        glScalef(1,1,2.5);
        draw_cube1();
        glPopMatrix();
        
        
        glPushMatrix();
        glTranslatef(translation_left_right,0,0);
        glTranslatef(0.95,0.07,1.1);
        glScalef(0.08,0.1,0.08);
        glColor3f(0,0.7,0);
        draw_Spaceman();
        glPopMatrix();
    glPopMatrix();

    glutSwapBuffers();
}

/* Iscrtavanje prepreka. */
void draw_cube() {
    glPushMatrix();
    glColor3f(0.2,0.1,0.51);
    glRotatef(-rotation_angle, 0, 1, 0);
    glTranslatef(0,0.1/2, -0.8-0.1/2);
    glutSolidCube(0.1);
    glPopMatrix();
}


void draw_cube1() {
    glPushMatrix();
    glColor3f(0.2,0.1,0.51);
    glRotatef(-rotation_angle, 0, 1, 0);
    glTranslatef(0,0.1/2, -(0.8+1.0/10)-0.1/2);
    glutSolidCube(0.1);
    glPopMatrix();
}

/* Iscrtavanje prstena. */
void draw_ring(float in_radius, float out_radius, int precision) {
    glBegin(GL_QUAD_STRIP);
    int i;
    for( i = 0; i <= precision; ++i )
    {
        float angle;
        /* Trazimo uglove za iscrtavanje tacaka [0, 2PI] */
        angle=2 * PI * ((i * 1.0) / precision);
        
        /* Iscrtavaju se grupisani poligoni. [GL_QUAD_STRIP]*/
        glVertex2f( in_radius * cos(angle), in_radius * sin(angle));
        glVertex2f( out_radius * cos(angle), out_radius * sin(angle));
    }
    glEnd();
}

/* Obrada događaja on on_reshape. */
void on_reshape(int width, int height)
{
    glViewport(0,0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (float) width / height, 1, 1000);
}

/* Obrada događaja on on_keyboard. */
void on_keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27: 
            exit(0);
            break;
        case 'm':
        case 'M':
            if (!animation_ongoing) {
                glutTimerFunc(50, on_timer, 0);
                animation_ongoing = 1;
            }
            break;
        case 'a':
        case 'A':
            if (translation_left_right>-0.2)
                translation_left_right -= 0.1;
            glutPostRedisplay();
            break;
        case 'd':
        case 'D':
            if (translation_left_right<0.2)
                translation_left_right += 0.1;
            glutPostRedisplay();
            break;
      
    }
}

/* Iscrtavanje prsten Saturna. */
void draw_Saturn_ring() {
    glPushMatrix();
        glRotatef(90, 1,0,0);
        
        int i=0;
        int num_of_speedways = 5;
    
        float inner = 0.8, outer = 0.9;
        float color_rgb = 0.5;
    
        for (i=0; i<num_of_speedways; i++) {
            glColor3f(color_rgb + 0.1*i,color_rgb + 0.1*i, color_rgb + 0.1*i);
            draw_ring(inner + 0.1*i, outer + 0.1*i, 100);
        }
    glPopMatrix();
}

/* Iscrtavanje sfere planete. */
void draw_Saturn_sphere() {
     glPushMatrix();
        glColor3f(106.0/255,42.0/255,14.0/255);
        glRotatef (rotation_angle, 0, 1.0, 0);
        glRotatef(90, 1,0,0);
        glutSolidSphere(0.7,50,50);
        
    glPopMatrix();
}

/* Iscrtavanje Spaceman karaktera. */
void draw_Spaceman() {
 
    glPushMatrix();
    glutSolidSphere(0.6, 50, 50);
    glPopMatrix();

    glColor3f(0,0.3,0);
        glPushMatrix();
        glTranslatef(-0.25,0.4,0);
        glPushMatrix();
    glRotatef(30,0,0,1);
    glRotatef(-90,1,0,0);
    glutSolidCone(0.25, 0.4, 50, 50);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.5,0,0);
    glRotatef(-30,0,0,1);
    glRotatef(-90,1,0,0);
    glutSolidCone(0.25, 0.4, 50, 50);
    glPopMatrix();
    glPopMatrix();
    
    glPopMatrix();
}

/* Obrada funkcije on_timer. */
void on_timer(int value) {
    if (value != 0) 
        return; 

    if (rotation_angle<180)
        rotation_angle=rotation_angle + 2;
    
    glutPostRedisplay();

    if (animation_ongoing)
        glutTimerFunc(50, on_timer, 0);
}
