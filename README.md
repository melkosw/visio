# visio
Visualization program for cosmological simulations stored in HDF5 hierarchy vector files containing info about cell sizes (or plain-grid binary files)


STEP 1: download dependecies
-------------------------------------------

In order to install Visio from source you have to install:

- QT 5.6 (I think a version above 5.1 suffice, but the 5.6 is the one I used to develop it)
(link: https://www.qt.io/download-open-source/#section-2)

- on Mac, in order to make QT 5.6 works, you have also to install xcode last version
​
- HDF5 library pre-build (link: https://www.hdfgroup.org/HDF5/release/obtain5.html)

- ffmpeg last version static build (link: https://ffmpeg.org/download.html)

All of this programs are free or open source. 
Qt have to be installed, while HDF5 have to be put "somewhere", the best choice is to copy it in the default directories, but you can let it be local somewhere, like "opt", your home folder, or wherever you prefer. 
Once you put it somewhere, write down the absolute install and lib folders for HDF5 libraries, since you should need them to change visio.pro file in source folder.
ffmpeg is a stand alone executable file, you will just put it in the right folder as last step.

STEP 2: Modifying visio.pro file
------------------------------------------ 

Now you must change visio.pro file. Open it with a text editor, it should appear something like:

[EXTRACT FROM VISIO.PRO]

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visio
TEMPLATE = app

LIBS += -L/usr/lib/x86_64-linux-gnu/hdf5/serial/lib -lhdf5 -lhdf5_cpp -lhdf5_hl

INCLUDEPATH += /usr/lib/x86_64-linux-gnu/hdf5/serial/include

[...]

you must replace the 
/usr/lib/x86_64-linux-gnu/hdf5/serial/lib 
and 
/usr/lib/x86_64-linux-gnu/hdf5/serial/include
with folders with the lib and include folders of HDF5 pre-build file you wrote down before, save it and exit.

STEP 3: Compiling all
------------------------------

Find the Qt installation directory and search the subfolder structure for a sub-directory that should be something like:

[QT] / [Version] / [compiler] / bin 

in this folder you will find qmake. Copy it in the visio source folder.
Enter visio source folder and type

./qmake visio.pro

Qmake should produce a MakeFile, now just type

make

and visio should be compiled. 
The compiled file is "visio" or "visio.app", depending on your OS (respectively linux or Mac)
in order to complete it you should put ffmpeg executable in the same folder as visio (for linux) or in the folder
visio.app/Contents/Mac_OS (for OS X)
