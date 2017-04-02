#ifndef MOVIE_H
#define MOVIE_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QPicture>
#include <QSlider>
#include <QString>
#include <QStringList>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QColor>
#include <QFont>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QGridLayout>
#include "color_selector.h"
#include "render_area.h"
#include "camera.h"
#include "text.h"

void delay(int);

class SimpleRenderArea : public QWidget
{
    Q_OBJECT

public:

    QImage *sheet;
    SimpleRenderArea(int x, int y, QWidget *parent = 0);
    ~SimpleRenderArea();
    //virtual void paintEvent(QPaintEvent* event);

public slots:

protected:

    virtual void paintEvent(QPaintEvent *);

private:

};

enum SimType {NO_FILE,PLAIN,HDF5,HDF5DAHLIA};

class FileList: public QStringList {

public:

    FileList(QStringList l):QStringList(l) {}
    FileList():QStringList() {}
    QString operator[](int);

};

class FileTypeList: public std::vector<SimType> {

public:

    FileTypeList(std::vector<SimType> l):std::vector<SimType>(l) {}
    FileTypeList():std::vector<SimType>() {}
    SimType operator[](int);

};

class Movie {

public:

    QString *defaultDir;

    unsigned int length;
    double fps;
    FileList fileNames;
    FileTypeList ftVector;
    std::vector<Camera> cameraRule;
    std::vector<int> framesRule;
    std::vector<std::vector<Text> > textRule;
    std::vector<ColorScale> colorScaleRule;

    void removeSim(int);
    void removeFrame(int);

    Movie():length(0),fps(25.0) {}

    void clearAll();

};


class MovieRule: public QDialog {

    Q_OBJECT

    bool *checkIfAccepted;

    Movie* m;
    int fpsVal;
    double timeVal;
    int framesVal;

    void updateFields();

public:

    MovieRule(Movie*, bool*, QWidget *parent = 0);
    ~MovieRule();

    QLabel *frameLabel;
    QLabel *timeLabel;
    QLabel *fpsLabel;

    QDoubleSpinBox *time;
    QSpinBox *fps;
    QSpinBox *frames;

    QPushButton *ok;
    QPushButton *cancel;

    QGridLayout * mainLayout;

public slots:

    void timeChanged(double);
    void framesChanged(int);
    void fpsChanged(int);
    virtual void accept();
    virtual void reject();

};

class FrameRule: public QDialog {

    Q_OBJECT

    bool *checkIfAccepted;

    Movie* m;
    double startTimeVal;
    double endTimeVal;
    int startFrameVal;
    int endFrameVal;
    int startPicVal;
    int endPicVal;
    double maxTimeVal;
    int maxFrameVal;
    int maxPicVal;

    void updateFields();
    void suspendSignals(bool);

public:

    FrameRule(Movie*, bool*, int, QWidget *parent = 0);
    ~FrameRule();

    QLabel *startTimeLabel;
    QLabel *endTimeLabel;
    QLabel *startFrameLabel;
    QLabel *endFrameLabel;
    QLabel *startPicLabel;
    QLabel *endPicLabel;

    QDoubleSpinBox *startTime;
    QDoubleSpinBox *endTime;
    QSpinBox *startFrame;
    QSpinBox *endFrame;
    QSpinBox *startPic;
    QSpinBox *endPic;

    QPushButton *reverse;

    QPushButton *ok;
    QPushButton *cancel;

    QGridLayout * mainLayout;

public slots:

    void startTimeChanged(double);
    void endTimeChanged(double);
    void startFrameChanged(int);
    void endFrameChanged(int);
    void reverseOrder();
    virtual void accept();
    virtual void reject();

};

class TextRule: public QDialog {

    Q_OBJECT

    bool *checkIfAccepted;

    Movie* m;
    double startTimeVal;
    double endTimeVal;
    int startFrameVal;
    int endFrameVal;
    double maxTimeVal;
    int maxFrameVal;
    QString text;
    QFont font;
    QColor color;
    QString *defaultDir;

    std::vector<std::vector<Text> > tmpTextRule;

    QString processString(QString,QString);

    void updateFields();
    void suspendSignals(bool);

public:

    TextRule(Movie*, bool*, int, QString *, QWidget *parent = 0);
    ~TextRule();

    QLabel *startTimeLabel;
    QLabel *endTimeLabel;
    QLabel *startFrameLabel;
    QLabel *endFrameLabel;
    QLabel *textLabel;
    QLabel *positionLabel;
    QLabel *colorLabel;
    QLabel *fontLabel;

    QDoubleSpinBox *startTime;
    QDoubleSpinBox *endTime;
    QSpinBox *startFrame;
    QSpinBox *endFrame;
    QSpinBox *red;
    QSpinBox *green;
    QSpinBox *blue;
    QDoubleSpinBox *xPos;
    QDoubleSpinBox *yPos;
    QPlainTextEdit *textEdit;
    QPushButton *clearAllText;
    QPushButton *saveText;
    QPushButton *loadText;

