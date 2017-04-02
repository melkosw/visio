
#include "render_universe.h"
#include <cmath>
#include <sstream>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QFileDialog>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QTextStream>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include "frame.h"
#include "rawhdata.h"

RenderUniverse::RenderUniverse(QString *dd, QWidget *parent):RenderArea(parent) {

    fileID=-1;
    ftVector.clear();
    minView = 1e10;
    maxView = -1e10;
    palette = ColorScale(0.0,1.0,0);
    pov=Camera(Frame(Coords(-0.65,-0.93,-1.34),Coords(0.44,0.55,0.71),Coords(-0.41,0.83,-0.39),Coords(-0.8,-0.12,0.59)),0.8);
    data=0;
    dataPreview=0;
    previewSize=64;
    previewBuilt=0;
    stop=false;
    defaultDir=dd;
    logData=false;

}

RenderUniverse::~RenderUniverse() {

    if (data) delete data;
    if (dataPreview) delete dataPreview;

}

void RenderUniverse::setLog(bool b) {

    logData=b;

}

void RenderUniverse::unlockMemoryData() {

    if (data) delete data;
    //if (dataPreview) delete dataPreview;
    data = 0;
    rawData=RawHData();
    //dataPreview = 0;
    fileID=-1;

}

void RenderUniverse::setText(std::vector<Text> t) {

    textToRender=t;

}

void RenderUniverse::setPalettes(ColorScale c) {

    minView = c.getMin();
    maxView = c.getMax();
    palette=c;

}

void RenderUniverse::setCamera(Camera c) {

    pov=c;

}

bool RenderUniverse::isDrawable() {

    return (fileID!=-1);

}

int RenderUniverse::makePreviews() {

    unlockMemoryData();

    if (!dataPreview) {
        dataPreview=new std::vector<std::vector<char> >(sourceFiles.size(),std::vector<char>(previewSize*previewSize*previewSize*4,0));
    } else {
        dataPreview->resize(sourceFiles.size(),std::vector<char>(previewSize*previewSize*previewSize*4,0));
    }

    emit progress("Building previews...");
    QCoreApplication::processEvents();

    for (int i=previewBuilt; i<sourceFiles.size(); i++) {

        std::cout << previewBuilt << " of " << sourceFiles.size()-1 << std::endl;
        std::cout << ftVector[i] << std::endl;

        if (ftVector[i]==PLAIN) {

            QFile file(sourceFiles[i]);
            if(!file.open(QIODevice::ReadOnly)) {
                QMessageBox::information(0,"error",file.errorString());
                return 1;
            }


            stop=false;

            unsigned long int fileSize = file.size();
            sideBox=(round(powl(fileSize/(4),0.3333333333L)));

            minView = palette.getMin();
            maxView = palette.getMax();

            if (stop) {
                unlockMemoryData();
                return 1;
            }

            char c[4];
            unsigned long int pos;
            double scale((1.0L*sideBox)/previewSize);
            for (unsigned int x=0; x<previewSize; x++) {
                for (unsigned int y=0; y<previewSize; y++) {
                    for (unsigned int z=0; z<previewSize; z++) {
                        pos=(z*scale+sideBox*y*scale+sideBox*sideBox*x*scale)*4;
                        file.seek(pos);
                        file.read(c,4);
                        for (unsigned int w=0; w<4; w++) {
                            ((*dataPreview)[i])[w+4*(z+y*previewSize+x*previewSize*previewSize)]=c[w];
                        }
                    }
                }
            }

            file.close();

        } else if (ftVector[i]==HDF5) {


            QStringList lf(h5ListOfFiles(sourceFiles[i]));
            std::vector<std::string> fileList;
            for (int j=0; j<lf.size(); j++) {

                std::cout << QString(lf[j]).toUtf8().constData() << std::endl;
                fileList.push_back(QString(lf[j]).toUtf8().constData());

            }

            RawHData dt;
            loadHDF5Emma(fileList,&dt);
            dt.makePreview(previewSize,(*dataPreview)[i]);
        } else if (ftVector[i]==HDF5DAHLIA) {


            QStringList lf(h5ListOfFiles(sourceFiles[i]));
            std::vector<std::string> fileList;
            for (int j=0; j<lf.size(); j++) {

                std::cout << QString(lf[j]).toUtf8().constData() << std::endl;
                fileList.push_back(QString(lf[j]).toUtf8().constData());

            }

            RawHData dt;
            loadHDF5Dahlia(fileList,&dt);
            dt.makePreview(previewSize,(*dataPreview)[i]);
        }

        emit progress(QString("Building preview: ")+QString::number(int((i+1.0)/sourceFiles.size()*100))+QString("%"));
        QCoreApplication::processEvents();
        previewBuilt++;
    }

    emit progress("Preview completed.");
    QCoreApplication::processEvents();

    return 0;

}


