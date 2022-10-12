/*
HADJ RABAH
ZINE EDDINE
UNIVERSITE PARIS 8

JEU DE BASKET-BALL
GL4D

Voici l'implementation d'un petit
debut de jeu de basket-ball en utilisant
GL4D.

https://github.com/noalien/GL4Dummies



*/

#include <assert.h>
#include <GL4D/gl4dp.h>
#include "rasterize.h"
#include <GL4D/gl4duw_SDL2.h>
#include <math.h>

static void init(void);
static void draw(void);
static void key(int keycode);
static void sortie(void);

//declaration des differentes surfaces
static surface_t * _terrain = NULL;
static surface_t * _panier = NULL;
static surface_t * _fresque = NULL;
static surface_t * _joeur = NULL;
static surface_t * _balle = NULL;
static surface_t * _cube_logo1 = NULL;
static surface_t * _cube_logo2 = NULL;

//variables pour la camera
static float _ycam = 3.0f;
static float _lcam = 3.0f;

//variables pour la balle et le joueur
static float _bx = 8.0f;
static float _by = -2.0f;
static float _bz = 0.0f;
static float _x = 0.0f; //10
static float _y = 0.0f; //-5
static float _z = 0.0f; //-3
static float tire_ball = 0;


int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, 
			 "Jeu de Basket-Ball - Hadj Rabah Zine Eddine - GL4D ", 
			 10, 10, 800, 600, 
			 GL4DW_SHOWN) ) {   
    return 1;
  }
  init();
  gl4duwKeyDownFunc(key);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

void init(void) {
  GLuint id;
  gl4dpInitScreen();
  SDL_GL_SetSwapInterval(0);
  
  _terrain = mk_cube();
  _panier = mk_cube();
  _fresque = mk_quad();
  _joeur = mk_cube();
  _balle = mk_sphere(32,32);
  _cube_logo1 =  mk_cube();
  _cube_logo2 = mk_cube();
  
  //mise en place des differentes textures
  //peut etre optimiser avec une boucle
  id = get_texture_from_BMP("images/ter.bmp");
  set_texture_id(  _terrain, id);
  id = get_texture_from_BMP("images/panier.bmp");
  set_texture_id(  _panier, id);
  id = get_texture_from_BMP("images/fresque.bmp");
  set_texture_id(  _fresque, id);
  id = get_texture_from_BMP("images/player.bmp");
  set_texture_id(  _joeur, id);
  id = get_texture_from_BMP("images/bll.bmp");
  set_texture_id(  _balle, id);
  id = get_texture_from_BMP("images/lk_logo.bmp");
  set_texture_id(  _cube_logo2, id);
  id = get_texture_from_BMP("images/lk_logo1.bmp");
  set_texture_id(  _cube_logo1, id);
  
  //Pour la texture, la couleur et la lumière
  enable_surface_option(  _terrain, SO_USE_TEXTURE);
  enable_surface_option(  _panier, SO_USE_TEXTURE);
  enable_surface_option(  _fresque, SO_USE_TEXTURE);
  enable_surface_option(  _joeur,SO_USE_TEXTURE);
  enable_surface_option(  _balle,SO_USE_TEXTURE);
  enable_surface_option(  _cube_logo1, SO_USE_TEXTURE);
  enable_surface_option(  _cube_logo2, SO_USE_TEXTURE);
  disable_surface_option(  _terrain, SO_USE_COLOR);
  disable_surface_option(  _panier, SO_USE_COLOR);
  disable_surface_option(  _fresque, SO_USE_COLOR);
  disable_surface_option(  _joeur, SO_USE_COLOR);
  disable_surface_option(  _balle, SO_USE_COLOR);
  disable_surface_option(  _cube_logo1, SO_USE_COLOR);
  disable_surface_option(  _cube_logo2, SO_USE_COLOR);
  enable_surface_option(  _terrain, SO_USE_LIGHTING);
  enable_surface_option(  _panier, SO_USE_LIGHTING);
  enable_surface_option(  _fresque, SO_USE_LIGHTING);
  enable_surface_option(  _joeur, SO_USE_LIGHTING);
  enable_surface_option(  _balle, SO_USE_LIGHTING);
  enable_surface_option(  _cube_logo1, SO_USE_LIGHTING);
  enable_surface_option(  _cube_logo2, SO_USE_LIGHTING);

  atexit(sortie);
}

