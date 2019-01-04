#include <GL/glut.h>

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "image.h"

#define PI (3.14159265359)
#define MAX_NUM_OF_STARS (50)
#define TIMER_INTERVAL_ON_START (30)
#define TIMER_MINIMAL_INTERVAL (15)

#define STATE_UP (1)
#define STATE_DOWN (0)

#define FILENAME0 "texture.bmp"
#define FILENAME1 "1.bmp"
#define FILENAME2 "2.bmp"
#define FILENAME3 "3.bmp"
#define FILENAME4 "4.bmp"
#define FILENAME5 "1.bmp"

static int jump_state = 0;
static GLuint names[6];

/* Deklaracija funkcija za obradu događaja. */
static void on_reshape(int width, int height);
static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);

static void on_timer(int value);
static void on_timer_main(int value);
static void on_timer_jump(int value);

/* Funkcija za iscrtavanje pomoćnih osa. 
static void draw_axes(void); */

/* Funkcije za iscrtavanje objekata na sceni (Zvezda, planeta, karaktera i prepreka). */
static void draw_ring(float in_radius, float out_radius, int precision);

static void draw_Saturn_ring(void);
static void draw_Saturn_sphere(void);
static void draw_Spaceman(void);

static void draw_barrier(float z_translation);
static void draw_barriers(void);

static void draw_stars(void);
static void draw_star_vortex(void);
static void draw_Uran(void);

/* Iscrtavanje glavnog menija. */
static void draw_meni(void);

/* Iscrtavanje pozadine i glavnih elemenata igrice. */
void draw_backscene(void);
void draw_main_objects(void);


/* Funkcije za generisanje inicijalnih vrednosti.*/
void generate_barriers(void);
void generate_random_stars(void);

/* Inicijalizacija parametara za postavljanje tekstura. */
static void initialize(void);

/* Pomoćne globalne promenjljive i strukture. */

/* Ugao rotacije prepreka. */
static int rotation_angle = 0;

/* Ugao rotacije sfere. */
static int rotation_angle_sphere = 0;

/* Ugao rotacije prstena oko sfere u početnom meniju. */
static int main_parametar=0;

/* Parametar translacije glavnog karaktera. */
static float translation_left_right = 0;
static float translation_up = 0;

/* Parametar koji označava stanja animacije. 
 * Početno stanje (0) stanje animcije u pokretu (1), stanje pauze (2), završno stanje (3). */
static int animation_ongoing = 0;

/* Broj milisekundi koji određuje periode ponovnog pozivanja funkcije on_timer. */
static unsigned int timer_interval = TIMER_INTERVAL_ON_START;

/* Identifikacije tajmera. */
static int timer_value = 0;
static int timer_value_main = 1;

/* Broj staze na kojoj se nalazi glavni karakter.*/
static int position_of_Spaceman=3;

/* Detekcija sudara. */
static void collision_detection(void);

/* Iscrtavanje niza karaktera. */
static void draw_strings(void* font, char* string, float raster_y);

/* Pomocna funkcija za sortiranje niza. */
static int compare(const void *p1, const void *p2) {
    return (*(int*)p1  - *(int*)p2)>=0;
}

/* Strukture koje opisuju položaj barijere i zvezda. */
typedef struct {
    int speedway_position;
    float z_translation;
} Barrier;

typedef struct {
    float x_up, y_up;
    float x_down, y_down;
} Star;


static Barrier barriers[5];
static Star stars_positions[MAX_NUM_OF_STARS];

/* Pomoćni niz za izbacivanje duplikata pri izboru prepreka. */
static int help[5];

/* Pomoćni parametar za skok glavnog karaktera. */
static int jump_flag = 0;


int main(int argc, char** argv) {
    
    /* Inicijalizacija GLUT-a. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    
    /* Kreiranje prozora. */
    glutCreateWindow(argv[0]);
    glutFullScreen();

    /* Registracija funkcija za obradu događaja. */
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
    
    /* OpenGL Inicijalizacija. */
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    /* Generisanje inicijanih vrednosti. */
    generate_barriers();
    generate_random_stars();
    
    /* Inicijalizacija parametara za postavljanje teksture i učitavanje tekstura. */
    initialize();

    /* Ulazi se u glavnu petlju. */
    glutMainLoop();
    
    return 0;
}

