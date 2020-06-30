#ifndef PUBHEADER_H
#define PUBHEADER_H
#include <vector>
#include <QStringList>
#include <QDir>
using namespace std;

struct ObjDet
{
    int idx;
    int id;
    float ltx;
    float lty;
    float width;
    float height;
    float confidence;
    bool added = false;
};
typedef vector<ObjDet> ImgDetVec;  //存放同一张图片中的边界信息
extern vector<ImgDetVec> all_det_vec;     //用于存放按图片名分组后的边信息

#endif // PUBHEADER_H
