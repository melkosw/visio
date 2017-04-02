#ifndef FRAME_H
#define FRAME_H

#define PIGRECO 3.141592653589793238462643383279L

#include <iostream>


struct IntCoords {
    int x,y,z;
    IntCoords(int xx = 0, int yy = 0, int zz = 0):x(xx),y(yy),z(zz) {}
};

class Coords {
public:
    double xPos,yPos,zPos;
    double epsilon;
    Coords(double xx = 0.0, double yy = 0.0, double zz = 0.0, double ee = 1e-10):xPos(xx),yPos(yy),zPos(zz),epsilon(ee) {}
    Coords(const Coords &);

    Coords operator+(const Coords &) const;
    Coords operator-(const Coords &) const;
    Coords operator-() const;
    double operator*(const Coords &) const;
    Coords operator^(const Coords &) const;
    bool operator==(const Coords &) const;
    bool operator!=(const Coords &) const;
    Coords unit() const;
    double norm() const;
    double sqnorm() const;
    bool isZero() const;
    Coords rotate(double, const Coords &, const Coords & = Coords(0.5,0.5,0.5)) const;

};

Coords floorl(const Coords);

struct Matrix {

    Coords row1,row2,row3;

    Matrix(Coords = Coords(0.0,0.0,0.0),Coords = Coords(0.0,0.0,0.0),Coords = Coords(0.0,0.0,0.0));
    void setZero();
    void setIdentity();
    Matrix transpose() const;
    double det() const;

};

Matrix rotationMatrix(double angle, Coords c);

Coords operator*(Matrix, const Coords &);

Coords operator*(double, const Coords &);

Coords operator*(const Coords &, double);

Coords operator/(const Coords &, double);

class Frame
{

public:

    Coords origin, xAxis, yAxis, zAxis;

    Frame(Coords = Coords(0.0,0.0,0.0), Coords = Coords(1.0,0.0,0.0), Coords = Coords(0.0,1.0,0.0), Coords = Coords(0.0,0.0,1.0));  // origin and orthonormal basis

    void translate(const Coords &);
    void rotate(double, const Coords &, const Coords & =Coords(0.5,0.5,0.5));
    Coords orig() const;
    Matrix base() const;

    void unitize();

    friend std::ostream& operator<<(std::ostream&, const Frame &);

};

std::ostream& operator<<(std::ostream&, const Coords &);

std::ostream& operator<<(std::ostream&, const Matrix &);

std::ostream& operator<<(std::ostream&, const Frame &);

#endif // FRAME_H
