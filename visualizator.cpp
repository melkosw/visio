
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QSlider>
#include <QPainter>
#include <QProcess>
#include <QMessageBox>
#include <QTextStream>
#include "visualizator.h"
#include "hdf5.h"

Visualizator::Visualizator(bool gui, QWidget *parent) :
    QWidget(parent)
{

    exeDir=QDir(QCoreApplication::applicationDirPath());
    ffmpegPath=(exeDir.absoluteFilePath("fmpeg"));
    ffmpegPath="\""+ffmpegPath+"\"";
    std::cout << "Searching ffmpeg in :" << ffmpegPath.toLocal8Bit().constData() << std::endl;
    QProcess ffmpegVerify;
    ffmpegVerify.setProcessChannelMode(QProcess::MergedChannels);
    ffmpegVerify.start(ffmpegPath+" -version");
    ffmpegVerify.waitForFinished(10000);
    int exitcode = ffmpegVerify.exitCode();
    QString report=(ffmpegVerify.readAllStandardOutput());
    report+=(ffmpegVerify.readAllStandardError());
    if (exitcode) {
            exeDir=QDir(QCoreApplication::applicationDirPath());
            exeDir.cd("visio.app");
            exeDir.cd("Contents");
            exeDir.cd("MacOS");
            ffmpegPath=(exeDir.absoluteFilePath("ffmpeg"));
            ffmpegPath="\""+ffmpegPath+"\"";
            std::cout << "Searching ffmpeg in :" << ffmpegPath.toLocal8Bit().constData() << std::endl;
            QProcess ffmpegVerify2;
            ffmpegVerify2.setProcessChannelMode(QProcess::MergedChannels);
            ffmpegVerify2.start(ffmpegPath+" -version");
            ffmpegVerify2.waitForFinished(10000);
            exitcode = ffmpegVerify2.exitCode();
            QString report=(ffmpegVerify2.readAllStandardOutput());
            report+=(ffmpegVerify2.readAllStandardError());
    }
    if (exitcode) {
        if (report.isEmpty()) {
            report=tr("ffmpeg not found on this system.");
        }
        if (gui) {
        QMessageBox::warning(this,tr("ffmpeg not working"),tr("<b>Some problem occurs with ffmpeg.</b><br>However single frames will be saved in bmp format in \"tmp\" inside the working directory.<br>"
                                                              "Report :<br><pre>")+report+tr("</pre>"),QMessageBox::Ok);
        } else {
            std::cout << "Some problem occurs with ffmpeg.\n" << report.toLocal8Bit().constData() << std::endl;
        }
    }

    leftButton=false;
    rightButton=false;

    leftPosX=0;
    leftPosY=0;
    rightPosX=0;
    rightPosY=0;
    bothPosX=0;
    bothPosY=0;
    hRes=480;
    vRes=480;

    currentFrame=0;

    defaultDir=".";

    fileChosen = false;
    currentLoadedFile="none";
    currentMovie=Movie();

    exitButton = new QPushButton(tr("Exit"));
    stopButton = new QPushButton(tr("Stop"));
    viewRA = new RenderUniverse(&defaultDir);
    viewRA->setPalettes(cs);
    hiddenRender = new RenderUniverse(&defaultDir);
    fileTypeLabel = new QLabel(tr("---"));
    titleLabel = new QLabel(tr("No files selected yet"));
    perspectiveCorrection = new QCheckBox("Perspective correction");
    perspectiveCorrection->setChecked(false);
    logarithmicData = new QCheckBox("Logarithmic data");
    logarithmicData->setChecked(false);
    operationLabel = new QLabel("No computation yet");
    operationLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    titleLabel->setFont(QFont("Courier New, Courier New, monospace"));
    cameraButton = new QPushButton(tr("Set Camera"));
    colorButton = new QPushButton(tr("Set Palettes"));
    fileh5Button = new QPushButton(tr("Add Emma-HDF5 snapshots"));
    fileButton = new QPushButton(tr("Add fixed grid snapshots"));
    fileh5DahliaButton = new QPushButton(tr("Add Dahlia-HDF5 snapshots"));
    removeFileButton = new QPushButton(tr("Remove snapshot"));
    viewButton = new QPushButton(tr("Render frame"));
    mainLayout = new QGridLayout();
    //paletteSelector = new ColorSelector(&cs,&defaultDir,"Palette selector");
    frameLabel = new QLabel("    No snapshots loaded    ");
    movieButton = new QPushButton("Preview movie (slow)");
    saveFrameButton = new QPushButton("Save frame");
    saveMovieButton = new QPushButton("Save movie");
    resolutionButton = new QPushButton("Set saving resolution");
    loadMovConf = new QPushButton("Load Movie config");
    saveMovConf = new QPushButton("Save Movie config");
    loadMovSims = new QPushButton("Load snapshots list");
    saveMovSims = new QPushButton("Save snapshots list");
    aboutButton = new QPushButton("Info on visio");
    aboutButton->resize(60,30);
    cameraWG = new CameraWidget(viewRA->getCamera(),this);
    movieWG = new MovieMaker(&currentMovie,&defaultDir,this);

    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 0);
    mainLayout->setColumnStretch(2, 0);
    mainLayout->setColumnStretch(3, 0);
    mainLayout->setColumnStretch(4, 0);
    mainLayout->setRowStretch(0, 0);
    mainLayout->setRowStretch(1, 0);
    mainLayout->setRowStretch(2, 0);
    mainLayout->setRowStretch(3, 0);
    mainLayout->setRowStretch(4, 0);
    mainLayout->setRowStretch(5, 0);
    mainLayout->setRowStretch(6, 0);
    mainLayout->setRowStretch(7, 8);
    mainLayout->setRowStretch(8, 0);
    mainLayout->setRowStretch(9, 0);
    mainLayout->setRowStretch(10, 0);
    mainLayout->setRowStretch(11, 2);
    mainLayout->setRowStretch(12, 0);

    mainLayout->setRowMinimumHeight(0,20);
    mainLayout->setRowMinimumHeight(1,30);
    mainLayout->setRowMinimumHeight(2,30);
    mainLayout->setRowMinimumHeight(3,30);
    mainLayout->setRowMinimumHeight(4,30);
    mainLayout->setRowMinimumHeight(5,30);
    mainLayout->setRowMinimumHeight(6,30);
    mainLayout->setRowMinimumHeight(7,30);
    mainLayout->setRowMinimumHeight(8,30);
    mainLayout->setRowMinimumHeight(9,40);
    mainLayout->setRowMinimumHeight(10,20);
    mainLayout->setRowMinimumHeight(11,20);
    mainLayout->setRowMinimumHeight(12,20);
    mainLayout->setRowMinimumHeight(13,20);

    mainLayout->setColumnMinimumWidth(0,240);
    mainLayout->setColumnMinimumWidth(1,20);
    mainLayout->setColumnMinimumWidth(2,60);
    mainLayout->setColumnMinimumWidth(3,60);
    mainLayout->setColumnMinimumWidth(4,60);
    mainLayout->setColumnMinimumWidth(5,60);

    mainLayout->addWidget(viewRA,1,0,10,3);
    mainLayout->addWidget(titleLabel,1,3,3,3);
    mainLayout->addWidget(fileTypeLabel,4,3,1,1);
    mainLayout->addWidget(logarithmicData,4,4,1,1);
    mainLayout->addWidget(fileButton,5,3,1,1);
    mainLayout->addWidget(fileh5Button,5,4,1,1);
    mainLayout->addWidget(fileh5DahliaButton,5,5,1,1);
    mainLayout->addWidget(removeFileButton,6,4,1,1);
    mainLayout->addWidget(loadMovSims,7,3,1,1);
    mainLayout->addWidget(colorButton,7,4,1,1);
    mainLayout->addWidget(loadMovConf,7,5,1,1);
    mainLayout->addWidget(saveMovSims,8,3,1,1);
    mainLayout->addWidget(viewButton,8,4,1,1);
    mainLayout->addWidget(saveMovConf,8,5,1,1);
    mainLayout->addWidget(movieButton,9,4,1,1);

    mainLayout->addWidget(perspectiveCorrection,4,5,1,1,Qt::AlignHCenter);
    mainLayout->addWidget(resolutionButton,10,3,1,1);
    mainLayout->addWidget(saveFrameButton,10,5,1,1);
    mainLayout->addWidget(saveMovieButton,10,4,1,1);

    mainLayout->addWidget(movieWG,11,0,2,4,Qt::AlignCenter);
    mainLayout->addWidget(cameraWG,11,4,2,2,Qt::AlignCenter);

    mainLayout->addWidget(operationLabel,13,0,1,4);
    mainLayout->addWidget(exitButton,13,4,1,1);
    mainLayout->addWidget(aboutButton,13,5,1,1,Qt::AlignRight);
    //mainLayout->addWidget(stopButton,13,2,1,1);

    viewButton->setEnabled(false);
    movieButton->setEnabled(false);
    saveFrameButton->setEnabled(false);
    saveMovieButton->setEnabled(false);

    setLayout(mainLayout);

    setWindowTitle(tr("Visio Movie"));

    connect(exitButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(colorButton, SIGNAL(clicked(bool)), this, SLOT(selectPalette()));
    connect(fileButton, SIGNAL(clicked()), this, SLOT(selectFiles()));
    connect(fileh5Button, SIGNAL(clicked()), this, SLOT(selecth5Files()));
    connect(fileh5DahliaButton, SIGNAL(clicked()), this, SLOT(selecth5DahliaFiles()));
    connect(removeFileButton, SIGNAL(clicked()), this, SLOT(removeFiles()));
    connect(viewButton, SIGNAL(clicked()), this, SLOT(renderNew()));
    connect(movieButton, SIGNAL(clicked()), this, SLOT(movie()));
    connect(saveFrameButton, SIGNAL(clicked()), this, SLOT(saveFrame()));
    connect(saveMovieButton, SIGNAL(clicked()), this, SLOT(saveMovie()));
    connect(loadMovConf, SIGNAL(clicked()), this, SLOT(loadConf()));
    connect(loadMovSims, SIGNAL(clicked()), this, SLOT(loadSims()));
    connect(saveMovConf, SIGNAL(clicked()), this, SLOT(saveConf()));
    connect(saveMovSims, SIGNAL(clicked()), this, SLOT(saveSims()));
    connect(aboutButton, SIGNAL(clicked()), this, SLOT(about()));
    connect(resolutionButton, SIGNAL(clicked()), this, SLOT(setResolution()));
    connect(perspectiveCorrection,SIGNAL(toggled(bool)),this,SLOT(changePerspectiveMode(bool)));
    connect(logarithmicData,SIGNAL(toggled(bool)),this,SLOT(changeLogData(bool)));
    connect(viewRA, SIGNAL(progress(QString)), this, SLOT(progress(QString)));
    connect(hiddenRender, SIGNAL(progress(QString)), this, SLOT(progress(QString)));
    connect(viewRA,SIGNAL(leftButtonPressed()), this, SLOT(registerLeftButton()));
    connect(viewRA,SIGNAL(rightButtonPressed()), this, SLOT(registerRightButton()));
    connect(viewRA,SIGNAL(leftButtonReleased()), this, SLOT(releaseLeftButton()));
    connect(viewRA,SIGNAL(rightButtonReleased()), this, SLOT(releaseRightButton()));
    connect(viewRA,SIGNAL(mouseMove()), this, SLOT(mouseMove()));
    connect(viewRA,SIGNAL(wheelMouse(int)), this, SLOT(wheelMouse(int)));
    connect(stopButton, SIGNAL(clicked(bool)), this, SLOT(breakRender()));
    connect(cameraWG, SIGNAL(cameraChanged(Camera)), this, SLOT(changeCamera(Camera)));
    connect(movieWG, SIGNAL(newTS(int)), this, SLOT(changeFrame(int)));
    //connect(paletteSelector, SIGNAL(accepted()), this, SLOT(acceptCS()));

    //updateRender();

}

