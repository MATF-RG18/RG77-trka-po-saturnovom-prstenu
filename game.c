#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <GL/glut.h>

#define PI (3.14159265359)

void on_reshape(int width, int height);
void on_display(void);
void on_keyboard(unsigned char key, int x, int y);
void on_timer(int value);

#include <GL/glut.h>

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define PI (3.14159265359)
#define MAX_NUM_OF_STARS (50)

/* Deklaracija funkcija za obradu događaja. */
void on_reshape(int width, int height);
void on_display(void);
void on_keyboard(unsigned char key, int x, int y);
void on_timer(int value);

/* Funkcija za iscrtavanje pomoćnih osa. */
void draw_axes();

/* Funkcije za iscrtavanje scene, prstena, karaktera i prepreka. */
void draw_ring(float in_radius, float out_radius, int precision);

void draw_Saturn_ring();
void draw_Saturn_sphere();
void draw_Spaceman();

void draw_barrier(float z_translation);
void draw_barriers();

void draw_stars();
void draw_star_vortex();

/* Iscrtavanje pozadine i glavnih elemenata igrice. */
void draw_backscene();
void draw_main_objects();

/* Funkcije za generisanje inicijalnih vrednosti.*/
void generate_barriers();
void generate_random_stars();

/* Pomoćne globalne promenjljive. */
static int rotation_angle = 0;
static float translation_left_right = 0;
static int animation_ongoing = 0;
static unsigned int timer_interval = 50;
static int timer_value = 0;

/* Strukture koje opisuju položaj barijere i zvezda. */
typedef struct {
    float z_translation[5];
}Barriers;

typedef struct {
    float x_up, y_up;
    float x_down, y_down;
}Star;

static Barriers barriers;
static Star stars_positions[MAX_NUM_OF_STARS];

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
    
    /* Generisanje inicijanih vrednosti. */
    generate_barriers();
    generate_random_stars();
    
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
    
    /* Obavlja se čišćenje bafera. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Podešavaju se parametri scene u koju se gleda i "oka" posmatrača. */
    gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);
        
    /* Iscrtavanje pozadine i main objekata. */
    draw_backscene();
    draw_main_objects();
    glutSwapBuffers();
}

/* Iscrtavanje barijera i transliranje po stazama. */
void draw_barrier(float z_translation) {
    glPushMatrix();
    glColor3f(0.2,0.1,0.51);
    glRotatef(-rotation_angle, 0, 1, 0);
    glTranslatef(0,0.1/2, -0.8-0.1/2-z_translation);
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
    /* Podešavanje pogleda. */
    glViewport(0,0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    /* Podešavanje perspektive projekcije.*/
    gluPerspective(30, (float) width / height, 1, 1000);
}

/* Obrada događaja on on_keyboard. */
void on_keyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    
    switch(key) {
        case 27: 
            exit(0);
            break;
        case 'g':
        case 'G':
            if (!animation_ongoing) {
                glutTimerFunc(timer_interval, on_timer, timer_value);
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
        case 'p':
        case 'P':
            animation_ongoing=0;
            break;
    }
}

/* Iscrtavanje Saturnovog prstena. */
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

/* Iscrtavanje planete. */
void draw_Saturn_sphere() {
     glPushMatrix();
     
        glColor3f(106.0/255,42.0/255,14.0/255);
        glRotatef (rotation_angle, 0, 1.0, 0);
        glRotatef(90, 1,0,0);
        glutSolidSphere(0.7,50,50);
        
    glPopMatrix();
}

/* Iscrtavanje glavnog karaktera. */
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

/* Obrada funkcije tajmera. */
void on_timer(int value) {
    if (value != timer_value) 
        return; 
    
    if (rotation_angle<180) {
        rotation_angle=rotation_angle + 2;
     
    glutPostRedisplay();

    }
    else {
        rotation_angle=0;    
        generate_barriers();
        if (timer_interval>20) {
            timer_interval -= 5;
        }
    }
    
    if (animation_ongoing)
            glutTimerFunc(timer_interval, on_timer, timer_value);
    
}

/* Inicijalizacija položaja prepreka. */
void generate_barriers() {
    int i, n;
    srand(time(NULL));
    for (i=0; i<5; i++) {
        n = rand() % 5;
        barriers.z_translation[i]=n/10.0;
    }
}

/* Iscrtavanje prepreka. */
void draw_barriers() {
    draw_barrier(barriers.z_translation[0]);
    draw_barrier(barriers.z_translation[1]);
    draw_barrier(barriers.z_translation[2]);
    draw_barrier(barriers.z_translation[3]);
}

/* Inicijalizacija položaja zvezda. */
void generate_random_stars() {
    srand(time(NULL));
    int i;
    for (i=0; i<MAX_NUM_OF_STARS; i++) {
        stars_positions[i].x_up=pow(-1, i) *4.0 * rand()/RAND_MAX;
        stars_positions[i].y_up=pow(-1, i+1) * 2.5 * rand()/RAND_MAX;
        
        stars_positions[i].x_down=pow(-1, i+1) * 4.0 * rand()/RAND_MAX;
        stars_positions[i].y_down=pow(-1, i+1) * 2.5 * rand()/RAND_MAX;
    }
}

/* Iscrtavanje zvezda. */
void draw_stars() {
    int i;
    glColor3f(1,1,1);
    
    glBegin(GL_POINTS);
    
    for (i=0; i<MAX_NUM_OF_STARS; i++) {
        glVertex3f(stars_positions[i].x_up, stars_positions[i].y_up, -5);    
        glVertex3f(stars_positions[i].x_down, stars_positions[i].y_down, -5);    
    }
    glEnd();
}

/* Iscrtavanje zvedanog vrtloga. */
void draw_star_vortex() {
    float i;
    glColor3f(1,1,1);
     glBegin(GL_POINTS);
    for(i=0; i<100; i+=1) {
        glVertex2f(cos(i)*1.0/i, sin(i)*1.0/i);
    }
    glEnd();
}

/* Iscrtavanje nebeskih tela na sceni, u pozadini. */
void draw_backscene() {
    glPushMatrix();
        draw_stars();
        glTranslatef(0.5,0.5,0);
        draw_star_vortex();
    glPopMatrix();
}

/* Iscrtavanje glabih objekata na sceni.*/
void draw_main_objects() {
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
        draw_barriers();
        glPopMatrix();
        
        
        glPushMatrix();
        glTranslatef(translation_left_right,0,0);
        glTranslatef(0.95,0.07,1.1);
        glScalef(0.07,0.1,0.08);
        glColor3f(0,0.7,0);
        draw_Spaceman();
        
        glPopMatrix(); 
    glPopMatrix();
}
