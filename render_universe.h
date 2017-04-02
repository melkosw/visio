#ifndef RENDER_UNIVERSE_H
#define RENDER_UNIVERSE_H

#include <QString>
#include <QRgb>
#include <vector>
#include <utility>
#include "camera.h"
#include "movie.h"
#include "text.h"
#include "render_area.h"
#include "color_selector.h"
#include "rawhdata.h"

QStringList h5ListOfFiles(QString);

QString SaveStraightLineHDF5(Coords, Coords, RawHData*, double);

class RenderUniverse;

struct DataTransfer {
    int xx;
    int centerX;
    int centerY;
    int dim;
    Coords direction;
    Camera pov;
    double side;
    Coords lastCheckPoint;
    unsigned int sideBoxToRender;
    QImage* sheet;
    std::vector<char>* dataToRender;
    RenderUniverse *ru;
    int perspectiveMode;
};

void straightLine(DataTransfer);
void straightLineHDF5(DataTransfer);

class RenderUniverse : public RenderArea {

    Q_OBJECT

public:

    RenderUniverse(QString *, QWidget *parent = 0);
    ~RenderUniverse();
    void setPalettes(ColorScale);
    void setCamera(Camera);
    void setText(std::vector<Text>);
    Camera getCamera() {return pov;}
    bool isDrawable();
    int loadData(int fileID);
    int makePreviews();
    void background(int);
    void draw(int, int, bool);
    double getMin() {return minView;}
    double getMax() {return maxView;}
    int searchMinAndMax(int, double &, double &);
    int getFrame() {return fileID;}
    void removeSim(int);
    ColorScale palette;
    void unlockMemoryData();
    void saveFrame(QString);
    bool stop;
    FileList sourceFiles;
    FileTypeList ftVector;
    int previewBuilt;
    QRgb integrate(const std::vector<IntCoords> &, const std::vector<double> &, std::vector<char>*, long int);
    QRgb integrateValues(const std::vector<DataPtr> &, const std::vector<double> &);
    RawHData rawData;
    void setLog(bool);

signals:

    void progress(QString);

protected:
    virtual void paintEvent(QPaintEvent *);

private:

    bool logData;
    QString *defaultDir;
    double minView,maxView;
    long int sideBox;
    std::vector<char> *data;
    std::vector<std::vector<char> > *dataPreview;
    long int previewSize;
    int fileID;
    Camera pov;
    std::vector<Text> textToRender;
    double toFloat(char *);
    void plotColorScale(QPainter *, int, int, int, int, double, double);

};

#endif // RENDER_UNIVERSE_H