void Visualizator::about() {

    QString text;
    QTextStream ts(&text);
    ts << "<b>Visio " << visioVersion << "." << visioSubversion << "</b><br>";
    ts << "Programmed in C++<br><br>";
    ts << "Using: <br>";
    ts << "QT 5.6 <br>";
    ts << "ffmpeg version 3.0.1<br>";
    ts << "HDF5-1.8.17 C++ API library<br>";
    ts << "<br>";
    ts << "Visio is coded by <a href=\"mailto:melko.software@gmail.com\">Marco Strambi (melko)</a>.<br>";

    QMessageBox::information(this,"Information on Visio",text);

}

void Visualizator::changeLogData(bool b) {

    viewRA->setLog(b);
    hiddenRender->setLog(b);
    viewPreview();

}

void Visualizator::saveSims(QString filename) {

    if (!currentMovie.length) return;

    if (filename.isEmpty()) {
        filename = QFileDialog::getSaveFileName(this, tr("Save simulations snapshot configuration"), defaultDir, tr("Text format file (*.txt)"),0,QFileDialog::DontUseNativeDialog);
        if (filename.isEmpty()) return;

        defaultDir=QFileInfo(filename).absolutePath();
    }

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QMessageBox::information(0,"error",file.errorString());
        return;
    }

    QTextStream store(&file);

    for (int i=0; i<currentMovie.fileNames.size(); i++) {

        store << currentMovie.fileNames[i].toLocal8Bit().constData() << '\n';
        if (currentMovie.ftVector[i]==PLAIN)
            store << "PLAIN\n";
        if (currentMovie.ftVector[i]==HDF5)
            store << "HDF5\n";

        if (currentMovie.ftVector[i]==HDF5DAHLIA)
            store << "HDF5DAHLIA\n";

    }

}

