#include <GL/glut.h>

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define PI (3.14159265359)
#define MAX_NUM_OF_STARS (50)
#define TIMER_INTERVAL_ON_START (50)
#define TIMER_MINIMAL_INTERVAL (15)

/* Deklaracija funkcija za obradu događaja. */
void on_reshape(int width, int height);
void on_display(void);
void on_keyboard(unsigned char key, int x, int y);
void on_timer(int value);
void on_timer_main(int value);

/* Funkcija za iscrtavanje pomoćnih osa. */
void draw_axes();

/* Funkcije za iscrtavanje objekata na sceni (zvezde, planete, karaktera i prepreka). */
void draw_ring(float in_radius, float out_radius, int precision);

void draw_Saturn_ring();
void draw_Saturn_sphere();
void draw_Spaceman();

void draw_barrier(float z_translation);
void draw_barriers();

void draw_stars();
void draw_star_vortex();
void draw_Uran();

/* Iscrtavanje glavnog menija. */
void draw_meni();

/* Iscrtavanje pozadine i glavnih elemenata igrice. */
void draw_backscene();
void draw_main_objects();


/* Funkcije za generisanje inicijalnih vrednosti.*/
void generate_barriers();
void generate_random_stars();

/* Pomoćne globalne promenjljive. */

/* Ugao rotacije prepreka. */
static int rotation_angle = 0;

/* Ugao rotacije prstena oko sfere u početnom meniju. */
static int main_parametar=0;

/* Parametar translacije glavnog karaktera. */
static float translation_left_right = 0;

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
void collision_detection();

/* Iscrtavanje niza karaktera. */
void draw_strings(void* font, char* string, float raster_y);

/* Pomocna funkcija za sortiranje niza. */
static int compare(const void *p1, const void *p2) {
    return (*(int*)p1  - *(int*)p2)>=0;
}

/* Strukture koje opisuju položaj barijere i zvezda. */
typedef struct {
    int speedway_position[4];
    float z_translation[4];
}Barriers;

typedef struct {
    float x_up, y_up;
    float x_down, y_down;
}Star;


static Barriers barriers;
static Star stars_positions[MAX_NUM_OF_STARS];

/* Pomoćni niz za izbacivanje duplikata pri izboru prepreka. */
static int help[4];


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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    
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
        
    switch(animation_ongoing) {
        case 0:
        /* Animacija u početnom stanju: Iscrtavanje menija i poruke. */
            glPushMatrix();
            draw_meni();
            glPopMatrix();
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, "      Welcome to Saturn! ", 0.3);
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " Press 'G' or 'g' to start.", -0.2);
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " Press 'P' or 'p' to pause.", -0.3);
            
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " To move Spaceman left press 'A' or 'a'.", -0.4);
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " To move Spaceman right press 'D' or 'd'.", -0.5);
            draw_strings(GLUT_BITMAP_TIMES_ROMAN_24, " Have fun :)", -0.6);
            
            break;
        case 1: 
        case 2:
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
        glColor3f(0.2,0.3,0.4);
        
        /* Rotacija prepreka oko y ose.*/
        glRotatef(-rotation_angle, 0, 1, 0);
        
        /* Postavljanje prepreka na adekvatne staze. */
        glTranslatef(0,0.1/2, -0.8-0.1/2-z_translation);
        
        /* Iscrtavanje prepreka.*/
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
            
            
        /* Karakter se translira ako je animacija u pokretu. */
        case 'a':
        case 'A':
            if (translation_left_right>-0.2 && animation_ongoing != 2 && animation_ongoing == 1) {
                translation_left_right -= 0.1;
                /* Karakter se pomera u stazu levo.*/
                position_of_Spaceman-=1;
                glutPostRedisplay();
            }
            break;
        case 'd':
        case 'D':
            if (translation_left_right<0.2 && animation_ongoing != 2 && animation_ongoing == 1) {
                translation_left_right += 0.1;
                /* Karakter se pomera u stazu desno.*/
                position_of_Spaceman+=1;
                glutPostRedisplay();
            }
            break;
            
        /* Animacija u pokretu se prebacuje u stanje pauze. */
        case 'p':
        case 'P':
            if (animation_ongoing == 1)
                animation_ongoing=2;
            break;
            
        /* Animacija se restatruje. */
        case 'r':
        case 'R':
            if (animation_ongoing == 3) {
                animation_ongoing=0;
                rotation_angle=0;
                timer_interval=TIMER_INTERVAL_ON_START;
                glutPostRedisplay();
            }
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
    
        /* Iscrtavanje 5 spojenih kružnih prstena. */
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

