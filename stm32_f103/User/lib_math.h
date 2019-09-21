#ifndef __LIB_MATH_H_
#define __LIB_MATH_H_

#include "lib_common.h"
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352
#include  "math.h"

#define A2R(A)  ((A)*PI/180)
#define R2A(A)  ((A)*180/PI)

#define ABS(x) ((x)>0?(x):-(x))
#define LIMIT(x, a, b) ((x)>(b)?(b) : ((x)<(a)?(a) : (x)))
#define IS_INSCALE(x, a, b) ((x)>(b)?0 : ((x)<(a)?0 : 1))
#define SQUARE(x) (x)*(x)
#define DISTANCE(x0, y0, x1, y1)  (sqrt((f32)((x1)-(x0))*((x1)-(x0))+(f32)((y1)-(y0))*((y1)-(y0))))
#define DISTANCE3(x0, y0, z0, x1, y1, z1)\
(sqrt(SQUARE((x1)-(x0))+SQUARE((y1)-(y0))+SQUARE((z1)-(z0))))
extern inline double limit(double x, double a, double b){ return x>b?b : x<a?a : x; }
extern inline double square(double x){ return x*x; }
extern inline double distance(double x0, double y0, double x1, double y1){ return sqrt(square(x1 - x0) + square(y1 - y0)); }
extern inline double distance3(double x0, double y0, double x1, double y1, double z0, double z1)
{ return sqrt(square(x1 - x0) + square(y1 - y0) + square(z1 - z0)); }
extern inline double min(double x, double y){ if(x<y)return x; else return y;}
typedef struct{
	double phi;
	double theta;
	double r;
}sph_vect;
void sph_invense(sph_vect* ret, sph_vect* v);
void sph_vadd(sph_vect* ret, sph_vect* v1, sph_vect* v2);
void sph_vsub(sph_vect* ret, sph_vect* v1, sph_vect* v2);
typedef struct{
	double theta;
	double rho;
}polor_vect;
void polor_invense(polor_vect*ret, polor_vect* v);
void polor_vadd(polor_vect* ret, polor_vect* v1, polor_vect* v2);
void polor_vsub(polor_vect* ret, polor_vect* v1, polor_vect* v2);

typedef struct{
	double x;
	double y;
	double z;
}card3_vect;
void card3_invense(card3_vect* ret, card3_vect* v);
void card3_vadd(card3_vect* ret, card3_vect* v1, card3_vect* v2);
void card3_vsub(card3_vect* ret, card3_vect* v1, card3_vect* v2);
double card3_distance(card3_vect* v1, card3_vect* v2);
void card3_rot_csys(card3_vect* ret, card3_vect* v, double roll, double pitch, double yaw, const char* order);
void card3_rot_vect(card3_vect* ret, card3_vect* v, double roll, double pitch, double yaw, const char* order);

void CardtSph(card3_vect*vc, sph_vect*vs);
void SphtCard(sph_vect*vs, card3_vect*vc);

double CosLaw_SolSide(double a, double b, double C);
double CosLaw_SolAng(double a, double b, double c);
double CosLaw_SolSecAng(double a, double b, double C);
void TriVect_SolAng(polor_vect* v1, polor_vect* v2, polor_vect* v3, int8 side);
#endif