    QPushButton *ok;
    QPushButton *cancel;

    QGridLayout *mainLayout;

public slots:

    void startTimeChanged(double);
    void endTimeChanged(double);
    void startFrameChanged(int);
    void endFrameChanged(int);
    void saveTextSlot();
    void loadTextSlot();

    void deleteText();

    virtual void accept();
    virtual void reject();

};

class DataCamera {
public:

    bool univRotCheck;
    bool camRotCheck;
    bool camTransCheck;
    bool fixedPointCheck;
    double xAxisCoordUniv;
    double yAxisCoordUniv;
    double zAxisCoordUniv;
    double rotUniv;
    double xCenterRotUniv;
    double yCenterRotUniv;
    double zCenterRotUniv;
    double rAxisCoordCam;
    double pAxisCoordCam;
    double yAxisCoordCam;
    double rotCam;
    double xPosFromCam;
    double yPosFromCam;
    double zPosFromCam;
    double xPosToCam;
    double yPosToCam;
    double zPosToCam;
    double zoomFromCam;
    double zoomToCam;
    double xPosFixedPoint;
    double yPosFixedPoint;
    double zPosFixedPoint;
    double startTime;
    double endTime;
    int startFrame;
    int endFrame;

    DataCamera():
        univRotCheck(0),
        camRotCheck(0),
        camTransCheck(0),
        fixedPointCheck(0),
        xAxisCoordUniv(0),
        yAxisCoordUniv(0),
        zAxisCoordUniv(1.0),
        rotUniv(0),
        xCenterRotUniv(0.5),
        yCenterRotUniv(0.5),
        zCenterRotUniv(0.5),
        rAxisCoordCam(0),
        pAxisCoordCam(0),
        yAxisCoordCam(1.0),
        rotCam(0),
        xPosFromCam(0),
        yPosFromCam(0),
        zPosFromCam(0),
        xPosToCam(0),
        yPosToCam(0),
        zPosToCam(0),
        zoomFromCam(1.0),
        zoomToCam(1.0),
        xPosFixedPoint(0.5),
        yPosFixedPoint(0.5),
        zPosFixedPoint(0.5),
        startTime(0),
        endTime(0),
        startFrame(0),
        endFrame(0) {}

};

class CameraRule: public QDialog {

    Q_OBJECT

    bool *checkIfAccepted;

    Movie* m;
    double startTimeVal;
    double endTimeVal;
    int startFrameVal;
    int endFrameVal;
    double maxTimeVal;
    int maxFrameVal;
    int currentFrame;
    DataCamera *dataCam;

    std::vector<Camera> tmpCameraRule;

    void updateFields();
    void suspendSignals(bool);

public:

    CameraRule(Movie*, bool*, int, DataCamera* , QWidget *parent = 0);
    ~CameraRule();

    QCheckBox *univRotCheck;
    QCheckBox *camRotCheck;
    QCheckBox *camTransCheck;
    QCheckBox *fixedPointCheck;
    QDoubleSpinBox *xAxisCoordUniv;
    QDoubleSpinBox *yAxisCoordUniv;
    QDoubleSpinBox *zAxisCoordUniv;
    QPushButton *xAxisButtonUniv;
    QPushButton *yAxisButtonUniv;
    QPushButton *zAxisButtonUniv;
    QDoubleSpinBox *rotUniv;
    QDoubleSpinBox *xCenterRotUniv;
    QDoubleSpinBox *yCenterRotUniv;
    QDoubleSpinBox *zCenterRotUniv;
    QDoubleSpinBox *rAxisCoordCam;
    QDoubleSpinBox *pAxisCoordCam;
    QDoubleSpinBox *yAxisCoordCam;
    QDoubleSpinBox *rotCam;
    QDoubleSpinBox *xPosFromCam;
    QDoubleSpinBox *yPosFromCam;
    QDoubleSpinBox *zPosFromCam;
    QDoubleSpinBox *xPosToCam;
    QDoubleSpinBox *yPosToCam;
    QDoubleSpinBox *zPosToCam;
    QDoubleSpinBox *zoomFromCam;
    QDoubleSpinBox *zoomToCam;
    QDoubleSpinBox *xPosFixedPoint;
    QDoubleSpinBox *yPosFixedPoint;
    QDoubleSpinBox *zPosFixedPoint;
    QPushButton *centerButtonUniv;
    QPushButton *rAxisButtonCam;
    QPushButton *pAxisButtonCam;
    QPushButton *yAxisButtonCam;
    QPushButton *fromButtonCam;
    QPushButton *toButtonCam;
    QPushButton *fixedPointButton;

    QLabel *axisRotUnivLabel;
    QLabel *centerRotUnivLabel;
    QLabel *axisRotCamLabel;
    QLabel *fromCamLabel;
    QLabel *toCamLabel;
    QLabel *rotUnivLabel;
    QLabel *rotCamLabel;
    QLabel *zoomFromLabel;
    QLabel *zoomToLabel;

