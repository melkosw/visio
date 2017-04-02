#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QString>
#include "frame.h"


struct Cube {
    Coords origin;
    double side;
    Cube(Coords o = Coords(0,0,0), double s = 1.0l):origin(o), side(s) {}
};

bool intersectCube(Coords, Coords, Cube, double, Cube &, Coords&, double&, bool=true );

class Camera
{

public:

    Frame position;
    double angle;
    double distance;

    Camera(Frame = Frame(),double = PIGRECO/4 ,double = 10.0);
    Camera(QString);
    QString toStr();
    void translate(const Coords &);
    void rotate(double, const Coords &, const Coords & =Coords(0.5,0.5,0.5));

};

class CameraWidget: public QWidget {

    Q_OBJECT

public:

    QDoubleSpinBox *originX;
    QDoubleSpinBox *originY;
    QDoubleSpinBox *originZ;
    QDoubleSpinBox *aheadX;
    QDoubleSpinBox *aheadY;
    QDoubleSpinBox *aheadZ;
    QDoubleSpinBox *rightX;
    QDoubleSpinBox *rightY;
    QDoubleSpinBox *rightZ;
    QDoubleSpinBox *topX;
    QDoubleSpinBox *topY;
    QDoubleSpinBox *topZ;
    QDoubleSpinBox *zoomSB;

    QSlider *zoomSL;

    QPushButton *acceptCamera;
    QPushButton *defaultCamera;

    QGridLayout *mainLayout;

    QLabel* cameraLabel;
    QLabel* aheadLabel;
    QLabel* rightLabel;
    QLabel* topLabel;
    QLabel* originLabel;
    QLabel* zoomLabel;

    CameraWidget(Camera c, QWidget *parent = 0);
    ~CameraWidget();

    void setCamera(Camera c);

private:

    Coords origin;
    Coords ahead;
    Coords right;
    Coords top;
    double zoom;
    void updateFields();
    void suspendSignals();
    void resumeSignals();

protected:

public slots:

    void fieldsChanged();
    void setDefaultCamera();
    void zoomSBChanged();
    void zoomSLChanged();
    void accept();

signals:

    void cameraChanged(Camera);

};

#endif // CAMERA_H