void Visualizator::saveConf(QString filename) {

    if (!currentMovie.length) return;

    if (filename.isEmpty()) {
        filename = QFileDialog::getSaveFileName(this, tr("Save movie configuration"), defaultDir, tr("Text format file (*.mcf)"),0,QFileDialog::DontUseNativeDialog);
        if (filename.isEmpty()) return;

        defaultDir=QFileInfo(filename).absolutePath();
    }


    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QMessageBox::information(0,"error",file.errorString());
        return;
    }

    QTextStream store(&file);

    if (logarithmicData->isChecked()) {
        store << "LOG_DATA\n";
    } else {
        store << "NO_LOG_DATA\n";
    }

    if (perspectiveMode) {
        store << "PERSPECTIVE_CORRECTION\n";
    } else {
        store << "NO_PERSPECTIVE_CORRECTION\n";
    }


    store << currentMovie.length << '\n';
    store << currentMovie.fps << '\n';
    store << hRes << '\n';
    store << vRes << '\n';

    for (unsigned int i=0; i<currentMovie.cameraRule.size(); i++) {
        store << currentMovie.cameraRule[i].toStr();
    }

    for (unsigned int i=0; i<currentMovie.framesRule.size(); i++) {
        store << currentMovie.framesRule[i] << ' ';
    }
    store << '\n';

    for (unsigned int i=0; i<currentMovie.textRule.size(); i++) {

        store << currentMovie.textRule[i].size() << '\n';

        for (unsigned int j=0; j<currentMovie.textRule[i].size(); j++) {
            store << currentMovie.textRule[i][j].toStr();
        }

    }

    for (unsigned int i=0; i<currentMovie.framesRule.size(); i++) {
        store << currentMovie.colorScaleRule[i].toStr();
    }

}

void Visualizator::loadSims(QString filename){

    QStringList sl;
    FileTypeList ft;

    if (filename.isEmpty()) {
        filename = QFileDialog::getOpenFileName(this, tr("Load simulations snapshot configuration"), defaultDir, tr("Text format file (*.txt)"),0,QFileDialog::DontUseNativeDialog);
        if (filename.isEmpty()) return;

        defaultDir=QFileInfo(filename).absolutePath();
    }

    progress(QString("Loading simulation list ")+filename+QString("..."));

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QMessageBox::information(0,"error",file.errorString());
        return;
    }

    QTextStream store(&file);
    QString f,t;
    f=store.readLine();
    t=store.readLine();
    while (!f.isEmpty()) {
        sl.push_back(f);
        if (t == "PLAIN")
            ft.push_back(PLAIN);
        if (t == "HDF5")
            ft.push_back(HDF5);
        if (t == "HDF5DAHLIA")
            ft.push_back(HDF5DAHLIA);
        f=store.readLine();
        t=store.readLine();
    }

    currentMovie.ftVector=ft;
    currentMovie.fileNames=FileList(sl);
    sourceFiles=FileList(sl);

    if (!currentMovie.length) {
        numberOfFrames+=currentMovie.fileNames.size();
        currentMovie.length+=currentMovie.fileNames.size();
        ColorScale cs=viewRA->palette;
        currentFrame=0;
        //vcs.push_back(cs);
        for (int i=0; i<currentMovie.fileNames.size(); i++) {
            currentMovie.cameraRule.push_back(viewRA->getCamera());
            currentMovie.framesRule.push_back(i);
            currentMovie.colorScaleRule.push_back(cs);
            currentMovie.textRule.push_back(std::vector<Text>(0));
        }
        currentLoadedFile=sourceFiles[currentFrame];
        movieWG->updateMovie(&currentMovie);  //warning!
    }

    viewRA->unlockMemoryData();
    viewRA->sourceFiles=currentMovie.fileNames;
    viewRA->ftVector=currentMovie.ftVector;
    viewRA->previewBuilt=0;
    viewRA->makePreviews();
    //changeFrame(0);
    if (currentMovie.length) {
        viewButton->setEnabled(true);
        movieButton->setEnabled(true);
        saveFrameButton->setEnabled(true);
        saveMovieButton->setEnabled(true);
    }
    viewPreview();

}