QStringList h5ListOfFiles(QString name) {

    QString positionFolder=QFileInfo(name).absolutePath();
    name=QFileInfo(name).fileName();
    int position=name.lastIndexOf('_');
    QString header(name.left(position));
    QString footer=name.right(name.length()-position);
    QString h2("grid_x"),h3("grid_y"),h4("grid_z"),h5("grid_l");
    QStringList l;
    l.push_back(positionFolder+QDir::separator()+header+footer);
    l.push_back(positionFolder+QDir::separator()+h2+footer);
    l.push_back(positionFolder+QDir::separator()+h3+footer);
    l.push_back(positionFolder+QDir::separator()+h4+footer);
    l.push_back(positionFolder+QDir::separator()+h5+footer);
    return l;
}

void RenderUniverse::removeSim(int index) {

    if (index==fileID) {
        fileID=-1;
        delete data;
        data=0;
    }

    ftVector.erase(ftVector.begin()+index);
    sourceFiles.erase(sourceFiles.begin()+index);
    dataPreview->erase(dataPreview->begin()+index);
    previewBuilt--;

}

int RenderUniverse::searchMinAndMax(int n, double& min, double& max) {

    if ((n<0)||(n>=sourceFiles.size())) return 1;
    if (ftVector[n]==PLAIN) {
        QFile file(sourceFiles[n]);
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(0,"error",file.errorString());
            return 1;
        }
        unsigned long int fileSize = file.size()/4;

        emit progress(QString("Searching min and max data in fixed grid file..."));
        QCoreApplication::processEvents();

        minView = palette.getMin();
        maxView = palette.getMax();

        unsigned long int numBuffers(1024);
        unsigned long int bufferSize=fileSize/numBuffers;
        char c[bufferSize];
        min=1e100;
        max=-1e100;

        for (unsigned int i=0; i<numBuffers; i++) {

            file.read(c,bufferSize);
            for (unsigned long int j=0; j<(bufferSize/4); j++) {
                double d=toFloat(&(c[j*4]));
                if (d<min) min=d;
                if (d>max) max=d;
            }

        }

        emit progress("Search completed.");
        QCoreApplication::processEvents();
        file.close();

        return 0;
    } else {

        RawHData tempRawData;

        emit progress(QString("Searching min and max data in HDF5 file..."));
        QCoreApplication::processEvents();

        QStringList lf(h5ListOfFiles(sourceFiles[n]));
        std::vector<std::string> fileList;
        for (int j=0; j<lf.size(); j++) {

            std::cout << QString(lf[j]).toUtf8().constData() << std::endl;
            fileList.push_back(QString(lf[j]).toUtf8().constData());

        }

        if (ftVector[n]==HDF5) {
            loadHDF5Emma(fileList,&tempRawData);
        }

        if (ftVector[n]==HDF5DAHLIA) {
            loadHDF5Dahlia(fileList,&tempRawData);
        }

        min=tempRawData.min();
        max=tempRawData.max();

        emit progress("Search completed.");
        QCoreApplication::processEvents();

        return 0;

    }

}

int RenderUniverse::loadData(int n) {

    if ((n<0)||(n>=sourceFiles.size())) return 1;
    fileID=n;
    if (ftVector[n]==PLAIN) {
        QFile file(sourceFiles[fileID]);
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(0,"error",file.errorString());
            return 1;
        }

        stop=false;

        //file.seek(0);

        unsigned long int fileSize = file.size();
        sideBox=(round(powl(fileSize/(4),0.3333333333L)));

        emit progress(QString("Loading data..."));
        QCoreApplication::processEvents();

        if (data) {
            delete data;
        }
        rawData=RawHData();

        minView = palette.getMin();
        maxView = palette.getMax();

        data=new std::vector<char>(fileSize,0);

        unsigned long int numBuffers(1024);
        unsigned long int bufferSize=fileSize/numBuffers;
        char c[bufferSize];

        for (unsigned int i=0; i<numBuffers; i++) {

            file.read(c,bufferSize);
            for (unsigned long int j=0; j<bufferSize; j++) {
                (*data)[j+i*bufferSize]=c[j];
            }
            if(stop) break;

        }

        emit progress("Loading completed.");
        QCoreApplication::processEvents();

        if (stop) {
            unlockMemoryData();
            return 1;
        }

        file.close();

        return 0;
    } else {

        if (data) {
            delete data;
            data=0;
        }
        rawData=RawHData();

        QStringList lf(h5ListOfFiles(sourceFiles[fileID]));
        std::vector<std::string> fileList;
        for (int j=0; j<lf.size(); j++) {

            std::cout << QString(lf[j]).toUtf8().constData() << std::endl;
            fileList.push_back(QString(lf[j]).toUtf8().constData());

        }

        if (ftVector[n]==HDF5) {
            loadHDF5Emma(fileList,&rawData);
        }

        if (ftVector[n]==HDF5DAHLIA) {
            loadHDF5Dahlia(fileList,&rawData);
        }


        return 0;

    }
}

double RenderUniverse::toFloat(char *c) {

    union Conversion{
        char ch[4];
        float fl;
    };
    Conversion conv;
    conv.ch[0]=c[0];
    conv.ch[1]=c[1];
    conv.ch[2]=c[2];
    conv.ch[3]=c[3];
    return conv.fl;

}

