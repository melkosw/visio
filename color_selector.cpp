
#include "color_selector.h"
#include "ui_color_selector.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <list>
#include <map>
#include <QGridLayout>
#include <QPainter>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>


void ColorSelector::plotTranspar() {

    int x=transparRA->width();
    int y=transparRA->height();
    int n(colorScaleList.size());

    QPainter p;
    p.begin(transparRA->sheet);
    QPen pen(QColor(255,0,0));
    pen.setWidthF(2.5);
    p.setPen(pen);
    double pos_i(colorScaleList[0]),pos_j;
    QColor color_i(colorValueList[0]),color_j;
    double start,start_a;
    double end,end_a;

    for (int i=1; i<n; i++) {

        pos_j=colorScaleList[i];
        color_j=colorValueList[i];
        start=pos_i;
        end=pos_j;
        start_a=color_i.alpha()/256.0;
        end_a=color_j.alpha()/256.0;
        p.drawLine(start*x,start_a*y,end*x,end_a*y);
        pos_i=pos_j;
        color_i=color_j;

    }

}

void ColorSelector::plotCurrentColorScale() {

    int x=colorStripRA->width();
    int y=colorStripRA->height();
    int n(colorScaleList.size());
    double pos_i(colorScaleList[0]),pos_j;
    QColor color_i(colorValueList[0]),color_j,background;
    double start,start_r,start_g,start_b,start_a;
    double end,end_r,end_g,end_b,end_a;
    double curr_r,curr_g,curr_b,curr_a,r,g,b;
    for (int i=1; i<n; i++) {

        pos_j=colorScaleList[i];
        color_j=colorValueList[i];

        start=pos_i*x;
        end=pos_j*x;
        start_r=color_i.red();
        start_g=color_i.green();
        start_b=color_i.blue();
        start_a=color_i.alpha();
        end_r=color_j.red();
        end_g=color_j.green();
        end_b=color_j.blue();
        end_a=color_j.alpha();

        for (int j=start; j<end; j++) {

            curr_r=start_r+(end_r-start_r)*(j-start)/(end-start);
            curr_g=start_g+(end_g-start_g)*(j-start)/(end-start);
            curr_b=start_b+(end_b-start_b)*(j-start)/(end-start);
            curr_a=start_a+(end_a-start_a)*(j-start)/(end-start);

            for (int k=10; k<y/2; k++) {

                background=colorStripRA->sheet->pixel(j,k);
                r=background.red()*(1.0-curr_a/255)+curr_r*(curr_a/255);
                g=background.green()*(1.0-curr_a/255)+curr_g*(curr_a/255);
                b=background.blue()*(1.0-curr_a/255)+curr_b*(curr_a/255);
                colorStripRA->sheet->setPixel(j,k,qRgb(r,g,b));

            }

            for (int k=y/2; k<y-10; k++) {
                colorStripRA->sheet->setPixel(j,k,qRgb(curr_r,curr_g,curr_b));

            }

        }

        pos_i=pos_j;
        color_i=color_j;

    }

    for (int i=0; i<n; i++) {

        pos_i=colorScaleList[i]*x;
        curr_r=0;
        curr_g=255;
        curr_b=0;
        if ((i==0)||(i==n-1)) {
            curr_r=0;
            curr_g=128;
            curr_b=128;
        }
        if (i==currentIndex) {
            curr_r=255;
            curr_g=0;
            curr_b=0;
        }

        for (int j=0;j<10;j++) {

            for (int k=-j+1;k<j;k++) {

                if ((pos_i+k<0)||(pos_i+k>x-1)) continue;
                colorStripRA->sheet->setPixel(pos_i+k,y-10+j,qRgb(curr_r,curr_g,curr_r));

            }

            if (i==currentIndex) {
                for (int k=-j+1;k<j;k++) {

                    if ((pos_i+k<0)||(pos_i+k>x-1)) continue;
                    colorStripRA->sheet->setPixel(pos_i+k,10-j,qRgb(curr_r,curr_g,curr_r));

                }
                for (int k=0; k<y; k++) colorStripRA->sheet->setPixel(pos_i,k,qRgb(curr_r,curr_g,curr_r));
            }

        }

    }

}


void ColorSelector::plotCurrentColorCircle() {

    int x=colorCircleRA->width();
    int y=colorCircleRA->height();
    double dim(x<y?x/2.0:y/2.0),x_rel,y_rel;

    x_rel=cos(currentColorAngle/180.0*3.1415296)*currentColorRadius;
    y_rel=sin(currentColorAngle/180.0*3.1415296)*currentColorRadius;
    int sq_x,sq_y,size(15);

    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {

            if ((fabs(i-size/2)<size/4)&&(fabs(j-size/2)<size/4)) continue;

            sq_x=-x_rel*dim+x/2+i-size/2;
            sq_y=-y_rel*dim+y/2+j-size/2;

            if ((sq_x>=0)&&(sq_y>=0)) colorCircleRA->sheet->setPixel(sq_x,sq_y,qRgb(((i+j)%2)*255,((i+j)%2)*255,((i+j)%2)*255));

        }
    }

}


void ColorSelector::plotCurrentColorDarkness() {

    int x=colorDarknessRA->width();
    int y=colorDarknessRA->height();
    int sq_x,sq_y,size(15);

    for (int i=0; i<x; i++) {
        for (int j=0; j<size; j++) {

            if ((fabs(j-size/2)<size/4)&&(i>size/4)&&((x-i)>size/4)) continue;

            sq_x=i;
            sq_y=j+y*(currentDarkness)-size/2;
            if (sq_y>=0) colorDarknessRA->sheet->setPixel(sq_x,sq_y,qRgb(((i+j)%2)*255,((i+j)%2)*255,((i+j)%2)*255));

        }
    }
}


void ColorSelector::plotCurrentColorAlpha() {

    int x=colorAlphaRA->width();
    int y=colorAlphaRA->height();
    int sq_x,sq_y,size(15);

    for (int i=0; i<x; i++) {
        for (int j=0; j<size; j++) {

            if ((fabs(j-size/2)<size/4)&&(i>size/4)&&((x-i)>size/4)) continue;

            sq_x=i;
            sq_y=j+y*(currentAlpha)-size/2;
            if (sq_y>=0) colorAlphaRA->sheet->setPixel(sq_x,sq_y,qRgb(((i+j)%2)*255,((i+j)%2)*255,((i+j)%2)*255));

        }
    }
}