void Visualizator::loadConf(QString filename){

    if (filename.isEmpty()) {
        filename = QFileDialog::getOpenFileName(this, tr("Load movie configuration"), defaultDir, tr("Text format file (*.mcf)"),0,QFileDialog::DontUseNativeDialog);
        if (filename.isEmpty()) return;

        defaultDir=QFileInfo(filename).absolutePath();
    }

    progress(QString("Loading movie configuration file ")+filename+QString("..."));

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QMessageBox::information(0,"error",file.errorString());
        return;
    }

    QTextStream store(&file);
    QString line,l,r;
    unsigned int n;

    store >> line;

    if (line=="LOG_DATA") {
        logarithmicData->setChecked(true);
    } else {
        logarithmicData->setChecked(false);
    }

    store >> line;

    if (line=="PERSPECTIVE_CORRECTION") {
        perspectiveCorrection->setChecked(true);
        perspectiveMode=1;
    } else {
        perspectiveCorrection->setChecked(false);
        perspectiveMode=0;
    }

    store >> currentMovie.length;
    store >> currentMovie.fps;
    store >> hRes;
    store >> vRes;

    currentMovie.framesRule.resize(currentMovie.length);
    currentMovie.cameraRule.resize(currentMovie.length);
    currentMovie.textRule.resize(currentMovie.length);
    currentMovie.colorScaleRule.resize(currentMovie.length);
    for (unsigned int i=0; i<currentMovie.length; i++) {
        line=store.readLine();
        if (line.isEmpty()) line=store.readLine();
        currentMovie.cameraRule[i]=(Camera(line));
    }
    for (unsigned int i=0; i<currentMovie.length; i++) {
        store >> line;
        currentMovie.framesRule[i]=(line.toInt());
    }
    for (unsigned int i=0; i<currentMovie.length; i++) {

        store >> line;
        if (line.isEmpty()) line=store.readLine();
        n=line.toInt();
        std::vector<Text> frameTexts(n);

        for (unsigned int j=0; j<n; j++) {

            r="";
            line="";
            while(r!="<END_OF_TEXT>")  {
                l=store.readLine();
                r=l.right(13);
                line=line+l+'\n';
            }
            frameTexts[j]=(Text(line));
        }

        currentMovie.textRule[i]=frameTexts;

    }


    for (unsigned int i=0; i<currentMovie.length; i++) {
        line=store.readLine();
        while (line.isEmpty()) line=store.readLine();
        currentMovie.colorScaleRule[i]=ColorScale(line);
    }
    //paletteSelector->setCS(&currentMovie.colorScaleRule[0]);
    cs=currentMovie.colorScaleRule[0];
    if (currentMovie.length) {
        viewButton->setEnabled(true);
        movieButton->setEnabled(true);
        saveFrameButton->setEnabled(true);
        saveMovieButton->setEnabled(true);
        viewRA->setText(currentMovie.textRule[0]);
        viewRA->setCamera(currentMovie.cameraRule[0]);
        viewRA->setPalettes(currentMovie.colorScaleRule[0]);
        movieWG->updateMovie(&currentMovie);
        movieWG->changeTS(0);
        cs=currentMovie.colorScaleRule[0];
        //paletteSelector->setCS(&currentMovie.colorScaleRule[0]);
    }
    viewPreview();

    progress("Movie configuration file loaded.");

}

void Visualizator::acceptCS() {

    for (int i=0; i<int(currentMovie.length); i++) {
        currentMovie.colorScaleRule[i]=cs;
    }
    viewRA->setPalettes(cs);
    if (currentMovie.length) {
        currentMovie.colorScaleRule[movieWG->currentTS]=cs;
        viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
        viewRA->setCamera(currentMovie.cameraRule[movieWG->currentTS]);
    }
    viewPreview();

}

void Visualizator::changeCamera(Camera c) {

    viewRA->setCamera(c);
    if (currentMovie.length) {
        currentMovie.cameraRule[movieWG->currentTS]=c;
        viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
        viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
    }
    viewPreview();

}

void Visualizator::breakRender() {

    viewRA->stop=true;
    hiddenRender->stop=true;

}

void Visualizator::checkFile() {

    if (currentLoadedFile=="none") return;
    if (viewRA->getFrame()!=currentFrame) {
        viewRA->loadData(currentFrame);
        currentLoadedFile=sourceFiles[currentFrame];
    }

}

void Visualizator::viewComplete() {

    cameraWG->setCamera(viewRA->getCamera());
    viewRA->draw(perspectiveMode,currentFrame,false);
    viewRA->update();

}

void Visualizator::viewPreview() {

    cameraWG->setCamera(viewRA->getCamera());
    viewRA->draw(perspectiveMode,currentFrame,true);
    viewRA->update();

}

void Visualizator::renderNew() {

    viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
    viewRA->setCamera(currentMovie.cameraRule[movieWG->currentTS]);
    viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
    viewComplete();

}

void Visualizator::movie() {

    std::stringstream ss;

    for (unsigned int i=0; i<currentMovie.length; i++) {
        movieWG->changeTS(int(i));
        cs=currentMovie.colorScaleRule[i];
        //paletteSelector->setCS(&currentMovie.colorScaleRule[i]);
    }

    cs=currentMovie.colorScaleRule[0];
    //paletteSelector->setCS(&currentMovie.colorScaleRule[0]);
    movieWG->changeTS(0);

}