    QLabel *startTimeLabel;
    QLabel *endTimeLabel;
    QLabel *startFrameLabel;
    QLabel *endFrameLabel;

    QDoubleSpinBox *startTime;
    QDoubleSpinBox *endTime;
    QSpinBox *startFrame;
    QSpinBox *endFrame;

    QPushButton *ok;
    QPushButton *cancel;

    QGridLayout *mainLayout;

public slots:

    void startTimeChanged(double);
    void endTimeChanged(double);
    void startFrameChanged(int);
    void endFrameChanged(int);

    virtual void accept();
    virtual void reject();

    void univRotSlot(int);
    void camRotSlot(int);
    void camTransSlot(int);
    void fixedPointSlot(int);

    void centerUnivSlot();
    void xAxisUnivSlot();
    void yAxisUnivSlot();
    void zAxisUnivSlot();
    void rAxisCamSlot();
    void pAxisCamSlot();
    void yAxisCamSlot();
    void fromCamSlot();
    void toCamSlot();
    void fixedPoinSlot();

};

class AddFrameRule: public QDialog {

    Q_OBJECT

    bool *checkIfAccepted;

    Movie* m;
    double startTimeVal;
    double lengthTimeVal;
    int startFrameVal;
    int lengthFrameVal;
    double maxTimeVal;
    int maxFrameVal;
    int maxPicVal;

    void updateFields();
    void suspendSignals(bool);

public:

    AddFrameRule(Movie*, bool*, int, QWidget *parent = 0);
    ~AddFrameRule();

    QLabel *startTimeLabel;
    QLabel *lengthTimeLabel;
    QLabel *startFrameLabel;
    QLabel *lengthFrameLabel;

    QDoubleSpinBox *startTime;
    QDoubleSpinBox *lengthTime;
    QSpinBox *startFrame;
    QSpinBox *lengthFrame;

    QPushButton *ok;
    QPushButton *cancel;

    QGridLayout * mainLayout;

public slots:

    void startTimeChanged(double);
    void lengthTimeChanged(double);
    void startFrameChanged(int);
    void lengthFrameChanged(int);
    virtual void accept();
    virtual void reject();

};

class RemoveFrameRule: public QDialog {

    Q_OBJECT

    bool *checkIfAccepted;

    Movie* m;
    double startTimeVal;
    double endTimeVal;
    int startFrameVal;
    int endFrameVal;
    double maxTimeVal;
    int maxFrameVal;
    int maxPicVal;

    void updateFields();
    void suspendSignals(bool);

public:

    RemoveFrameRule(Movie*, bool*, int, QWidget *parent = 0);
    ~RemoveFrameRule();

    QLabel *startTimeLabel;
    QLabel *endTimeLabel;
    QLabel *startFrameLabel;
    QLabel *endFrameLabel;

    QDoubleSpinBox *startTime;
    QDoubleSpinBox *endTime;
    QSpinBox *startFrame;
    QSpinBox *endFrame;

    QPushButton *ok;
    QPushButton *cancel;

    QGridLayout * mainLayout;

public slots:

    void startTimeChanged(double);
    void endTimeChanged(double);
    void startFrameChanged(int);
    void endFrameChanged(int);
    virtual void accept();
    virtual void reject();

};







class MovieMaker: public QWidget {

    Q_OBJECT

public:

    SimpleRenderArea *sra;

    QString *defaultDir;

    QSlider *timeStampSL;

    QPushButton *prevTimeStamp;
    QPushButton *nextTimeStamp;

    QLabel *timeStampLabel;
    QDoubleSpinBox * timeStampSB;

    QLabel *frameLabel;
    QSpinBox *frameVal;

    QLabel *numTSLabel;
    QSpinBox *numTSVal;

    QPushButton *setMovieRule;
    QPushButton *setFrameRule;
    QPushButton *setCameraRule;
    QPushButton *setPaletteRule;
    QPushButton *setTextRule;
    QPushButton *addFrames;
    QPushButton *removeFrames;

    QGridLayout *mainLayout;

    DataCamera* cameraMem;

    MovieMaker(Movie *m, QString *, QWidget *parent);
    ~MovieMaker();
    void updateMovie(Movie *m);
    int currentTS;

private:

    double timeSpan;
    Movie *currentMovie;
    void suspendSignals(bool val);

protected:

    virtual void paintEvent(QPaintEvent *);

private slots:

    void nextTS();
    void prevTS();
    void changeTS(double);
    void reinitialize();
    void newFrame(int);
    void setMovie();
    void setFrame();
    void setCamera();
    void setText();
    void addFramesSlot();
    void removeFramesSlot();
    //void setPalette();

public slots:

    void changeTS(int);

signals:

    void newTS(int);

};

#endif // MOVIE_H