ColorSelector::ColorSelector(ColorScale *c, QString *dd, std::string name, QWidget *parent) :
    QDialog(parent)
{

    minimumScale=c->getMin();
    maximumScale=c->getMax();
    nameScale=name;
    cols=c;
    oldCols=*c;
    colorScaleList.push_back(0.0);
    colorScaleList.push_back(1.0);
    colorValueList.push_back(QColor(0,0,0,0));
    colorValueList.push_back(QColor(255,0,0,255));

    if (cols->isOk()) {
        colorScaleList=cols->scale;
        colorValueList=cols->color;
    }

    int n(colorScaleList.size());
    currentIndex=n-1;
    dragStrip=false;
    defaultDir=dd;


    colorCircleRA = new RatioRenderArea(1.0);
    colorCircleRA->setMinimumSize(250,250);
    colorDarknessRA = new RenderArea;
    colorDarknessRA->setMinimumSize(50,100);
    colorAlphaRA = new RenderArea;
    colorAlphaRA->setMinimumSize(50,100);
    colorStripRA = new RenderArea;
    colorStripRA->setMinimumSize(400,50);
    transparRA = new RenderArea;
    transparRA->setMinimumSize(400,100);
    colorBoxRA = new RenderArea;
    colorBoxRA->setMinimumSize(100,50);
    addCheckColorButton = new QPushButton(tr("Add color"));
    spreadScaleButton = new QPushButton(tr("Spread colors"));
    savePaletteButton = new QPushButton(tr("Save palette"));
    loadPaletteButton = new QPushButton(tr("Load palette"));
    removeCheckColorButton = new QPushButton(tr("Remove color"));
    linearButton = new QPushButton(tr("Linear transparency"));
    quadraticButton = new QPushButton(tr("Quadratic transparency"));
    exponentialButton = new QPushButton(tr("Exponential transparency"));
    rainbowButton = new QPushButton(tr("Rainbow"));
    reverseButton = new QPushButton(tr("Reverse colors"));
    exitButton = new QPushButton(tr("OK"));
    cancelButton = new QPushButton(tr("Cancel"));
    expConstant = new QSlider(Qt::Horizontal);
    expConstant->setMinimum(-1000);
    expConstant->setMaximum(1000);
    expConstant->setValue(500);

    if (nameScale.empty()) nameScale="Quantity";

    currentColorLabel = new QLabel(tr("Current color"));
    minimumScaleEdit = new QDoubleSpinBox();
    maximumScaleEdit = new QDoubleSpinBox();
    minimumScaleEdit->setDecimals(8);
    maximumScaleEdit->setDecimals(8);
    minimumScaleEdit->setMinimum(-1e20);
    minimumScaleEdit->setMaximum(maximumScale);
    maximumScaleEdit->setMinimum(minimumScale);
    maximumScaleEdit->setMaximum(1e20);
    minimumScaleEdit->setValue(minimumScale);
    maximumScaleEdit->setValue(maximumScale);
    std::stringstream ss;
    ss << "Color index (0-" << n-1 << ")";
    currentIndexLabel = new QLabel(tr(ss.str().c_str()));
    currentScaleLabel = new QLabel(tr(nameScale.c_str()));
    colorWheelLabel = new QLabel("Color Wheel");
    brightnessLabel = new QLabel("Brightness");
    alphaLabel = new QLabel("Transparency");
    currentRedLabel = new QLabel("Red:");
    currentGreenLabel = new QLabel("Green:");
    currentBlueLabel = new QLabel("Blue:");
    currentAlphaLabel = new QLabel("Alpha:");
    colorScaleLabel = new QLabel("Color Scale");
    transparencyLabel = new QLabel("Transparency");

    redchLineEdit = new QSpinBox();
    redchLineEdit->setMinimum(0);
    redchLineEdit->setMaximum(255);
    greenchLineEdit = new QSpinBox();
    greenchLineEdit->setMinimum(0);
    greenchLineEdit->setMaximum(255);
    bluechLineEdit = new QSpinBox();
    bluechLineEdit->setMinimum(0);
    bluechLineEdit->setMaximum(255);
    alphachLineEdit = new QSpinBox();
    alphachLineEdit->setMinimum(0);
    alphachLineEdit->setMaximum(255);
    brightnessLineEdit = new QSpinBox();
    brightnessLineEdit->setMinimum(-100);
    brightnessLineEdit->setMaximum(100);
    alphaLineEdit = new QSpinBox();
    alphaLineEdit->setMinimum(0);
    alphaLineEdit->setMaximum(100);
    currentIndexLineEdit = new QSpinBox();
    currentIndexLineEdit->setMinimum(0);
    currentIndexLineEdit->setMaximum(n-1);
    currentIndexLineEdit->setValue(currentIndex);
    currentScaleLineEdit = new QDoubleSpinBox();
    currentScaleLineEdit->setDecimals(8);
    currentScaleLineEdit->setMinimum(minimumScale);
    currentScaleLineEdit->setMaximum(maximumScale);
    currentScaleLineEdit->setDecimals(3);
    currentScaleLineEdit->setValue(colorScaleList[currentIndex]*(maximumScale-minimumScale)+minimumScale);

    hSeparator = new QFrame();
    hSeparator->setFrameShape(QFrame::HLine);
    hSeparator->setMinimumSize(200,3);

    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancelAndClose()));
    connect(exitButton, SIGNAL(clicked(bool)), this, SLOT(saveAndClose()));


    mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 0);
    mainLayout->setColumnStretch(2, 0);
    mainLayout->setColumnStretch(3, 0);
    mainLayout->setColumnStretch(4, 0);
    mainLayout->setColumnStretch(5, 0);
    mainLayout->setColumnStretch(6, 0);
    mainLayout->setColumnStretch(7, 0);
    mainLayout->setRowStretch(0, 0);
    mainLayout->setRowStretch(1, 0);
    mainLayout->setRowStretch(2, 0);
    mainLayout->setRowStretch(3, 0);
    mainLayout->setRowStretch(4, 0);
    mainLayout->setRowStretch(5, 1);
    mainLayout->setRowStretch(6, 0);
    mainLayout->setRowStretch(7, 0);
    mainLayout->setRowStretch(8, 0);
    mainLayout->setRowStretch(9, 0);
    mainLayout->setRowStretch(10, 0);
    mainLayout->setRowStretch(11, 0);
    mainLayout->setRowStretch(12, 0);
    mainLayout->setRowStretch(13, 0);
    mainLayout->setRowStretch(14, 0);
    mainLayout->setColumnMinimumWidth(0,200);
    mainLayout->setColumnMinimumWidth(1,10);
    mainLayout->setColumnMinimumWidth(2,50);
    mainLayout->setColumnMinimumWidth(3,10);
    mainLayout->setColumnMinimumWidth(4,50);
    mainLayout->setColumnMinimumWidth(5,10);
    mainLayout->setColumnMinimumWidth(6,30);
    mainLayout->setColumnMinimumWidth(7,70);
    mainLayout->setRowMinimumHeight(0,20);
    mainLayout->setRowMinimumHeight(1,20);
    mainLayout->setRowMinimumHeight(2,20);
    mainLayout->setRowMinimumHeight(3,20);
    mainLayout->setRowMinimumHeight(4,20);
    mainLayout->setRowMinimumHeight(5,100);
    mainLayout->setRowMinimumHeight(6,20);
    mainLayout->setRowMinimumHeight(7,10);
    mainLayout->setRowMinimumHeight(8,20);
    mainLayout->setRowMinimumHeight(9,50);
    mainLayout->setRowMinimumHeight(10,20);
    mainLayout->setRowMinimumHeight(11,50);
    mainLayout->setRowMinimumHeight(12,50);
    mainLayout->setRowMinimumHeight(13,10);
    mainLayout->setRowMinimumHeight(14,50);
    mainLayout->setRowMinimumHeight(15,10);
    mainLayout->setRowMinimumHeight(16,50);
    mainLayout->setRowMinimumHeight(17,10);
    mainLayout->setRowMinimumHeight(18,50);
    // row 0
    mainLayout->addWidget(colorWheelLabel, 0, 0, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(brightnessLabel, 0, 2, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(alphaLabel, 0, 4, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(currentColorLabel, 0, 6, 1, 2, Qt::AlignTop|Qt::AlignHCenter);
    // row 1
    mainLayout->addWidget(colorCircleRA, 1, 0, 6, 1);
    mainLayout->addWidget(colorDarknessRA, 1, 2, 5, 1);
    mainLayout->addWidget(colorAlphaRA, 1, 4, 5, 1);
    mainLayout->addWidget(currentRedLabel, 1, 6, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(redchLineEdit, 1, 7, 1, 1, Qt::AlignLeft);
    // row 2
    mainLayout->addWidget(currentGreenLabel, 2, 6, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(greenchLineEdit, 2, 7, 1, 1, Qt::AlignLeft);
    // row 3
    mainLayout->addWidget(currentBlueLabel, 3, 6, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(bluechLineEdit, 3, 7, 1, 1, Qt::AlignLeft);
    // row 4
    mainLayout->addWidget(currentAlphaLabel, 4, 6, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(alphachLineEdit, 4, 7, 1, 1, Qt::AlignLeft);
    // row 5
    mainLayout->addWidget(colorBoxRA, 5, 6, 1, 2, Qt::AlignHCenter|Qt::AlignTop);
    // row 6
    mainLayout->addWidget(brightnessLineEdit, 6, 2, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(alphaLineEdit, 6, 4, 1, 1, Qt::AlignHCenter);
    // row 7
    mainLayout->addWidget(hSeparator, 7, 0, 1, 8);
    // row 8
    mainLayout->addWidget(colorScaleLabel, 8, 0, 1, 8, Qt::AlignHCenter);
    // row 9
    mainLayout->addWidget(colorStripRA, 9, 0, 1, 8);
    // row 10
    mainLayout->addWidget(transparencyLabel, 10, 0, 1, 8, Qt::AlignHCenter);
    // row 11
    mainLayout->addWidget(transparRA, 11, 0, 1, 8);
    // row 12
    mainLayout->addWidget(maximumScaleEdit, 12, 6, 1, 2, Qt::AlignRight);
    mainLayout->addWidget(minimumScaleEdit, 12, 0, 1, 2, Qt::AlignLeft);
    mainLayout->addWidget(currentIndexLabel, 12, 2, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(currentIndexLineEdit, 12, 4, 1, 1, Qt::AlignCenter);

    mainLayout->addWidget(currentScaleLabel, 13, 2, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(currentScaleLineEdit, 13, 4, 1, 1, Qt::AlignCenter);

    mainLayout->addWidget(addCheckColorButton, 14, 1, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(removeCheckColorButton, 14, 2, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(spreadScaleButton, 14, 3, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(rainbowButton, 14, 4, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(reverseButton, 14, 5, 1, 1, Qt::AlignCenter);

    mainLayout->addWidget(linearButton, 16, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(quadraticButton, 16, 2, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(exponentialButton, 16, 4, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(expConstant, 16, 6, 1, 1, Qt::AlignCenter);

    mainLayout->addWidget(exitButton, 18, 6, 1, 1, Qt::AlignLeft);
    mainLayout->addWidget(loadPaletteButton, 18, 2, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(savePaletteButton, 18, 4, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(cancelButton, 18, 7, 1, 1, Qt::AlignCenter);

    setLayout(mainLayout);

    setWindowTitle(tr("Palette selector"));

    redchLineEdit->setValue((colorValueList[currentIndex]).red());
    greenchLineEdit->setValue((colorValueList[currentIndex]).green());
    bluechLineEdit->setValue((colorValueList[currentIndex]).blue());
    alphachLineEdit->setValue((colorValueList[currentIndex]).alpha());
    setFieldsFromColor();

    connect(linearButton,SIGNAL(clicked()),this,SLOT(linearTransparency()));
    connect(quadraticButton,SIGNAL(clicked()),this,SLOT(quadraticTransparency()));
    connect(exponentialButton,SIGNAL(clicked()),this,SLOT(exponentialTransparency()));
    connect(rainbowButton,SIGNAL(clicked()),this,SLOT(rainbow()));
    connect(reverseButton,SIGNAL(clicked()),this,SLOT(reverse()));
    connect(addCheckColorButton,SIGNAL(clicked()),this,SLOT(addColor()));
    connect(removeCheckColorButton,SIGNAL(clicked()),this,SLOT(removeColor()));
    connect(spreadScaleButton,SIGNAL(clicked()),this,SLOT(spreadScale()));
    connect(savePaletteButton,SIGNAL(clicked()),this,SLOT(savePalette()));
    connect(loadPaletteButton,SIGNAL(clicked()),this,SLOT(loadPalette()));
    connect(brightnessLineEdit,SIGNAL(valueChanged(int)),this,SLOT(setColorFromFields()));
    connect(alphaLineEdit,SIGNAL(valueChanged(int)),this,SLOT(setColorFromFields()));
    connect(redchLineEdit,SIGNAL(valueChanged(int)),this,SLOT(setFieldsFromColor()));
    connect(greenchLineEdit,SIGNAL(valueChanged(int)),this,SLOT(setFieldsFromColor()));
    connect(bluechLineEdit,SIGNAL(valueChanged(int)),this,SLOT(setFieldsFromColor()));
    connect(alphachLineEdit,SIGNAL(valueChanged(int)),this,SLOT(setFieldsFromColor()));
    connect(colorDarknessRA,SIGNAL(leftButtonPressed()),this,SLOT(darknessPressed()));
    connect(colorDarknessRA,SIGNAL(leftDrag()),this,SLOT(darknessPressed()));
    connect(colorCircleRA,SIGNAL(leftButtonPressed()),this,SLOT(circlePressed()));
    connect(colorCircleRA,SIGNAL(leftDrag()),this,SLOT(circlePressed()));
    connect(colorAlphaRA,SIGNAL(leftButtonPressed()),this,SLOT(alphaPressed()));
    connect(colorAlphaRA,SIGNAL(leftDrag()),this,SLOT(alphaPressed()));
    connect(colorStripRA,SIGNAL(leftButtonPressed()),this,SLOT(stripPressed()));
    connect(colorStripRA,SIGNAL(leftButtonReleased()),this,SLOT(stripReleased()));
    connect(colorStripRA,SIGNAL(leftDrag()),this,SLOT(stripPressed()));
    connect(currentIndexLineEdit,SIGNAL(valueChanged(int)),this,SLOT(changeCurrentIndex()));
    connect(currentScaleLineEdit,SIGNAL(valueChanged(double)),this,SLOT(changeCurrentScale()));
    connect(minimumScaleEdit,SIGNAL(valueChanged(double)),this,SLOT(changeCurrentRange()));
    connect(maximumScaleEdit,SIGNAL(valueChanged(double)),this,SLOT(changeCurrentRange()));

    updateRender();

}

void ColorSelector::linearTransparency() {

    double x;
    double y0(colorValueList.front().alpha());
    double y1(colorValueList.back().alpha());
    std::vector<QColor>::iterator j = colorValueList.begin();
    for (std::vector<double>::iterator i = colorScaleList.begin(); i!= colorScaleList.end(); i++) {
        x=*i;
        j->setAlpha((y1-y0)*x+y0);
        j++;
    }
    updateFieldsColor();
    setFieldsFromColor();

}

void ColorSelector::quadraticTransparency() {

    double x;
    double y0(colorValueList.front().alpha());
    double y1(colorValueList.back().alpha());
    std::vector<QColor>::iterator j = colorValueList.begin();
    for (std::vector<double>::iterator i = colorScaleList.begin(); i!= colorScaleList.end(); i++) {
        x=*i;
        j->setAlpha((y1-y0)*x*x+y0);
        j++;
    }
    updateFieldsColor();
    setFieldsFromColor();

}

void ColorSelector::exponentialTransparency() {

    double x;
    double y0(colorValueList.front().alpha());
    double y1(colorValueList.back().alpha());
    double k(expl(expConstant->value()/100.0)-1.0);
    if (k==0.0) {
        linearTransparency();
        return;
    }
    std::vector<QColor>::iterator j = colorValueList.begin();
    for (std::vector<double>::iterator i = colorScaleList.begin(); i!= colorScaleList.end(); i++) {
        x=*i;
        j->setAlpha((y1-y0)/k*powl(k+1.0,x)+(y0*(k+1)-y1)/(k));
        j++;
    }
    updateFieldsColor();
    setFieldsFromColor();

}

void ColorSelector::reverse() {


    int toRestore(currentIndex);
    unsigned int n(colorScaleList.size());

    std::vector<double> tempColorScale(n);
    std::vector<QColor> tempColorValue(n);
    for (unsigned int i=0; i<n; i++){
        tempColorScale[n-i-1]=1.0-colorScaleList[i];
        std::cout << colorScaleList[i] << " -> " << colorValueList[i].red() << ", " << colorValueList[i].green() << ", " << colorValueList[i].blue() << std::endl;
        tempColorValue[n-i-1]=colorValueList[i];
        tempColorValue[n-i-1].setAlpha(colorValueList[n-i-1].alpha());
    }

    colorScaleList=tempColorScale;
    colorValueList=tempColorValue;
    currentIndex=n-1-toRestore;
    currentColor=colorValueList[currentIndex];
    currentColorAngle=(currentIndex*360.0)/n;
    currentColorRadius=1.0;
    currentAlpha=colorValueList[currentIndex].alpha();
    update();
    block();
    redchLineEdit->setValue(currentColor.red());
    greenchLineEdit->setValue(currentColor.green());
    bluechLineEdit->setValue(currentColor.blue());
    alphachLineEdit->setValue(currentColor.alpha());
    release();
    setFieldsFromColor();

}

void ColorSelector::rainbow() {


    int toRestore(currentIndex);
    unsigned int n(colorScaleList.size());


    for (unsigned int i=0; i<n; i++) {
        currentIndex=i;
        currentColorAngle=((int(i)-toRestore)*360.0)/n;
        if (currentColorAngle<0.0) currentColorAngle+=360.0;
        currentColorRadius=1.0;
        currentAlpha=colorValueList[i].alpha();
        colorValueList[i]=getColorWheel();
        colorValueList[i].setAlpha(currentAlpha);
    }

    currentIndex=toRestore;
    currentColor=colorValueList[currentIndex];
    currentColorAngle=(currentIndex*360.0)/n;
    currentColorRadius=1.0;
    currentAlpha=colorValueList[currentIndex].alpha();
    update();
    block();
    redchLineEdit->setValue(currentColor.red());
    greenchLineEdit->setValue(currentColor.green());
    bluechLineEdit->setValue(currentColor.blue());
    alphachLineEdit->setValue(currentColor.alpha());
    release();
    setFieldsFromColor();

}

void ColorSelector::spreadScale() {

    int n(colorScaleList.size()-1);

    for (int i=1; i<n; i++) {
        colorScaleList[i]=(i*1.0)/n;
    }

    updateFieldsColor();

}

int ColorSelector::savePalette() {

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Palette File"), *defaultDir, tr("Palette (*.plt)"),0,QFileDialog::DontUseNativeDialog);
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QMessageBox::information(0,"error",file.errorString());
        return 1;
    }

    *defaultDir = QFileInfo(fileName).absolutePath();

    QTextStream store(&file);

    int n(colorScaleList.size());

    store << minimumScale << '\n' << maximumScale << '\n' << currentIndex << '\n' << n << '\n';

    for (int i=0; i<n; i++) {
        store << colorScaleList[i] << '\n';
        store << colorValueList[i].red() << '\n';
        store << colorValueList[i].green() << '\n';
        store << colorValueList[i].blue() << '\n';
        store << colorValueList[i].alpha() << '\n';
    }

    file.close();

    return 0;

}

int ColorSelector::loadPalette() {

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Palette File"), *defaultDir, tr("Palette (*.plt)"),0,QFileDialog::DontUseNativeDialog);
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QMessageBox::information(0,"error",file.errorString());
        return 1;
    }

    *defaultDir = QFileInfo(fileName).absolutePath();

    QTextStream store(&file);

    int n;

    store >> minimumScale >> maximumScale >> currentIndex >> n;

    if ((n<2)||(minimumScale>maximumScale)) {
        file.close();
        return 1;
    }

    std::vector<double> csl;
    std::vector<QColor> cvl;

    double scale;
    int r,g,b,a;

    for (int i=0; i<n; i++) {
        store >> scale >> r >> g >> b >> a;
        csl.push_back(scale);
        cvl.push_back(QColor(r,g,b,a));
    }

    csl[0]=0.0;
    csl[n-1]=1.0;
    colorScaleList=csl;
    colorValueList=cvl;

    file.close();

    block();

    minimumScaleEdit->setMinimum(-1e20);
    minimumScaleEdit->setMaximum(maximumScale);
    maximumScaleEdit->setMinimum(minimumScale);
    maximumScaleEdit->setMaximum(1e20);
    minimumScaleEdit->setValue(minimumScale);
    maximumScaleEdit->setValue(maximumScale);
    currentScaleLineEdit->setMaximum(maximumScale);
    currentScaleLineEdit->setMinimum(minimumScale);

    updateFieldsColor();
    setFieldsFromColor();

    release();

    update();

    return 0;

}

void ColorSelector::changeCurrentRange() {


    double oldMin(minimumScale),oldMax(maximumScale);
    minimumScale=minimumScaleEdit->value();
    maximumScale=maximumScaleEdit->value();
    currentScaleLineEdit->setMaximum(maximumScale);
    currentScaleLineEdit->setMinimum(minimumScale);

    minimumScaleEdit->setMaximum(maximumScale);
    maximumScaleEdit->setMinimum(minimumScale);

    block();

    int n(colorScaleList.size()),storeIndex(currentIndex);

    for (int i=1; i<n-1; i++) {

        currentIndex=i;
        double delta(maximumScale-minimumScale);
        if (delta>0)
            colorScaleList[i]=((colorScaleList[i]*(oldMax-oldMin)+oldMin)-minimumScale)/delta;
        else
            colorScaleList[i]=0.0;
        if (colorScaleList[i]<0.0) colorScaleList[i]=0.0;
        if (colorScaleList[i]>1.0) colorScaleList[i]=1.0;
        sort();

    }

    currentIndex=storeIndex;
    changeCurrentIndex();

    release();

    updateFieldsColor();

}

QColor ColorSelector::getColorWheel() {

    double gm(currentColorAngle);
    double rad(currentColorRadius);
    double r(0),g(0),b(0);

    if ((0<=gm)&&(gm<60)) {
        r=1;
        g=(gm)/60;
        b=0;
    }
    if ((60<=gm)&&(gm<120)) {
        r=(120-gm)/60;
        g=1;
        b=0;
    }
    if ((120<=gm)&&(gm<180)) {
        r=0;
        g=1;
        b=(gm-120)/60;
    }
    if ((180<=gm)&&(gm<240)) {
        r=0;
        g=(240-gm)/60;
        b=1;
    }
    if ((240<=gm)&&(gm<300)) {
        r=(gm-240)/60;
        g=0;
        b=1;
    }
    if ((300<=gm)&&(gm<360)) {
        r=1;
        g=0;
        b=(360-gm)/60;
    }

    r=r*rad+0.5*(1-rad);
    g=g*rad+0.5*(1-rad);
    b=b*rad+0.5*(1-rad);

    r=r*255.0;
    g=g*255.0;
    b=b*255.0;

    if (r>255) r=255;
    if (g>255) g=255;
    if (b>255) b=255;

    return QColor(r,g,b);

}

void ColorSelector::setFieldsFromColor() {

    block();

    (colorValueList[currentIndex])=QColor(redchLineEdit->value(),greenchLineEdit->value(),bluechLineEdit->value(),alphachLineEdit->value());

    int r((colorValueList[currentIndex]).red());
    int g((colorValueList[currentIndex]).green());
    int b((colorValueList[currentIndex]).blue());
    int a((colorValueList[currentIndex]).alpha());
    int min,max;

    min=r;
    max=r;
    if (g<min) min=g;
    if (g>max) max=g;
    if (b<min) min=b;
    if (b>max) max=b;

    currentDarkness=1.0-((max+min)/2.0)/255.0;

    currentColorRadius=(max-min)/255.0;

    r=r-min;
    g=g-min;
    b=b-min;
    max=max-min;
    if (max>0) {
        if ((max==r)&&(g==0)) {
            currentColorAngle=360.0-60.0*b/max;
        }
        if ((max==r)&&(b==0)) {
            currentColorAngle=60.0*g/max;
        }
        if ((max==g)&&(b==0)) {
            currentColorAngle=120-60.0*r/max;
        }
        if ((max==g)&&(r==0)) {
            currentColorAngle=120+60.0*b/max;
        }
        if ((max==b)&&(r==0)) {
            currentColorAngle=240.0-60.0*g/max;
        }
        if ((max==b)&&(g==0)) {
            currentColorAngle=240.0+60.0*r/max;
        }
    } else {

        currentColorAngle=0;

    }

    if (currentColorAngle>=360.0) currentColorAngle-=360.0;
    currentAlpha=(a/255.0);

    //std::cout << "RGBA changed" << std::endl;
    //std::cout << "Color changed!   -> R :" << currentColor.red() << "\tG :" << currentColor.green() << "\tB :" << currentColor.blue() << "\tA :" << currentColor.alpha() << std::endl;
    //std::cout << "Angle :" << currentColorAngle << "\tRadius :" << currentColorRadius << std::endl;

    updateFieldsDAndA();

    release();

    this->update();

}

void ColorSelector::setColorFromFields() {

    block();

    QColor pureColor=getColorWheel();

    currentDarkness=1.0-(brightnessLineEdit->value()+100.0)/200.0;
    currentAlpha=1.0-(alphaLineEdit->value())/100.0;
    double r(pureColor.red());
    double g(pureColor.green());
    double b(pureColor.blue());
    double a(currentAlpha);

    if (currentDarkness<0.5) {
        r=255.0-510.0*currentDarkness+r*(2.0*currentDarkness);
        g=255.0-510.0*currentDarkness+g*(2.0*currentDarkness);
        b=255.0-510.0*currentDarkness+b*(2.0*currentDarkness);
    } else {
        r=2.0*(1.0-currentDarkness)*r;
        g=2.0*(1.0-currentDarkness)*g;
        b=2.0*(1.0-currentDarkness)*b;
    }

    //std::cout << currentDarkness << ")\t" << r << "   " << g << "   " << b << std::endl;

    a=a*255;
    if (a>255) a=255;

    (colorValueList[currentIndex])=QColor(r,g,b,a);

    updateFieldsColor();

    release();

    this->update();

}

void ColorSelector::updateFieldsColor() {

    double value(colorScaleList[currentIndex]*(maximumScale-minimumScale)+minimumScale);

    redchLineEdit->setValue((colorValueList[currentIndex]).red());
    greenchLineEdit->setValue((colorValueList[currentIndex]).green());
    bluechLineEdit->setValue((colorValueList[currentIndex]).blue());
    alphachLineEdit->setValue((colorValueList[currentIndex]).alpha());
    int n(colorScaleList.size());
    std::stringstream ss;
    ss << "Color index (0-" << n-1 << ")";
    currentIndexLineEdit->setMaximum(n-1);
    currentIndexLabel->setText(tr(ss.str().c_str()));
    currentIndexLineEdit->setValue(currentIndex);
    currentScaleLineEdit->setValue(value);


}

void ColorSelector::updateFieldsDAndA() {

    redchLineEdit->setValue((colorValueList[currentIndex]).red());
    greenchLineEdit->setValue((colorValueList[currentIndex]).green());
    bluechLineEdit->setValue((colorValueList[currentIndex]).blue());
    alphachLineEdit->setValue((colorValueList[currentIndex]).alpha());
    alphaLineEdit->setValue((1-currentAlpha)*100);
    brightnessLineEdit->setValue(100-currentDarkness*200);

}

void ColorSelector::updateFields() {

    updateFieldsDAndA();
    updateFieldsColor();

}

void ColorSelector::updateRender() {

    int x=colorCircleRA->width();
    int y=colorCircleRA->height();
    colorCircleRA->sheet->fill(qRgba(0,0,0,255));
    double dim(x<y?x/2.0:y/2.0),distance,x_rel,y_rel,angle;
    double r(0.0),g(0.0),b(0.0);
    double mr(0.5),mg(0.5),mb(0.5);
    int int_r,int_g,int_b;

    for(int i=0; i<y; i++) {
        for (int j=0; j<x; j++) {

            y_rel=(i-y/2)/dim;
            x_rel=(j-x/2)/dim;
            distance=x_rel*x_rel+y_rel*y_rel;
            if (distance < 1.0) {
                angle=atan2(y_rel,x_rel)*180/3.1415296+180.0;
                if ((angle>=0.0)&&(angle<60.0)) {
                    r=1.0;
                    g=(angle-0.0)/60.0;
                    b=0.0;
                }

                if ((angle>=60.0)&&(angle<120.0)) {
                    r=(120.0-angle)/60.0;
                    g=1.0;
                    b=0.0;
                }

                if ((angle>=120.0)&&(angle<180.0)) {
                    r=0.0;
                    g=1.0;
                    b=(angle-120.0)/60.0;
                }

                if ((angle>=180.0)&&(angle<240.0)) {
                    r=0.0;
                    g=(240.0-angle)/60.0;
                    b=1.0;
                }

                if ((angle>=240.0)&&(angle<300.0)) {
                    r=(angle-240.0)/60.0;
                    g=0.0;
                    b=1.0;
                }

                if ((angle>=300.0)) {
                    r=1.0;
                    g=0.0;
                    b=(360.0-angle)/60.0;
                }

                distance=sqrtl(distance);
                r=r*distance+mr*(1.0-distance);
                g=g*distance+mg*(1.0-distance);
                b=b*distance+mb*(1.0-distance);

                int_r=r*256;
                int_g=g*256;
                int_b=b*256;

                if (int_r>255) int_r=255;
                if (int_g>255) int_g=255;
                if (int_b>255) int_b=255;

                colorCircleRA->sheet->setPixel(j,i,qRgb(int_r,int_g,int_b));
            }
        }
    }


    x=colorDarknessRA->width();
    y=colorDarknessRA->height();
    QColor pureColor=getColorWheel();
    for(int i=0; i<y; i++) {

        int_r=pureColor.red();
        int_g=pureColor.green();
        int_b=pureColor.blue();

        if (i<=y/2) {

            int_r=(int_r*i)/(y/2.0)+255*(y/2.0-i)/(y/2.0);
            int_g=(int_g*i)/(y/2.0)+255*(y/2.0-i)/(y/2.0);
            int_b=(int_b*i)/(y/2.0)+255*(y/2.0-i)/(y/2.0);

        } else {

            int_r=int_r*(y-i)/(y/2.0);
            int_g=int_g*(y-i)/(y/2.0);
            int_b=int_b*(y-i)/(y/2.0);

        }


        for (int j=0; j<x; j++) {

            colorDarknessRA->sheet->setPixel(j,i,qRgb(int_r,int_g,int_b));

        }
    }

    x=colorAlphaRA->width();
    y=colorAlphaRA->height();
    int colorA,colorB,color,sizeSquare(25);
    for(int i=0; i<y; i++) {
        for (int j=0; j<x; j++) {

            if (((i+y-j)%sizeSquare)<(sizeSquare/2))
                colorA=0;
            else
                colorA=1;

            if (((i+y+j)%sizeSquare)<(sizeSquare/2))
                colorB=0;
            else
                colorB=1;

            color=colorA^colorB;

            int_r=256*color;
            int_g=256*color;
            int_b=256*color;

            if (int_r>255) int_r=255;
            if (int_g>255) int_g=255;
            if (int_b>255) int_b=255;

            int_r=(colorValueList[currentIndex]).red()*((i*1.0)/y)+int_r*(1.0-(i*1.0)/y);
            int_g=(colorValueList[currentIndex]).green()*((i*1.0)/y)+int_g*(1.0-(i*1.0)/y);
            int_b=(colorValueList[currentIndex]).blue()*((i*1.0)/y)+int_b*(1.0-(i*1.0)/y);

            colorAlphaRA->sheet->setPixel(j,i,qRgb(int_r,int_g,int_b));

        }
    }

    x=colorBoxRA->width();
    y=colorBoxRA->height();
    for(int i=0; i<y; i++) {
        for (int j=0; j<x; j++) {

            if (((i+y+x-j)%sizeSquare)<(sizeSquare/2))
                colorA=0;
            else
                colorA=1;

            if (((i+y+x+j)%sizeSquare)<(sizeSquare/2))
                colorB=0;
            else
                colorB=1;

            color=colorA^colorB;

            int_r=256*color;
            int_g=256*color;
            int_b=256*color;

            if (int_r>255) int_r=255;
            if (int_g>255) int_g=255;
            if (int_b>255) int_b=255;

            int_r=(colorValueList[currentIndex]).red()*(currentAlpha)+int_r*(1.0-currentAlpha);
            int_g=(colorValueList[currentIndex]).green()*(currentAlpha)+int_g*(1.0-currentAlpha);
            int_b=(colorValueList[currentIndex]).blue()*(currentAlpha)+int_b*(1.0-currentAlpha);

            colorBoxRA->sheet->setPixel(j,i,qRgb(int_r,int_g,int_b));

        }
    }

    x=colorStripRA->width();
    y=colorStripRA->height();
    int colorC;
    for(int i=0; i<y; i++) {
        for (int j=0; j<x; j++) {

            if ((i>10)&&(i<y-10)) {
                if (((i*3-10+sizeSquare/2)%sizeSquare)<(sizeSquare/8))
                    colorA=0;
                else
                    colorA=1;

                if (fabsl(remainder(j,x/10.0))<(sizeSquare/12))
                    colorB=0;
                else
                    colorB=1;

                if (fabsl(remainder(j,x/100.0))<(sizeSquare/16))
                    colorC=1;
                else
                    colorC=0;

                color=colorA*colorB;

                int_r=256*color;
                int_g=256*color;
                int_b=256*color;

                if ((int_r)&&(colorC)) {
                    int_r=128;
                    int_g=128;
                    int_b=128;
                }

                if (int_r>255) int_r=255;
                if (int_g>255) int_g=255;
                if (int_b>255) int_b=255;

            } else {

                int_r=200;
                int_g=200;
                int_b=200;

            }

            colorStripRA->sheet->setPixel(j,i,qRgb(int_r,int_g,int_b));

        }
    }


    x=transparRA->width();
    y=transparRA->height();
    for(int i=0; i<y; i++) {
        for (int j=0; j<x; j++) {

            if (i>j) {
                if (((i-j)%sizeSquare)<(sizeSquare/2))
                    colorA=0;
                else
                    colorA=1;
            } else {
                if (((j-i)%sizeSquare)<(sizeSquare/2))
                    colorA=1;
                else
                    colorA=0;
            }

            if (((i+j)%sizeSquare)<(sizeSquare/2))
                colorB=0;
            else
                colorB=1;

            color=colorA^colorB;

            int_r=256*color;
            int_g=256*color;
            int_b=256*color;

            if (int_r>255) int_r=255;
            if (int_g>255) int_g=255;
            if (int_b>255) int_b=255;

            int_r=128*((i*1.0)/y)+int_r*(1.0-(i*1.0)/y);
            int_g=128*((i*1.0)/y)+int_g*(1.0-(i*1.0)/y);
            int_b=128*((i*1.0)/y)+int_b*(1.0-(i*1.0)/y);

            transparRA->sheet->setPixel(j,i,qRgb(int_r,int_g,int_b));

        }
    }

    plotCurrentColorCircle();
    plotCurrentColorDarkness();
    plotCurrentColorAlpha();
    plotCurrentColorScale();
    plotTranspar();

}


void ColorSelector::resizeEvent(QResizeEvent * event) {

    /*int x=colorCircleRA->width();
    int y=colorCircleRA->height();
    if (x<y)
        x=y;
    else
        y=x;*/
    QWidget::resizeEvent(event);
    updateRender();

}

void ColorSelector::paintEvent(QPaintEvent * event) {

    QWidget::paintEvent(event);
    updateRender();
    colorCircleRA->update();
    colorDarknessRA->update();
    colorAlphaRA->update();
    colorStripRA->update();
    colorBoxRA ->update();
    transparRA->update();

}


ColorSelector::~ColorSelector()
{

    delete colorCircleRA;
    delete colorDarknessRA;
    delete colorStripRA;
    delete colorBoxRA;
    delete transparRA;
    delete transparencyLabel;
    delete addCheckColorButton;
    delete removeCheckColorButton;
    delete exitButton;
    delete cancelButton;
    delete spreadScaleButton;
    delete savePaletteButton;
    delete loadPaletteButton;
    delete rainbowButton;
    delete linearButton;
    delete quadraticButton;
    delete exponentialButton;
    delete expConstant;
    delete currentColorLabel;
    delete minimumScaleEdit;
    delete maximumScaleEdit;
    delete currentIndexLabel;
    delete currentScaleLabel;
    delete mainLayout;

    delete colorWheelLabel;
    delete brightnessLabel;
    delete alphaLabel;
    delete currentRedLabel;
    delete currentGreenLabel;
    delete currentBlueLabel;
    delete currentAlphaLabel;
    delete colorScaleLabel;

    delete redchLineEdit;
    delete greenchLineEdit;
    delete bluechLineEdit;
    delete alphachLineEdit;
    delete brightnessLineEdit;
    delete alphaLineEdit;
    delete currentIndexLineEdit;
    delete currentScaleLineEdit;
    delete hSeparator;
}


QSize ColorSelector::sizeHint() const
{
    return QSize(640, 640);
}

QSize ColorSelector::minimumSizeHint() const
{
    return QSize(200, 200);
}

void ColorSelector::block() {

    state1.push(redchLineEdit->blockSignals(true));
    state2.push(greenchLineEdit->blockSignals(true));
    state3.push(bluechLineEdit->blockSignals(true));
    state4.push(alphachLineEdit->blockSignals(true));
    state5.push(brightnessLineEdit->blockSignals(true));
    state6.push(alphaLineEdit->blockSignals(true));
    state7.push(currentIndexLineEdit->blockSignals(true));
    state8.push(currentScaleLineEdit->blockSignals(true));
    state9.push(minimumScaleEdit->blockSignals(true));
    state10.push(maximumScaleEdit->blockSignals(true));

}

void ColorSelector::release() {

    if (state1.empty()) return;
    redchLineEdit->blockSignals(state1.top());
    greenchLineEdit->blockSignals(state2.top());
    bluechLineEdit->blockSignals(state3.top());
    alphachLineEdit->blockSignals(state4.top());
    brightnessLineEdit->blockSignals(state5.top());
    alphaLineEdit->blockSignals(state6.top());
    currentIndexLineEdit->blockSignals(state7.top());
    currentScaleLineEdit->blockSignals(state8.top());
    minimumScaleEdit->blockSignals(state9.top());
    maximumScaleEdit->blockSignals(state10.top());

    state1.pop();
    state2.pop();
    state3.pop();
    state4.pop();
    state5.pop();
    state6.pop();
    state7.pop();
    state8.pop();
    state9.pop();
    state10.pop();

}

void ColorSelector::darknessPressed() {

    block();

    int sy=colorDarknessRA->height();
    double y=((colorDarknessRA->y)*1.0)/sy;
    brightnessLineEdit->setValue(100-(y)*200);

    release();

    setColorFromFields();

}

void ColorSelector::alphaPressed() {

    block();

    int sy=colorAlphaRA->height();
    double y=((sy-colorAlphaRA->y)*1.0)/sy;
    alphaLineEdit->setValue((y)*100);

    release();

    setColorFromFields();


}

void ColorSelector::circlePressed() {

    block();

    int sx=colorCircleRA->width();
    int sy=colorCircleRA->height();
    double dim(sx<sy?sx/2.0:sy/2.0);
    double x=colorCircleRA->x;
    double y=colorCircleRA->y;
    currentColorAngle=atan2(sy/2.0-y,sx/2.0-x)*180/3.1415296;
    if (currentColorAngle<0.0) currentColorAngle+=360.0;
    currentColorRadius=sqrtl((x-(sx/2.0))*(x-(sx/2.0))+(y-(sy/2.0))*(y-(sy/2.0)))/dim;
    if (currentColorRadius>1.0) currentColorRadius=1.0;

    release();

    setColorFromFields();


}

void ColorSelector::addColor() {

    double start,end,delta(0);
    std::vector<double>::iterator i=colorScaleList.begin(),posScale;
    std::vector<QColor>::iterator j=colorValueList.begin(),posColor;
    int k=0,index(2);


    for ( ; (i+1)!=colorScaleList.end();) {
        start=*i;
        end=*(i+1);
        if (end-start>=delta) {
            delta=end-start;
            posScale=i+1;
            posColor=j+1;
            index=k+1;
        }
        i++;
        j++;
        k++;
    }

    double scale=*posScale-delta/2.0;

    block();

    QColor color(0,0,0,0);

    redchLineEdit->setValue(color.red());
    greenchLineEdit->setValue(color.green());
    bluechLineEdit->setValue(color.blue());
    alphachLineEdit->setValue(color.alpha());

    int n(colorScaleList.size());
    std::stringstream ss;
    ss << "Color index (0-" << n << ")";
    currentIndexLabel->setText(tr(ss.str().c_str()));

    currentIndex=index;

    colorScaleList.insert(posScale,scale);
    colorValueList.insert(posColor,color);

    currentIndexLineEdit->setMaximum(colorScaleList.size()-1);

    updateFieldsColor();

    setFieldsFromColor();

    release();

    update();

}

void ColorSelector::removeColor() {

    int n(colorScaleList.size()-1);
    if ((currentIndex==0)||(currentIndex==n)) return;

    std::vector<double>::iterator i=colorScaleList.begin()+currentIndex;
    std::vector<QColor>::iterator j=colorValueList.begin()+currentIndex;

    colorScaleList.erase(i);
    colorValueList.erase(j);

    currentIndexLineEdit->setMaximum(n);

    //QColor color=(colorValueList[currentIndex]);

    block();

    updateFieldsColor();


    std::stringstream ss;
    ss << "Color index (0-" << n-1 << ")";
    currentIndexLabel->setText(tr(ss.str().c_str()));

    //redchLineEdit->setValue(color.red());
    //greenchLineEdit->setValue(color.green());
    //bluechLineEdit->setValue(color.blue());
    //alphachLineEdit->setValue(color.alpha());

    setFieldsFromColor();

    release();

    update();

}

void ColorSelector::stripPressed() {

    if (!dragStrip) {

        block();

        int n(colorScaleList.size()),pos(colorStripRA->x),min(5),st_pos;

        st_pos=(colorScaleList[0]*(colorStripRA->width()));
        if (abs(st_pos-pos)<min) {
            min=abs(st_pos-pos);
            currentIndex=0;
        }

        st_pos=(colorScaleList[n-1]*(colorStripRA->width()));
        if (abs(st_pos-pos)<min) {
            min=abs(st_pos-pos);
            currentIndex=n-1;
        }

        min=5;

        for (int i=1; i<n-1; i++) {
            st_pos=(colorScaleList[i]*(colorStripRA->width()));
            if (abs(st_pos-pos)<min) {
                min=abs(st_pos-pos);
                currentIndex=i;
            }
        }

        updateFieldsColor();
        setFieldsFromColor();

        release();

        update();

        dragStrip=true;

    } else {

        int n(colorScaleList.size());
        if ((currentIndex==0)||(currentIndex==n-1)) return;
        double pos((1.0*colorStripRA->x)/(colorStripRA->width()));
        colorScaleList[currentIndex]=pos;
        sort();

        updateFieldsColor();
        update();

    }

}

void ColorSelector::stripReleased() {

    dragStrip=false;

}

void ColorSelector::sort() {

    int n(colorScaleList.size());

    if ((currentIndex==0)||(currentIndex==n-1)) return;

    double bef(colorScaleList[currentIndex-1]);
    double pos(colorScaleList[currentIndex]);
    double aft(colorScaleList[currentIndex+1]);

    if (pos<bef) {

        if (currentIndex==1) {
            colorScaleList[1]=bef;
            return;
        }

        double tmp_pos(bef);
        QColor tmp_col(colorValueList[currentIndex-1]);
        colorScaleList[currentIndex-1]=colorScaleList[currentIndex];
        colorValueList[currentIndex-1]=colorValueList[currentIndex];
        colorScaleList[currentIndex]=tmp_pos;
        colorValueList[currentIndex]=tmp_col;
        currentIndex-=1;

        sort();

    }

    if (pos>aft) {

        if (currentIndex==n-2) {
            colorScaleList[n-2]=aft;
            return;
        }

        double tmp_pos(aft);
        QColor tmp_col(colorValueList[currentIndex+1]);
        colorScaleList[currentIndex+1]=colorScaleList[currentIndex];
        colorValueList[currentIndex+1]=colorValueList[currentIndex];
        colorScaleList[currentIndex]=tmp_pos;
        colorValueList[currentIndex]=tmp_col;
        currentIndex+=1;

        sort();

    }

}

ColorScale ColorSelector::getCS() {

    //cols->setScale(colorScaleList);
    //cols->setRange(minimumScale,maximumScale);
    //cols->setColor(colorValueList);
    return oldCols;

}

void ColorSelector::cancelAndClose() {

    *cols=oldCols;


    block();

//    minimumScaleEdit->setMinimum(-1e20);
//    minimumScaleEdit->setMaximum(maximumScale);
//    maximumScaleEdit->setMinimum(minimumScale);
//    maximumScaleEdit->setMaximum(1e20);
//    minimumScaleEdit->setValue(minimumScale);
//    maximumScaleEdit->setValue(maximumScale);
//    currentScaleLineEdit->setMaximum(maximumScale);
//    currentScaleLineEdit->setMinimum(minimumScale);

    updateFieldsColor();
    setFieldsFromColor();

    release();

    update();
    this->hide();

}

void ColorSelector::saveAndClose() {

    cols->setScale(colorScaleList);
    cols->setRange(minimumScale,maximumScale);
    cols->setColor(colorValueList);
    oldCols=*cols;
    emit accepted();
    this->hide();

}



void ColorSelector::changeCurrentIndex() {

    currentIndex=currentIndexLineEdit->value();

    block();

    updateFieldsColor();
    setFieldsFromColor();

    release();

    update();

}

void ColorSelector::changeCurrentScale() {

    int n(colorScaleList.size());
    if ((currentIndex==0)||(currentIndex==n-1)) {
        currentScaleLineEdit->setValue(colorScaleList[currentIndex]*(maximumScale-minimumScale)+minimumScale);
        return;
    }
    double delta(maximumScale-minimumScale);
    double pos(currentScaleLineEdit->value()-minimumScale);
    if (delta>0)
        pos/=delta;
    else
        pos=0.0;

    colorScaleList[currentIndex]=pos;
    sort();

    updateFieldsColor();
    update();

}

ColorScale::ColorScale(QString input) {

    QTextStream store(&input);
    QString sscale,sred,sgreen,sblue,salpha,smin,smax,sn;
    unsigned int n;


    store >> smin >> smax >> sn;

    n=sn.toInt();
    minimum=smin.toDouble();
    maximum=smax.toDouble();
    scale.resize(n);
    color.resize(n);

    for (unsigned int i=0; i<n; i++) {

        store >> sscale >> sred >> sgreen >> sblue >> salpha;
        scale[i]=sscale.toDouble();
        color[i]=QColor(sred.toInt(),sgreen.toInt(),sblue.toInt(),salpha.toInt());

    }

}

QString ColorScale::toStr() {

    QString output;
    QTextStream store(&output);

    store << minimum << ' ' << maximum << ' ' << scale.size();

    for (unsigned int i=0; i<scale.size(); i++) {

        store << ' ' << scale[i] << ' ' << color[i].red() << ' ' << color[i].green() << ' ' << color[i].blue() << ' ' << color[i].alpha();

    }

    store << '\n';

    return output;

}

void ColorScale::setScale(const std::vector<double> &vec) {

    scale=vec;

}

void ColorScale::setColor(const std::vector<QColor> &vec) {

    color=vec;

}

QColor ColorScale::getColor(double val) {

    if (val<=0) return color.front();
    if (val>=1) return color.back();

    int n=scale.size();
    double bef(scale[0]),aft(scale[1]);
    int i(1);

    for ( ; i<n; i++) {

        bef=scale[i-1];
        aft=scale[i];
        if ((bef<=val)&&(val<aft)) break;

    }

    int r1,r2,g1,g2,b1,b2,a1,a2;
    double pos((val-bef)/(aft-bef));

    r1=color[i-1].red();
    g1=color[i-1].green();
    b1=color[i-1].blue();
    a1=color[i-1].alpha();

    r2=color[i].red();
    g2=color[i].green();
    b2=color[i].blue();
    a2=color[i].alpha();

    return QColor(r1+(r2-r1)*pos,g1+(g2-g1)*pos,b1+(b2-b1)*pos,a1+(a2-a1)*pos);

}

bool ColorScale::isOk() {

    if (scale.size()!=color.size()) return false;
    if (scale.size()<2) return false;

    for (unsigned int i=0; i<(scale.size()-1); i++) {
        if (scale[i]>scale[i+1]) return false;
    }

    return true;

}

void ColorScale::setRange(double min,double max) {

    if (min>max) return;
    minimum=min;
    maximum=max;

}

double ColorScale::getMin() {

    return minimum;
}

double ColorScale::getMax(){

    return maximum;

}

ColorScale::ColorScale(double min, double max, int predef) {

    // insert 4 predefined scales
    scale.clear();
    color.clear();
    if (min>max) {
        maximum=min;
        minimum=max;
    } else {
        minimum=min;
        maximum=max;
    }
    scale.push_back(0.0);
    scale.push_back(0.2);
    scale.push_back(0.4);
    scale.push_back(0.6);
    scale.push_back(0.8);
    scale.push_back(1.0);
    switch (predef) {
    case 0:
        color.push_back(QColor(255,0,0,1));
        color.push_back(QColor(255,255,0,2));
        color.push_back(QColor(0,255,255,4));
        color.push_back(QColor(0,0,255,8));
        color.push_back(QColor(255,0,255,32));
        color.push_back(QColor(255,255,255,255));
        break;
    default:
        color.push_back(QColor(0,0,0,1));
        color.push_back(QColor(128,0,0,4));
        color.push_back(QColor(255,0,0,8));
        color.push_back(QColor(255,128,128,20));
        color.push_back(QColor(128,255,255,40));
        color.push_back(QColor(255,255,255,255));
        break;
    }

}