void Visualizator::saveMovie(QString file) {

    bool fromConsole(true);
    if (file.isEmpty()) {
        fromConsole=false;
        file=QFileDialog::getSaveFileName(this, tr("Save Frame"), defaultDir, tr("Lossless MOV (*.mov)"),0,QFileDialog::DontUseNativeDialog);
        if (file.isEmpty()) return;

        defaultDir=QFileInfo(file).absolutePath();
    }

    viewRA->unlockMemoryData();

    hiddenRender->setVisible(false);
    hiddenRender->setDim(hRes,vRes);
    hiddenRender->resize(hRes,vRes);
    hiddenRender->setPalettes(viewRA->palette);
    hiddenRender->setCamera(viewRA->getCamera());
    hiddenRender->sourceFiles=currentMovie.fileNames;
    hiddenRender->ftVector=viewRA->ftVector;
    QDir defDir=defaultDir;
    if (!defDir.cd("visio_movie")) {
        defDir.mkdir("visio_movie");
        defDir.cd("visio_movie");
    }
    if (defDir.cd("tmp_frames")) {
        defDir.removeRecursively();
        defDir.cdUp();
    }
    defDir.mkdir("tmp_frames");
    defDir.cd("tmp_frames");

    std::stringstream sfps;
    sfps << currentMovie.fps;
    QString fpsStr(sfps.str().c_str());

    std::stringstream ss;
    QString message;

    for (int i=0; i<int(currentMovie.length); i++) {

        QTextStream ts(&message);
        ts << "Frame n." << i << " of " << currentMovie.length;
        progress(message);
        message="";
        movieWG->blockSignals(true);
        movieWG->timeStampSL->setValue(i);
        movieWG->blockSignals(false);
        hiddenRender->setCamera(currentMovie.cameraRule[i]);
        hiddenRender->setPalettes(currentMovie.colorScaleRule[i]);
        hiddenRender->setText(currentMovie.textRule[i]);
        hiddenRender->draw(perspectiveMode,currentMovie.framesRule[i],false);
        if (!fromConsole)
        {
            viewRA->setCamera(currentMovie.cameraRule[i]);
            viewRA->setPalettes(currentMovie.colorScaleRule[i]);
            viewRA->setText(currentMovie.textRule[i]);
            currentFrame=currentMovie.framesRule[i];
            viewPreview();
        }
        ss.str("");
        ss << defDir.absolutePath().toUtf8().constData() << "/frame_"<< i << ".bmp";
        std::cout << "Saving frame " << i << " in:\n" << defDir.absolutePath().toUtf8().constData() << std::endl;
        hiddenRender->saveFrame(ss.str().c_str());
    }

    operationLabel->setText("Building movie");
    operationLabel->update();

    hiddenRender->unlockMemoryData();

    QString ffmpegCommand=ffmpegPath+" -y -framerate ";
    ffmpegCommand+=fpsStr;
    ffmpegCommand+=" -i \"";
    ffmpegCommand+=defDir.absolutePath();
    ffmpegCommand+="/frame_%d.bmp\" -vcodec qtrle ";
    ffmpegCommand+=" \"";
    ffmpegCommand+=file;
    ffmpegCommand+="\"";

    std::cout << ffmpegCommand.toUtf8().constData() << std::endl;

    QProcess ffmpegProgram;
    ffmpegProgram.setProcessChannelMode(QProcess::MergedChannels);
    ffmpegProgram.start(ffmpegCommand);
    ffmpegProgram.waitForFinished();
    QString output(ffmpegProgram.readAllStandardOutput());
    ffmpegProgram.terminate();
    std::cout << output.toUtf8().constData() << std::endl;

    operationLabel->setText("Done");
    movieWG->changeTS(0);

}

void Visualizator::saveFrame(QString file,unsigned int frame) {

    if (file.isEmpty()) {   // GUI mode
        file=QFileDialog::getSaveFileName(this, tr("Save Frame"),
                                                  defaultDir,
                                                  tr("Lossless bitmap (*.bmp);; JPeg (*.jpg)"),
                                                  0,
                                                  QFileDialog::DontUseNativeDialog);

        if (file.isEmpty()) return;

        defaultDir=QFileInfo(file).absolutePath();

        viewRA->unlockMemoryData();

        hiddenRender->setDim(hRes,vRes);
        hiddenRender->resize(hRes,vRes);
        hiddenRender->setPalettes(viewRA->palette);
        hiddenRender->setCamera(viewRA->getCamera());
        if (currentMovie.length) hiddenRender->setText(currentMovie.textRule[movieWG->currentTS]);
        hiddenRender->sourceFiles=currentMovie.fileNames;
        hiddenRender->ftVector=viewRA->ftVector;
        hiddenRender->draw(perspectiveMode,currentMovie.framesRule[movieWG->currentTS],false);
        hiddenRender->saveFrame(file);

        hiddenRender->unlockMemoryData();

        movieWG->changeTS(movieWG->currentTS);

    } else {  // term mode

        if (frame>currentMovie.length) {
            frame=0;
            std::cout << "Frame out of bounds, rendering frame 0." << std::endl;
        }

        viewRA->unlockMemoryData();

        hiddenRender->setDim(hRes,vRes);
        hiddenRender->resize(hRes,vRes);
        hiddenRender->setPalettes(currentMovie.colorScaleRule[frame]);
        hiddenRender->setCamera(currentMovie.cameraRule[frame]);
        if (currentMovie.length) hiddenRender->setText(currentMovie.textRule[frame]);
        hiddenRender->sourceFiles=currentMovie.fileNames;
        hiddenRender->ftVector=viewRA->ftVector;
        hiddenRender->draw(perspectiveMode,currentMovie.framesRule[frame],false);
        hiddenRender->saveFrame(file);

        hiddenRender->unlockMemoryData();

        movieWG->changeTS(movieWG->currentTS);

    }
//    if (currentMovie.length) {
//        currentLoadedFile=sourceFiles[currentFrame];
//        viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
//        viewRA->setCamera(currentMovie.cameraRule[movieWG->currentTS]);
//        viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
//    }
//    viewPreview();

}

void Visualizator::setResolution() {

    Resolution r(&hRes,&vRes,this);
    r.setModal(true);
    r.exec();
}

void Visualizator::changePerspectiveMode(bool correction) {

    if (correction) {
        perspectiveMode=1;
    } else {
        perspectiveMode=0;
    }

//    if (currentMovie.length) {
//        viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
//        viewRA->setCamera(currentMovie.cameraRule[movieWG->currentTS]);
//        viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
//    }
    viewPreview();

}

void Visualizator::registerLeftButton() {

    if (!leftButton) {
        leftButton=true;
        if (!rightButton) {
            leftPosX=viewRA->x;
            leftPosY=viewRA->y;
        } else {
            bothPosX=viewRA->x;
            bothPosY=viewRA->y;
        }
    }

}

void Visualizator::registerRightButton() {

    if (!rightButton) {
        rightButton=true;
        if (!leftButton) {
            rightPosX=viewRA->x;
            rightPosY=viewRA->y;
        } else {
            bothPosX=viewRA->x;
            bothPosY=viewRA->y;
        }
    }

}

void Visualizator::releaseLeftButton() {

    leftButton=false;
    if (currentMovie.length) {
        viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
        viewRA->setCamera(currentMovie.cameraRule[movieWG->currentTS]);
        viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
    }
    viewPreview();

}

