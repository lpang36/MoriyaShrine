#include <math.h>
#include <assert.h>
#include <vector>
#include <iostream>

class Image {
  public:
    int width, height, depth;
    std::vector< std::vector< std::vector<int> > > mat;
    std::vector< std::vector<bool> > valid;
    Image();
    Image(std::vector< std::vector< std::vector<int> > > img);
    Image(std::vector< std::vector< std::vector<int> > > img, std::vector< std::vector<bool> > val);
    ~Image();
    void colorFilter(int r, int g, int b, double tolerance);
    void erode(int r);
    void dilate(int r);
    std::vector<int> largestConnComp(Image& img);
    void subtractColor(int r, int g, int b);
    void flatten();
    void threshhold(int thresh);
    void scaleDown(int w, int h);
    int hammingDist(Image img);
    std::vector<int> detectFace();
};
