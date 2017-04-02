
#include "movie.h"
#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QTime>
#include <QRegularExpression>
#include <QString>
#include <QDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QCoreApplication>
#include <sstream>
#include <cmath>

void delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

QString FileList::operator[](int n) {

    if (n<(this->size()))
        return QStringList::operator[](n);
    else
        return QString("");

}

SimType FileTypeList::operator[](int n) {

    if (n<int(this->size()))
        return std::vector<SimType>::operator[](n);
    else
        return NO_FILE;

}

SimpleRenderArea::SimpleRenderArea(int x, int y, QWidget *parent)
    : QWidget(parent)
{

    sheet = new QImage(x,y,QImage::Format_RGB888);
    this->setMinimumSize(x,y);

}

void SimpleRenderArea::paintEvent(QPaintEvent *) {

    QPainter p(this);
    p.drawImage(QPoint(0,0),*sheet);

}

//void SimpleRenderArea::paintEvent(QPaintEvent* event) {

//    int x=width();
//    int y=height();
//    QPainter p(sheet);
//    QPen moviePen(Qt::black,2,Qt::SolidLine);
//    QPen iPen(Qt::lightGray,1,Qt::SolidLine);
//    QPen ePen(Qt::gray,1,Qt::SolidLine);
//    QPen backPen(Qt::white,1,Qt::SolidLine);
//    p.setPen(backPen);
//    p.drawRect(0,0,x-1,y-1);

//    QWidget::paintEvent(event);

//}

SimpleRenderArea::~SimpleRenderArea()
{

    delete sheet;

}

void Movie::clearAll() {

    cameraRule.clear();
    framesRule.clear();
    colorScaleRule.clear();
    textRule.clear();
    fileNames.clear();
    ftVector.clear();
    length=0;
    fps=25;
}

void Movie::removeSim(int index) {

    bool check(true);

    while(check) {
        check=false;
        for (int i=length-1; i>=0; i--) {
            if (framesRule[i]==index) {
                removeFrame(i);
                check=true;
                break;
            }
        }
    }

    for (int i=0; i<int(length); i++) {
        if (framesRule[i]>index) framesRule[i]--;
    }
    fileNames.erase(fileNames.begin()+index);
    ftVector.erase(ftVector.begin()+index);

}

void Movie::removeFrame(int index) {

    framesRule.erase(framesRule.begin()+index);
    cameraRule.erase(cameraRule.begin()+index);
    textRule.erase(textRule.begin()+index);
    colorScaleRule.erase(colorScaleRule.begin()+index);
    length--;

}

MovieMaker::~MovieMaker() {

    delete sra;
    delete timeStampLabel;
    delete timeStampSB;
    delete timeStampSL;
    delete prevTimeStamp;
    delete nextTimeStamp;
    delete frameLabel;
    delete frameVal;
    delete setMovieRule;
    delete setFrameRule;
    delete setCameraRule;
    delete setTextRule;
    delete setPaletteRule;
    delete addFrames;
    delete removeFrames;
    delete numTSLabel;
    delete numTSVal;
    delete mainLayout;

}

