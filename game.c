#include <stdlib.h>
#include <GL/glut.h>

void on_reshape(int width, int height);
void on_display(void);
void on_keyboard(unsigned char key, int x, int y);
void drawAxes();

static int rotation_angle = 0;

int main(int argc, char** argv) {
    
    /* Inicijalizacija GLUT-a. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    
    /* Kreiranje prozora. */
    glutInitWindowSize(800, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    
    /* Registracija funkcija za obradu događaja. */
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
    
    /* OpenGL Inicijalizacija. */
    glClearColor(0, 0, 0, 0);
    
    /* Ulazi se u glavnu petlju. */
    glutMainLoop();
    
    return 0;
}

/* Pomoćna funkcija za iscrtavanje koordinatnih osa. */
void drawAxes()
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
    drawAxes();
    
    glColor3f(1,1,1);
    
    
    
    glPushMatrix();
        glRotatef (rotation_angle, 0, 1.0, 0);
        glRotatef(90, 1,0,0);
        glutWireSphere(0.7,50,50);
    glPopMatrix();
    
    
    glPushMatrix();
        glColor3f(0.5,0.5,0.5);
        glRotatef (rotation_angle, 0, 1.0, 0);
        glRotatef(70, 1,0,0);
        glutWireTorus(0.1,1,50,50);   
    glPopMatrix();

    
    glutSwapBuffers();
}


/* Obrada događaja on on_reshape. */
void on_reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (0, 0, 2, 0, 0, 0.0, 0.0, 1.0, 0.0);
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
