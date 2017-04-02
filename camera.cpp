#include "camera.h"
#include <cmath>
#include <QTextStream>

Camera::Camera(Frame f, double a, double d):position(f),angle(a),distance(d) {}

Camera::Camera(QString input) {

    QTextStream store(&input);
    QString x,y,z,a,d;

    store >> x >> y >> z;
    position.origin.xPos=x.toDouble();
    position.origin.yPos=y.toDouble();
    position.origin.zPos=z.toDouble();
    store >> x >> y >> z;
    position.xAxis.xPos=x.toDouble();
    position.xAxis.yPos=y.toDouble();
    position.xAxis.zPos=z.toDouble();
    store >> x >> y >> z;
    position.yAxis.xPos=x.toDouble();
    position.yAxis.yPos=y.toDouble();
    position.yAxis.zPos=z.toDouble();
    store >> x >> y >> z;
    position.zAxis.xPos=x.toDouble();
    position.zAxis.yPos=y.toDouble();
    position.zAxis.zPos=z.toDouble();
    store >> a >> d;
    angle=a.toDouble();
    distance=d.toDouble();

}

QString Camera::toStr() {

    QString output;
    QTextStream store(&output);

    store << position.origin.xPos << ' ' << position.origin.yPos << ' ' << position.origin.zPos << ' ';
    store << position.xAxis.xPos << ' ' << position.xAxis.yPos << ' ' << position.xAxis.zPos << ' ';
    store << position.yAxis.xPos << ' ' << position.yAxis.yPos << ' ' << position.yAxis.zPos << ' ';
    store << position.zAxis.xPos << ' ' << position.zAxis.yPos << ' ' << position.zAxis.zPos << ' ';
    store << angle << ' ' << distance << '\n';

    return output;

}

void Camera::translate(const Coords &v) {
    position.translate(v);
}

void Camera::rotate(double a, const Coords &v, const Coords & c) {
    position.rotate(a,v,c);
}


bool intersectCube(Coords sp, Coords d, Cube c, double ns, Cube &nc, Coords &new_in, double & length, bool stopAtEdge) {

    if (stopAtEdge) {
        if ((c.origin.xPos<0.0)||(c.origin.xPos>=1.0)||
                (c.origin.yPos<0.0)||(c.origin.yPos>=1.0)||
                (c.origin.zPos<0.0)||(c.origin.zPos>=1.0))
            return false;
    }

    length=0;

    Coords in=new_in;
    Coords diff(c.origin-sp);
    double time;
    if (d.xPos<=0) diff.xPos=diff.xPos+c.side;
    if (d.yPos<=0) diff.yPos=diff.yPos+c.side;
    if (d.zPos<=0) diff.zPos=diff.zPos+c.side;

    bool check( (sp.xPos>=0) && (sp.xPos<1.0) && (sp.yPos>=0) && (sp.yPos<1.0) && (sp.zPos>=0) && (sp.zPos<1.0) );

    if (check) {

        //in=(ns*5.0)*d+sp;
        in=sp;
        if ((in.xPos>=c.origin.xPos)&&(in.xPos<(c.origin.xPos+c.side))&&(in.yPos>=c.origin.yPos)&&(in.yPos<(c.origin.yPos+c.side))&&(in.zPos>=c.origin.zPos)&&(in.zPos<(c.origin.zPos+c.side))) {
            //std::cout << "X in the bounds!" << std::endl;
            new_in=in+in.epsilon*d;
            nc.origin=floorl(new_in/ns)*ns;
            //std::cout << "Starting point:" << sp << "\nCube to intersect :" << c.origin << "\nTime :" << time << "\nDirection :" << d << "\nIntersection :" << in << "\nNew cube" <<  ns << "   " << nc.origin << std::endl;
            nc.side=ns;
            //std::cout << in << std::endl;
            return true;
        }

    }

    if (d.xPos!=0) {
        time=diff.xPos/d.xPos;
        if (time>0) {
            in=time*d+sp;
            //std::cout << "Intersection with X="<< diff.xPos+sp.xPos << " ->" << in << std::endl;
            if ((in.yPos>=c.origin.yPos)&&(in.yPos<=(c.origin.yPos+c.side))&&(in.zPos>=c.origin.zPos)&&(in.zPos<=(c.origin.zPos+c.side))) {
                //std::cout << "X in the bounds!" << std::endl;
                length=time;
                new_in=in+in.epsilon*d;
                nc.origin=floorl(new_in/ns)*ns;
                //std::cout << "Starting point:" << sp << "\nCube to intersect :" << c.origin << "\nTime :" << time << "\nDirection :" << d << "\nIntersection :" << in << "\nNew cube" <<  ns << "   " << nc.origin << std::endl;
                nc.side=ns;
                //std::cout << in << std::endl;
                return true;
            }
        }
    }
    if (d.yPos!=0) {
        time=diff.yPos/d.yPos;
        if (time>0) {
            in=time*d+sp;
            //std::cout << "Intersection with Y="<< diff.yPos+sp.yPos << " ->" << in << std::endl;
            if ((in.xPos>c.origin.xPos)&&(in.xPos<(c.origin.xPos+c.side))&&(in.zPos>=c.origin.zPos)&&(in.zPos<=(c.origin.zPos+c.side))) {
                //std::cout << "Y in the bounds!" << std::endl;
                length=time;
                new_in=in+in.epsilon*d;
                nc.origin=floorl(new_in/ns)*ns;
                //std::cout << "Starting point:" << sp << "\nCube to intersect :" << c.origin << "\nTime :" << time << "\nDirection :" << d << "\nIntersection :" << in << "\nNew cube" <<  ns << "   " << nc.origin << std::endl;
                nc.side=ns;
                //std::cout << in << std::endl;
                return true;
            }
        }
    }
    if (d.zPos!=0) {
        time=diff.zPos/d.zPos;
        if (time>0) {
            in=time*d+sp;
            //std::cout << "Intersection with Z="<< diff.zPos+sp.zPos << " ->" << in << std::endl;
            if ((in.yPos>c.origin.yPos)&&(in.yPos<(c.origin.yPos+c.side))&&(in.xPos>c.origin.xPos)&&(in.xPos<(c.origin.xPos+c.side))) {
                //std::cout << "Z in the bounds!" << std::endl;
                length=time;
                new_in=in+in.epsilon*d;
                nc.origin=floorl(new_in/ns)*ns;
                //std::cout << "Starting point:" << sp << "\nCube to intersect :" << c.origin << "\nTime :" << time << "\nDirection :" << d << "\nIntersection :" << in << "\nNew cube" <<  ns << "   " << nc.origin << std::endl;
                nc.side=ns;
                //std::cout << in << std::endl;
                return true;
            }
        }
    }

    return false;
}