/* Inicijalizacija položaja prepreka. */
void generate_barriers() {
    int i;
    srand(time(NULL));
    
    /* Generisanje 4 random broja i smestanje u pomocni niz. */
    for (i=0; i<4; i++) {
        help[i] = (rand() % 5)+1;
    }

    /* Sortiranje niza zbog lakseg izbacivanja duplikata i izbegavanja suvisnog iscrtavanja prepreka. */
    qsort(help, 4, sizeof(int), compare);
    
    /* Zapis jedinstvenih polozaja u stazama i parametra translacije po z osi. */
    for (i=0; i<3; i++) {
        if (help[i]<help[i+1]) {
            barriers.speedway_position[i]=help[i];
            barriers.z_translation[i]=(help[i]-1)/10.0;
        } else {
            barriers.speedway_position[i]=0;
            barriers.z_translation[i]=0;
        }
    }
    
    barriers.speedway_position[3]=help[3];
    barriers.z_translation[i]=(help[3]-1)/10.0;
    
}

/* Iscrtavanje prepreka. */
void draw_barriers() {
    int i;
    
    /* Ako je funkcija koja generise slucajne polozaje oznacila staze iscrtavanja
    * sledi iscrtavanje tih prepreka. (Staze su numerisane brojevima od 1 do 5)*/
       
    for (i=0; i<4; i++) 
        if (barriers.speedway_position[i] != 0)
            draw_barrier(barriers.z_translation[i]);
    
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

void collision_detection() {
    int i;
    int danger_zone_parametar=0;
    /* Rotation_angle = 114 -> Sudar na prvoj stazi, 
     * Rotation_angle = 112 -> Sudar na drugoj stazi... 
     * 110 108 106 */
    for (i=0; i<4; i++)  {
            if (barriers.speedway_position[i] != 0) {
                if (rotation_angle==(114-danger_zone_parametar) && barriers.speedway_position[i]==position_of_Spaceman) {
                    
                    animation_ongoing=3;
                }
            }
        danger_zone_parametar+=2;
    }
    
    /* Ispisivanje poruke pri završetku animacije. */
    if (animation_ongoing == 3) {
        glColor3f(1,1,1);
        draw_strings(GLUT_BITMAP_TIMES_ROMAN_24,"                   EXIT         ", 0.4);
        draw_strings(GLUT_BITMAP_TIMES_ROMAN_24,"If you want to play again press 'R' or 'r'. ", 0.3);
        draw_strings(GLUT_BITMAP_TIMES_ROMAN_24,"If yuu want to quit press 'esc'. ", 0.2);
        
    }

}

/* Iscrtavanje početnog menija. */
void draw_meni() {
    glPushMatrix();
        glutTimerFunc(timer_interval, on_timer_main, timer_value_main);
        
        glPushMatrix();
        draw_stars();
        glTranslatef(1,0,0);
        draw_star_vortex();
        glPopMatrix();
        
        glPushMatrix();
            glTranslatef(-0.5,0.2,0);
            glScalef(0.5,0.5,0.5);
            draw_Saturn_sphere();
            glRotatef(main_parametar,0,1,0);
            glRotatef(15,1,0,1);
            draw_Saturn_ring();
        glPopMatrix();
    glPopMatrix();
}


/* Obrada main tajmera. */
void on_timer_main(int value) {
    main_parametar+=5;
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


void draw_Uran() {
    glPushMatrix();
    glTranslatef(2,1,-3);
    glColor4f(0.2,0.2,0.8,0.5);
    glutSolidSphere(0.1,50,50);
    glPopMatrix();
}
