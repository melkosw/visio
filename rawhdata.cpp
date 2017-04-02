
#include "rawhdata.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

bool RawElement::isIn(double xx, double yy, double zz) {

    double r(powl(0.5l,res));
    return (((x<xx+epsilonRaw)&&(xx<x+r))&&((y<yy+epsilonRaw)&&(yy<y+r))&&((z<zz+epsilonRaw)&&(zz<z+r)));

}

bool operator<(const RawElement& re, const RawElement& fr) {

    if (re.res!=fr.res) return re.res>fr.res;
    if (re.x<fr.x-epsilonRaw/2) return true;
    if ((fabsl(fr.x-re.x)<epsilonRaw)&&(re.y<fr.y-epsilonRaw/2)) return true;
    if ((fabsl(fr.x-re.x)<epsilonRaw)&&(fabsl(fr.y-re.y)<epsilonRaw)&&(re.z<fr.z-epsilonRaw/2)) return true;
    return false;

}

bool operator>(const RawElement& re, const RawElement& fr) {

    return !(re<fr);

}

bool lessX(const RawElement& re, const RawElement& fr) {

    return re.x<fr.x;

}

bool lessY(const RawElement& re, const RawElement& fr) {

    return re.y<fr.y;

}

bool lessZ(const RawElement& re, const RawElement& fr) {

    return re.z<fr.z;

}

bool moreX(const RawElement& re, const RawElement& fr) {

    return re.x>fr.x;

}

bool moreY(const RawElement& re, const RawElement& fr) {

    return re.y>fr.y;

}

bool moreZ(const RawElement& re, const RawElement& fr) {

    return re.z>fr.z;

}

bool someMore(const RawElement& re, const RawElement& fr) {

    return ((re.x>fr.x)||(re.y>fr.y)||(re.z>fr.z));

}

RawHData::RawHData() {

    data.clear();

}

void RawHData::setData(double *x, double *y, double *z, double *v, int *r, unsigned long long n) {

    data.clear();

    for (unsigned long long i=0; i<n; i++) {
        RawElement el;
        el.x=x[i];
        el.y=y[i];
        el.z=z[i];
        el.val=v[i];
        el.res=r[i];
        data.push_back(el);
    }

    std::cout << data.size() << std::endl;

    std::sort(data.begin(),data.end());

    resIndex.clear();

    if (n>0) {

        resIndex.push_back(std::pair<DataPtr,int>(data.begin(),data[0].res));

        for (DataPtr i=data.begin(); i!=data.end(); i++) {

            if ((resIndex.back().second)!=(i->res)) {
                resIndex.push_back(std::pair<DataPtr,int>(i,i->res));
            }

        }

    }

//    for (unsigned int i=0; i<resIndex.size(); i++) {
//        std::cout << resIndex[i].second << std::endl;
//        std::cout << resIndex[i].first->res << std::endl << std::endl;
//    }

}

float RawHData::at(double x,double y,double z) {

    DataPtr index,startSearch,endSearch;
    int n(resIndex.size());

    for (int k=0; k<n; k++) {

        startSearch=resIndex[k].first;
        if (k<n-1) {
            endSearch=resIndex[k+1].first;
        } else {
            endSearch=data.end();
        }

        double res(powl(0.5,resIndex[k].second));

        RawElement pos;
        pos.x=floorl(x/res)*res;
        pos.y=floorl(y/res)*res;
        pos.z=floorl(z/res)*res;
        pos.val=0;
        pos.res=resIndex[k].second;

        index=(upper_bound(startSearch,endSearch,pos));
        if (index!=data.begin()) index--;

        if (index->isIn(x,y,z)) {

            return index->val;
        }

    }

    return 0;

}

void RawHData::infoPos(Coords cp,DataPtr& id) {

    double x(cp.xPos);
    double y(cp.yPos);
    double z(cp.zPos);
    id=data.end();
    DataPtr index,startSearch,endSearch;
    int n(resIndex.size());

    for (int k=0; k<n; k++) {

        startSearch=resIndex[k].first;
        if (k<n-1) {
            endSearch=resIndex[k+1].first;
        } else {
            endSearch=data.end();
        }

        double res(powl(0.5,resIndex[k].second));

        RawElement pos;
        pos.x=floorl(x/res)*res;
        pos.y=floorl(y/res)*res;
        pos.z=floorl(z/res)*res;
        pos.val=0;
        pos.res=resIndex[k].second;

        index=(upper_bound(startSearch,endSearch,pos));
        if (index!=data.begin()) index--;

        if (index->isIn(x,y,z)) {
            id=index;
            return;
        }

    }

    return;

}