/* Pomoćna funkcija za iscrtavanje koordinatnih osa. 
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
}*/

/* Obrada događaja on on_display. */
void on_display(void) {
    
    /* Obavlja se čišćenje bafera. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Podešavaju se parametri scene u koju se gleda i "oka" posmatrača. */
    gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);
        
    switch(animation_ongoing) {
        case 0:
        /* Animacija u početnom stanju: Iscrtavanje menija i poruke. */
            glPushMatrix();
            draw_meni();
            glPopMatrix();
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, "      Welcome to Saturn! ", 0.3);
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " Press 'G' or 'g' to start.", 0);
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " Press 'P' or 'p' to pause.", -0.1);
            
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " To move Spaceman left press 'A' or 'a'.", -0.2);
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " To move Spaceman right press 'D' or 'd'.", -0.3);
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " To make Spaceman jump press space.", -0.4);
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " To exit game press escape.", -0.5);
            
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " Have fun :)", -0.6);
            
            break;
        case 1:
            draw_backscene();
            draw_main_objects();
            collision_detection();
            break;
        case 2:
            draw_backscene();
            draw_main_objects();
            collision_detection();
            glColor3f(1,1,1);
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24,"         GAME PAUSED         ", 0.3);
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24,"  Press 'G' or 'g' to resume.  ", 0.2);
            break;
        case 3:
        /* Animacija u pokretu, pauzi ili na završetku: Iscrtavanje scene, main objekata i poruke. */
            draw_backscene();
            draw_main_objects();
            collision_detection();
            break;
    }
    glutSwapBuffers();
}

/* Iscrtavanje barijera i transliranje po stazama. */
void draw_barrier(float z_translation) {
    glPushMatrix();
        
    /* Uključivanje osvetljenja za prepreke. */
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 };
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    glColor3f(0.2,0.2,0.2);
        
        /* Rotacija prepreka oko y ose.*/
        glRotatef(-rotation_angle, 0, 1, 0);
        
        /* Postavljanje prepreka na adekvatne staze. */
        glTranslatef(0,0.1/2, -0.8-0.1/2-z_translation);
        
        /* Iscrtavanje prepreka.*/
        glutSolidCube(0.1);
    
    glDisable(GL_LIGHT0);    
    glDisable(GL_LIGHTING);
    glPopMatrix();
}