MovieMaker::MovieMaker(Movie *m, QString *dd, QWidget *parent):QWidget(parent) {

    if (m) {
        timeSpan = (m->length/m->fps);
    } else {
        timeSpan = 0;
    }

    cameraMem = new DataCamera;

    defaultDir=dd;

    currentMovie=m;

    sra = new SimpleRenderArea(700,150,this);
    sra->resize(700,150);
    timeStampSL = new QSlider(Qt::Horizontal);
    timeStampSL->setMouseTracking(false);
    timeStampSL->setTracking(false);
    timeStampSL->setSingleStep(1);
    timeStampSL->setMinimum(0);
    prevTimeStamp = new QPushButton("<<");
    nextTimeStamp = new QPushButton(">>");
    timeStampLabel = new QLabel("Frame :");
    timeStampSB = new QDoubleSpinBox();
    timeStampSB->setKeyboardTracking(false);
    frameLabel = new QLabel("Snapshot in this frame:");
    frameVal = new QSpinBox();
    frameVal->setKeyboardTracking(false);
    frameVal->setValue(0);
    numTSLabel = new QLabel("Frame :");
    numTSVal = new QSpinBox();
    numTSVal->setKeyboardTracking(false);
    numTSVal->setValue(0);
    setFrameRule = new QPushButton("Frames");
    setCameraRule = new QPushButton("Camera");
    setTextRule = new QPushButton("Text");
    setPaletteRule = new QPushButton("Colors");
    setMovieRule = new QPushButton("Movie");
    addFrames = new QPushButton("Add frames");
    removeFrames = new QPushButton("Del frames");
    mainLayout = new QGridLayout();


    mainLayout->setColumnMinimumWidth(0,40);
    mainLayout->setColumnMinimumWidth(1,40);
    mainLayout->setColumnMinimumWidth(2,40);
    mainLayout->setColumnMinimumWidth(3,40);
    mainLayout->setColumnMinimumWidth(4,40);
    mainLayout->setColumnMinimumWidth(5,40);
    mainLayout->setColumnMinimumWidth(6,40);
    mainLayout->setColumnMinimumWidth(7,40);

    mainLayout->setRowMinimumHeight(0,80);
    mainLayout->setRowMinimumHeight(1,30);
    mainLayout->setRowMinimumHeight(2,30);
    mainLayout->setRowMinimumHeight(3,30);

    mainLayout->addWidget(sra,0,0,1,8,Qt::AlignCenter);

    mainLayout->addWidget(timeStampSL,1,0,1,8);

    mainLayout->addWidget(prevTimeStamp,2,0,1,1,Qt::AlignCenter);
    mainLayout->addWidget(timeStampLabel,2,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(timeStampSB,2,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(numTSLabel,2,3,1,1,Qt::AlignCenter);
    mainLayout->addWidget(numTSVal,2,4,1,1,Qt::AlignCenter);
    mainLayout->addWidget(frameLabel,2,5,1,1,Qt::AlignCenter);
    mainLayout->addWidget(frameVal,2,6,1,1,Qt::AlignCenter);
    mainLayout->addWidget(nextTimeStamp,2,7,1,1,Qt::AlignCenter);

    mainLayout->addWidget(setMovieRule,3,0,1,1,Qt::AlignCenter);
    mainLayout->addWidget(setFrameRule,3,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(setCameraRule,3,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(setTextRule,3,3,1,1,Qt::AlignCenter);
    mainLayout->addWidget(addFrames,3,4,1,1,Qt::AlignCenter);
    mainLayout->addWidget(removeFrames,3,5,1,1,Qt::AlignCenter);
    //mainLayout->addWidget(setPaletteRule,3,4,1,1,Qt::AlignCenter);

    setLayout(mainLayout);

    reinitialize();
    emit newTS(0);

    connect(nextTimeStamp, SIGNAL(clicked(bool)), this, SLOT(nextTS()));
    connect(prevTimeStamp, SIGNAL(clicked(bool)), this, SLOT(prevTS()));
    connect(timeStampSL, SIGNAL(valueChanged(int)), this, SLOT(changeTS(int)));
    connect(timeStampSB, SIGNAL(valueChanged(double)), this, SLOT(changeTS(double)));
    connect(frameVal, SIGNAL(valueChanged(int)), this, SLOT(newFrame(int)));
    connect(numTSVal, SIGNAL(valueChanged(int)), this, SLOT(changeTS(int)));

    connect(setMovieRule, SIGNAL(clicked(bool)), this, SLOT(setMovie()));
    connect(setFrameRule, SIGNAL(clicked(bool)), this, SLOT(setFrame()));
    connect(setTextRule, SIGNAL(clicked(bool)), this, SLOT(setText()));
    connect(setCameraRule, SIGNAL(clicked(bool)), this, SLOT(setCamera()));
    connect(addFrames, SIGNAL(clicked(bool)), this, SLOT(addFramesSlot()));
    connect(removeFrames, SIGNAL(clicked(bool)), this, SLOT(removeFramesSlot()));

}

void MovieMaker::addFramesSlot() {

    if (!currentMovie) return;
    if (!currentMovie->length) return;
    bool isAccepted;
    int c=currentTS;

    AddFrameRule f(currentMovie,&isAccepted, currentTS, this);
    f.setModal(true);
    f.exec();

    if (!isAccepted) return;
    suspendSignals(true);
    reinitialize();
    suspendSignals(false);
    changeTS(c);
    emit newTS(currentTS);

}

void MovieMaker::removeFramesSlot() {

    if (!currentMovie) return;
    if (!currentMovie->length) return;
    bool isAccepted;
    int c=currentTS;

    RemoveFrameRule f(currentMovie,&isAccepted, currentTS, this);
    f.setModal(true);
    f.exec();

    if (!isAccepted) return;
    suspendSignals(true);
    reinitialize();
    suspendSignals(false);
    if (c>int(currentMovie->length)) c=int(currentMovie->length)-1;
    changeTS(c);
    emit newTS(currentTS);

}

void MovieMaker::setFrame() {

    if (!currentMovie) return;
    if (!currentMovie->length) return;
    bool isAccepted;
    int c=currentTS;

    FrameRule f(currentMovie,&isAccepted, currentTS, this);
    f.setModal(true);
    f.exec();

    if (!isAccepted) return;
    suspendSignals(true);
    reinitialize();
    suspendSignals(false);
    changeTS(c);
    emit newTS(currentTS);

}

void MovieMaker::setText() {

    if (!currentMovie) return;
    if (!currentMovie->length) return;
    bool isAccepted;
    int c=currentTS;

    TextRule f(currentMovie,&isAccepted, currentTS, defaultDir, this);
    f.setModal(true);
    f.exec();

    if (!isAccepted) return;
    suspendSignals(true);
    reinitialize();
    suspendSignals(false);
    changeTS(c);
    emit newTS(currentTS);

}

void MovieMaker::setCamera() {

    if (!currentMovie) return;
    if (!currentMovie->length) return;
    bool isAccepted;
    int c=currentTS;

    CameraRule f(currentMovie,&isAccepted, currentTS, cameraMem, this);
    f.setModal(true);
    f.exec();

    if (!isAccepted) return;
    suspendSignals(true);
    reinitialize();
    suspendSignals(false);
    changeTS(c);
    emit newTS(currentTS);

}

void MovieMaker::setMovie() {

    if (!currentMovie) return;
    if (!currentMovie->length) return;
    int c=currentTS;
    bool isAccepted;

    MovieRule m(currentMovie,&isAccepted,this);
    m.setModal(true);
    m.exec();

    if (!isAccepted) return;
    if (c>=int(currentMovie->length)) c=int(currentMovie->length)-1;
    suspendSignals(true);
    reinitialize();
    suspendSignals(false);
    changeTS(c);

}

void MovieMaker::newFrame(int frame) {

    delay(100);
    currentMovie->framesRule[currentTS]=frame;
    emit newTS(currentTS);
}

void MovieMaker::paintEvent(QPaintEvent *) {

    int x=sra->width();
    int y=sra->height();
    QPainter p(sra->sheet);
    QPen moviePen(QColor(0,0,0));
    moviePen.setWidthF(2.5);
    QPen iPen(QColor(192,192,192));
    QPen ePen(QColor(96,96,96));
    p.fillRect(QRect(0,0,x,y),Qt::white);
    p.setPen(ePen);
    p.drawLine(0,y-10,x,y-10);
    p.setPen(iPen);
    p.setFont(QFont("system",8));
    int totalX(currentMovie->length-1),totalY(currentMovie->fileNames.size()-1);
    double checkX(totalX),checkY(totalY);
    int scaleX(1),scaleY(1);


    if (totalX>=0) {

        if (totalY>0) {
            if (totalX>9) {
                double v=log10(totalX*1.0)+0.1;
                scaleX=pow(10.0,floorl(v));
                if (totalX/scaleX<3) scaleX/=10.0;
                if (scaleX==0) scaleX=1;
                checkX=(totalX*1.0)/scaleX;

                double w=log10(totalY*1.0)+0.1;
                scaleY=pow(10.0,floorl(w));
                if ((scaleY>=10)&&(totalY/scaleY<2)) scaleY/=10;
                checkY=(totalY*1.0)/scaleY;
            }


            if (totalX>0) {

                double spaceX((x-0.0)/checkX);
                double spaceY((y-15.0)/checkY);
                for (int i=0; i<checkX+0.1; i++) {
                    p.setPen(iPen);
                    p.drawLine(i*spaceX,y-10,i*spaceX,5);
                    p.setPen(moviePen);
                    std::stringstream ss;
                    ss << int(i*scaleX+.1);
                    //int offset(i==0?0:-15);
                    p.drawText(i*spaceX-3,y,ss.str().c_str());
                }
                for (int i=1; i<checkY+0.1; i++) {
                    p.setPen(iPen);
                    p.drawLine(0,y-(i*spaceY+10),x,y-(i*spaceY+10));
                    p.setPen(moviePen);
                    std::stringstream ss;
                    ss << int(i*scaleY+.1);
                    p.drawText(5,y-(i*spaceY+8),ss.str().c_str());
                }

                p.setPen(moviePen);
                double deltax=(x-0.0)/(totalX);
                double deltay=(y-15.0)/(totalY);
                for (int i=0; i<totalX; i++) {
                    p.drawLine(i*deltax,y-(currentMovie->framesRule[i]*deltay+10),(i+1)*deltax,y-(currentMovie->framesRule[i+1]*deltay+10));
                }
            } else {
                p.setPen(moviePen);
                p.drawText(x/2-30,y/2,"Single frame");
            }
        } else {

            if (totalX>0) {

                double v=log10(totalX*1.0)+0.1;
                scaleX=pow(10.0,floorl(v));
                if (totalX/scaleX<3) scaleX/=10.0;
                if (scaleX==0) scaleX=1;
                checkX=(totalX*1.0)/scaleX;

                double spaceX((x-0.0)/checkX);
                for (int i=0; i<checkX+0.1; i++) {
                    p.setPen(iPen);
                    p.drawLine(i*spaceX,y-10,i*spaceX,5);
                    p.setPen(moviePen);
                    std::stringstream ss;
                    ss << int(i*scaleX+.1);
                    //int offset(i==0?0:-15);
                    p.drawText(i*spaceX,y,ss.str().c_str());
                }
                p.drawText(x/2-30,y/2,"No images");

            } else {
                p.setPen(moviePen);
                p.drawText(x/2-30,y/2,"Single frame");
            }

        }
    } else {
        p.setPen(moviePen);
        p.drawText(x/2-30,y/2,"No movie yet!");
    }
    p.end();

    sra->update();

}

void MovieMaker::suspendSignals(bool val) {

    nextTimeStamp->blockSignals(val);
    prevTimeStamp->blockSignals(val);
    timeStampSL->blockSignals(val);
    timeStampSB->blockSignals(val);
    frameVal->blockSignals(val);
    numTSVal->blockSignals(val);

}

void MovieMaker::nextTS() {

    if (!currentMovie) return;
    if (currentTS<(int(currentMovie->length)-1)) {
        suspendSignals(true);
        currentTS++;
        timeStampSL->setValue(currentTS);
        timeStampSB->setValue(currentTS/currentMovie->fps);
        frameVal->setValue(currentMovie->framesRule[currentTS]);
        numTSVal->setValue(currentTS);
        suspendSignals(false);
        emit newTS(currentTS);
    }

}

void MovieMaker::prevTS() {

    if (!currentMovie) return;
    if (currentTS>0) {
        suspendSignals(true);
        currentTS--;
        timeStampSL->setValue(currentTS);
        timeStampSB->setValue(currentTS/currentMovie->fps);
        frameVal->setValue(currentMovie->framesRule[currentTS]);
        numTSVal->setValue(currentTS);
        suspendSignals(false);
        emit newTS(currentTS);
    }

}

void MovieMaker::changeTS(int val) {

    if (!currentMovie) return;
    suspendSignals(true);
    delay(200);
    currentTS=val;
    timeStampSB->setValue(currentTS/currentMovie->fps);
    frameVal->setValue(currentMovie->framesRule[currentTS]);
    numTSVal->setValue(currentTS);
    timeStampSL->setValue(currentTS);
    suspendSignals(false);
    emit newTS(currentTS);

}

void MovieMaker::changeTS(double val) {

    if (!currentMovie) return;
    suspendSignals(true);
    delay(200);
    currentTS=(val*currentMovie->fps+0.5);
    if (currentTS<0) currentTS=0;
    if (currentTS>int(currentMovie->length-1)) currentTS=currentMovie->length-1;
    timeStampSL->setValue(currentTS);
    timeStampSB->setValue(currentTS/currentMovie->fps);
    numTSVal->setValue(currentTS);
    frameVal->setValue(currentMovie->framesRule[currentTS]);
    suspendSignals(false);
    emit newTS(currentTS);

}

void MovieMaker::updateMovie(Movie *m) {

    currentMovie=m;
    reinitialize();
    if (currentMovie->length) emit newTS(0);

}

void MovieMaker::reinitialize() {

    currentTS=0;
    suspendSignals(true);
    if ((currentMovie->length)!=0) {
        frameVal->setMinimum(0);
        frameVal->setMaximum(int(currentMovie->fileNames.size())-1);
        timeStampSB->setEnabled(true);
        timeStampSL->setEnabled(true);
        prevTimeStamp->setEnabled(true);
        nextTimeStamp->setEnabled(true);
        frameVal->setEnabled(true);
        numTSVal->setEnabled(true);
        timeStampSL->setMaximum(int(currentMovie->length)-1);
        timeStampSL->setValue(0);
        timeStampSB->setValue(0);
        timeStampSB->setSingleStep(1.0/(currentMovie->fps)+0.000001);
        timeStampSB->setMaximum((int(currentMovie->length)-1.0)/(currentMovie->fps)+0.000001);
        frameVal->setValue(currentMovie->framesRule[currentTS]);
        numTSVal->setValue(currentTS);
        numTSVal->setMaximum(int(currentMovie->length)-1);
    } else {
        timeStampSB->setEnabled(false);
        timeStampSL->setEnabled(false);
        prevTimeStamp->setEnabled(false);
        nextTimeStamp->setEnabled(false);
        frameVal->setEnabled(false);
        numTSVal->setEnabled(false);
    }
    suspendSignals(false);
    update();

}

//void MovieMaker::paintEvent(QPaintEvent *event) {

//    QWidget::paintEvent(event);

//}

MovieRule::MovieRule(Movie *movie, bool *b, QWidget *parent):QDialog(parent) {

    checkIfAccepted=b;
    *checkIfAccepted=false;

    frames = new QSpinBox();
    time = new QDoubleSpinBox();
    fps= new QSpinBox();
    ok = new QPushButton("Accept");
    cancel = new QPushButton ("Reject");

    frameLabel = new QLabel("Num. of frames");
    timeLabel = new QLabel("Total time [s]");
    fpsLabel = new QLabel("Fps");

    cancel->setDefault(true);

    mainLayout = new QGridLayout();

    m=movie;
    if (!m) reject();
    if(!m->length) reject();

    setWindowTitle("Generic Movie Settings");

    framesVal=m->length;
    fpsVal=int(m->fps+0.5);
    timeVal=(framesVal*1.0)/fpsVal;

    frames->setMaximum(900000);
    time->setMaximum(3600.0);
    fps->setMaximum(120);
    frames->setMinimum(0);
    time->setMinimum(0.0);
    fps->setMinimum(1);

    mainLayout->setColumnMinimumWidth(0,40);
    mainLayout->setColumnMinimumWidth(1,40);
    mainLayout->setColumnMinimumWidth(2,40);
    mainLayout->setColumnMinimumWidth(3,40);

    mainLayout->setRowMinimumHeight(0,30);
    mainLayout->setRowMinimumHeight(1,30);
    mainLayout->setRowMinimumHeight(2,10);
    mainLayout->setRowMinimumHeight(3,30);

    mainLayout->addWidget(frameLabel,0,0,1,1,Qt::AlignCenter);
    mainLayout->addWidget(frames,0,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(fpsLabel,0,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(fps,0,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(timeLabel,1,0,1,1,Qt::AlignCenter);
    mainLayout->addWidget(time,1,1,1,1,Qt::AlignCenter);

    mainLayout->addWidget(ok,3,0,1,2,Qt::AlignCenter);
    mainLayout->addWidget(cancel,3,2,1,2,Qt::AlignCenter);

    setLayout(mainLayout);

    updateFields();

    connect(frames, SIGNAL(valueChanged(int)), this, SLOT(framesChanged(int)));
    connect(time, SIGNAL(valueChanged(double)), this, SLOT(timeChanged(double)));
    connect(fps, SIGNAL(valueChanged(int)), this, SLOT(fpsChanged(int)));
    connect(ok, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

}

void MovieRule::updateFields() {

    fps->blockSignals(true);
    time->blockSignals(true);
    frames->blockSignals(true);
    fps->setValue(fpsVal);
    frames->setValue(framesVal);
    time->setValue(timeVal);
    fps->blockSignals(false);
    time->blockSignals(false);
    frames->blockSignals(false);

}

void MovieRule::framesChanged(int i) {

    framesVal=i;
    timeVal=(framesVal*1.0)/fpsVal;
    updateFields();

}

void MovieRule::timeChanged(double d) {

    timeVal=d;
    framesVal=(timeVal*fpsVal);
    updateFields();

}

void MovieRule::fpsChanged(int d) {

    fpsVal=d;
    timeVal=(1.0*framesVal)/fpsVal;
    updateFields();

}

void MovieRule::accept() {

    Camera tmpCamera(m->cameraRule.back());
    int tmpFrame(m->framesRule.back());
    std::vector<Text> tmpText(m->textRule.back());
    ColorScale  tmpCS(m->colorScaleRule.back());



    m->fps=fpsVal;
    m->length=framesVal;
    m->cameraRule.resize(framesVal,tmpCamera);
    m->framesRule.resize(framesVal,tmpFrame);
    m->textRule.resize(framesVal,tmpText);
    m->colorScaleRule.resize(framesVal,tmpCS);

    if (!m->length) {
        m->cameraRule.push_back(tmpCamera);
        m->framesRule.push_back(0);
        m->textRule.push_back(std::vector<Text>());
        m->colorScaleRule.push_back(tmpCS);
        m->length++;
    }

    *checkIfAccepted=true;
    QDialog::accept();

}

void MovieRule::reject() {

    *checkIfAccepted=false;
    QDialog::reject();

}

MovieRule::~MovieRule() {

    delete ok;
    delete cancel;
    delete fps;
    delete time;
    delete frames;
    delete frameLabel;
    delete timeLabel;
    delete fpsLabel;
    delete mainLayout;

}

FrameRule::FrameRule(Movie *movie, bool *b, int currentFrame, QWidget *parent):QDialog(parent) {

    checkIfAccepted=b;
    *checkIfAccepted=false;

    startTime = new QDoubleSpinBox();
    endTime = new QDoubleSpinBox();
    startFrame = new QSpinBox();
    endFrame = new QSpinBox();
    startPic = new QSpinBox();
    endPic = new QSpinBox();
    reverse = new QPushButton("Reverse order");
    ok = new QPushButton("Accept");
    cancel = new QPushButton ("Reject");

    startTimeLabel = new QLabel("Apply rule from time [s]");
    endTimeLabel = new QLabel(" to time [s]");
    startFrameLabel = new QLabel("Apply rule from frame");
    endFrameLabel = new QLabel(" to frame");
    startPicLabel = new QLabel("First Snapshot :");
    endPicLabel = new QLabel("Last Snapshot :");

    cancel->setDefault(true);

    mainLayout = new QGridLayout();

    m=movie;
    if (!m) reject();
    if(!m->length) reject();

    setWindowTitle("Link snapshots to frames");

    maxFrameVal=int(m->length)-1;
    maxTimeVal=(int(m->length)*1.0l)/(m->fps);
    maxPicVal=int(m->fileNames.size())-1;

    startTimeVal=(currentFrame*1.0l)/(m->fps);
    endTimeVal=maxTimeVal;
    startFrameVal=currentFrame;
    endFrameVal=maxFrameVal;
    startPicVal=0;
    endPicVal=maxPicVal;

    startTime->setMaximum(maxTimeVal);
    endTime->setMaximum(maxTimeVal);
    startFrame->setMaximum(maxFrameVal);
    endFrame->setMaximum(maxFrameVal);
    startPic->setMaximum(maxPicVal);
    endPic->setMaximum(maxPicVal);

    startTime->setMinimum(0.0);
    endTime->setMinimum(0.0);
    startFrame->setMinimum(0);
    endFrame->setMinimum(0);
    startPic->setMinimum(0);
    endPic->setMinimum(0);

    startTime->setKeyboardTracking(false);
    endTime->setKeyboardTracking(false);
    startFrame->setKeyboardTracking(false);
    endFrame->setKeyboardTracking(false);

    startTime->setSingleStep(1.0l/(m->fps));
    endTime->setSingleStep(1.0l/(m->fps));

    mainLayout->setColumnMinimumWidth(0,40);
    mainLayout->setColumnMinimumWidth(1,40);
    mainLayout->setColumnMinimumWidth(2,40);
    mainLayout->setColumnMinimumWidth(3,40);

    mainLayout->setRowMinimumHeight(0,30);
    mainLayout->setRowMinimumHeight(1,30);
    mainLayout->setRowMinimumHeight(2,10);
    mainLayout->setRowMinimumHeight(3,30);
    mainLayout->setRowMinimumHeight(4,30);
    mainLayout->setRowMinimumHeight(5,10);
    mainLayout->setRowMinimumHeight(6,30);

    mainLayout->addWidget(startTimeLabel,0,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(startTime,0,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(endTimeLabel,0,2,1,1,Qt::AlignRight);
    mainLayout->addWidget(endTime,0,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(startFrameLabel,1,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(startFrame,1,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(endFrameLabel,1,2,1,1,Qt::AlignRight);
    mainLayout->addWidget(endFrame,1,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(startPicLabel,3,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(startPic,3,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(endPicLabel,3,2,1,1,Qt::AlignRight);
    mainLayout->addWidget(endPic,3,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(reverse,4,1,1,3,Qt::AlignCenter);

    mainLayout->addWidget(ok,6,0,1,2,Qt::AlignCenter);
    mainLayout->addWidget(cancel,6,2,1,2,Qt::AlignCenter);

    setLayout(mainLayout);

    updateFields();

    connect(startTime, SIGNAL(valueChanged(double)), this, SLOT(startTimeChanged(double)));
    connect(endTime, SIGNAL(valueChanged(double)), this, SLOT(endTimeChanged(double)));
    connect(startFrame, SIGNAL(valueChanged(int)), this, SLOT(startFrameChanged(int)));
    connect(endFrame, SIGNAL(valueChanged(int)), this, SLOT(endFrameChanged(int)));
    connect(reverse, SIGNAL(clicked(bool)), this, SLOT(reverseOrder()));

    connect(ok, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

}

void FrameRule::suspendSignals(bool b) {

    startTime->blockSignals(b);
    endTime->blockSignals(b);
    startFrame->blockSignals(b);
    endFrame->blockSignals(b);

}

void FrameRule::updateFields() {

    startTime->blockSignals(true);
    endTime->blockSignals(true);
    startFrame->blockSignals(true);
    endFrame->blockSignals(true);
    startPic->blockSignals(true);
    endPic->blockSignals(true);

    startTime->setValue(startTimeVal);
    endTime->setValue(endTimeVal);
    startFrame->setValue(startFrameVal);
    endFrame->setValue(endFrameVal);
    startPic->setValue(startPicVal);
    endPic->setValue(endPicVal);

    startTime->blockSignals(false);
    endTime->blockSignals(false);
    startFrame->blockSignals(false);
    endFrame->blockSignals(false);
    startPic->blockSignals(false);
    endPic->blockSignals(false);

}

void FrameRule::reverseOrder() {

    int temp=startPic->value();
    startPic->setValue(endPic->value());
    endPic->setValue(temp);

}

void FrameRule::startTimeChanged(double d) {

    suspendSignals(true);
    endTime->setMinimum(d);
    startFrame->setValue(int(d*m->fps)+0.00001);
    suspendSignals(false);

}

void FrameRule::endTimeChanged(double d) {

    suspendSignals(true);
    startTime->setMaximum(d);
    endFrame->setValue(int(d*m->fps)+0.00001);
    suspendSignals(false);

}

void FrameRule::startFrameChanged(int i) {

    suspendSignals(true);
    endFrame->setMinimum(i);
    startTime->setValue((i*1.0l)/m->fps);
    suspendSignals(false);

}

void FrameRule::endFrameChanged(int i) {

    suspendSignals(true);
    startFrame->setMaximum(i);
    endTime->setValue((i*1.0l)/m->fps);
    suspendSignals(false);

}

void FrameRule::accept() {

    // TO DO
    startFrameVal=startFrame->value();
    endFrameVal=endFrame->value();
    startPicVal=startPic->value();
    endPicVal=endPic->value();
    double delta;
    if (endFrameVal!=startFrameVal) {
        delta = (((fabs(endPicVal-startPicVal)+1)*1.0l)/(1.0l*(endFrameVal-startFrameVal+1)));
    } else {
        delta = 0;
    }
    for (int i=0; i<=(endFrameVal-startFrameVal); i++) {
        if (endPicVal>startPicVal) {
            m->framesRule[i+startFrameVal]=int(delta*i)+startPicVal;
        } else {
            m->framesRule[i+startFrameVal]=startPicVal-int(delta*i);
        }
    }
    *checkIfAccepted=true;
    QDialog::accept();

}

void FrameRule::reject() {

    *checkIfAccepted=false;
    QDialog::reject();

}

FrameRule::~FrameRule() {

    delete ok;
    delete cancel;
    delete startTime;
    delete endTime;
    delete startFrame;
    delete endFrame;
    delete startPic;
    delete endPic;
    delete mainLayout;

}

TextRule::TextRule(Movie *movie, bool *b, int currentFrame, QString *dd, QWidget *parent):QDialog(parent) {

    checkIfAccepted=b;
    *checkIfAccepted=false;
    defaultDir=dd;

    startTime = new QDoubleSpinBox();
    endTime = new QDoubleSpinBox();
    startFrame = new QSpinBox();
    endFrame = new QSpinBox();
    red = new QSpinBox();
    green = new QSpinBox();
    blue = new QSpinBox();
    xPos = new QDoubleSpinBox();
    yPos = new QDoubleSpinBox();
    textEdit = new QPlainTextEdit();

    clearAllText = new QPushButton("Delete all text in frames");
    loadText = new QPushButton("Load text");
    saveText = new QPushButton("Save text");
    ok = new QPushButton("Accept");
    cancel = new QPushButton ("Reject");

    startTimeLabel = new QLabel("Apply rule from time [s]");
    endTimeLabel = new QLabel(" to time [s]");
    startFrameLabel = new QLabel("Apply rule from frame");
    endFrameLabel = new QLabel(" to frame");
    textLabel = new QLabel("Text");
    positionLabel = new QLabel("Relative Position X,Y [0-1]");
    colorLabel = new QLabel("Color R,G,B [0-255]");
    fontLabel = new QLabel("Font");

    cancel->setDefault(true);

    mainLayout = new QGridLayout();

    m=movie;
    if (!m) reject();
    if(!m->length) reject();

    setWindowTitle("Link text to frames");

    tmpTextRule=m->textRule;

    maxFrameVal=int(m->length)-1;
    maxTimeVal=(int(m->length)*1.0l)/(m->fps);

    startTimeVal=(currentFrame*1.0l)/(m->fps);
    endTimeVal=maxTimeVal;
    startFrameVal=currentFrame;
    endFrameVal=maxFrameVal;

    startTime->setMaximum(maxTimeVal);
    endTime->setMaximum(maxTimeVal);
    startFrame->setMaximum(maxFrameVal);
    endFrame->setMaximum(maxFrameVal);
    xPos->setMaximum(1.0);
    yPos->setMaximum(1.0);
    red->setMaximum(255);
    green->setMaximum(255);
    blue->setMaximum(255);

    startTime->setMinimum(0.0);
    endTime->setMinimum(0.0);
    startFrame->setMinimum(0);
    endFrame->setMinimum(0);
    xPos->setMinimum(0.0);
    yPos->setMinimum(0.0);
    red->setMinimum(0);
    green->setMinimum(0);
    blue->setMinimum(0);

    startTime->setKeyboardTracking(false);
    endTime->setKeyboardTracking(false);
    startFrame->setKeyboardTracking(false);
    endFrame->setKeyboardTracking(false);

    xPos->setValue(0.1);
    yPos->setValue(0.1);
    red->setValue(0);
    green->setValue(255);
    blue->setValue(128);

    //textEdit->setAcceptRichText(false);

    startTime->setSingleStep(1.0l/(m->fps));
    endTime->setSingleStep(1.0l/(m->fps));
    xPos->setSingleStep(0.05);
    yPos->setSingleStep(0.05);

    mainLayout->setColumnMinimumWidth(0,40);
    mainLayout->setColumnMinimumWidth(1,40);
    mainLayout->setColumnMinimumWidth(2,40);
    mainLayout->setColumnMinimumWidth(3,100);
    mainLayout->setColumnStretch(3,1.0);

    mainLayout->setRowMinimumHeight(0,30);
    mainLayout->setRowMinimumHeight(1,30);
    mainLayout->setRowMinimumHeight(2,10);
    mainLayout->setRowMinimumHeight(3,30);
    mainLayout->setRowMinimumHeight(4,10);
    mainLayout->setRowMinimumHeight(5,30);
    mainLayout->setRowMinimumHeight(6,30);
    mainLayout->setRowMinimumHeight(7,30);
    mainLayout->setRowMinimumHeight(8,150);
    mainLayout->setRowStretch(8,1.0);
    mainLayout->setRowMinimumHeight(9,10);
    mainLayout->setRowMinimumHeight(10,30);
    mainLayout->setRowMinimumHeight(11,10);
    mainLayout->setRowMinimumHeight(12,30);

    mainLayout->addWidget(startTimeLabel,0,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(startTime,0,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(endTimeLabel,0,2,1,1,Qt::AlignRight);
    mainLayout->addWidget(endTime,0,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(startFrameLabel,1,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(startFrame,1,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(endFrameLabel,1,2,1,1,Qt::AlignRight);
    mainLayout->addWidget(endFrame,1,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(clearAllText,3,0,1,4,Qt::AlignCenter);

    mainLayout->addWidget(positionLabel,5,0,1,2,Qt::AlignRight);
    mainLayout->addWidget(xPos,5,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(yPos,5,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(colorLabel,6,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(red,6,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(green,6,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(blue,6,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(textLabel,7,0,1,1,Qt::AlignCenter);

    mainLayout->addWidget(textEdit,8,0,1,4);

    mainLayout->addWidget(loadText,10,0,1,2,Qt::AlignCenter);
    mainLayout->addWidget(saveText,10,2,1,2,Qt::AlignCenter);

    mainLayout->addWidget(ok,12,0,1,2,Qt::AlignCenter);
    mainLayout->addWidget(cancel,12,2,1,2,Qt::AlignCenter);

    setLayout(mainLayout);

    updateFields();

    connect(startTime, SIGNAL(valueChanged(double)), this, SLOT(startTimeChanged(double)));
    connect(endTime, SIGNAL(valueChanged(double)), this, SLOT(endTimeChanged(double)));
    connect(startFrame, SIGNAL(valueChanged(int)), this, SLOT(startFrameChanged(int)));
    connect(endFrame, SIGNAL(valueChanged(int)), this, SLOT(endFrameChanged(int)));

    connect(clearAllText, SIGNAL(clicked(bool)), this, SLOT(deleteText()));

    connect(loadText, SIGNAL(clicked(bool)), this, SLOT(loadTextSlot()));
    connect(saveText, SIGNAL(clicked(bool)), this, SLOT(saveTextSlot()));

    connect(ok, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

}

void TextRule::loadTextSlot() {

    QStringList sl;
    QString filename = QFileDialog::getOpenFileName(this, tr("Load text format file"), *defaultDir, tr("Text format file (*.tff)"),0,QFileDialog::DontUseNativeDialog);
    if (filename.isEmpty()) return;

    *defaultDir=QFileInfo(filename).absolutePath();

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QMessageBox::information(0,"error",file.errorString());
        return;
    }

    for (int i=0; i<5; i++) {
        sl.push_back(file.readLine());
    }

    QString text(file.readAll());

    xPos->setValue(sl[0].toDouble());
    yPos->setValue(sl[1].toDouble());
    red->setValue(sl[2].toDouble());
    green->setValue(sl[3].toDouble());
    blue->setValue(sl[4].toDouble());

    textEdit->setPlainText(text);

}

void TextRule::saveTextSlot() {

    QString filename = QFileDialog::getSaveFileName(this, tr("Save text format file"), *defaultDir, tr("Text format file (*.tff)"),0,QFileDialog::DontUseNativeDialog);
    if (filename.isEmpty()) return;

    *defaultDir=QFileInfo(filename).absolutePath();

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QMessageBox::information(0,"error",file.errorString());
        return;
    }

    QTextStream store(&file);

    store << xPos->value() << '\n';
    store << yPos->value() << '\n';
    store << red->value() << '\n';
    store << green->value() << '\n';
    store << blue->value() << '\n';

    store << textEdit->toPlainText();

}

void TextRule::suspendSignals(bool b) {

    startTime->blockSignals(b);
    endTime->blockSignals(b);
    startFrame->blockSignals(b);
    endFrame->blockSignals(b);

}

void TextRule::updateFields() {

    startTime->blockSignals(true);
    endTime->blockSignals(true);
    startFrame->blockSignals(true);
    endFrame->blockSignals(true);

    startTime->setValue(startTimeVal);
    endTime->setValue(endTimeVal);
    startFrame->setValue(startFrameVal);
    endFrame->setValue(endFrameVal);

    startTime->blockSignals(false);
    endTime->blockSignals(false);
    startFrame->blockSignals(false);
    endFrame->blockSignals(false);

}

void TextRule::startTimeChanged(double d) {

    suspendSignals(true);
    endTime->setMinimum(d);
    startFrame->setValue(int(d*m->fps)+0.00001);
    suspendSignals(false);

}

void TextRule::endTimeChanged(double d) {

    suspendSignals(true);
    startTime->setMaximum(d);
    endFrame->setValue(int(d*m->fps)+0.00001);
    suspendSignals(false);

}

void TextRule::startFrameChanged(int i) {

    suspendSignals(true);
    endFrame->setMinimum(i);
    startTime->setValue((i*1.0l)/m->fps);
    suspendSignals(false);

}

void TextRule::endFrameChanged(int i) {

    suspendSignals(true);
    startFrame->setMaximum(i);
    endTime->setValue((i*1.0l)/m->fps);
    suspendSignals(false);

}

void TextRule::deleteText() {

    startFrameVal=startFrame->value();
    endFrameVal=endFrame->value();
    for (int i=startFrameVal; i<=(endFrameVal); i++) {
        (tmpTextRule[i]).clear();
    }
    *checkIfAccepted=true;
    m->textRule=tmpTextRule;
    QDialog::accept();

}

QString TextRule::processString(QString source, QString fileName) {

    QString z,mpc;


    for (int i=0; i<source.size(); i++) {
    }

    if (fileName.contains("_zstart")) {

    } else {

        int index=fileName.indexOf(QRegExp("_z\\d"));
        int index2=fileName.indexOf("_",index+1);
        if (index==-1) {
            z="";

        } else {

            if (index2==-1) {
                z=fileName.mid(index+2);
            } else {
                z=fileName.mid(index+2,index2-index-2);
            }
        }

        index=fileName.indexOf(QRegExp("\\dMpc"));
        int index1=fileName.left(index).lastIndexOf('_');
        if (index1==-1) index1=0;
        index2=fileName.indexOf("_",index1+1);
        if (index==-1) {
            mpc="";

        } else {

            if (index2==-1) {
                mpc=fileName.mid(index1+1);
            } else {
                mpc=fileName.mid(index1+1,index2-index1-1);
            }
        }

    }

    source.replace("\\z",z);
    source.replace("\\Mpc",mpc);


    return source;

}

void TextRule::accept() {

    // TO DO
    startFrameVal=startFrame->value();
    endFrameVal=endFrame->value();
    for (int i=startFrameVal; i<=(endFrameVal); i++) {
        (tmpTextRule[i]).push_back(Text(xPos->value(),yPos->value(),processString(textEdit->toPlainText(),m->fileNames[m->framesRule[i]]),QFont("system",10,Qt::PlainText),QColor(red->value(),green->value(),blue->value())));
    }
    *checkIfAccepted=true;
    m->textRule=tmpTextRule;
    QDialog::accept();

}

void TextRule::reject() {

    *checkIfAccepted=false;
    QDialog::reject();

}

TextRule::~TextRule() {

    delete mainLayout;
    delete startTime;
    delete endTime;
    delete startFrame;
    delete endFrame;
    delete red;
    delete green;
    delete blue;
    delete xPos;
    delete yPos;
    delete textEdit;
    delete clearAllText;
    delete ok;
    delete cancel;
    delete startTimeLabel;
    delete endTimeLabel;
    delete startFrameLabel;
    delete endFrameLabel;
    delete textLabel;
    delete positionLabel;
    delete colorLabel;
    delete fontLabel;

}


CameraRule::CameraRule(Movie *movie, bool *b, int cf, DataCamera* d, QWidget *parent):QDialog(parent) {

    checkIfAccepted=b;
    *checkIfAccepted=false;

    currentFrame=cf;

    dataCam=d;

    startTime = new QDoubleSpinBox();
    startTime->setValue(d->startTime);
    endTime = new QDoubleSpinBox();
    endTime->setValue(d->endTime);
    startFrame = new QSpinBox();
    startFrame->setValue(d->startFrame);
    endFrame = new QSpinBox();
    endFrame->setValue(d->endFrame);

    univRotCheck = new QCheckBox("Rotation of the universe");
    univRotCheck->setChecked(d->univRotCheck);

    axisRotUnivLabel = new QLabel("Rotation axis [X,Y,Z] :");

    xAxisCoordUniv = new QDoubleSpinBox();
    xAxisCoordUniv->setDecimals(6);
    xAxisCoordUniv->setValue(d->xAxisCoordUniv);
    yAxisCoordUniv = new QDoubleSpinBox();
    yAxisCoordUniv->setDecimals(6);
    yAxisCoordUniv->setValue(d->yAxisCoordUniv);
    zAxisCoordUniv = new QDoubleSpinBox();
    zAxisCoordUniv->setDecimals(6);
    zAxisCoordUniv->setValue(d->zAxisCoordUniv);

    xAxisButtonUniv = new QPushButton("Set X axis");
    yAxisButtonUniv = new QPushButton("Set Y axis");
    zAxisButtonUniv = new QPushButton("Set Z axis");

    rotUnivLabel = new QLabel("Angle [deg] :");

    rotUniv = new QDoubleSpinBox();
    rotUniv->setDecimals(6);
    rotUniv->setMaximum(3600.0);
    rotUniv->setMinimum(-3600.0);
    rotUniv->setValue(d->rotUniv);

    centerRotUnivLabel = new QLabel("Rotation center [X,Y,Z] :");

    xCenterRotUniv = new QDoubleSpinBox();
    xCenterRotUniv->setDecimals(6);
    xCenterRotUniv->setValue(d->xCenterRotUniv);
    yCenterRotUniv = new QDoubleSpinBox();
    yCenterRotUniv->setDecimals(6);
    yCenterRotUniv->setValue(d->yCenterRotUniv);
    zCenterRotUniv = new QDoubleSpinBox();
    zCenterRotUniv->setDecimals(6);
    zCenterRotUniv->setValue(d->zCenterRotUniv);

    centerButtonUniv = new QPushButton("Center");

    // if not fixed point

    camRotCheck = new QCheckBox("Rotation of the camera");
    camRotCheck->setChecked(d->camRotCheck);

    axisRotCamLabel = new QLabel("Rotation axis [R,P,Y] :");

    rAxisCoordCam = new QDoubleSpinBox();
    rAxisCoordCam->setDecimals(6);
    rAxisCoordCam->setValue(d->rAxisCoordCam);
    pAxisCoordCam = new QDoubleSpinBox();
    pAxisCoordCam->setDecimals(6);
    pAxisCoordCam->setValue(d->pAxisCoordCam);
    yAxisCoordCam = new QDoubleSpinBox();
    yAxisCoordCam->setDecimals(6);
    yAxisCoordCam->setValue(d->yAxisCoordCam);

    rAxisButtonCam = new QPushButton("Set Roll axis");
    pAxisButtonCam = new QPushButton("Set Pitch axis");
    yAxisButtonCam = new QPushButton("Set Yaw axis");

    rotCamLabel = new QLabel("Angle [deg] :");

    rotCam = new QDoubleSpinBox();
    rotCam->setDecimals(6);
    rotCam->setMaximum(3600.0);
    rotCam->setMinimum(-3600.0);
    rotCam->setValue(d->rotCam);
    camRotCheck->setChecked(d->camRotCheck);

    // end if not
    camTransCheck = new QCheckBox("Translation of the camera");
    camTransCheck->setChecked(d->camTransCheck);

    fromCamLabel = new QLabel("From [X,Y,Z] :");

    xPosFromCam = new QDoubleSpinBox();
    xPosFromCam->setDecimals(6);
    xPosFromCam->setValue(d->xPosFromCam);
    yPosFromCam = new QDoubleSpinBox();
    yPosFromCam->setDecimals(6);
    yPosFromCam->setValue(d->yPosFromCam);
    zPosFromCam = new QDoubleSpinBox();
    zPosFromCam->setDecimals(6);
    zPosFromCam->setValue(d->zPosFromCam);

    fromButtonCam = new QPushButton("Here");
    toButtonCam = new QPushButton("Here");

    zoomFromLabel = new QLabel("Zoom :");

    zoomFromCam = new QDoubleSpinBox();
    zoomFromCam->setValue(d->zoomFromCam);

    toCamLabel = new QLabel("To [X,Y,Z] :");

    xPosToCam = new QDoubleSpinBox();
    xPosToCam->setDecimals(6);
    xPosToCam->setValue(d->xPosToCam);
    yPosToCam = new QDoubleSpinBox();
    yPosToCam->setDecimals(6);
    yPosToCam->setValue(d->yPosToCam);
    zPosToCam = new QDoubleSpinBox();
    zPosToCam->setDecimals(6);
    zPosToCam->setValue(d->zPosToCam);

    zoomToLabel = new QLabel("Zoom :");

    zoomToCam = new QDoubleSpinBox();
    zoomToCam->setValue(d->zoomToCam);

    fixedPointCheck = new QCheckBox("Look at a fixed coordinate");
    fixedPointCheck->setChecked(d->fixedPointCheck);

    xPosFixedPoint = new QDoubleSpinBox();
    xPosFixedPoint->setDecimals(6);
    xPosFixedPoint->setValue(d->xPosFixedPoint);
    yPosFixedPoint = new QDoubleSpinBox();
    yPosFixedPoint->setDecimals(6);
    yPosFixedPoint->setValue(d->yPosFixedPoint);
    zPosFixedPoint = new QDoubleSpinBox();
    zPosFixedPoint->setDecimals(6);
    zPosFixedPoint->setValue(d->zPosFixedPoint);

    fixedPointButton = new QPushButton("Center");

    ok = new QPushButton("Accept");
    cancel = new QPushButton ("Reject");

    startTimeLabel = new QLabel("Apply rule from time [s]");
    endTimeLabel = new QLabel(" to time [s]");
    startFrameLabel = new QLabel("Apply rule from frame");
    endFrameLabel = new QLabel(" to frame");

    mainLayout = new QGridLayout();

    m=movie;
    if (!m) reject();
    if(!m->length) reject();

    setWindowTitle("Set camera movements");

    tmpCameraRule=m->cameraRule;

    maxFrameVal=int(m->length)-1;
    maxTimeVal=(int(m->length)*1.0l)/(m->fps);

    startTimeVal=(currentFrame*1.0l)/(m->fps);
    endTimeVal=maxTimeVal;
    startFrameVal=currentFrame;
    endFrameVal=maxFrameVal;

    startTime->setMaximum(maxTimeVal);
    endTime->setMaximum(maxTimeVal);
    startFrame->setMaximum(maxFrameVal);
    endFrame->setMaximum(maxFrameVal);

    startTime->setMinimum(0.0);
    endTime->setMinimum(0.0);
    startFrame->setMinimum(0);
    endFrame->setMinimum(0);

    startTime->setKeyboardTracking(false);
    endTime->setKeyboardTracking(false);
    startFrame->setKeyboardTracking(false);
    endFrame->setKeyboardTracking(false);

    startTime->setSingleStep(1.0l/(m->fps));
    endTime->setSingleStep(1.0l/(m->fps));

//    univRotCheck->setChecked(false);
//    camRotCheck->setChecked(false);
//    camTransCheck->setChecked(false);
//    fixedPointCheck->setChecked(false);

    xAxisCoordUniv->setMaximum(10.0);
    yAxisCoordUniv->setMaximum(10.0);
    zAxisCoordUniv->setMaximum(10.0);
    xCenterRotUniv->setMaximum(10.0);
    yCenterRotUniv->setMaximum(10.0);
    zCenterRotUniv->setMaximum(10.0);
    rAxisCoordCam->setMaximum(10.0);
    pAxisCoordCam->setMaximum(10.0);
    yAxisCoordCam->setMaximum(10.0);
    xPosFromCam->setMaximum(10.0);
    yPosFromCam->setMaximum(10.0);
    zPosFromCam->setMaximum(10.0);
    zoomFromCam->setMaximum(8.0);
    xPosToCam->setMaximum(10.0);
    yPosToCam->setMaximum(10.0);
    zPosToCam->setMaximum(10.0);
    zoomToCam->setMaximum(8.0);
    xPosFixedPoint->setMaximum(10.0);
    yPosFixedPoint->setMaximum(10.0);
    zPosFixedPoint->setMaximum(10.0);

    xAxisCoordUniv->setMinimum(-10.0);
    yAxisCoordUniv->setMinimum(-10.0);
    zAxisCoordUniv->setMinimum(-10.0);
    xCenterRotUniv->setMinimum(-10.0);
    yCenterRotUniv->setMinimum(-10.0);
    zCenterRotUniv->setMinimum(-10.0);
    rAxisCoordCam->setMinimum(-10.0);
    pAxisCoordCam->setMinimum(-10.0);
    yAxisCoordCam->setMinimum(-10.0);
    xPosFromCam->setMinimum(-10.0);
    yPosFromCam->setMinimum(-10.0);
    zPosFromCam->setMinimum(-10.0);
    zoomFromCam->setMinimum(0.5);
    xPosToCam->setMinimum(-10.0);
    yPosToCam->setMinimum(-10.0);
    zPosToCam->setMinimum(-10.0);
    zoomToCam->setMinimum(0.5);
    xPosFixedPoint->setMinimum(-10.0);
    yPosFixedPoint->setMinimum(-10.0);
    zPosFixedPoint->setMinimum(-10.0);

//    zoomFromCam->setValue(1.0);
//    zoomToCam->setValue(1.0);

    mainLayout->setColumnMinimumWidth(0,40);
    mainLayout->setColumnMinimumWidth(1,40);
    mainLayout->setColumnMinimumWidth(2,40);
    mainLayout->setColumnMinimumWidth(3,40);
    mainLayout->setColumnMinimumWidth(4,40);
    mainLayout->setColumnMinimumWidth(5,40);
    mainLayout->setColumnMinimumWidth(6,40);

    mainLayout->setRowMinimumHeight(0,30);
    mainLayout->setRowMinimumHeight(1,30);
    mainLayout->setRowMinimumHeight(2,30);
    mainLayout->setRowMinimumHeight(3,30);
    mainLayout->setRowMinimumHeight(4,30);
    mainLayout->setRowMinimumHeight(5,30);
    mainLayout->setRowMinimumHeight(6,30);
    mainLayout->setRowMinimumHeight(7,30);
    mainLayout->setRowMinimumHeight(8,30);
    mainLayout->setRowMinimumHeight(9,30);
    mainLayout->setRowMinimumHeight(10,30);
    mainLayout->setRowMinimumHeight(11,30);
    mainLayout->setRowMinimumHeight(12,30);
    mainLayout->setRowMinimumHeight(13,30);
    mainLayout->setRowMinimumHeight(14,30);
    mainLayout->setRowMinimumHeight(15,30);
    mainLayout->setRowMinimumHeight(16,30);

    mainLayout->addWidget(startTimeLabel,0,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(startTime,0,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(endTimeLabel,0,2,1,1,Qt::AlignRight);
    mainLayout->addWidget(endTime,0,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(startFrameLabel,1,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(startFrame,1,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(endFrameLabel,1,2,1,1,Qt::AlignRight);
    mainLayout->addWidget(endFrame,1,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(univRotCheck,3,0,1,4,Qt::AlignLeft);

    mainLayout->addWidget(axisRotUnivLabel,4,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(xAxisCoordUniv,4,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(yAxisCoordUniv,4,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(zAxisCoordUniv,4,3,1,1,Qt::AlignCenter);
    mainLayout->addWidget(rotUnivLabel,4,4,1,1,Qt::AlignRight);
    mainLayout->addWidget(rotUniv,4,5,1,1,Qt::AlignCenter);

    mainLayout->addWidget(xAxisButtonUniv,5,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(yAxisButtonUniv,5,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(zAxisButtonUniv,5,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(centerRotUnivLabel,6,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(xCenterRotUniv,6,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(yCenterRotUniv,6,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(zCenterRotUniv,6,3,1,1,Qt::AlignCenter);
    mainLayout->addWidget(centerButtonUniv,6,5,1,1,Qt::AlignCenter);

    mainLayout->addWidget(camRotCheck,7,0,1,4,Qt::AlignLeft);

    mainLayout->addWidget(axisRotCamLabel,8,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(rAxisCoordCam,8,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(pAxisCoordCam,8,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(yAxisCoordCam,8,3,1,1,Qt::AlignCenter);
    mainLayout->addWidget(rotCamLabel,8,4,1,1,Qt::AlignRight);
    mainLayout->addWidget(rotCam,8,5,1,1,Qt::AlignCenter);

    mainLayout->addWidget(rAxisButtonCam,9,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(pAxisButtonCam,9,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(yAxisButtonCam,9,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(camTransCheck,10,0,1,4,Qt::AlignLeft);

    mainLayout->addWidget(fromCamLabel,11,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(xPosFromCam,11,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(yPosFromCam,11,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(zPosFromCam,11,3,1,1,Qt::AlignCenter);
    mainLayout->addWidget(zoomFromLabel,11,4,1,1,Qt::AlignRight);
    mainLayout->addWidget(zoomFromCam,11,5,1,1,Qt::AlignCenter);
    mainLayout->addWidget(fromButtonCam,11,6,1,1,Qt::AlignCenter);

    mainLayout->addWidget(toCamLabel,12,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(xPosToCam,12,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(yPosToCam,12,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(zPosToCam,12,3,1,1,Qt::AlignCenter);
    mainLayout->addWidget(zoomToLabel,12,4,1,1,Qt::AlignRight);
    mainLayout->addWidget(zoomToCam,12,5,1,1,Qt::AlignCenter);
    mainLayout->addWidget(toButtonCam,12,6,1,1,Qt::AlignCenter);

    mainLayout->addWidget(fixedPointCheck,13,1,1,4,Qt::AlignLeft);

    mainLayout->addWidget(xPosFixedPoint,14,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(yPosFixedPoint,14,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(zPosFixedPoint,14,3,1,1,Qt::AlignCenter);
    mainLayout->addWidget(fixedPointButton,14,5,1,1,Qt::AlignCenter);

    mainLayout->addWidget(ok,16,0,1,1,Qt::AlignCenter);
    mainLayout->addWidget(cancel,16,6,1,1,Qt::AlignCenter);

    setLayout(mainLayout);

//    centerUnivSlot();
//    yAxisUnivSlot();
//    rAxisCamSlot();
//    fromCamSlot();
//    toCamSlot();
//    fixedPoinSlot();

    cancel->setDefault(true);

    univRotSlot(univRotCheck->isChecked());
    camRotSlot(camRotCheck->isChecked());
    camTransSlot(camTransCheck->isChecked());
    fixedPointSlot(fixedPointCheck->isChecked());

    updateFields();

    connect(startTime, SIGNAL(valueChanged(double)), this, SLOT(startTimeChanged(double)));
    connect(endTime, SIGNAL(valueChanged(double)), this, SLOT(endTimeChanged(double)));
    connect(startFrame, SIGNAL(valueChanged(int)), this, SLOT(startFrameChanged(int)));
    connect(endFrame, SIGNAL(valueChanged(int)), this, SLOT(endFrameChanged(int)));

    connect(univRotCheck,SIGNAL(stateChanged(int)),this,SLOT(univRotSlot(int)));
    connect(camRotCheck,SIGNAL(stateChanged(int)),this,SLOT(camRotSlot(int)));
    connect(camTransCheck,SIGNAL(stateChanged(int)),this,SLOT(camTransSlot(int)));
    connect(fixedPointCheck,SIGNAL(stateChanged(int)),this,SLOT(fixedPointSlot(int)));

    connect(centerButtonUniv,SIGNAL(clicked(bool)),this,SLOT(centerUnivSlot()));
    connect(xAxisButtonUniv,SIGNAL(clicked(bool)),this,SLOT(xAxisUnivSlot()));
    connect(yAxisButtonUniv,SIGNAL(clicked(bool)),this,SLOT(yAxisUnivSlot()));
    connect(zAxisButtonUniv,SIGNAL(clicked(bool)),this,SLOT(zAxisUnivSlot()));
    connect(rAxisButtonCam,SIGNAL(clicked(bool)),this,SLOT(rAxisCamSlot()));
    connect(pAxisButtonCam,SIGNAL(clicked(bool)),this,SLOT(pAxisCamSlot()));
    connect(yAxisButtonCam,SIGNAL(clicked(bool)),this,SLOT(yAxisCamSlot()));
    connect(fromButtonCam,SIGNAL(clicked(bool)),this,SLOT(fromCamSlot()));
    connect(toButtonCam,SIGNAL(clicked(bool)),this,SLOT(toCamSlot()));
    connect(fixedPointButton,SIGNAL(clicked(bool)),this,SLOT(fixedPoinSlot()));

    connect(ok, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

}

void CameraRule::centerUnivSlot() {

    xCenterRotUniv->setValue(0.5);
    yCenterRotUniv->setValue(0.5);
    zCenterRotUniv->setValue(0.5);

}

void CameraRule::xAxisUnivSlot() {

    xAxisCoordUniv->setValue(1.0);
    yAxisCoordUniv->setValue(0.0);
    zAxisCoordUniv->setValue(0.0);

}

void CameraRule::yAxisUnivSlot() {

    xAxisCoordUniv->setValue(0.0);
    yAxisCoordUniv->setValue(1.0);
    zAxisCoordUniv->setValue(0.0);

}

void CameraRule::zAxisUnivSlot() {

    xAxisCoordUniv->setValue(0.0);
    yAxisCoordUniv->setValue(0.0);
    zAxisCoordUniv->setValue(1.0);

}

void CameraRule::rAxisCamSlot() {

    rAxisCoordCam->setValue(1.0);
    pAxisCoordCam->setValue(0.0);
    yAxisCoordCam->setValue(0.0);

}

void CameraRule::pAxisCamSlot() {

    rAxisCoordCam->setValue(0.0);
    pAxisCoordCam->setValue(1.0);
    yAxisCoordCam->setValue(0.0);

}

void CameraRule::yAxisCamSlot() {

    rAxisCoordCam->setValue(0.0);
    pAxisCoordCam->setValue(0.0);
    yAxisCoordCam->setValue(1.0);

}

void CameraRule::fromCamSlot() {

    xPosFromCam->setValue(m->cameraRule[currentFrame].position.origin.xPos);
    yPosFromCam->setValue(m->cameraRule[currentFrame].position.origin.yPos);
    zPosFromCam->setValue(m->cameraRule[currentFrame].position.origin.zPos);
    zoomFromCam->setValue(0.8/(m->cameraRule[currentFrame].angle));

}

void CameraRule::toCamSlot() {

    xPosToCam->setValue(m->cameraRule[currentFrame].position.origin.xPos);
    yPosToCam->setValue(m->cameraRule[currentFrame].position.origin.yPos);
    zPosToCam->setValue(m->cameraRule[currentFrame].position.origin.zPos);
    zoomToCam->setValue(0.8/(m->cameraRule[currentFrame].angle));

}

void CameraRule::fixedPoinSlot() {

    xPosFixedPoint->setValue(0.5);
    yPosFixedPoint->setValue(0.5);
    zPosFixedPoint->setValue(0.5);

}

void CameraRule::univRotSlot(int b) {

    axisRotUnivLabel->setEnabled(b);

    xAxisCoordUniv->setEnabled(b);
    yAxisCoordUniv->setEnabled(b);
    zAxisCoordUniv->setEnabled(b);

    xAxisButtonUniv->setEnabled(b);
    yAxisButtonUniv->setEnabled(b);
    zAxisButtonUniv->setEnabled(b);

    rotUniv->setEnabled(b);

    centerRotUnivLabel->setEnabled(b);

    xCenterRotUniv->setEnabled(b);
    yCenterRotUniv->setEnabled(b);
    zCenterRotUniv->setEnabled(b);

    centerButtonUniv->setEnabled(b);

}

void CameraRule::camRotSlot(int b) {

    if (b) {
        fixedPointCheck->setChecked(false);
        fixedPointSlot(false);
    }

    axisRotCamLabel->setEnabled(b);

    rAxisCoordCam->setEnabled(b);
    pAxisCoordCam->setEnabled(b);
    yAxisCoordCam->setEnabled(b);

    rAxisButtonCam->setEnabled(b);
    pAxisButtonCam->setEnabled(b);
    yAxisButtonCam->setEnabled(b);

    rotCam->setEnabled(b);

}

void CameraRule::camTransSlot(int b) {

    fromCamLabel->setEnabled(b);

    xPosFromCam->setEnabled(b);
    yPosFromCam->setEnabled(b);
    zPosFromCam->setEnabled(b);

    fromButtonCam->setEnabled(b);
    toButtonCam->setEnabled(b);

    zoomFromCam->setEnabled(b);

    toCamLabel->setEnabled(b);

    xPosToCam->setEnabled(b);
    yPosToCam->setEnabled(b);
    zPosToCam->setEnabled(b);

    zoomToCam->setEnabled(b);

    fixedPointCheck->setEnabled(b);

    xPosFixedPoint->setEnabled(b && (fixedPointCheck->isChecked()));
    yPosFixedPoint->setEnabled(b && (fixedPointCheck->isChecked()));
    zPosFixedPoint->setEnabled(b && (fixedPointCheck->isChecked()));

    fixedPointButton->setEnabled(fixedPointCheck->isChecked());

}

void CameraRule::fixedPointSlot(int b) {

    if (b) {
        camRotCheck->setChecked(false);
        camRotSlot(false);
    }

    xPosFixedPoint->setEnabled(b);
    yPosFixedPoint->setEnabled(b);
    zPosFixedPoint->setEnabled(b);

    fixedPointButton->setEnabled(b);

}

void CameraRule::suspendSignals(bool b) {

    startTime->blockSignals(b);
    endTime->blockSignals(b);
    startFrame->blockSignals(b);
    endFrame->blockSignals(b);

}

void CameraRule::updateFields() {

    startTime->blockSignals(true);
    endTime->blockSignals(true);
    startFrame->blockSignals(true);
    endFrame->blockSignals(true);

    startTime->setValue(startTimeVal);
    endTime->setValue(endTimeVal);
    startFrame->setValue(startFrameVal);
    endFrame->setValue(endFrameVal);

    startTime->blockSignals(false);
    endTime->blockSignals(false);
    startFrame->blockSignals(false);
    endFrame->blockSignals(false);

}

void CameraRule::startTimeChanged(double d) {

    suspendSignals(true);
    endTime->setMinimum(d);
    startFrame->setValue(int(d*m->fps)+0.00001);
    suspendSignals(false);

}

void CameraRule::endTimeChanged(double d) {

    suspendSignals(true);
    startTime->setMaximum(d);
    endFrame->setValue(int(d*m->fps)+0.00001);
    suspendSignals(false);

}

void CameraRule::startFrameChanged(int i) {

    suspendSignals(true);
    endFrame->setMinimum(i);
    startTime->setValue((i*1.0l)/m->fps);
    suspendSignals(false);

}

void CameraRule::endFrameChanged(int i) {

    suspendSignals(true);
    startFrame->setMaximum(i);
    endTime->setValue((i*1.0l)/m->fps);
    suspendSignals(false);

}

void CameraRule::accept() {

    startFrameVal=startFrame->value();
    endFrameVal=endFrame->value();

    Camera startCamera=tmpCameraRule[startFrameVal];
    Camera currentCamera=startCamera;
    double angleU(0.0),angleR(0.0),angleZoom;
    Coords axisU(0.0,1.0,0.0),axisR(0.0,1.0,0.0);
    Coords posU(0.0,0.0,0.0),posR(0.0,0.0,0.0);
    Coords pathR(0.0,0.0,0.0);
    Coords path(Coords(xPosToCam->value()-xPosFromCam->value(),yPosToCam->value()-yPosFromCam->value(),zPosToCam->value()-zPosFromCam->value()));

    int len=endFrameVal-startFrameVal;
    int index;
    for (int i=startFrameVal; i<=endFrameVal; i++) {

        index=i;
        if (!len) {
            len=1;
            i=startFrameVal+1;
        }

        currentCamera=startCamera;
        posR=currentCamera.position.orig();
        posU=Coords(zCenterRotUniv->value(),yCenterRotUniv->value(),xCenterRotUniv->value());
        pathR=(((i-startFrameVal)*1.0)/(len))*path;
        angleZoom=(((i-startFrameVal)*1.0)*(0.8/zoomToCam->value()-0.8/zoomFromCam->value())/(len))+0.8/zoomFromCam->value();

        if (univRotCheck->isChecked()) {

            angleU = (i-startFrameVal)*(rotUniv->value()/(len));
            axisU=Coords(zAxisCoordUniv->value(),yAxisCoordUniv->value(),xAxisCoordUniv->value());
            //axis=startCamera.position.base().transpose()*axis;
            axisU.unit();

            currentCamera.rotate(angleU/180.0*3.1415296,axisU,posU);
            pathR=pathR.rotate(angleU/180.0*3.1415296,axisU,Coords(0.0,0.0,0.0));

        }



        if (camRotCheck->isChecked()) {

            angleR = (i-startFrameVal)*(rotCam->value()/(len));
            axisR=rAxisCoordCam->value()*currentCamera.position.xAxis+ yAxisCoordCam->value()*currentCamera.position.yAxis+ pAxisCoordCam->value()*currentCamera.position.zAxis;
            axisR.unit();
            currentCamera.rotate(angleR/180.0*3.1415296,axisR,posR);

        }

        if (camTransCheck->isChecked()) {

            //pathR=pathR.rotate(-angleU/180.0*3.1415296,axisU,Coords(0.0,0.0,0.0));
            currentCamera.translate(pathR);
            currentCamera.angle=angleZoom;

            if (fixedPointCheck->isChecked()) {

                currentCamera.position.xAxis=(Coords(xPosFixedPoint->value(),yPosFixedPoint->value(),zPosFixedPoint->value())-currentCamera.position.orig()).unit();
                currentCamera.position.unitize();

            }

        }


        tmpCameraRule[index]=currentCamera;

    }

    *checkIfAccepted=true;
    m->cameraRule=tmpCameraRule;

    dataCam->univRotCheck=univRotCheck->isChecked();
    dataCam->camRotCheck=camRotCheck->isChecked();
    dataCam->camTransCheck=camTransCheck->isChecked();
    dataCam->fixedPointCheck=fixedPointCheck->isChecked();
    dataCam->xAxisCoordUniv=xAxisCoordUniv->value();
    dataCam->yAxisCoordUniv=yAxisCoordUniv->value();
    dataCam->zAxisCoordUniv=zAxisCoordUniv->value();
    dataCam->rotUniv=rotUniv->value();
    dataCam->xCenterRotUniv=xCenterRotUniv->value();
    dataCam->yCenterRotUniv=yCenterRotUniv->value();
    dataCam->zCenterRotUniv=zCenterRotUniv->value();
    dataCam->rAxisCoordCam=rAxisCoordCam->value();
    dataCam->pAxisCoordCam=pAxisCoordCam->value();
    dataCam->yAxisCoordCam=yAxisCoordCam->value();
    dataCam->rotCam=rotCam->value();
    dataCam->xPosFromCam=xPosFromCam->value();
    dataCam->yPosFromCam=yPosFromCam->value();
    dataCam->zPosFromCam=zPosFromCam->value();
    dataCam->xPosToCam=xPosToCam->value();
    dataCam->yPosToCam=yPosToCam->value();
    dataCam->zPosToCam=zPosToCam->value();
    dataCam->zoomFromCam=zoomFromCam->value();
    dataCam->zoomToCam=zoomToCam->value();
    dataCam->xPosFixedPoint=xPosFixedPoint->value();
    dataCam->yPosFixedPoint=yPosFixedPoint->value();
    dataCam->zPosFixedPoint=zPosFixedPoint->value();
    dataCam->startTime=startTime->value();
    dataCam->endTime=endTime->value();
    dataCam->startFrame=startFrame->value();
    dataCam->endFrame=endFrame->value(); {}

    QDialog::accept();

}

void CameraRule::reject() {

    *checkIfAccepted=false;
    QDialog::reject();

}

CameraRule::~CameraRule() {

    delete mainLayout;
    delete startTime;
    delete endTime;
    delete startFrame;
    delete endFrame;
    delete ok;
    delete cancel;
    delete startTimeLabel;
    delete endTimeLabel;
    delete startFrameLabel;
    delete endFrameLabel;
    delete univRotCheck;
    delete camRotCheck;
    delete camTransCheck;
    delete fixedPointCheck;
    delete xAxisCoordUniv;
    delete yAxisCoordUniv;
    delete zAxisCoordUniv;
    delete xAxisButtonUniv;
    delete yAxisButtonUniv;
    delete zAxisButtonUniv;
    delete rotUniv;
    delete xCenterRotUniv;
    delete yCenterRotUniv;
    delete zCenterRotUniv;
    delete rAxisCoordCam;
    delete pAxisCoordCam;
    delete yAxisCoordCam;
    delete rotCam;
    delete xPosFromCam;
    delete yPosFromCam;
    delete zPosFromCam;
    delete xPosToCam;
    delete yPosToCam;
    delete zPosToCam;
    delete zoomFromCam;
    delete zoomToCam;
    delete xPosFixedPoint;
    delete yPosFixedPoint;
    delete zPosFixedPoint;
    delete centerButtonUniv;
    delete rAxisButtonCam;
    delete pAxisButtonCam;
    delete yAxisButtonCam;
    delete fromButtonCam;
    delete toButtonCam;
    delete fixedPointButton;
    delete axisRotUnivLabel;
    delete centerRotUnivLabel;
    delete axisRotCamLabel;
    delete fromCamLabel;
    delete toCamLabel;
    delete rotUnivLabel;
    delete rotCamLabel;
    delete zoomFromLabel;
    delete zoomToLabel;

}




AddFrameRule::AddFrameRule(Movie *movie, bool *b, int currentFrame, QWidget *parent):QDialog(parent) {

    checkIfAccepted=b;
    *checkIfAccepted=false;

    startTime = new QDoubleSpinBox();
    lengthTime = new QDoubleSpinBox();
    startFrame = new QSpinBox();
    lengthFrame = new QSpinBox();
    ok = new QPushButton("Accept");
    cancel = new QPushButton ("Reject");

    startTimeLabel = new QLabel("Insert new frame(s) after this time [s]");
    lengthTimeLabel = new QLabel(" Time span [s]");
    startFrameLabel = new QLabel("Insert new frame(s) after this frame");
    lengthFrameLabel = new QLabel(" N. of frames");

    cancel->setDefault(true);

    mainLayout = new QGridLayout();

    m=movie;
    if (!m) reject();
    if(!m->length) reject();

    setWindowTitle("Insert frames");

    maxFrameVal=int(m->length)-1;
    maxTimeVal=(int(m->length)*1.0l)/(m->fps);
    maxPicVal=int(m->fileNames.size())-1;

    startTimeVal=(currentFrame*1.0l)/(m->fps);
    lengthTimeVal=1.0l/m->fps;
    startFrameVal=currentFrame;
    lengthFrameVal=1;

    startTime->setMaximum(maxTimeVal);
    lengthTime->setMaximum(3600.0);
    startFrame->setMaximum(maxFrameVal);
    lengthFrame->setMaximum(10000);

    startTime->setMinimum(-1.0l/m->fps);
    lengthTime->setMinimum(1.0l/m->fps);
    startFrame->setMinimum(-1);
    lengthFrame->setMinimum(1);

    startTime->setKeyboardTracking(false);
    lengthTime->setKeyboardTracking(false);
    startFrame->setKeyboardTracking(false);
    lengthFrame->setKeyboardTracking(false);

    startTime->setSingleStep(1.0l/(m->fps));
    lengthTime->setSingleStep(1.0l/(m->fps));

    mainLayout->setColumnMinimumWidth(0,40);
    mainLayout->setColumnMinimumWidth(1,40);
    mainLayout->setColumnMinimumWidth(2,40);
    mainLayout->setColumnMinimumWidth(3,40);

    mainLayout->setRowMinimumHeight(0,30);
    mainLayout->setRowMinimumHeight(1,30);
    mainLayout->setRowMinimumHeight(2,10);
    mainLayout->setRowMinimumHeight(3,30);

    mainLayout->addWidget(startTimeLabel,0,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(startTime,0,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(lengthTimeLabel,0,2,1,1,Qt::AlignRight);
    mainLayout->addWidget(lengthTime,0,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(startFrameLabel,1,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(startFrame,1,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(lengthFrameLabel,1,2,1,1,Qt::AlignRight);
    mainLayout->addWidget(lengthFrame,1,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(ok,3,0,1,2,Qt::AlignCenter);
    mainLayout->addWidget(cancel,3,2,1,2,Qt::AlignCenter);

    setLayout(mainLayout);

    updateFields();

    connect(startTime, SIGNAL(valueChanged(double)), this, SLOT(startTimeChanged(double)));
    connect(lengthTime, SIGNAL(valueChanged(double)), this, SLOT(lengthTimeChanged(double)));
    connect(startFrame, SIGNAL(valueChanged(int)), this, SLOT(startFrameChanged(int)));
    connect(lengthFrame, SIGNAL(valueChanged(int)), this, SLOT(lengthFrameChanged(int)));

    connect(ok, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

}

void AddFrameRule::suspendSignals(bool b) {

    startTime->blockSignals(b);
    lengthTime->blockSignals(b);
    startFrame->blockSignals(b);
    lengthFrame->blockSignals(b);

}

void AddFrameRule::updateFields() {

    startTime->blockSignals(true);
    lengthTime->blockSignals(true);
    startFrame->blockSignals(true);
    lengthFrame->blockSignals(true);

    startTime->setValue(startTimeVal);
    lengthTime->setValue(lengthTimeVal);
    startFrame->setValue(startFrameVal);
    lengthFrame->setValue(lengthFrameVal);

    startTime->blockSignals(false);
    lengthTime->blockSignals(false);
    startFrame->blockSignals(false);
    lengthFrame->blockSignals(false);

}

void AddFrameRule::startTimeChanged(double d) {

    suspendSignals(true);
    startFrame->setValue(int(d*m->fps)+0.00001);
    suspendSignals(false);

}

void AddFrameRule::lengthTimeChanged(double d) {

    suspendSignals(true);
    lengthFrame->setValue(int(d*m->fps)+0.00001);
    suspendSignals(false);

}

void AddFrameRule::startFrameChanged(int i) {

    suspendSignals(true);
    startTime->setValue((i*1.0l)/m->fps);
    suspendSignals(false);

}

void AddFrameRule::lengthFrameChanged(int i) {

    suspendSignals(true);
    lengthTime->setValue((i*1.0l)/m->fps);
    suspendSignals(false);

}

void AddFrameRule::accept() {

    // TO DO
    startFrameVal=startFrame->value();
    lengthFrameVal=lengthFrame->value();
    std::vector<Camera>::iterator iCamera=m->cameraRule.begin()+1+startFrameVal;
    std::vector<int>::iterator iFrame=m->framesRule.begin()+1+startFrameVal;
    std::vector<std::vector<Text> >::iterator iText=m->textRule.begin()+1+startFrameVal;
    std::vector<ColorScale>::iterator iColorScale=m->colorScaleRule.begin()+1+startFrameVal;

    Camera c;
    int f(0);
    std::vector<Text> t;
    ColorScale cs;

    if (startFrameVal>=0) {

        c=m->cameraRule[startFrameVal];
        f=m->framesRule[startFrameVal];
        t=m->textRule[startFrameVal];
        cs=m->colorScaleRule[startFrameVal];

    } else if (m->length>0) {

        c=m->cameraRule[0];
        f=m->framesRule[0];
        t=m->textRule[0];
        cs=m->colorScaleRule[0];

    }

    m->cameraRule.insert(iCamera,lengthFrameVal,c);
    m->framesRule.insert(iFrame,lengthFrameVal,f);
    m->textRule.insert(iText,lengthFrameVal,t);
    m->colorScaleRule.insert(iColorScale,lengthFrameVal,cs);

    m->length=m->length+lengthFrameVal;

    *checkIfAccepted=true;
    QDialog::accept();

}

void AddFrameRule::reject() {

    *checkIfAccepted=false;
    QDialog::reject();

}

AddFrameRule::~AddFrameRule() {

    delete ok;
    delete cancel;
    delete startTime;
    delete lengthTime;
    delete startFrame;
    delete lengthFrame;
    delete mainLayout;

}

RemoveFrameRule::RemoveFrameRule(Movie *movie, bool *b, int currentFrame, QWidget *parent):QDialog(parent) {

    checkIfAccepted=b;
    *checkIfAccepted=false;

    startTime = new QDoubleSpinBox();
    endTime = new QDoubleSpinBox();
    startFrame = new QSpinBox();
    endFrame = new QSpinBox();
    ok = new QPushButton("Accept");
    cancel = new QPushButton ("Reject");

    startTimeLabel = new QLabel("Delete frame(s) from time [s]");
    endTimeLabel = new QLabel(" to time [s]");
    startFrameLabel = new QLabel("Delete frame(s) from frame");
    endFrameLabel = new QLabel(" to frame");

    cancel->setDefault(true);

    mainLayout = new QGridLayout();

    m=movie;
    if (!m) reject();
    if(!m->length) reject();

    setWindowTitle("Delete frames");

    maxFrameVal=int(m->length)-1;
    maxTimeVal=(int(m->length)*1.0l)/(m->fps);
    maxPicVal=int(m->fileNames.size())-1;

    startTimeVal=(currentFrame*1.0l)/(m->fps);
    endTimeVal=maxTimeVal;
    startFrameVal=currentFrame;
    endFrameVal=maxFrameVal;

    startTime->setMaximum(maxTimeVal);
    endTime->setMaximum(maxTimeVal);
    startFrame->setMaximum(maxFrameVal);
    endFrame->setMaximum(maxFrameVal);

    startTime->setMinimum(0.0);
    endTime->setMinimum(0.0);
    startFrame->setMinimum(0);
    endFrame->setMinimum(0);

    startTime->setKeyboardTracking(false);
    endTime->setKeyboardTracking(false);
    startFrame->setKeyboardTracking(false);
    endFrame->setKeyboardTracking(false);

    startTime->setSingleStep(1.0l/(m->fps));
    endTime->setSingleStep(1.0l/(m->fps));

    mainLayout->setColumnMinimumWidth(0,40);
    mainLayout->setColumnMinimumWidth(1,40);
    mainLayout->setColumnMinimumWidth(2,40);
    mainLayout->setColumnMinimumWidth(3,40);

    mainLayout->setRowMinimumHeight(0,30);
    mainLayout->setRowMinimumHeight(1,30);
    mainLayout->setRowMinimumHeight(2,10);
    mainLayout->setRowMinimumHeight(3,30);

    mainLayout->addWidget(startTimeLabel,0,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(startTime,0,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(endTimeLabel,0,2,1,1,Qt::AlignRight);
    mainLayout->addWidget(endTime,0,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(startFrameLabel,1,0,1,1,Qt::AlignRight);
    mainLayout->addWidget(startFrame,1,1,1,1,Qt::AlignCenter);
    mainLayout->addWidget(endFrameLabel,1,2,1,1,Qt::AlignRight);
    mainLayout->addWidget(endFrame,1,3,1,1,Qt::AlignCenter);

    mainLayout->addWidget(ok,3,0,1,2,Qt::AlignCenter);
    mainLayout->addWidget(cancel,3,2,1,2,Qt::AlignCenter);

    setLayout(mainLayout);

    updateFields();

    connect(startTime, SIGNAL(valueChanged(double)), this, SLOT(startTimeChanged(double)));
    connect(endTime, SIGNAL(valueChanged(double)), this, SLOT(endTimeChanged(double)));
    connect(startFrame, SIGNAL(valueChanged(int)), this, SLOT(startFrameChanged(int)));
    connect(endFrame, SIGNAL(valueChanged(int)), this, SLOT(endFrameChanged(int)));

    connect(ok, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

}

void RemoveFrameRule::suspendSignals(bool b) {

    startTime->blockSignals(b);
    endTime->blockSignals(b);
    startFrame->blockSignals(b);
    endFrame->blockSignals(b);

}

void RemoveFrameRule::updateFields() {

    startTime->blockSignals(true);
    endTime->blockSignals(true);
    startFrame->blockSignals(true);
    endFrame->blockSignals(true);

    startTime->setValue(startTimeVal);
    endTime->setValue(endTimeVal);
    startFrame->setValue(startFrameVal);
    endFrame->setValue(endFrameVal);

    startTime->blockSignals(false);
    endTime->blockSignals(false);
    startFrame->blockSignals(false);
    endFrame->blockSignals(false);

}

void RemoveFrameRule::startTimeChanged(double d) {

    suspendSignals(true);
    endTime->setMinimum(d);
    startFrame->setValue(int(d*m->fps)+0.00001);
    suspendSignals(false);

}

void RemoveFrameRule::endTimeChanged(double d) {

    suspendSignals(true);
    startTime->setMaximum(d);
    endFrame->setValue(int(d*m->fps)+0.00001);
    suspendSignals(false);

}

void RemoveFrameRule::startFrameChanged(int i) {

    suspendSignals(true);
    endFrame->setMinimum(i);
    startTime->setValue((i*1.0l)/m->fps);
    suspendSignals(false);

}

void RemoveFrameRule::endFrameChanged(int i) {

    suspendSignals(true);
    startFrame->setMaximum(i);
    endTime->setValue((i*1.0l)/m->fps);
    suspendSignals(false);

}

void RemoveFrameRule::accept() {

    // TO DO
    startFrameVal=startFrame->value();
    endFrameVal=endFrame->value();

    Camera c=m->cameraRule[startFrameVal];
    ColorScale cs=m->colorScaleRule[startFrameVal];

    std::vector<Camera>::iterator iCamera=m->cameraRule.begin()+startFrameVal;
    std::vector<int>::iterator iFrame=m->framesRule.begin()+startFrameVal;
    std::vector<std::vector<Text> >::iterator iText=m->textRule.begin()+startFrameVal;
    std::vector<ColorScale>::iterator iColorScale=m->colorScaleRule.begin()+startFrameVal;

    std::vector<Camera>::iterator fCamera=m->cameraRule.begin()+endFrameVal+1;
    std::vector<int>::iterator fFrame=m->framesRule.begin()+endFrameVal+1;
    std::vector<std::vector<Text> >::iterator fText=m->textRule.begin()+endFrameVal+1;
    std::vector<ColorScale>::iterator fColorScale=m->colorScaleRule.begin()+endFrameVal+1;

    m->length=m->length-(endFrameVal-startFrameVal+1);

    m->cameraRule.erase(iCamera,fCamera);
    m->framesRule.erase(iFrame,fFrame);
    m->textRule.erase(iText,fText);
    m->colorScaleRule.erase(iColorScale,fColorScale);

    if (!m->length) {
        m->cameraRule.push_back(c);
        m->framesRule.push_back(0);
        m->textRule.push_back(std::vector<Text>());
        m->colorScaleRule.push_back(cs);
        m->length++;
    }

    *checkIfAccepted=true;
    QDialog::accept();

}

void RemoveFrameRule::reject() {

    *checkIfAccepted=false;
    QDialog::reject();

}

RemoveFrameRule::~RemoveFrameRule() {

    delete ok;
    delete cancel;
    delete startTime;
    delete endTime;
    delete startFrame;
    delete endFrame;
    delete mainLayout;

}
