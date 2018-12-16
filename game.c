#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define PI 3.14159265359

void on_reshape(int width, int height);
void on_display(void);
void on_keyboard(unsigned char key, int x, int y);

void draw_axes();
void draw_ring(float in_radius, float out_radius, int precision);

void draw_Saturn_ring();
void draw_Saturn_sphere();

static int rotation_angle = 0;

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

    glColor3f(1,1,1);
    
    glPushMatrix();
        glScalef(1.5,1.5,1.5);
        glTranslatef(-0.70,-0.2,0.33);
        draw_Saturn_ring();
        draw_Saturn_sphere();
    glPopMatrix();
    
    glutSwapBuffers();
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
    gluPerspective(40, (float) width / height, 1, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (0,0,3, 0, 0, 0.0, 0.0, 1.0, 0.0);
        
    
}

/* Obrada događaja on on_keyboard. */
void on_keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27: 
            exit(0);
            break;
        case 'm':
            rotation_angle += 5;
            glutPostRedisplay();
            break;
    }
}

void draw_Saturn_ring() {
    glPushMatrix();
        glScalef(1,1,1.5);
        glRotatef(100, 1,0,0);
        
        int i=0;
        int precision = 100;
        int num_of_speedways = 5;
    
        float inner = 0.8, outer = 0.9;
        float color_rgb = 0.5;
    
        for (i=0; i<num_of_speedways; i++) {
            glColor3f(color_rgb + 0.1*i,color_rgb + 0.1*i, color_rgb + 0.1*i);
            draw_ring(inner + 0.1*i, outer + 0.1*i, precision);
        }
    glPopMatrix();
}

void draw_Saturn_sphere() {
     glPushMatrix();
        glColor3f(106.0/255,42.0/255,14.0/255);
        glRotatef (rotation_angle, 0, 1.0, 0);
        glRotatef(90, 1,0,0);
        glutSolidSphere(0.7,50,50);
        
    glPopMatrix();
}