void draw(void) {

  static float a = 0.0f;
  static float anim_bx = 0.0f;
  
  float model_view_matrix[16], projection_matrix[16], nmv[16];
  gl4dpClearScreen();
  clear_depth_map();
  MFRUSTUM(projection_matrix, -0.05f, 0.05f, -0.05f, 0.05f, 0.1f, 1000.0f);
  MIDENTITY(model_view_matrix);
  lookAt(model_view_matrix, 32.0f +_lcam, 30.0f+_ycam, 40, 0, 0, 0, 0, 1, 0);

  //fresque decor
  memcpy(nmv, model_view_matrix, sizeof nmv); 
  translate(nmv, -20.0f, 10.0f, -40.0f);
  scale(nmv,40.0f,15.0f,40.0f);
  transform_n_rasterize(_fresque, nmv, projection_matrix);
  
  //balle
  memcpy(nmv, model_view_matrix, sizeof nmv); 
  translate(nmv,  _bx, _by , -3.0f +_bz);
  rotate(nmv, a*1.2, 0.0f, 0.0f, 1.0f);
  transform_n_rasterize(_balle, nmv, projection_matrix);

  //joueur
  memcpy(nmv, model_view_matrix, sizeof nmv); 
  translate(nmv, 10.0f +_x , -5.0f + _y, -3.0f + _z);
  rotate(nmv, a, 0.0f, 1.0f, 0.0f);
  scale(nmv,1.0f,4.0f,1.0f);
  transform_n_rasterize(_joeur, nmv, projection_matrix);

  //panier de basket
  int _py,_pz;
  for( _py = 0; _py < 13 ; _py++){
    memcpy(nmv, model_view_matrix, sizeof nmv); 
    translate(nmv, -31.0f, -7.0f + _py, -3.0f);
    rotate(nmv, a, 0.0f, 0.0f, 0.0f);
    transform_n_rasterize(_panier, nmv, projection_matrix);
    if(_py == 12){
      for( _pz = 0; _pz < 7;_pz++){
        memcpy(nmv, model_view_matrix, sizeof nmv); 
        translate(nmv, -31.0f, -7.0f +_py ,-6.0f+ _pz);
        rotate(nmv, a, 0.0f, 0.0f, 0.0f);
        transform_n_rasterize(_panier, nmv, projection_matrix);
      }
    }

  }
  
  //cube logo 2
  memcpy(nmv, model_view_matrix, sizeof nmv); 
  rotate(nmv, a, 0.0f, 0.0f, 1.0f);
  translate(nmv, 12.0f, 13.0f , 23.0f);
  rotate(nmv, a, 0.0f, 1.0f, 1.0f);
  scale(nmv,2.0f,2.0f,2.0f);
  transform_n_rasterize(_cube_logo2, nmv, projection_matrix);
  //cube logo 1
  memcpy(nmv, model_view_matrix, sizeof nmv); 
  rotate(nmv, a, 0.0f,1.0f, 1.0f);
  translate(nmv, 4.0f, -7.0f , 20.0f);
  rotate(nmv, a, 0.0f, 1.0f, 1.0f);
  scale(nmv,1.0f,1.0f,1.0f);
  transform_n_rasterize(_cube_logo1, nmv, projection_matrix);

  //terrain
  memcpy(nmv, model_view_matrix, sizeof nmv); 
  translate(nmv, 0.0f, -32.0f , 0.0f);
  scale(nmv,4.0f,20.0f,2.0f);
  transform_n_rasterize(_terrain, nmv, projection_matrix);
  //terrain
  memcpy(nmv, model_view_matrix, sizeof nmv); 
  translate(nmv, 0.0f, -9.0f, -3.0f);
  scale(nmv,30.0f,1.0f,25.0f);
  transform_n_rasterize(_terrain, nmv, projection_matrix);

  
  if(tire_ball >0){
    _bx = _x;
    if(_bx > -31.0f){
      _bx +=anim_bx;
    }
  }
  
  gl4dpScreenHasChanged();
  gl4dpUpdateScreen(NULL);

  anim_bx -= 0.05f;
  a += 0.5f;
}

/*
gestion du clavier

z,q,s,d - deplacement
e - saut
p - quitter
b - lance la balle
haut,droite,gauche,bas - camera

*/
void key(int keycode) {
  switch(keycode) {
  case GL4DK_UP:
    _ycam += 1.05f;
    break;
  case GL4DK_DOWN:
    _ycam -= 1.05f;
    break;
  case GL4DK_LEFT:
    _lcam += 1.05f;    
    break;
  case GL4DK_RIGHT:
    _lcam -= 1.05f;
    break;
  case GL4DK_q:
    _x -= 1.05f;
    _bx -= 1.05f;
    if(_x < -36.0f){
      _x += 1.05f;
    }
    break;
  case GL4DK_d:
    _x += 1.05f;
    _bx += 1.05f;
    if(_x > 14.0f){
      _x -= 1.05f;
    }
    break;
  case GL4DK_z:
    _z -= 1.05f;
    _bz -= 1.05f;
    if(_z <-18.0f){
      _z += 1.05f;
    }
    break;
  case GL4DK_e:
    _y += 5.05f;
    //a implementer
    break;
  case GL4DK_f:
    _y -= 1.05f;
    //a implementer
    break;
  case GL4DK_s:
    _z += 1.05f;
    _bz += 1.05f;
    if(_z >18.0f){
      _z -= 1.05f;
    }
    break;
  case GL4DK_p:
    exit(0);
    break;
  case GL4DK_b:
    tire_ball = 1;
    break;
  default: break;
  }
}

//liberation des differentes surfaces
void sortie(void) {
  if(_terrain) {
    free_surface(_terrain);
    _terrain = NULL;
  }
  if(_panier) {
    free_surface(_panier);
    _panier = NULL;
  }if(_fresque) {
    free_surface(_fresque);
    _fresque = NULL;
  }
  if(_joeur) {
    free_surface(_joeur);
    _joeur = NULL;
  }if(_balle) {
    free_surface(_balle);
    _balle = NULL;
  }
  if(_cube_logo1) {
    free_surface(_cube_logo1);
    _cube_logo1 = NULL;
  }
  if(_cube_logo2) {
    free_surface(_cube_logo2);
    _cube_logo2 = NULL;
  }
  gl4duClean(GL4DU_ALL);
}