CameraWidget::~CameraWidget() {

    delete originX;
    delete originY;
    delete originZ;
    delete aheadX;
    delete aheadY;
    delete aheadZ;
    delete rightX;
    delete rightY;
    delete rightZ;
    delete topX;
    delete topY;
    delete topZ;
    delete cameraLabel;
    delete originLabel;
    delete aheadLabel;
    delete rightLabel;
    delete topLabel;
    delete zoomSB;
    delete zoomSL;
    delete acceptCamera;
    delete mainLayout;

}

CameraWidget::CameraWidget(Camera c, QWidget *parent):QWidget(parent) {

    ahead=c.position.zAxis;
    right=c.position.xAxis;
    top=c.position.yAxis;
    origin=c.position.origin;
    zoom=0.8/c.angle;

    mainLayout = new QGridLayout();

    aheadX = new QDoubleSpinBox();
    aheadY = new QDoubleSpinBox();
    aheadZ = new QDoubleSpinBox();
    rightX = new QDoubleSpinBox();
    rightY = new QDoubleSpinBox();
    rightZ = new QDoubleSpinBox();
    topX = new QDoubleSpinBox();
    topY = new QDoubleSpinBox();
    topZ = new QDoubleSpinBox();
    originX = new QDoubleSpinBox();
    originY = new QDoubleSpinBox();
    originZ = new QDoubleSpinBox();
    aheadX->setKeyboardTracking(false);
    aheadY->setKeyboardTracking(false);
    aheadZ->setKeyboardTracking(false);
    rightX->setKeyboardTracking(false);
    rightY->setKeyboardTracking(false);
    rightZ->setKeyboardTracking(false);
    topX->setKeyboardTracking(false);
    topY->setKeyboardTracking(false);
    topZ->setKeyboardTracking(false);
    originX->setKeyboardTracking(false);
    originY->setKeyboardTracking(false);
    originZ->setKeyboardTracking(false);
    aheadX->setDecimals(6);
    aheadY->setDecimals(6);
    aheadZ->setDecimals(6);
    rightX->setDecimals(6);
    rightY->setDecimals(6);
    rightZ->setDecimals(6);
    topX->setDecimals(6);
    topY->setDecimals(6);
    topZ->setDecimals(6);
    originX->setDecimals(6);
    originY->setDecimals(6);
    originZ->setDecimals(6);

    aheadX->setSingleStep(0.01);
    aheadY->setSingleStep(0.01);
    aheadZ->setSingleStep(0.01);
    rightX->setSingleStep(0.01);
    rightY->setSingleStep(0.01);
    rightZ->setSingleStep(0.01);
    topX->setSingleStep(0.01);
    topY->setSingleStep(0.01);
    topZ->setSingleStep(0.01);
    originX->setSingleStep(0.001);
    originY->setSingleStep(0.001);
    originZ->setSingleStep(0.001);

    aheadX->setMinimum(-1000.0);
    aheadY->setMinimum(-1000.0);
    aheadZ->setMinimum(-1000.0);
    rightX->setMinimum(-1000.0);
    rightY->setMinimum(-1000.0);
    rightZ->setMinimum(-1000.0);
    topX->setMinimum(-1000.0);
    topY->setMinimum(-1000.0);
    topZ->setMinimum(-1000.0);
    originX->setMinimum(-1000.0);
    originY->setMinimum(-1000.0);
    originZ->setMinimum(-1000.0);

    aheadX->setMaximum(1000.0);
    aheadY->setMaximum(1000.0);
    aheadZ->setMaximum(1000.0);
    rightX->setMaximum(1000.0);
    rightY->setMaximum(1000.0);
    rightZ->setMaximum(1000.0);
    topX->setMaximum(1000.0);
    topY->setMaximum(1000.0);
    topZ->setMaximum(1000.0);
    originX->setMaximum(1000.0);
    originY->setMaximum(1000.0);
    originZ->setMaximum(1000.0);

    topX->setEnabled(false);
    topY->setEnabled(false);
    topZ->setEnabled(false);

    zoomSB = new QDoubleSpinBox();
    zoomSB->setKeyboardTracking(false);
    zoomSB->setSingleStep(0.1);
    zoomSL = new QSlider();
    zoomSL->setMouseTracking(false);
    zoomSL->setOrientation(Qt::Horizontal);
    zoomSL->setMaximum(75);
    zoomSL->setMinimum(0);
    zoomSB->setMaximum(8.0);
    zoomSB->setMinimum(0.5);
    cameraLabel = new QLabel("Camera");
    aheadLabel = new QLabel("Ahead (X,Y,Z)");
    rightLabel = new QLabel("Right (X,Y,Z)");
    topLabel = new QLabel("Top (X,Y,Z)");
    originLabel = new QLabel("Origin (X,Y,Z)");
    zoomLabel = new QLabel("Zoom");
    acceptCamera = new QPushButton("Set Camera");
    defaultCamera = new QPushButton("Default");

    mainLayout->setColumnMinimumWidth(0,100);
    mainLayout->setColumnMinimumWidth(1,30);
    mainLayout->setColumnMinimumWidth(2,30);
    mainLayout->setColumnMinimumWidth(3,30);

    mainLayout->setRowMinimumHeight(0,30);
    mainLayout->setRowMinimumHeight(1,30);
    mainLayout->setRowMinimumHeight(2,30);
    mainLayout->setRowMinimumHeight(3,30);
    mainLayout->setRowMinimumHeight(4,30);
    mainLayout->setRowMinimumHeight(5,30);
    mainLayout->setRowMinimumHeight(6,30);

    mainLayout->addWidget(cameraLabel,0,0,1,4,Qt::AlignCenter);

    mainLayout->addWidget(originLabel,1,0,1,1,Qt::AlignCenter);
    mainLayout->addWidget(originX,1,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(originY,1,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(originZ,1,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(aheadLabel,2,0,1,1,Qt::AlignCenter);
    mainLayout->addWidget(aheadX,2,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(aheadY,2,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(aheadZ,2,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(rightLabel,3,0,1,1,Qt::AlignCenter);
    mainLayout->addWidget(rightX,3,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(rightY,3,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(rightZ,3,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(topLabel,4,0,1,1,Qt::AlignCenter);
    mainLayout->addWidget(topX,4,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(topY,4,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(topZ,4,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(zoomLabel,5,0,1,1,Qt::AlignCenter);
    mainLayout->addWidget(zoomSL,5,1,1,2);
    mainLayout->addWidget(zoomSB,5,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(defaultCamera,6,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(acceptCamera,6,3,1,1,Qt::AlignCenter);

    updateFields();

    setLayout(mainLayout);

    connect(acceptCamera, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(defaultCamera, SIGNAL(clicked(bool)), this, SLOT(setDefaultCamera()));

    connect(aheadX, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));
    connect(aheadY, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));
    connect(aheadZ, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));

    connect(rightX, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));
    connect(rightY, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));
    connect(rightZ, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));

    connect(topX, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));
    connect(topY, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));
    connect(topZ, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));

    connect(originX, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));
    connect(originY, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));
    connect(originZ, SIGNAL(valueChanged(double)), this, SLOT(fieldsChanged()));

    connect(zoomSB, SIGNAL(valueChanged(double)), this, SLOT(zoomSBChanged()));

    connect(zoomSL, SIGNAL(valueChanged(int)), this, SLOT(zoomSLChanged()));

}

void CameraWidget::setDefaultCamera() {

    origin=Coords(-0.65,-0.93,-1.34);
    ahead=Coords(0.44,0.55,0.71);
    right=Coords(-0.8,-0.12,0.59);
    top=Coords(-0.41,0.83,-0.39);
    zoomSB->setValue(1.0);
    suspendSignals();
    zoomSBChanged();
    updateFields();
    resumeSignals();
    accept();

}

void CameraWidget::fieldsChanged() {

    origin.xPos=originX->value();
    origin.yPos=originY->value();
    origin.zPos=originZ->value();
    top.xPos=topX->value();
    top.yPos=topY->value();
    top.zPos=topZ->value();
    right.xPos=rightX->value();
    right.yPos=rightY->value();
    right.zPos=rightZ->value();
    ahead.xPos=aheadX->value();
    ahead.yPos=aheadY->value();
    ahead.zPos=aheadZ->value();

    //accept();

    //updateFields();

}

void CameraWidget::zoomSBChanged() {

    zoom=zoomSB->value();
    zoomSL->setValue(zoom*10-5);

}

void CameraWidget::zoomSLChanged() {

    zoom=(zoomSL->value()+5.0)/10.0;
    zoomSB->setValue(zoom);

}

void CameraWidget::updateFields() {

    originX->setValue(origin.xPos);
    originY->setValue(origin.yPos);
    originZ->setValue(origin.zPos);
    aheadX->setValue(ahead.xPos);
    aheadY->setValue(ahead.yPos);
    aheadZ->setValue(ahead.zPos);
    rightX->setValue(right.xPos);
    rightY->setValue(right.yPos);
    rightZ->setValue(right.zPos);
    topX->setValue(top.xPos);
    topY->setValue(top.yPos);
    topZ->setValue(top.zPos);
    zoomSL->setValue((zoom*10-5));
    zoomSB->setValue(zoom);

}

void CameraWidget::accept() {

    fieldsChanged();

    ahead=ahead.unit();

    right=right-(ahead*right)*ahead;
    right=right.unit();

    top = -ahead^right;

    if ((right.isZero())||(top.isZero())||(ahead.isZero())) {
        ahead=Coords(1.0,0.0,0.0);
        top=Coords(0.0,1.0,0.0);
        right=Coords(0.0,0.0,1.0);
    }

    suspendSignals();
    updateFields();
    resumeSignals();

    Camera c(Frame(origin,ahead,top,right),0.8/zoom,10);
    emit cameraChanged(c);

}


void CameraWidget::setCamera(Camera c) {

    suspendSignals();
    origin=c.position.origin;
    ahead=c.position.xAxis;
    top=c.position.yAxis;
    right=c.position.zAxis;
    zoom=0.8/(c.angle);
    updateFields();
    resumeSignals();

}

void CameraWidget::suspendSignals() {

    aheadX->blockSignals(true);
    aheadY->blockSignals(true);
    aheadZ->blockSignals(true);
    rightX->blockSignals(true);
    rightY->blockSignals(true);
    rightZ->blockSignals(true);
    topX->blockSignals(true);
    topY->blockSignals(true);
    topZ->blockSignals(true);
    originX->blockSignals(true);
    originY->blockSignals(true);
    originZ->blockSignals(true);
    zoomSL->blockSignals(true);

}

void CameraWidget::resumeSignals() {

    aheadX->blockSignals(false);
    aheadY->blockSignals(false);
    aheadZ->blockSignals(false);
    rightX->blockSignals(false);
    rightY->blockSignals(false);
    rightZ->blockSignals(false);
    topX->blockSignals(false);
    topY->blockSignals(false);
    topZ->blockSignals(false);
    originX->blockSignals(false);
    originY->blockSignals(false);
    originZ->blockSignals(false);
    zoomSL->blockSignals(false);

}