/* Iscrtavanje prstena. */
void draw_ring(float in_radius, float out_radius, int precision) {
    GLUquadric* disk = gluNewQuadric();
    gluQuadricTexture(disk, GL_TRUE);
    gluDisk(disk, (GLdouble) in_radius , (GLdouble) out_radius, (GLint) precision, (GLint) precision);        
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
    
    /* Na taster 'g' ili 'G'  se pokreće anmacija, 
     * tasteri 'a', 'A', 'd', 'D' su rezervisani za kretanje karaktera po stazama,
     * pritiskom na taster 'p' ili 'P' animacija se pauzira,
     * pritiskom na taster 'r' ili 'R' animacija se restatruje nakon završetka,
     * pritiskom na taster "esc" animacija se prekida. */
    
    
    /* Parametar animation_ongoing može imati vrednosti 
     * 0 (Animacija nije pokrenuta), 1 (Animacija je pokrenuta),  2 (Animacija je pauzirana), 3 (Animacija je završena). */
    switch(key) {
        case 27: 
            exit(0);
            break;
        case 'g':
        case 'G':            
            /* Animacija se pokreće ako je u početnom stanju ili u stanju pauze. */
            if (animation_ongoing == 0 || animation_ongoing == 2 ) {
                glutTimerFunc(timer_interval, on_timer, timer_value);
                animation_ongoing = 1;
            }
            break;
            
            
        /* Karakter se translira levo ili desno ako je animacija u pokretu. */
        /* Translacija u stazu levo. */
        case 'a':
        case 'A':
            if (translation_left_right>-0.2 && animation_ongoing != 2 && animation_ongoing == 1) {
                
                /* Karakter se pomera ako nije u stanju skoka. */
                if (jump_flag == 0) {
                    translation_left_right -= 0.1;
                    
                    /* Karakter se pomera u stazu levo.*/
                    position_of_Spaceman-=1;
                    glutPostRedisplay();
                }
            }
            break;
            
        /* Translacija u stazu desno. */
        case 'd':
        case 'D':
            if (translation_left_right<0.2 && animation_ongoing != 2 && animation_ongoing == 1) {
                
                /* Karakter se pomera ako nije u stanju skoka. */
                if (jump_flag == 0) {
                    translation_left_right += 0.1;
                    /* Karakter se pomera u stazu desno.*/
                    position_of_Spaceman+=1;
                    glutPostRedisplay();
                }
            }
            break;
            
            
        /* Animacija u pokretu se prebacuje u stanje pauze. */
        case 'p':
        case 'P':
            if (animation_ongoing == 1) {
                animation_ongoing=2;
            }
            break;
            
        /* Animacija se restartuje. */
        case 'r':
        case 'R':
            if (animation_ongoing == 3) {
                /* TODO CHECK! */
                jump_flag = 0;
                translation_up = 0;
                rotation_angle=0;
                timer_interval=TIMER_INTERVAL_ON_START;
                animation_ongoing=0;
                glutPostRedisplay();
            }
            break;
            
        case ' ':
            if (animation_ongoing == 1) {
                if (jump_flag != 1) {
                    jump_flag = 1;
                    jump_state = STATE_UP;
                    glutTimerFunc(10, on_timer_jump, 3);
                }
            }
        }
}

/* Iscrtavanje Saturnovog prstena. */
void draw_Saturn_ring() {
    glPushMatrix();
        glRotatef(90, 1,0,0);
        
        int i=0;
        int num_of_speedways = 5;
    
        float inner = 0.8, outer = 0.9;
    
        /* Iscrtavanje 5 spojenih kružnih prstena. */
        for (i=0; i<num_of_speedways; i++) {
            
            glBindTexture(GL_TEXTURE_2D, names[i+1]);
            glEnable(GL_TEXTURE_2D);
            
             draw_ring(inner + 0.1*i, outer + 0.1*i, 100);
        
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
        }
    glPopMatrix();
}


/* Iscrtavanje planete. */
void draw_Saturn_sphere() {
    
    glPushMatrix();
    
        glBindTexture(GL_TEXTURE_2D, names[0]);
        glEnable(GL_TEXTURE_2D);
            
        GLUquadric* sphere = gluNewQuadric();
        gluQuadricTexture(sphere, GL_TRUE);
        
        glRotatef(90,1,0,0);
        gluSphere(sphere, (GLdouble) 0.7, (GLint) 50, (GLint) 50);        

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    
}