void RawHData::makePreview(unsigned long long r, std::vector<char> & d) {

    // create a preview binary from data
    double delta=1.0l/r;
    union Conversion{
        char ch[4];
        float fl;
    };

    Conversion conv;
    for (unsigned long long x=0; x<r; x++) {
        for (unsigned long long y=0; y<r; y++) {
            for (unsigned long long z=0; z<r; z++) {
                conv.fl=at(x*delta+delta/2,y*delta+delta/2,z*delta+delta/2);
                d[(x*r*r+y*r+z)*4]=conv.ch[0];
                d[(x*r*r+y*r+z)*4+1]=conv.ch[1];
                d[(x*r*r+y*r+z)*4+2]=conv.ch[2];
                d[(x*r*r+y*r+z)*4+3]=conv.ch[3];
            }
        }
    }

}

void loadHDF5Emma(std::vector<std::string> s, RawHData *rd) {

    H5File file_d( s[0], H5F_ACC_RDONLY );
    H5File file_x( s[1], H5F_ACC_RDONLY );
    H5File file_y( s[2], H5F_ACC_RDONLY );
    H5File file_z( s[3], H5F_ACC_RDONLY );
    H5File file_l( s[4], H5F_ACC_RDONLY );

    DataSet dataset0 = file_d.openDataSet("data");
    DataSet dataset1 = file_x.openDataSet("data");
    DataSet dataset2 = file_y.openDataSet("data");
    DataSet dataset3 = file_z.openDataSet("data");
    DataSet dataset4 = file_l.openDataSet("data");

    DataSpace filespace0 = dataset0.getSpace();
    DataSpace filespace1 = dataset1.getSpace();
    DataSpace filespace2 = dataset2.getSpace();
    DataSpace filespace3 = dataset3.getSpace();
    DataSpace filespace4 = dataset4.getSpace();

    hsize_t dims[1];    // dataset dimensions

    filespace0.getSimpleExtentDims( dims, NULL);

    std::vector<double> data_out0=std::vector<double>(dims[0],0.0);  // buffer for dataset to be read
    std::vector<double> data_out1=std::vector<double>(dims[0],0.0);  // buffer for dataset to be read
    std::vector<double> data_out2=std::vector<double>(dims[0],0.0);  // buffer for dataset to be read
    std::vector<double> data_out3=std::vector<double>(dims[0],0.0);  // buffer for dataset to be read
    std::vector<int> data_out4=std::vector<int>(dims[0],0.0);  // buffer for dataset to be read

    double* d0=&(data_out0[0]);
    double* d1=&(data_out1[0]);
    double* d2=&(data_out2[0]);
    double* d3=&(data_out3[0]);
    int* d4=&(data_out4[0]);

    dataset0.read( d0, PredType::NATIVE_DOUBLE, filespace0, DataSpace::ALL );
    dataset1.read( d1, PredType::NATIVE_DOUBLE, filespace1, DataSpace::ALL );
    dataset2.read( d2, PredType::NATIVE_DOUBLE, filespace2, DataSpace::ALL );
    dataset3.read( d3, PredType::NATIVE_DOUBLE, filespace3, DataSpace::ALL );
    dataset4.read( d4, PredType::NATIVE_INT, filespace4, DataSpace::ALL );

    rd->setData(d1,d2,d3,d0,d4,dims[0]);

    std::cout << "RawData built!" << std::endl;

}