void Visualizator::releaseRightButton() {

    rightButton=false;
    if (currentMovie.length) {
        viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
        viewRA->setCamera(currentMovie.cameraRule[movieWG->currentTS]);
        viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
    }
    viewPreview();

}

void Visualizator::wheelMouse(int d) {

    Camera c(viewRA->getCamera());
    double factor(1.0),ang(c.angle);

    if (d<0)
        factor=1.05;
    else
        factor=1/1.05;

    ang*=factor;

    if (ang>1.6) ang=1.6;
    if (ang<0.1) ang =0.1;

    c.angle=ang;

    viewRA->setCamera(c);
    if (currentMovie.length) {
        currentMovie.cameraRule[movieWG->currentTS]=c;
        viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
        viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
    }
    viewPreview();

}

void Visualizator::mouseMove() {

    Camera c(viewRA->getCamera());

    if (leftButton&&rightButton) {
        int deltaX(viewRA->x-bothPosX);
        int deltaY(viewRA->y-bothPosY);
        bothPosX=viewRA->x;
        bothPosY=viewRA->y;

        c.translate((-deltaX/200.0)*c.position.base().row3);
        c.translate((-deltaY/200.0)*c.position.base().row2);

        if (currentMovie.length) currentMovie.cameraRule[movieWG->currentTS]=c;
        viewRA->setCamera(c);

        return;
    }

    if (leftButton) {
        int deltaX(viewRA->x-leftPosX);
        int deltaY(viewRA->y-leftPosY);
        leftPosX=viewRA->x;
        leftPosY=viewRA->y;

        c.rotate(-deltaX/200.0,c.position.base().row2);
        c.rotate(deltaY/200.0,c.position.base().row3);

        if (currentMovie.length) currentMovie.cameraRule[movieWG->currentTS]=c;
        viewRA->setCamera(c);

        return;
    }

    if (rightButton) {
        int deltaX(viewRA->x-rightPosX);
        int deltaY(viewRA->y-rightPosY);
        rightPosX=viewRA->x;
        rightPosY=viewRA->y;

        c.rotate(deltaX/200.0,c.position.base().row1);
        c.translate((-deltaY/200.0)*c.position.base().row1);

        if (currentMovie.length) currentMovie.cameraRule[movieWG->currentTS]=c;
        viewRA->setCamera(c);

        return;
    }
}

/*
void Visualizator::updateRender() {



}
*/
void Visualizator::resizeEvent(QResizeEvent * event) {

    QWidget::resizeEvent(event);
    if (currentMovie.length) {
        viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
        viewRA->setCamera(currentMovie.cameraRule[movieWG->currentTS]);
        viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
    }
    viewPreview();

}
/*
void Visualizator::paintEvent(QPaintEvent * event) {

    QWidget::paintEvent(event);
    updateRender();

}
*/

Visualizator::~Visualizator()
{

    delete mainLayout;
    delete viewRA;
    delete hiddenRender;
    delete exitButton;
    delete cameraButton;
    delete colorButton;
    delete fileButton;
    delete fileh5Button;
    delete fileh5DahliaButton;
    delete removeFileButton;
    delete viewButton;
    delete perspectiveCorrection;
    delete logarithmicData;
    delete titleLabel;
    delete operationLabel;
    delete fileTypeLabel;
    delete frameLabel;
    delete movieButton;
    delete saveFrameButton;
    delete saveMovieButton;
    delete resolutionButton;
    delete aboutButton;
    delete cameraWG;
    delete movieWG;
    //if (paletteSelector) delete paletteSelector;

}

QSize Visualizator::sizeHint() const
{
    return QSize(800, 600);
}

QSize Visualizator::minimumSizeHint() const
{
    return QSize(640, 480);
}

void Visualizator::selecth5DahliaFiles() {


    QStringList fileNameList = QFileDialog::getOpenFileNames(this, tr("Open File"), defaultDir, tr("Anything (*)"),0,QFileDialog::DontUseNativeDialog);
    QString fileName;

    if (fileNameList.isEmpty()) return;

    defaultDir=QFileInfo(fileNameList[0]).absolutePath();

    for (int i=0; i<fileNameList.size(); i++) {

        fileName=fileNameList[i];

        int offset(sourceFiles.size());
        sourceFiles.append(fileName);
        currentMovie.fileNames.append(fileName);

        //currentMovie=Movie();

        //currentFrame=0;
        //std::vector<ColorScale> vcs;
        numberOfFrames++;
        currentMovie.length++;
        ColorScale ccs=viewRA->palette;
        //vcs.push_back(cs);
        currentMovie.cameraRule.push_back(viewRA->getCamera());
        currentMovie.framesRule.push_back(offset);
        currentMovie.colorScaleRule.push_back(ccs);
        currentMovie.textRule.push_back(std::vector<Text>(0));
        currentMovie.ftVector.push_back(HDF5DAHLIA);

        currentLoadedFile=sourceFiles[currentFrame];
        movieWG->updateMovie(&currentMovie);  //warning!

        viewButton->setEnabled(true);
        movieButton->setEnabled(true);
        saveFrameButton->setEnabled(true);
        saveMovieButton->setEnabled(true);

        viewRA->unlockMemoryData();
        viewRA->sourceFiles=currentMovie.fileNames;
        viewRA->ftVector=currentMovie.ftVector;
        viewRA->makePreviews();
        //changeFrame(0);

        if (offset==0) {
            double min,max;
            viewRA->searchMinAndMax(0,min,max);
            std::cout << min << "   " << max << std::endl;
            if (logarithmicData->isChecked()) {
                if (min>0) min=log10l(min);
                if (max>0) max=log10l(max);
                if (min>max) min=max;
            }
            currentMovie.colorScaleRule[0].setRange(min,max);
            cs=currentMovie.colorScaleRule[0];
            //paletteSelector->setCS(&currentMovie.colorScaleRule[0]);
        }

        if (currentMovie.length) {
            viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
            viewRA->setCamera(currentMovie.cameraRule[movieWG->currentTS]);
            viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
        }
        viewPreview();

    }

}