void straightLineHDF5(DataTransfer d) {

    Coords direction,rotAxis;
    double ds;
    int xx= d.xx;
    int centerX= d.centerX;
    int centerY= d.centerY;
    int dim= d.dim;
    Camera pov= d.pov;
    double side= d.side;
    Coords lastCheckPoint= d.lastCheckPoint;
    double length= 10;
    QImage* sheet= d.sheet;
    RenderUniverse *ru= d.ru;
    std::vector<DataPtr> intersection;
    std::vector<double> through;
    Cube universe(Coords(0,0,0),1.0);
    DataPtr id;
    Coords startingPoint=pov.position.orig();
    Cube actualCube,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26;
    Matrix axis(pov.position.base());
    double resPixel(pov.angle/dim);

    for (int yy=0; yy<dim; yy++) {

        if (d.perspectiveMode==1) {
            ds=sqrtl((yy-dim/2)*(yy-dim/2)*1.0+(xx-dim/2)*(xx-dim/2)*1.0)/dim;
            rotAxis=1.0*(xx-dim/2)*axis.row2-1.0*(yy-dim/2)*axis.row3;
            direction=(axis.row1.rotate(pov.angle*ds,-rotAxis.unit(),Coords(0.0,0.0,0.0)));
        } else {
            direction=(axis.row1+(xx-dim/2)*resPixel*axis.row3+(yy-dim/2)*resPixel*axis.row2);
        }

        startingPoint=pov.position.orig();
        intersection.clear();
        through.clear();

        bool check(intersectCube(startingPoint,direction,universe,side,actualCube,lastCheckPoint,length));

        while (check) {

            ru->rawData.infoPos(lastCheckPoint,id);

            //if (id==(ru->rawData.data.end())) continue;

            through.push_back(length);
            intersection.push_back(id);
            side=powl(0.5l,id->res);
            actualCube=Cube(Coords(id->x,id->y,id->z),side);
            startingPoint=lastCheckPoint;

            c1=actualCube;
            c2=actualCube;
            c3=actualCube;
            c4=actualCube;
            c5=actualCube;
            c6=actualCube;
            c1.origin=c1.origin+Coords(side,0.0,0.0);
            c2.origin=c2.origin+Coords(0.0,side,0.0);
            c3.origin=c3.origin+Coords(0.0,0.0,side);
            c4.origin=c4.origin+Coords(-side,0.0,0.0);
            c5.origin=c5.origin+Coords(0.0,-side,0.0);
            c6.origin=c6.origin+Coords(0.0,0.0,-side);
            if ((check=intersectCube(startingPoint,direction,c1,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            if ((check=intersectCube(startingPoint,direction,c2,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            if ((check=intersectCube(startingPoint,direction,c3,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            if ((check=intersectCube(startingPoint,direction,c4,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            if ((check=intersectCube(startingPoint,direction,c5,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            if ((check=intersectCube(startingPoint,direction,c6,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            c7=actualCube;
            c8=actualCube;
            c9=actualCube;
            c10=actualCube;
            c11=actualCube;
            c12=actualCube;
            c13=actualCube;
            c14=actualCube;
            c15=actualCube;
            c16=actualCube;
            c17=actualCube;
            c18=actualCube;
            c19=actualCube;
            c20=actualCube;
            c21=actualCube;
            c22=actualCube;
            c23=actualCube;
            c24=actualCube;
            c25=actualCube;
            c26=actualCube;
            c7.origin=c7.origin+  Coords(-side,0.0,side);
            c8.origin=c8.origin+  Coords(side,0.0,side);
            c9.origin=c9.origin+  Coords(side,0.0,-side);
            c10.origin=c10.origin+Coords(-side,0.0,-side);

            c11.origin=c11.origin+Coords(-side,side,-side);
            c12.origin=c12.origin+Coords(0.0,side,-side);
            c13.origin=c13.origin+Coords(side,side,-side);
            c14.origin=c14.origin+Coords(side,side,0.0);
            c15.origin=c15.origin+Coords(side,side,side);
            c16.origin=c16.origin+Coords(0.0,side,side);
            c17.origin=c17.origin+Coords(-side,side,side);
            c18.origin=c18.origin+Coords(-side,side,0.0);

            c19.origin=c19.origin+Coords(-side,-side,-side);
            c20.origin=c20.origin+Coords(0.0,-side,-side);
            c21.origin=c21.origin+Coords(side,-side,-side);
            c22.origin=c22.origin+Coords(side,-side,0.0);
            c23.origin=c23.origin+Coords(side,-side,side);
            c24.origin=c24.origin+Coords(0.0,-side,side);
            c25.origin=c25.origin+Coords(-side,-side,side);
            c26.origin=c26.origin+Coords(-side,-side,0.0);

            if ((check=intersectCube(startingPoint,direction,c7,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c8,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c9,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c10,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c11,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c12,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c13,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c14,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c15,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c16,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c17,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c18,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c19,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c20,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c21,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c22,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c23,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c24,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c25,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c26,side,actualCube,lastCheckPoint,length))) {
                continue;
            }


        }

        int count=intersection.size();
        if (count>0) {
            through.push_back(0);
            sheet->setPixel(xx+centerX-dim/2,yy+centerY-dim/2,ru->integrateValues(intersection, through));
        } else {
            sheet->setPixel(xx+centerX-dim/2,yy+centerY-dim/2,qRgb(0,0,0));
        }

    }

}

QString SaveStraightLineHDF5(Coords direction, Coords startingPoint, RawHData* rawData, double totalLength) {

    double side=1.0/16384;
    Coords lastCheckPoint;
    Coords sp(startingPoint);
    Coords dr(direction);
    double length= 10;
    std::vector<DataPtr> intersection;
    std::vector<double> through;
    Cube universe(Coords(0,0,0),1.0);
    DataPtr id;
    Cube actualCube,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26;

    intersection.clear();
    through.clear();

    bool check(intersectCube(startingPoint,direction,universe,side,actualCube,lastCheckPoint,length));

    while (totalLength>0) {

        if (lastCheckPoint.xPos>=1.0) lastCheckPoint.xPos-=1.0;
        if (lastCheckPoint.yPos>=1.0) lastCheckPoint.yPos-=1.0;
        if (lastCheckPoint.zPos>=1.0) lastCheckPoint.zPos-=1.0;
        if (lastCheckPoint.xPos<0.0) lastCheckPoint.xPos+=1.0;
        if (lastCheckPoint.yPos<0.0) lastCheckPoint.yPos+=1.0;
        if (lastCheckPoint.zPos<0.0) lastCheckPoint.zPos+=1.0;
        rawData->infoPos(lastCheckPoint,id);

        through.push_back(length);
        totalLength-=length;
        intersection.push_back(id);
        side=powl(0.5l,id->res);
        actualCube=Cube(Coords(id->x,id->y,id->z),side);
        startingPoint=lastCheckPoint;
        c1=actualCube;
        c2=actualCube;
        c3=actualCube;
        c4=actualCube;
        c5=actualCube;
        c6=actualCube;
        c1.origin=c1.origin+Coords(side,0.0,0.0);
        c2.origin=c2.origin+Coords(0.0,side,0.0);
        c3.origin=c3.origin+Coords(0.0,0.0,side);
        c4.origin=c4.origin+Coords(-side,0.0,0.0);
        c5.origin=c5.origin+Coords(0.0,-side,0.0);
        c6.origin=c6.origin+Coords(0.0,0.0,-side);
        if ((check=intersectCube(startingPoint,direction,c1,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }
        if ((check=intersectCube(startingPoint,direction,c2,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }
        if ((check=intersectCube(startingPoint,direction,c3,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }
        if ((check=intersectCube(startingPoint,direction,c4,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }
        if ((check=intersectCube(startingPoint,direction,c5,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }
        if ((check=intersectCube(startingPoint,direction,c6,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }
        c7=actualCube;
        c8=actualCube;
        c9=actualCube;
        c10=actualCube;
        c11=actualCube;
        c12=actualCube;
        c13=actualCube;
        c14=actualCube;
        c15=actualCube;
        c16=actualCube;
        c17=actualCube;
        c18=actualCube;
        c19=actualCube;
        c20=actualCube;
        c21=actualCube;
        c22=actualCube;
        c23=actualCube;
        c24=actualCube;
        c25=actualCube;
        c26=actualCube;
        c7.origin=c7.origin+  Coords(-side,0.0,side);
        c8.origin=c8.origin+  Coords(side,0.0,side);
        c9.origin=c9.origin+  Coords(side,0.0,-side);
        c10.origin=c10.origin+Coords(-side,0.0,-side);

        c11.origin=c11.origin+Coords(-side,side,-side);
        c12.origin=c12.origin+Coords(0.0,side,-side);
        c13.origin=c13.origin+Coords(side,side,-side);
        c14.origin=c14.origin+Coords(side,side,0.0);
        c15.origin=c15.origin+Coords(side,side,side);
        c16.origin=c16.origin+Coords(0.0,side,side);
        c17.origin=c17.origin+Coords(-side,side,side);
        c18.origin=c18.origin+Coords(-side,side,0.0);

        c19.origin=c19.origin+Coords(-side,-side,-side);
        c20.origin=c20.origin+Coords(0.0,-side,-side);
        c21.origin=c21.origin+Coords(side,-side,-side);
        c22.origin=c22.origin+Coords(side,-side,0.0);
        c23.origin=c23.origin+Coords(side,-side,side);
        c24.origin=c24.origin+Coords(0.0,-side,side);
        c25.origin=c25.origin+Coords(-side,-side,side);
        c26.origin=c26.origin+Coords(-side,-side,0.0);

        if ((check=intersectCube(startingPoint,direction,c7,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c8,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c9,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c10,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c11,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c12,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c13,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c14,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c15,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c16,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c17,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c18,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c19,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c20,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c21,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c22,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c23,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c24,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c25,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

        if ((check=intersectCube(startingPoint,direction,c26,side,actualCube,lastCheckPoint,length,false))) {
            continue;
        }

    }

    QString stg;
    QTextStream ts(&stg);

    rawData->infoPos(sp,id);
    ts.setRealNumberNotation(QTextStream::FixedNotation);
    ts.setRealNumberPrecision(8);
    ts << "\nStarting X\t      Y      \t      Z      \t     Val     \n" << sp.xPos << '\t' << sp.yPos << '\t' << sp.zPos << '\t' << id->val << '\n';
    ts << "Direction\n" << dr.xPos << '\t' << dr.yPos << '\t' << dr.zPos << '\n';
    ts << "N\t      X      \t      Y      \t      Z      \tLiv\t     Val     \tIntersection\t    Length   \n";

    int count=intersection.size();
    through.push_back(0);
    //std::cout << count << "   " << through.size() << std::endl;
    DataPtr dp;
    double ll(0.0);
    for (int i=0; i<count; i++) {
        dp=intersection[i];
        ll+=through[i];
        ts << i << '\t' << dp->x << '\t' << dp->y << '\t' << dp->z << '\t' << dp->res << '\t' << dp->val << '\t' << through[i] << '\t' << ll << '\n';
    }
    return stg;

}

QString saveStraightLine(Coords, Coords) {

    return "not yet implemented.\n";

}

void straightLine(DataTransfer d) {

    Coords direction,rotAxis;
    double ds;
    int xx= d.xx;
    int centerX= d.centerX;
    int centerY= d.centerY;
    int dim= d.dim;
    Camera pov= d.pov;
    double side= d.side;
    Coords lastCheckPoint= d.lastCheckPoint;
    double length= 10;
    unsigned int sideBoxToRender= d.sideBoxToRender;
    QImage* sheet= d.sheet;
    std::vector<char> *dataToRender= d.dataToRender;
    RenderUniverse *ru= d.ru;
    std::vector<IntCoords> intersection;
    std::vector<double> through;
    //Coords path;
    Cube universe(Coords(0,0,0),1.0);
    IntCoords cubeCoords;
    Coords startingPoint=pov.position.orig();
    Cube actualCube,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26;
    Matrix axis(pov.position.base());
    double resPixel(pov.angle/dim);

    for (int yy=0; yy<dim; yy++) {

        if (d.perspectiveMode==1) {
            ds=sqrtl((yy-dim/2)*(yy-dim/2)*1.0+(xx-dim/2)*(xx-dim/2)*1.0)/dim;
            rotAxis=1.0*(xx-dim/2)*axis.row2-1.0*(yy-dim/2)*axis.row3;
            direction=(axis.row1.rotate(pov.angle*ds,-rotAxis.unit(),Coords(0.0,0.0,0.0)));
        } else {
            direction=(axis.row1+(xx-dim/2)*resPixel*axis.row3+(yy-dim/2)*resPixel*axis.row2);
        }

        startingPoint=pov.position.orig();
        intersection.clear();
        through.clear();

        bool check(intersectCube(startingPoint,direction,universe,side,actualCube,lastCheckPoint,length));

        while(check) {
            cubeCoords.x=int(floor(actualCube.origin.xPos*sideBoxToRender)+0.1);
            cubeCoords.y=int(floor(actualCube.origin.yPos*sideBoxToRender)+0.1);
            cubeCoords.z=int(floor(actualCube.origin.zPos*sideBoxToRender)+0.1);
            through.push_back(length);
            intersection.push_back(cubeCoords);
            //path=lastCheckPoint-startingPoint;
            startingPoint=lastCheckPoint;
            c1=actualCube;
            c2=actualCube;
            c3=actualCube;
            c4=actualCube;
            c5=actualCube;
            c6=actualCube;
            c1.origin=c1.origin+Coords(side,0.0,0.0);
            c2.origin=c2.origin+Coords(0.0,side,0.0);
            c3.origin=c3.origin+Coords(0.0,0.0,side);
            c4.origin=c4.origin+Coords(-side,0.0,0.0);
            c5.origin=c5.origin+Coords(0.0,-side,0.0);
            c6.origin=c6.origin+Coords(0.0,0.0,-side);
            if ((check=intersectCube(startingPoint,direction,c1,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            if ((check=intersectCube(startingPoint,direction,c2,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            if ((check=intersectCube(startingPoint,direction,c3,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            if ((check=intersectCube(startingPoint,direction,c4,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            if ((check=intersectCube(startingPoint,direction,c5,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            if ((check=intersectCube(startingPoint,direction,c6,side,actualCube,lastCheckPoint,length))) {
                continue;
            }
            c7=actualCube;
            c8=actualCube;
            c9=actualCube;
            c10=actualCube;
            c11=actualCube;
            c12=actualCube;
            c13=actualCube;
            c14=actualCube;
            c15=actualCube;
            c16=actualCube;
            c17=actualCube;
            c18=actualCube;
            c19=actualCube;
            c20=actualCube;
            c21=actualCube;
            c22=actualCube;
            c23=actualCube;
            c24=actualCube;
            c25=actualCube;
            c26=actualCube;
            c7.origin=c7.origin+  Coords(-side,0.0,side);
            c8.origin=c8.origin+  Coords(side,0.0,side);
            c9.origin=c9.origin+  Coords(side,0.0,-side);
            c10.origin=c10.origin+Coords(-side,0.0,-side);

            c11.origin=c11.origin+Coords(-side,side,-side);
            c12.origin=c12.origin+Coords(0.0,side,-side);
            c13.origin=c13.origin+Coords(side,side,-side);
            c14.origin=c14.origin+Coords(side,side,0.0);
            c15.origin=c15.origin+Coords(side,side,side);
            c16.origin=c16.origin+Coords(0.0,side,side);
            c17.origin=c17.origin+Coords(-side,side,side);
            c18.origin=c18.origin+Coords(-side,side,0.0);

            c19.origin=c19.origin+Coords(-side,-side,-side);
            c20.origin=c20.origin+Coords(0.0,-side,-side);
            c21.origin=c21.origin+Coords(side,-side,-side);
            c22.origin=c22.origin+Coords(side,-side,0.0);
            c23.origin=c23.origin+Coords(side,-side,side);
            c24.origin=c24.origin+Coords(0.0,-side,side);
            c25.origin=c25.origin+Coords(-side,-side,side);
            c26.origin=c26.origin+Coords(-side,-side,0.0);

            if ((check=intersectCube(startingPoint,direction,c7,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c8,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c9,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c10,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c11,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c12,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c13,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c14,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c15,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c16,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c17,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c18,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c19,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c20,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c21,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c22,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c23,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c24,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c25,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

            if ((check=intersectCube(startingPoint,direction,c26,side,actualCube,lastCheckPoint,length))) {
                continue;
            }

        }

        int count=intersection.size();
        if (count>0) {
            through.push_back(0);
            sheet->setPixel(xx+centerX-dim/2,yy+centerY-dim/2,ru->integrate(intersection, through, dataToRender, sideBoxToRender));
        } else {
            sheet->setPixel(xx+centerX-dim/2,yy+centerY-dim/2,qRgb(0,0,0));
        }

    }

    //if (stop) break;

}

void RenderUniverse::draw(int perspectiveMode, int frame, bool preview) {

    std::vector<char>* dataToRender;
    stop=false;
    unsigned int sideBoxToRender;
    if (frame<0) {
        frame=0;
    }

    if (preview) {
        if (!dataPreview) {
            background(perspectiveMode);
            return;
        }
        if (!dataPreview->size()) {
            background(perspectiveMode);
            return;
        }
        if (int(dataPreview->size())<=frame) {
            background(perspectiveMode);
            return;
        }
        dataToRender=&((*dataPreview)[frame]);
        sideBoxToRender=previewSize;
    } else {
        if (frame!=fileID) loadData(frame);
        if (ftVector[frame]==PLAIN) {
            dataToRender=data;
            sideBoxToRender=sideBox;
        } else {
            dataToRender=0;
            sideBoxToRender=16384; // adjust
        }
    }

    int x(width()),y(height());
    int centerX(x/2),centerY(y/2);
    int dim(x<y?x:y);
    int in_x=centerX-dim/2;
    int in_y=centerY-dim/2;
    QRgb color=qRgb(0,0,0);
    for (int xx=0; xx<x; xx++) {
        for (int yy=0; yy<y; yy++) {
            if ((xx<in_x)||(yy<in_y)||(xx>in_x+dim)||(yy>in_y+dim)) {
                color=qRgb(64,64,64);
                sheet->setPixel(xx,yy,color);
            }
            else {
                color=qRgb(0,0,0);
            }
        }
    }


    if ((ftVector[fileID]==PLAIN)&&(!dataToRender)) return;
    if ((ftVector[fileID]==HDF5)&&(!rawData.data.size())) return;

    Coords lastCheckPoint;
    double side(1.0/sideBoxToRender);
    DataTransfer d;
    QFuture<void> f1[2048];

    for (int xx=0; xx<dim; xx++) {

        d.xx= xx;
        d.centerX= centerX;
        d.centerY= centerY;
        d.dim= dim;
        d.pov= pov;
        d.side= side;
        d.lastCheckPoint= lastCheckPoint;
        d.perspectiveMode= perspectiveMode;
        d.sideBoxToRender= sideBoxToRender;
        d.sheet= sheet;
        d.dataToRender= dataToRender;
        d.ru= this;
        if ((!preview)&&((ftVector[fileID]==HDF5)||(ftVector[fileID]==HDF5DAHLIA))) {

            f1[xx] = QtConcurrent::run(straightLineHDF5, d);

        } else {

            f1[xx] = QtConcurrent::run(straightLine, d);

        }
    }


    emit progress("Rendering...");
    QCoreApplication::processEvents();

    if (stop) {
        unlockMemoryData();
        return;
    }

    for (int i=0; i<dim; i++) {
        while(!f1[i].isFinished()) {};
    }

    int xp,yp;
    QPainter p;
    p.begin(sheet);
    QString str;

    for (std::vector<Text>::iterator i=textToRender.begin(); i!=textToRender.end(); i++) {
        p.setPen(QPen(i->color));
        p.setFont(i->font);
        xp=int(dim*(i->relPosX));
        yp=int(dim*(i->relPosY));
        str=i->textString;
        if (str.left(11)==QString("\\colorscale")) {
            QStringList par=str.mid(11).simplified().split(QRegExp(" "));
            double xSpace(0.8),ySpace(0.05),zero(0.0),tick(20);
            if (par.length()>0) {
                if (par[0].toDouble()>0) xSpace=par[0].toDouble();
            }
            if (par.length()>1) ySpace=par[1].toDouble();
            if (par.length()>2) zero=par[2].toDouble();
            if (par.length()>3) tick=par[3].toDouble();

            plotColorScale(&p,xp+in_x,yp+in_y,int(xSpace*dim),int(ySpace*dim),zero,tick);

        } else {
            QTextDocument td;
            td.setHtml(i->textString);
            p.save();
            p.translate(xp+in_x,yp+in_y);
            QAbstractTextDocumentLayout::PaintContext ctx;

            ctx.palette.setColor(QPalette::Text, p.pen().color());

            td.documentLayout()->draw(&p, ctx);
            p.restore();
        }
    }

    p.end();
    emit progress("Rendering completed.");
    QCoreApplication::processEvents();

    update();

}

void RenderUniverse::plotColorScale(QPainter *p, int x, int y, int w, int h, double z, double t) {

    QColor opaque;

    z=z-(floor(z/t)*t);
    double startMin=floor(minView/t)*t,val;
    if (z+startMin<minView) z+=t;
    z+=startMin;
    int n=0,pos;
    while (z+n*t<=maxView) n++;

    if (w>h) {  //horizontal

        for (int i=0; i<w; i++) {

            opaque=palette.getColor((1.0*i)/w);
            opaque.setAlpha(255);
            p->setPen(opaque);
            p->drawLine(i+x,y,i+x,y+h/2);

        }

        p->fillRect(x,y+h/2+1,w,h/2,Qt::black);
        p->setPen(QPen(Qt::white));

        for (int i=0; i<n; i++) {

            std::stringstream ss;
            val=z+i*t;
            ss << val;
            pos=x+w*(val-minView)/(maxView-minView);
            p->drawLine(pos,y+h/4,pos,y+3*h/4);
            p->drawText(QRect(pos,y+3*h/4,width(),height()),Qt::AlignLeft,ss.str().c_str());

        }

    } else { //vertical

        for (int i=0; i<h; i++) {

            opaque=palette.getColor(1.0-(1.0*i)/h);
            opaque.setAlpha(255);
            p->setPen(opaque);
            p->drawLine(x,i+y,x+w/2,i+y);

        }

        p->fillRect(x+w/2+1,y,w/2,h,Qt::black);

        p->setPen(QPen(Qt::white));

        for (int i=0; i<n; i++) {

            std::stringstream ss;
            val=z+i*t;
            ss << val;
            pos=y+h-h*(val-minView)/(maxView-minView);
            p->drawLine(x+w/4,pos,x+w*3/4,pos);
            p->drawText(QRect(x+w,pos-11,width(),height()),Qt::AlignLeft,ss.str().c_str());

        }

    }

}

void RenderUniverse::background(int perspectiveMode) {

    int x(width()),y(height());
    int centerX(x/2),centerY(y/2);
    int dim(x<y?x:y);
    int in_x=centerX-dim/2;
    int in_y=centerY-dim/2;
    QRgb color=qRgb(0,0,0);
    for (int xx=0; xx<x; xx++) {
        for (int yy=0; yy<y; yy++) {
            if ((xx<in_x)||(yy<in_y)||(xx>in_x+dim)||(yy>in_y+dim))
                color=qRgb(64,64,64);
            else
                color=qRgb(0,0,0);
            sheet->setPixel(xx,yy,color);
        }
    }

    Matrix axis(pov.position.base());
    Coords startingPoint(pov.position.orig());

    Coords lastCheckPoint;
    Coords diff;
    int r,g,b,dist;
    Cube universe(Coords(0,0,0),1.0),actualCube;
    //double side(1.0/sideBox),axisWd(0.003);
    double side(1.0/16),axisWd(0.003);
    double ds;
    double resPixel(pov.angle/dim);
    double length;
    Coords rotAxis,direction;

    for (int xx=0; xx<dim; xx++) {

        for (int yy=0; yy<dim; yy++) {

            if (((fabsl(xx-dim/2)<10)&&(fabsl(yy-dim/2)<2))||((fabsl(xx-dim/2)<2)&&(fabsl(yy-dim/2)<10))) {
                if ((fabsl(yy-dim/2)>2)||(fabsl(xx-dim/2)>2)) {
                    sheet->setPixel(xx+centerX-dim/2,yy+centerY-dim/2,qRgb(0,255,255));
                    continue;
                }
            }

            if (perspectiveMode==1) {
                ds=sqrtl((yy-dim/2)*(yy-dim/2)*1.0+(xx-dim/2)*(xx-dim/2)*1.0)/dim;
                rotAxis=1.0*(xx-dim/2)*axis.row2-1.0*(yy-dim/2)*axis.row3;
                Coords p(0.5,0.4,0.5);
                direction=(axis.row1.rotate(pov.angle*ds,-rotAxis.unit(),Coords(0.0,0.0,0.0)));
            } else {
                direction=(axis.row1+(xx-dim/2)*resPixel*axis.row3+(yy-dim/2)*resPixel*axis.row2);
            }
            if (intersectCube(startingPoint,direction,universe,side,actualCube,lastCheckPoint,length)) {
                diff=lastCheckPoint-startingPoint;
                dist=(255-diff.sqnorm()*10);
                if (dist<20) dist=20;
                if (dist>200) dist=200;
                r=g=b=dist;
                if ((lastCheckPoint.xPos-floorl(lastCheckPoint.xPos*10)/10<3*axisWd)&&
                    (lastCheckPoint.yPos-floorl(lastCheckPoint.yPos*10)/10<3*axisWd)&&
                    (lastCheckPoint.zPos-floorl(lastCheckPoint.zPos*10)/10<3*axisWd)) {
                    r=255;
                    g=0;
                    b=255;
                }
                if ((lastCheckPoint.xPos<axisWd)&&(lastCheckPoint.yPos<axisWd)) {
                    r=255;
                    g=0;
                    b=0;
                }
                if ((lastCheckPoint.yPos<axisWd)&&(lastCheckPoint.zPos<axisWd)) {
                    r=0;
                    g=255;
                    b=0;
                }
                if ((lastCheckPoint.zPos<axisWd)&&(lastCheckPoint.xPos<axisWd)) {
                    r=0;
                    g=0;
                    b=255;
                }
                if ((lastCheckPoint.xPos<axisWd)&&(lastCheckPoint.yPos<axisWd)&&(lastCheckPoint.zPos<axisWd)) {
                    r=255;
                    g=255;
                    b=255;
                }
                sheet->setPixel(xx+centerX-dim/2,yy+centerY-dim/2,qRgb(r,g,b));
            }
        }
    }

    update();
}

void RenderUniverse::paintEvent(QPaintEvent *) {

    QPainter painter(this);
    painter.drawImage(QPoint(0,0),*sheet);


}

void RenderUniverse::saveFrame(QString file) {

    if (file.isEmpty()) file=QFileDialog::getSaveFileName(this, tr("Save Frame"),
                                                          *defaultDir,
                                                          tr("Lossless bitmap (*.bmp);; JPeg (*.jpg)"),
                                                          0,
                                                          QFileDialog::DontUseNativeDialog);

    if (file.isEmpty()) return;

    *defaultDir=QFileInfo(file).absolutePath();

    sheet->save(file);

}


QRgb RenderUniverse::integrateValues(const std::vector<DataPtr> &c, const std::vector<double> &through) {
    int n(c.size());
    double opacity;
    double r(0),g(0),b(0),weight(0),relWeight,weightThrough,d;
    QColor color;
    for (int i=0; i<n; i++) {
        weightThrough=through[i+1]*128;
        if (minView<maxView) {
            d=c[i]->val;
            if (logData) {
                if (d>0)
                    d=log10l(d);
                else
                    d=1e-10;
            }
            color=(palette.getColor(((d-minView)/(maxView-minView))));
        } else {
            color=palette.getColor(0.5);
        }
        opacity=(color.alpha()/255.0)*weightThrough;
        if (opacity>1) opacity=1;
        relWeight=(1.0-weight)*opacity;
        r=r+color.red()*relWeight;
        g=g+color.green()*relWeight;
        b=b+color.blue()*relWeight;
        weight=weight+relWeight;
    }
    return qRgb(r,g,b);

}


QRgb RenderUniverse::integrate(const std::vector<IntCoords> &c, const std::vector<double> &through, std::vector<char>* dataToIntegrate, long int sideBoxToIntegrate) {

    int n(c.size());
    double d,opacity;
    quint64 index;
    double r(0),g(0),b(0),weight(0),relWeight,weightThrough;
    QColor color;
    for (int i=0; i<n; i++) {

        weightThrough=through[i+1]*128;
        index=c[i].x*sideBoxToIntegrate*sideBoxToIntegrate*4+c[i].y*sideBoxToIntegrate*4+c[i].z*4;
        if (index>(dataToIntegrate)->size()) {
            color=QColor(0,0,0,0);
        } else {

            if (minView<maxView) {
                d=toFloat(&(*dataToIntegrate)[index]);
                if (logData) {
                    if (d>0)
                        d=log10l(d);
                    else
                        d=1e-10;
                }
                color=(palette.getColor(((d-minView)/(maxView-minView))));
            } else {
                color=palette.getColor(0.5);
            }
        }
        opacity=(color.alpha()/255.0)*weightThrough;
        if (opacity>1) opacity=1;
        relWeight=(1.0-weight)*opacity;
        r=r+color.red()*relWeight;
        g=g+color.green()*relWeight;
        b=b+color.blue()*relWeight;
        weight=weight+relWeight;
    }
    return qRgb(r,g,b);


    /*
    int dataToIntegratePerPixel=1;
    for (int i=0; i<detail; i++) dataToIntegratePerPixel*=2;
    int greyScale=(c.size()*dataToIntegratePerPixel)/16+64;
    if (greyScale>255) greyScale=255;
    return qRgb(greyScale,greyScale,greyScale);
    */

}

