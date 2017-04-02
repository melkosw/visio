
#include "frame.h"
#include <cmath>
#include <iostream>

Matrix::Matrix(Coords r1, Coords r2, Coords r3):row1(r1),row2(r2),row3(r3) {}

void Matrix::setZero() {

    row1=Coords(0.0,0.0,0.0);
    row2=Coords(0.0,0.0,0.0);
    row3=Coords(0.0,0.0,0.0);

}

void Matrix::setIdentity() {

    row1=Coords(1.0,0.0,0.0);
    row2=Coords(0.0,1.0,0.0);
    row3=Coords(0.0,0.0,1.0);

}

Matrix Matrix::transpose() const {

    Coords col1,col2,col3;
    col1.xPos=row1.xPos;
    col1.yPos=row2.xPos;
    col1.zPos=row3.xPos;
    col2.xPos=row1.yPos;
    col2.yPos=row2.yPos;
    col2.zPos=row3.yPos;
    col3.xPos=row1.zPos;
    col3.yPos=row2.zPos;
    col3.zPos=row3.zPos;

    return Matrix(col1,col2,col3);

}

double Matrix::det() const {

    double f1(row1.xPos*row2.yPos*row3.zPos);
    double f2(row1.yPos*row2.zPos*row3.xPos);
    double f3(row1.zPos*row2.xPos*row3.yPos);
    double f4(row1.xPos*row2.zPos*row3.yPos);
    double f5(row1.yPos*row2.xPos*row3.zPos);
    double f6(row1.zPos*row2.yPos*row3.xPos);

    return f1+f2+f3-f4-f5-f6;

}

Coords::Coords(const Coords &c) {

    xPos=c.xPos;
    yPos=c.yPos;
    zPos=c.zPos;
    epsilon=c.epsilon;

}

Coords Coords::operator+(const Coords &c) const {

    return Coords(xPos+c.xPos,yPos+c.yPos,zPos+c.zPos);

}

Coords Coords::operator-(const Coords &c) const {

    return Coords(xPos-c.xPos,yPos-c.yPos,zPos-c.zPos);

}

Coords Coords::operator-() const {

    return Coords(-xPos,-yPos,-zPos);

}

double Coords::operator*(const Coords &c) const {

    return xPos*c.xPos+yPos*c.yPos+zPos*c.zPos;

}

Coords Coords::operator^(const Coords &c) const {

    return Coords(yPos*c.zPos-zPos*c.yPos,zPos*c.xPos-xPos*c.zPos, xPos*c.yPos-yPos*c.xPos);

}

bool Coords::operator==(const Coords &c) const {

    return (fabsl(xPos-c.xPos)<=epsilon)&&(fabsl(yPos-c.yPos)<=epsilon)&&(fabsl(zPos-c.zPos)<=epsilon);

}

bool Coords::operator!=(const Coords &c) const {

    return !(*this==c);

}

Coords Coords::unit() const {

    double n(norm());
    if (n>0)
        return Coords(xPos/n,yPos/n,zPos/n);
    else
        return Coords();

}

double Coords::norm() const {

    return sqrtl(sqnorm());

}

double Coords::sqnorm() const {

    return xPos*xPos+yPos*yPos+zPos*zPos;

}

bool Coords::isZero() const {

    return (fabsl(xPos)<epsilon)&&(fabsl(yPos)<epsilon)&&(fabsl(zPos)<epsilon);

}

Coords floorl(const Coords c) {
    return Coords(floorl(c.xPos+c.epsilon),floorl(c.yPos+c.epsilon),floorl(c.zPos+c.epsilon));
}

Matrix rotationMatrix(double angle, Coords c) {

    double cosTheta(cosl(angle));
    double sinTheta(sinl(angle));
    c=c.unit();
    Coords r1(cosTheta+c.xPos*c.xPos*(1.0-cosTheta), c.xPos*c.yPos*(1.0-cosTheta)-c.zPos*sinTheta, c.xPos*c.zPos*(1.0-cosTheta)+c.yPos*sinTheta);
    Coords r2(c.xPos*c.yPos*(1.0-cosTheta)+c.zPos*sinTheta, cosTheta+c.yPos*c.yPos*(1.0-cosTheta), c.yPos*c.zPos*(1.0-cosTheta)-c.xPos*sinTheta);
    Coords r3(c.xPos*c.zPos*(1.0-cosTheta)-c.yPos*sinTheta, c.yPos*c.zPos*(1.0-cosTheta)+c.xPos*sinTheta, cosTheta+c.zPos*c.zPos*(1.0-cosTheta));

    return Matrix(r1,r2,r3);

}

Coords Coords::rotate(double angle, const Coords &c, const Coords& center) const {

    return rotationMatrix(angle,c)*((*this)-center)+center;

}

Coords operator*(double k, const Coords &c) {

    return Coords(k*c.xPos,k*c.yPos,k*c.zPos);

}

Coords operator*(const Coords &c, double k) {

    return Coords(k*c.xPos,k*c.yPos,k*c.zPos);

}

Coords operator/(const Coords &c, double k) {

    return Coords(1.0l/k*c.xPos,1.0l/k*c.yPos,1.0l/k*c.zPos);

}

Coords operator*(Matrix m, const Coords & v) {

    return Coords(m.row1*v,m.row2*v,m.row3*v);

}


Frame::Frame(Coords o, Coords ax, Coords ay, Coords az):origin(o),xAxis(ax),yAxis(ay),zAxis(az) {


    unitize();

}

void Frame::unitize() {

    xAxis=xAxis.unit();

    yAxis=yAxis-(xAxis*yAxis)*xAxis;
    yAxis=yAxis.unit();

    Coords candidateZAxis(xAxis^yAxis);
//    if ((candidateZAxis*zAxis)>=0)
        zAxis=candidateZAxis;
//    else
//        zAxis=-candidateZAxis;

    if ((xAxis.isZero())||(yAxis.isZero())||(zAxis.isZero())) {
        xAxis=Coords(1.0,0.0,0.0);
        yAxis=Coords(0.0,1.0,0.0);
        zAxis=Coords(0.0,0.0,1.0);
    }

}

void Frame::translate(const Coords & c) {

    origin=origin+c;

}

void Frame::rotate(double angle, const Coords &c, const Coords & center) {   // angle in radiants

    Matrix rotMatrix=rotationMatrix(angle,c);

    //std::cout << "rotating" << std::endl << xAxis << std::endl  << yAxis << std::endl  << zAxis << std::endl ;
    //std::cout << "With :" << std::endl << rotMatrix << std::endl << std::endl;

    origin=(rotMatrix*(origin-center))+center;
    xAxis=rotMatrix*xAxis;
    yAxis=rotMatrix*yAxis;
    zAxis=rotMatrix*zAxis;

}


Coords Frame::orig() const {

    return origin;

}

Matrix Frame::base() const {

    return Matrix(xAxis,yAxis,zAxis);

}


std::ostream& operator<<(std::ostream& o, const Coords & c) {

    return o << '[' << c.xPos << ';' << c.yPos << ';' << c.zPos << ']';

}

std::ostream& operator<<(std::ostream& o, const Matrix & m) {

    return o << m.row1 << std::endl << m.row2 << std::endl << m.row3 << std::endl;

}

std::ostream& operator<<(std::ostream& o, const Frame & f) {

    return o << "O:  " << f.origin << std::endl << "e1: " << f.xAxis << std::endl << "e2: " << f.yAxis << std::endl << "e3: " << f.zAxis << std::endl;

}