void loadHDF5Dahlia(std::vector<std::string> s, RawHData *rd) {

    H5File file_d( s[0], H5F_ACC_RDONLY );
    H5File file_x( s[1], H5F_ACC_RDONLY );
    H5File file_y( s[2], H5F_ACC_RDONLY );
    H5File file_z( s[3], H5F_ACC_RDONLY );
    H5File file_l( s[4], H5F_ACC_RDONLY );

    DataSet dataset0 = file_d.openDataSet("data");
    DataSet dataset1 = file_x.openDataSet("data");
    DataSet dataset2 = file_y.openDataSet("data");
    DataSet dataset3 = file_z.openDataSet("data");
    DataSet dataset4 = file_l.openDataSet("data");

    DataSpace filespace0 = dataset0.getSpace();
    DataSpace filespace1 = dataset1.getSpace();
    DataSpace filespace2 = dataset2.getSpace();
    DataSpace filespace3 = dataset3.getSpace();
    DataSpace filespace4 = dataset4.getSpace();

    hsize_t dims[1];    // dataset dimensions

    filespace0.getSimpleExtentDims( dims, NULL);

    std::vector<double> data_out0=std::vector<double>(dims[0],0.0);  // buffer for dataset to be read
    std::vector<double> data_out1=std::vector<double>(dims[0],0.0);  // buffer for dataset to be read
    std::vector<double> data_out2=std::vector<double>(dims[0],0.0);  // buffer for dataset to be read
    std::vector<double> data_out3=std::vector<double>(dims[0],0.0);  // buffer for dataset to be read
    std::vector<int> data_out4=std::vector<int>(dims[0],0.0);  // buffer for dataset to be read

    double* d0=&(data_out0[0]);
    double* d1=&(data_out1[0]);
    double* d2=&(data_out2[0]);
    double* d3=&(data_out3[0]);
    int* d4=&(data_out4[0]);

    dataset0.read( d0, PredType::NATIVE_DOUBLE, filespace0, DataSpace::ALL );
    dataset1.read( d1, PredType::NATIVE_DOUBLE, filespace1, DataSpace::ALL );
    dataset2.read( d2, PredType::NATIVE_DOUBLE, filespace2, DataSpace::ALL );
    dataset3.read( d3, PredType::NATIVE_DOUBLE, filespace3, DataSpace::ALL );
    dataset4.read( d4, PredType::NATIVE_INT, filespace4, DataSpace::ALL );

    double res;

    // coordinate correction: dahlia set cube coordinate at center and not at vertex
    for (unsigned long int i = 0; i<dims[0]; i++) {
        res=powl(0.5,d4[i]+1);
        d1[i]=d1[i]-res;
        d2[i]=d2[i]-res;
        d3[i]=d3[i]-res;
    }

    rd->setData(d1,d2,d3,d0,d4,dims[0]);

    std::cout << "RawData built!" << std::endl;

}


int RawHData::maxLev() {
    int level=-1;
    for (DataPtr i=data.begin(); i!=data.end(); i++)
        if ((i->res)>level) level=i->res;
    return level;
}

int RawHData::minLev() {
    int level=1e5;
    for (DataPtr i=data.begin(); i!=data.end(); i++)
        if ((i->res)<level) level=i->res;
    return level;
}

unsigned int RawHData::length() {
    return data.size();
}

double RawHData::max() {
    double mx(-1e63);
    for (DataPtr i=data.begin(); i!=data.end(); i++)
        if ((i->val)>mx) mx=i->val;
    return mx;
}

double RawHData::min() {
    double mn(1e63);
    for (DataPtr i=data.begin(); i!=data.end(); i++)
        if ((i->val)<mn) mn=i->val;
    return mn;
}

std::vector<int> RawHData::searchForLevel(double mn, double mx) {
    std::vector<int> report;
    int n(maxLev());
    report.resize(n,0);
    for (int i=0; i<n+1; i++) {
        report[i]=numLev(i,mn,mx);
    }
    return report;
}

int RawHData::numLev(int l, double mn, double mx) {
    int count(0);
    for(DataPtr i=data.begin(); i!=data.end(); i++) {
        if ((i->res==l)&&(i->val>mn)&&(i->val<mx)) count++;
    }
    return count;
}

std::vector<DataPtr> RawHData::listLev(int l) {
    std::vector<DataPtr> idList;
    for(DataPtr i=data.begin(); i!=data.end(); i++) {
        if (i->res==l) idList.push_back(i);
    }
    return idList;
}

