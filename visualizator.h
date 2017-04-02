#ifndef VISUALIZATOR_H
#define VISUALIZATOR_H

#include <vector>
#include <QString>
#include <QDialog>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QDir>
#include <QColor>
#include <QCheckBox>
#include "color_selector.h"
#include "camera.h"
#include "movie.h"
#include "render_universe.h"

#define FILEBUFFER 16777216

extern const int visioVersion;
extern const int visioSubversion;


class Resolution : public QDialog {

    Q_OBJECT

public:

    explicit Resolution(int *, int *, QWidget *parent = 0);
    ~Resolution();

private:

    QSpinBox *heightSpinBox;
    QSpinBox *widthSpinBox;
    QPushButton *acceptButton;
    QPushButton *cancelButton;
    QGridLayout *mainLayout;
    QLabel *xLabel;
    QLabel *wLabel;
    QLabel *hLabel;

    int *wid;
    int *hei;

public slots:

    virtual void accept();

};

class Visualizator;

class SourceFileRemover : public QDialog {
    Q_OBJECT

    int numFiles;
    Movie *currentMovie;
    RenderUniverse * currentRU;
    Visualizator *currentV;

public:

    explicit SourceFileRemover(Movie* m, RenderUniverse *ru, Visualizator *parent = 0);
    ~SourceFileRemover();

    QLabel *title;

    QListWidget *sa;

    QPushButton *ok;
    QPushButton *cancel;
    QPushButton *all;

    QGridLayout *mainLayout;


public slots:

    virtual void accept();
    virtual void reject();
    void toggleAll();

};

class Visualizator : public QWidget
{
    Q_OBJECT

public:
    explicit Visualizator(bool gui=false, QWidget *parent = 0);
    ~Visualizator();

    virtual void resizeEvent(QResizeEvent *) ;

    virtual QSize minimumSizeHint() const ;
    virtual QSize sizeHint() const ;
    void removeSim(int);

protected:


private:

    QDir exeDir;
    QString ffmpegPath;
    QString defaultDir;

    void checkFile();

    bool leftButton;
    bool rightButton;

    int leftPosX;
    int leftPosY;
    int rightPosX;
    int rightPosY;
    int bothPosX;
    int bothPosY;
    int perspectiveMode;
    int numberOfFrames;
    int currentFrame;
    int hRes;
    int vRes;

    bool fileChosen;
    FileList sourceFiles;
    QString currentLoadedFile;
    long int sourceSize;
    long int sourceSide;

    Camera viewCamera;

    QGridLayout *mainLayout;

    RenderUniverse *viewRA;
    RenderUniverse *hiddenRender;

    QPushButton *stopButton;
    QPushButton *exitButton;
    QPushButton *cameraButton;
    QPushButton *colorButton;
    QPushButton *removeFileButton;
    QPushButton *fileButton;
    QPushButton *fileh5Button;
    QPushButton *fileh5DahliaButton;
    QPushButton *viewButton;
    QPushButton *movieButton;
    QPushButton *resolutionButton;
    QPushButton *saveFrameButton;
    QPushButton *saveMovieButton;
    QPushButton *loadMovConf;
    QPushButton *saveMovConf;
    QPushButton *loadMovSims;
    QPushButton *saveMovSims;
    QPushButton *aboutButton;

    QCheckBox *perspectiveCorrection;
    QCheckBox *logarithmicData;

    QLabel *titleLabel;
    QLabel *fileTypeLabel;
    QLabel *operationLabel;
    QLabel *frameLabel;

    //ColorSelector *paletteSelector;

    Resolution *resolutionQuery;

    CameraWidget *cameraWG;

    MovieMaker *movieWG;

    Movie currentMovie;

    ColorScale cs;

public slots:

    void viewComplete();
    void viewPreview();
    void renderNew();
    void selectFiles();
    void selecth5Files();
    void selecth5DahliaFiles();
    void removeFiles();
    void selectPalette();
    void breakRender();
    void progress(QString);
    void registerLeftButton();
    void registerRightButton();
    void releaseLeftButton();
    void releaseRightButton();
    void mouseMove();
    void wheelMouse(int);
    void changePerspectiveMode(bool);
    void changeLogData(bool);
    void changeFrame(int);
    void movie();
    void saveFrame(QString = "", unsigned int = 0);
    void saveMovie(QString = "");
    void setResolution();
    void changeCamera(Camera);
    void acceptCS();
    void saveSims(QString = "");
    void saveConf(QString = "");
    void loadSims(QString = "");
    void loadConf(QString = "");
    void about();

};

#endif // VISUALIZATOR_H