void Visualizator::selecth5Files() {

    QStringList fileNameList = QFileDialog::getOpenFileNames(this, tr("Open File"), defaultDir, tr("Anything (*)"),0,QFileDialog::DontUseNativeDialog);
    QString fileName;

    if (fileNameList.isEmpty()) return;

    defaultDir=QFileInfo(fileNameList[0]).absolutePath();

    for (int i=0; i<fileNameList.size(); i++) {

        fileName=fileNameList[i];

        int offset(sourceFiles.size());
        sourceFiles.append(fileName);
        currentMovie.fileNames.append(fileName);

        //currentMovie=Movie();

        //currentFrame=0;
        //std::vector<ColorScale> vcs;
        numberOfFrames++;
        currentMovie.length++;
        ColorScale ccs=viewRA->palette;
        //vcs.push_back(cs);
        currentMovie.cameraRule.push_back(viewRA->getCamera());
        currentMovie.framesRule.push_back(offset);
        currentMovie.colorScaleRule.push_back(ccs);
        currentMovie.textRule.push_back(std::vector<Text>(0));
        currentMovie.ftVector.push_back(HDF5);

        currentLoadedFile=sourceFiles[currentFrame];
        movieWG->updateMovie(&currentMovie);  //warning!

        viewButton->setEnabled(true);
        movieButton->setEnabled(true);
        saveFrameButton->setEnabled(true);
        saveMovieButton->setEnabled(true);

        viewRA->unlockMemoryData();
        viewRA->sourceFiles=currentMovie.fileNames;
        viewRA->ftVector=currentMovie.ftVector;
        viewRA->makePreviews();
        //changeFrame(0);

        if (offset==0) {
            double min,max;
            viewRA->searchMinAndMax(0,min,max);
            std::cout << min << "   " << max << std::endl;
            if (logarithmicData->isChecked()) {
                if (min>0) min=log10l(min);
                if (max>0) max=log10l(max);
                if (min>max) min=max;
            }
            currentMovie.colorScaleRule[0].setRange(min,max);
            cs=currentMovie.colorScaleRule[0];
            //paletteSelector->setCS(&currentMovie.colorScaleRule[0]);
        }

        if (currentMovie.length) {
            viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
            viewRA->setCamera(currentMovie.cameraRule[movieWG->currentTS]);
            viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
        }
        viewPreview();

    }

}

void Visualizator::removeFiles() {

    if (currentMovie.fileNames.isEmpty()) return;

    SourceFileRemover s(&currentMovie,viewRA,this);
    s.setModal(true);
    s.exec();

    movieWG->updateMovie(&currentMovie);  //warning!

}

void Visualizator::selectFiles() {

    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"), defaultDir, tr("Anything (*)"),0,QFileDialog::DontUseNativeDialog);

    if (fileNames.isEmpty()) return;

    defaultDir=QFileInfo(fileNames[0]).absolutePath();

    int offset(sourceFiles.size());
    sourceFiles.append(fileNames);
    currentMovie.fileNames.append(fileNames);

    numberOfFrames+=fileNames.size();
    currentMovie.length+=fileNames.size();
    ColorScale ccs=viewRA->palette;
    //vcs.push_back(cs);
    for (int i=0; i<fileNames.size(); i++) {
        currentMovie.cameraRule.push_back(viewRA->getCamera());
        currentMovie.framesRule.push_back(i+offset);
        currentMovie.colorScaleRule.push_back(ccs);
        currentMovie.textRule.push_back(std::vector<Text>(0));
        currentMovie.ftVector.push_back(PLAIN);
    }
    currentLoadedFile=sourceFiles[currentFrame];
    movieWG->updateMovie(&currentMovie);  //warning!

    viewButton->setEnabled(true);
    movieButton->setEnabled(true);
    saveFrameButton->setEnabled(true);
    saveMovieButton->setEnabled(true);

    viewRA->unlockMemoryData();
    viewRA->sourceFiles=currentMovie.fileNames;
    viewRA->ftVector=currentMovie.ftVector;
    viewRA->makePreviews();
    //changeFrame(0);

    if (offset==0) {
        double min,max;
        viewRA->searchMinAndMax(0,min,max);
        std::cout << min << "   " << max << std::endl;
        if (logarithmicData->isChecked()) {
            if (min>0) min=log10l(min);
            if (max>0) max=log10l(max);
            if (min>max) min=max;
        }
        currentMovie.colorScaleRule[0].setRange(min,max);
        std::cout << currentMovie.colorScaleRule[0].getMin() << "   " << currentMovie.colorScaleRule[0].getMax()  << std::endl;
        cs=currentMovie.colorScaleRule[0];
        //paletteSelector->setCS(&currentMovie.colorScaleRule[0]);
    }

    if (currentMovie.length) {
        viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
        viewRA->setCamera(currentMovie.cameraRule[movieWG->currentTS]);
        viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
    }
    viewPreview();

}

void Visualizator::removeSim(int index) {

    sourceFiles.erase(sourceFiles.begin()+index);
    if (currentFrame) {
        currentFrame=0;
    }

}

void Visualizator::selectPalette() {

    ColorSelector c(&cs,&defaultDir,"",this);
    c.setModal(true);
    c.exec();

    acceptCS();

}

void Visualizator::progress(QString s) {
    std::cout << s.toLocal8Bit().constData() << std::endl;
    operationLabel->setText(s);
    update();
}

