#include "lib_math.h"
#include "math.h"

// Solve ret = -v
void sph_invense(sph_vect* ret, sph_vect* v){
	card3_vect vc;
	SphtCard(v, &vc);
	vc.x = -vc.x;
	vc.y = -vc.y;
	vc.z = -vc.z;
	CardtSph(&vc, ret);
}
// add two vectors in sph coodinate.
void sph_vadd(sph_vect* ret, sph_vect* v1, sph_vect* v2) {
	double x, y, z;
	double cost1 = cos(v1->theta), cost2 = cos(v2->theta);
	x = v1->r * cost1 * cos(v1->phi) + v2->r * cost2 * cos(v2->phi);
	y = v1->r * cost1 * sin(v1->phi) + v2->r * cost2 * sin(v2->phi);
	z = v1->r * sin(v1->theta) + v2->r * sin(v2->theta);

	double sx = x * x, sy = y * y, sz = z * z;
	ret->phi = atan2(y, x);
	ret->theta = atan2(z, sqrt(sx + sy));
	ret->r = sqrt(sx + sy + sz);
}
// solve ret = v1 - v2
void sph_vsub(sph_vect* ret, sph_vect* v1, sph_vect* v2){
	ret->theta = v1->theta - v2->theta;
	ret->phi = v1->phi - v2->phi;
	card3_vect cv1, cv2;
	SphtCard(v1, &cv1);
	SphtCard(v2, &cv2);
	ret->r = card3_distance(&cv1, &cv2);
	/* implemence way2:
	sph_vect vtmp;
	sph_invense(&vtmp, v2);
	sph_vadd(ret, v1, &vtmp);*/
}
void polor_invense(polor_vect*ret, polor_vect* v){
	polor_vect tmp = *v;
	tmp.theta += PI;
	if(tmp.theta > PI) tmp.theta -= 2*PI;
	ret->rho = tmp.rho;
	ret->theta = tmp.theta;
}
//ret = v1-v2
void polor_vsub(polor_vect* ret, polor_vect* v1, polor_vect* v2){
	polor_vect tmp;
	polor_invense(&tmp, v2);
	polor_vadd(ret, v1, &tmp);
}
// add two vectors in polor coodinate.
void polor_vadd(polor_vect* ret, polor_vect* v1, polor_vect* v2){
	double x = v1->rho * cos(v1->theta) + v2->rho * cos(v2->theta);
	double y = v1->rho * sin(v1->theta) + v2->rho * sin(v2->theta);
	ret->rho = sqrt(x*x + y*y);
	ret->theta = atan2(y, x);
}
void card3_invense(card3_vect* ret, card3_vect* v){
	ret->x = -v->x;
	ret->y = -v->y;
	ret->z = -v->z;
}
void card3_vadd(card3_vect* ret, card3_vect* v1, card3_vect* v2){
	ret->x = v1->x + v2->x;
	ret->y = v1->y + v2->y;
	ret->z = v1->z + v2->z;
}
// Solve ret = v1-v2
void card3_vsub(card3_vect* ret, card3_vect* v1, card3_vect* v2){
	ret->x = v1->x - v2->x;
	ret->y = v1->y - v2->y;
	ret->z = v1->z - v2->z;
}
double card3_distance(card3_vect* v1, card3_vect* v2){ 
	return sqrt(square(v1->x - v2->x) + square(v1->y - v2->y) + square(v1->z - v2->z)) ;
}
/* !!! The function heae is replaced by the later one.
// rotation convert the csys(right hand vector system) of v to anothor csys.
// first rotate yaw, tnen pitch, last roll.
// ret is the result after transform of v.
// please note that roll, pitch and yaw is the rotation of csys.
void card3_rot_csys(card3_vect* ret, card3_vect* v, double roll, double pitch, double yaw) {
	double cosa = cos(roll), cosb = cos(pitch), cosc = cos(yaw);
	double sina = sin(roll), sinb = sin(pitch), sinc = sin(yaw);
	double sinasinb = sina * sinb, cosasinb = cosa * sinb;
	ret->x = v->x * cosb * cosc + v->y * cosb * sinc - v->z * sinb;
	ret->y = v->x * (-cosa * sinc + sinasinb * cosc) + v->y * (cosa * cosc + sinasinb * sinc) + v->z * sina * cosb;
	ret->z = v->x * (sina * sinc + cosasinb * cosc) + v->y * (-sina * cosc + cosasinb * sinc) + v->z * cosa * cosb;
}
// rotation convert the vector v to anothor vector.
// ret is the result after transform of v.
// please note that roll, pitch and yaw is the rotation of vector in the same csys.
void card3_rot_vect(card3_vect* ret, card3_vect* v, double roll, double pitch, double yaw) {
	double cosa = cos(roll), cosb = cos(pitch), cosc = cos(yaw);
	double sina = -sin(roll), sinb = -sin(pitch), sinc = -sin(yaw);
	double sinasinb = sina * sinb, cosasinb = cosa * sinb;
	ret->x = v->x * cosb * cosc + v->y * cosb * sinc - v->z * sinb;
	ret->y = v->x * (-cosa * sinc + sinasinb * cosc) + v->y * (cosa * cosc + sinasinb * sinc) + v->z * sina * cosb;
	ret->z = v->x * (sina * sinc + cosasinb * cosc) + v->y * (-sina * cosc + cosasinb * sinc) + v->z * cosa * cosb;
}*/
// implemence of csys rotation with recursion
// for example, order = "xy" or "XY" mean that 
// rotate vector v firstly x axis by roll then rotate y by pitch.
void card3_rot_csys(card3_vect* ret, card3_vect* v, double roll, double pitch, double yaw, const char* order) {
	card3_vect tmp;
	char sel = (order[0] | 32);
	if (sel == 'x') {
		double cosa = cos(roll), sina = sin(roll);
		tmp.x = v->x;
		tmp.y = cosa * v->y + sina * v->z;
		tmp.z = -sina * v->y + cosa * v->z;
	}
	else if (sel == 'y') {
		double cosb = cos(pitch), sinb = sin(pitch);
		tmp.x = cosb * v->x - sinb * v->z;
		tmp.y = v->y;
		tmp.z = sinb * v->x + cosb * v->z;
	}
	else if (sel == 'z') {
		double cosc = cos(yaw), sinc = sin(yaw);
		tmp.x = cosc * v->x + sinc * v->y;
		tmp.y = -sinc * v->x + cosc * v->y;
		tmp.z = v->z;
	}
	else return;
	ret->x = tmp.x;
	ret->y = tmp.y;
	ret->z = tmp.z;
	card3_rot_csys(ret, &tmp, roll, pitch, yaw, order + 1);
}
// implemence of csys rotation with recursion
// for example, order = "xy" or "XY" mean that 
// rotate vector v firstly x axis by roll then rotate y by pitch.
void card3_rot_vect(card3_vect* ret, card3_vect* v, double roll, double pitch, double yaw, const char* order) {
	card3_vect tmp;
	char sel = (order[0] | 32);
	if (sel == 'x') {
		double cosa = cos(roll), sina = -sin(roll);
		tmp.x = v->x;
		tmp.y = cosa * v->y + sina * v->z;
		tmp.z = -sina * v->y + cosa * v->z;
	}
	else if (sel == 'y') {
		double cosb = cos(pitch), sinb = -sin(pitch);
		tmp.x = cosb * v->x - sinb * v->z;
		tmp.y = v->y;
		tmp.z = sinb * v->x + cosb * v->z;
	}
	else if (sel == 'z') {
		double cosc = cos(yaw), sinc = -sin(yaw);
		tmp.x = cosc * v->x + sinc * v->y;
		tmp.y = -sinc * v->x + cosc * v->y;
		tmp.z = v->z;
	}
	else return;
	ret->x = tmp.x;
	ret->y = tmp.y;
	ret->z = tmp.z;
	card3_rot_csys(ret, &tmp, roll, pitch, yaw, order + 1);
}
// return the angle C in rad system.
double CosLaw_SolAng(double a, double b, double c){
	return acos( (SQUARE(a) + SQUARE(b) - SQUARE(c))/(2*a*b) );
}
// return the third side, c length. 
// Note that in rad system.
double CosLaw_SolSide(double a, double b, double C){
	return sqrt( SQUARE(a) + SQUARE(b) - 2*a*b*cos(C));
}
// return the second angle, B in rad 
// when two sides and included angle are inputed.
double CosLaw_SolSecAng(double a, double b, double C){
	double c = CosLaw_SolSide(a, b, C);
	return CosLaw_SolAng(a, c, b);
}
// Solve three vects in polor coordinates when length of all vect and theta of v3 is known.
// parameter side = 1 present that v1 and v2 is on the left side of v3 while -1 on the right.
// please note that v1 + v2 = v3 in the result.
void TriVect_SolAng(polor_vect* v1, polor_vect* v2, polor_vect* v3, int8 side){
	double A2 = CosLaw_SolAng(v1->rho, v3->rho, v2->rho);
	v1->theta = v3->theta + A2*side;
	double A3 = CosLaw_SolAng(v1->rho, v2->rho, v3->rho);
	v2->theta = v1->theta - side*(PI-A3);
}
//card vect3 transform to sph vect
void CardtSph(card3_vect*vc, sph_vect*vs){
	double sx = vc->x * vc->x, sy = vc->y * vc->y, sz = vc->z * vc->z;
	vs->phi = atan2(vc->y, vc->x);
	vs->theta = atan2(vc->z, sqrt(sx + sy));
	vs->r = sqrt(sx + sy + sz);
}
// sph vect transform tocard vect3
void SphtCard(sph_vect*vs, card3_vect*vc){
	double cost1 = cos(vs->theta);
	vc->x = vs->r * cost1 * cos(vs->phi);
	vc->y = vs->r * cost1 * sin(vs->phi);
	vc->z = vs->r * sin(vs->theta);
}