/* Iscrtavanje glavnog karaktera. */
void draw_Spaceman() {
 
    glPushMatrix();
        glColor3f(0.1,0.5,0.4);
        glutSolidSphere(0.6, 50, 50);
    glPopMatrix();

    glColor3f((float)rand()/RAND_MAX,(float)rand()/RAND_MAX,(float)rand()/RAND_MAX);
   
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

/* Inicijalizacija položaja prepreka. */
void generate_barriers() {
    int i;
    srand(time(NULL));
    
    /* Generisanje 4 random broja i smestanje u pomocni niz. */
    for (i=0; i<5; i++) {
        help[i] = (rand() % 5)+1;
    }

    /* Sortiranje niza zbog lakseg izbacivanja duplikata i izbegavanja suvisnog iscrtavanja prepreka. */
    qsort(help, 5, sizeof(int), compare);
    
    /* Zapis jedinstvenih polozaja u stazama i parametra translacije po z osi. */
    for (i=0; i<4; i++) {
        if (help[i]<help[i+1]) {
            barriers[i].speedway_position=help[i];
            barriers[i].z_translation=(help[i]-1)/10.0;
        } else {
            barriers[i].speedway_position=0;
            barriers[i].z_translation=0;
        }
    }
    
    barriers[4].speedway_position=help[4];
    barriers[4].z_translation=(help[4]-1)/10.0;
}

/* Iscrtavanje prepreka. */
void draw_barriers() {
    int i;
    
    /* Ako je funkcija koja generise slucajne polozaje oznacila staze iscrtavanja
    * sledi iscrtavanje tih prepreka. (Staze su numerisane brojevima od 1 do 5)*/
       
    for (i=0; i<5; i++) 
        if (barriers[i].speedway_position != 0)
            draw_barrier(barriers[i].z_translation);
    
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
    glColor3f(1,1,1);
    int i;
    
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
        draw_Uran();
        draw_stars();
        glTranslatef(0.5,0.5,0);
        draw_star_vortex();
        glTranslatef(0.7,-0.4,0);
        draw_star_vortex();
    glPopMatrix();
}

/* Iscrtavanje glavnih objekata na sceni.*/
void draw_main_objects() {
    
    glPushMatrix();
    
        glRotatef(10,1,0,0);
        glTranslatef(-0.70,-0.1,0.33);
        
        glPushMatrix();
        glRotatef(-rotation_angle_sphere,0,1,0);
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
        glTranslatef(translation_left_right, translation_up, 0);
        glTranslatef(0.94,0.07,1.2);
        glScalef(0.07,0.1,0.08);
        glColor3f(0,0.7,0);
        draw_Spaceman();
        
        glPopMatrix(); 
    glPopMatrix();

}


/* Detekcija sudara. */
void collision_detection() {
    int i;
    int danger_zone_parametar=0;
    for (i=0; i<5; i++)  {
            switch (barriers[i].speedway_position) {
                case 1:
                    danger_zone_parametar = 0;
                    break;
                case 2:
                    danger_zone_parametar = 2;
                    break;
                case 3:
                    danger_zone_parametar = 4;
                    break;
                case 4:
                    danger_zone_parametar = 6;
                    break;
                case 5:
                    danger_zone_parametar = 8;
                    break;
            }
                
            if (rotation_angle==(116-danger_zone_parametar) && barriers[i].speedway_position==position_of_Spaceman && translation_up==0)    {
                jump_flag = 0;
                translation_up = 0;
                animation_ongoing=3;
            }
        }           
    
    /* Ispisivanje poruke pri završetku animacije. */
    if (animation_ongoing == 3) {
        glColor3f(1,1,1);
        draw_strings(GLUT_BITMAP_TIMES_ROMAN_24,"            GAME OVER          ", 0.3);
        draw_strings(GLUT_BITMAP_TIMES_ROMAN_24,"Press 'R' or 'r' to play again.", 0.2);
    }

}

/* Iscrtavanje početnog menija. */
void draw_meni() {
    
    glutTimerFunc(timer_interval, on_timer_main, timer_value_main);
    
    glPushMatrix();
        glPushMatrix();
        draw_stars();
        glTranslatef(1,0,0);
        draw_star_vortex();
        glPopMatrix();
        
        glPushMatrix();
            
            glTranslatef(-0.5,0.2,0);
            glScalef(0.4,0.4,0.4);
            glPushMatrix();
            glRotatef(-rotation_angle_sphere, 0,1,0);
            draw_Saturn_sphere();
            
            glPushMatrix();
            glRotatef(10,1,0,0);
            draw_Saturn_ring();
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();

}

/* Obrada funkcije tajmera za obradu rotacije prepreka. */
void on_timer(int value) {
    
    if (value != timer_value) 
        return; 
    
    /* Ugao se povećava do 180 stepeni. */
    if (rotation_angle<180) {
        rotation_angle += 2;
        glutPostRedisplay();
    }
    /* Ugao se restatruje i generišu se nove prepreke. */
    else {
        rotation_angle=0;    
        generate_barriers();
        /* Interval pozivanja tajmera se smanjuje do minimalne definisane vrednosti,  
         * što dovodi do ubrzavanja rotacije prepreka dok se ne dostigne ta minimalna vrednost, 
         * nakon toga brzina rotacije ostaje ista. */
        if (timer_interval>TIMER_MINIMAL_INTERVAL) {
            timer_interval -= 5;
        }
    }
    
    /* Nastavljanje animacije. */
    if (animation_ongoing == 1)
            glutTimerFunc(timer_interval, on_timer, timer_value);
    
}

/* Obrada funkcije main tajmera za rotaciju Saturna. */
void on_timer_main(int value) {
    if (animation_ongoing == 0) {
        rotation_angle_sphere+=2;
        main_parametar+=10;
    }
    
    glutPostRedisplay();
}

/* Obrada funkcije jump tajmera za skok glavnog karaktera. */


void on_timer_jump(int value) {
    
    /* Ako je animacija pokrenuta i stanje skoka postavljeno na STATE_UP 
     * glavni karakter se translira pozitivnom vrednošću po y osi. */
    if (animation_ongoing == 1) {
    
        /* Ako je animacija pokrenuta i stanje skoka postavljeno na STATE_UP 
         * glavni karakter se translira pozitivnom vrednošću po y osi. */
        if (translation_up < 0.3 && jump_state == STATE_UP) {
            translation_up += 0.02;
        } 
        /* Inače se stanje postavlja na STATE_DOWN koje je okidač za promenu smera translacije glavnog karaktera. */
        else if (translation_up >= 0.3) {
                jump_state = STATE_DOWN;
            }
        
        /* Ako je animacija pokrenuta i stanje skoka postavljeno na STATE_DOWN 
         * glavni karakter se translira negativnom vrednošću po y osi sve dok se ne vrati u početno stanje. */
        if (jump_state == STATE_DOWN && translation_up > 0)
            translation_up -= 0.015;
        
        /* Ako karakter vraćen u početno stanje, restartuju se parametri skoka. */
        if (translation_up <= 0) {
            jump_flag = 0;
            translation_up = 0;
        }
    }
    
    /* Ako smo u stanju skoka, nastavljamo sa animacijom. */
    if (jump_flag == 1) {
        glutTimerFunc(10,on_timer_jump, 3);
    }
    
    
        glutPostRedisplay();    
}


/* Iscrtavanje niza karaktera. */
void draw_strings(void* font, char* string, float raster_y) {
    glRasterPos3f(0.1,raster_y,0);
    
    int i;
    int len = strlen(string);
    
    for (i=0; i<len; i++)
        glutBitmapCharacter(font, string[i]);
}


/* Iscrtavanje Urana. */
void draw_Uran() {
    glPushMatrix();
    glTranslatef(2,1,-3);
    glColor4f(0.2,0.2,0.8,0.3);
    glutSolidSphere(0.1,50,50);
    glPopMatrix();
}

/* Inicijalizacija parametara za postavljanje tekstura. */
static void initialize(void)
{
    /* Deklaracija teskture učitane iz fajla. */
    Image * image;

    /* Uključuju se teksture. */
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);

    /* Inicijalizuje se promenljiva image koji ce sadrzati teksture ucitane iz fajlova. */
    image = image_init(0, 0);

    /* Kreira se prva tekstura. */
    /* Generisu se identifikatori tekstura. */
    glGenTextures(6, names);

    image_read(image, FILENAME0);

    glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    image_read(image, FILENAME1);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    
    image_read(image, FILENAME2);

    glBindTexture(GL_TEXTURE_2D, names[2]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    
    image_read(image, FILENAME3);

    glBindTexture(GL_TEXTURE_2D, names[3]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    
    image_read(image, FILENAME4);

    glBindTexture(GL_TEXTURE_2D, names[4]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    
    image_read(image, FILENAME5);

    glBindTexture(GL_TEXTURE_2D, names[5]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    
    
    /* Iskljucujemo aktivnu teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);
}