void Visualizator::changeFrame(int f) {

    cameraWG->setCamera(currentMovie.cameraRule[f]);
    //paletteSelector->setCS(&(currentMovie.colorScaleRule[f]));
    cs=currentMovie.colorScaleRule[f];

    currentFrame=currentMovie.framesRule[f];
    currentLoadedFile=sourceFiles[currentFrame];
    viewRA->setPalettes(cs);
    viewRA->setCamera(currentMovie.cameraRule[f]);

    std::stringstream ss;
    ss << "Frame " << currentFrame+1 << '/' << numberOfFrames;
    frameLabel->setText(tr(ss.str().c_str()));
    ss.str("");

    std::string s(sourceFiles[currentFrame].toUtf8().constData());
    int pos=s.find_last_of("\\/");
    s=s.substr(pos+1);
    int c(s.size());
    for(int i=1; i<=c/30; i++)
    {
        s.insert(i*30-1, " ");
    }

    ss << "Frame file : \n" << s;
    titleLabel->setText(tr(ss.str().c_str()));

    ss.str("");
    switch (currentMovie.ftVector[currentFrame]) {
        case (PLAIN) :
        ss << "Fixed grid file";
        break;
    case (HDF5) :
        ss << "HDF5-Emma set of 5 files";
        break;
    case (HDF5DAHLIA) :
        ss << "HDF5-Dahlia set of 5 files";
        break;
    default:
        ss << "---";
    }
    fileTypeLabel->setText(tr(ss.str().c_str()));

    if (currentMovie.length) {
        viewRA->setText(currentMovie.textRule[movieWG->currentTS]);
        viewRA->setCamera(currentMovie.cameraRule[movieWG->currentTS]);
        viewRA->setPalettes(currentMovie.colorScaleRule[movieWG->currentTS]);
    }
    viewPreview();

}

Resolution::Resolution(int *w, int *h, QWidget *parent):QDialog(parent) {

    hei=h;
    wid=w;

    heightSpinBox = new QSpinBox();
    widthSpinBox = new QSpinBox();
    acceptButton = new QPushButton("Accept");
    cancelButton = new QPushButton("Reject");
    xLabel = new QLabel(tr("x"));
    wLabel = new QLabel(tr("Width"));
    hLabel = new QLabel(tr("Heigth"));
    mainLayout = new QGridLayout();

    heightSpinBox->setMinimum(128);
    heightSpinBox->setMaximum(2048);
    widthSpinBox->setMinimum(128);
    widthSpinBox->setMaximum(2048);

    if (hei) {
        heightSpinBox->setValue(*hei);
    }
    if (wid) {
        widthSpinBox->setValue(*wid);
    }

    this->setFixedSize(220,130);

    mainLayout->setColumnMinimumWidth(0,100);
    mainLayout->setColumnMinimumWidth(1,20);
    mainLayout->setColumnMinimumWidth(2,100);

    mainLayout->setRowMinimumHeight(0,30);
    mainLayout->setRowMinimumHeight(1,60);
    mainLayout->setRowMinimumHeight(2,10);
    mainLayout->setRowMinimumHeight(3,30);

    mainLayout->addWidget(wLabel,0,0,1,1);
    mainLayout->addWidget(hLabel,0,2,1,1);
    mainLayout->addWidget(heightSpinBox,1,2,1,1);
    mainLayout->addWidget(xLabel,1,1,1,1);
    mainLayout->addWidget(widthSpinBox,1,0,1,1);
    mainLayout->addWidget(cancelButton,3,0,1,1);
    mainLayout->addWidget(acceptButton,3,2,1,1);

    setLayout(mainLayout);

    setWindowTitle(tr("Resolution"));


    connect(acceptButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

}

Resolution::~Resolution() {

    delete heightSpinBox;
    delete widthSpinBox;
    delete acceptButton;
    delete cancelButton;
    delete xLabel;
    delete wLabel;
    delete hLabel;
    delete mainLayout;

};

void Resolution::accept() {

    *wid=widthSpinBox->value();
    *hei=heightSpinBox->value();
    QDialog::accept();

}

SourceFileRemover::SourceFileRemover(Movie* m, RenderUniverse *ru, Visualizator *parent):QDialog(parent) {

    numFiles=m->fileNames.size();
    currentMovie=m;
    currentRU=ru;
    currentV=parent;

    if (!numFiles) reject();

    sa = new QListWidget();
    ok = new QPushButton("Accept");
    all = new QPushButton("Toggle all");
    cancel = new QPushButton("Reject");
    title = new QLabel("Select simulation to remove from memory");
    sa->resize(700,500);

    for (int i=0; i<numFiles; i++) {

        QListWidgetItem *f = new QListWidgetItem(m->fileNames[i],sa);
        f->setFlags(f->flags() | Qt::ItemIsUserCheckable);
        f->setCheckState(Qt::Unchecked);

    }

    mainLayout = new QGridLayout();

    mainLayout->setColumnMinimumWidth(0,50);
    mainLayout->setColumnMinimumWidth(1,700);
    mainLayout->setColumnMinimumWidth(2,50);
    mainLayout->setColumnStretch(0,0);
    mainLayout->setColumnStretch(1,1);
    mainLayout->setColumnStretch(2,0);

    mainLayout->setRowMinimumHeight(0,50);
    mainLayout->setRowMinimumHeight(1,500);
    mainLayout->setRowMinimumHeight(2,30);
    mainLayout->setRowMinimumHeight(3,30);
    mainLayout->setRowStretch(0,0);
    mainLayout->setRowStretch(1,1);
    mainLayout->setRowStretch(2,0);

    mainLayout->addWidget(title,0,0,1,3,Qt::AlignCenter);

    mainLayout->addWidget(sa,1,0,1,3);

    mainLayout->addWidget(all,2,0,1,1,Qt::AlignCenter);

    mainLayout->addWidget(ok,3,0,1,1,Qt::AlignLeft);
    mainLayout->addWidget(cancel,3,1,1,1,Qt::AlignRight);

    setLayout(mainLayout);

    setWindowTitle(tr("Remove simulation from memory"));


    connect(ok, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
    connect(all, SIGNAL(clicked(bool)), this, SLOT(toggleAll()));


}

SourceFileRemover::~SourceFileRemover() {

    delete title;
    delete sa;
    delete ok;
    delete cancel;
    delete mainLayout;

}

void SourceFileRemover::toggleAll() {

    for (int i=numFiles-1; i>=0; i--) {
        if (sa->item(i)->checkState())
            sa->item(i)->setCheckState(Qt::Unchecked);
        else
            sa->item(i)->setCheckState(Qt::Checked);
    }

}

void SourceFileRemover::accept() {

    for (int i=numFiles-1; i>=0; i--) {
        if(sa->item(i)->checkState()) {

            currentMovie->removeSim(i);
            currentRU->removeSim(i);
            currentV->removeSim(i);

        }
    }

    currentV->viewPreview();

    QDialog::accept();

}

void SourceFileRemover::reject() {

    QDialog::reject();

}
