#ifndef RAWHDATA_H
#define RAWHDATA_H

#include <vector>
#include <string>
#include "frame.h"
#include "H5Cpp.h"

using namespace H5;

static const double epsilonRaw = 1e-11;



struct RawElement {

    double x,y,z;
    float val;
    int res;

    bool isIn(double, double, double);
    RawElement():x(0),y(0),z(0),val(0),res(0) {}

};

typedef std::vector<RawElement>::iterator DataPtr;

bool operator<(const RawElement&,const RawElement&);
bool operator>(const RawElement&,const RawElement&);
bool lessX(const RawElement&,const RawElement&);
bool lessY(const RawElement&,const RawElement&);
bool lessZ(const RawElement&,const RawElement&);
bool moreX(const RawElement&,const RawElement&);
bool moreY(const RawElement&,const RawElement&);
bool moreZ(const RawElement&,const RawElement&);
bool someMore(const RawElement&, const RawElement&);

class RawHData {

public:

    std::vector<RawElement> data;
    std::vector<std::pair<DataPtr,int> > resIndex;
    RawHData();
    void setData(double*, double*, double*, double*, int*, unsigned long long);
    float at(double,double,double);
    void infoPos(Coords,DataPtr&);
    void makePreview(unsigned long long r, std::vector<char>&);
    int maxLev();
    int minLev();
    unsigned int length();
    double max();
    double min();
    std::vector<int> searchForLevel(double,double);
    int numLev(int, double mn, double mx);
    std::vector<DataPtr> listLev(int l);

};

void loadHDF5Emma(std::vector<std::string>,RawHData*);
void loadHDF5Dahlia(std::vector<std::string>,RawHData*);

#endif // RAWHDATA_H
