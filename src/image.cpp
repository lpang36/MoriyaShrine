#include <math.h>
#include <assert.h>

using namespace std;

class Image {
  private:
    int width, height, depth;
    vector<vector<vector<int>>> mat;
    vector<vector<bool>> valid;
  public:
    Image(vector<vector<vector<int>>> img);
    Image(vector<vector<vector<int>>> img, vector<vector<bool>> val);
    ~Image();
    void colorFilter(int r, int g, int b, double tolerance);
    void erode(int r);
    void dilate(int r);
    vector<int> largestConnComp(Image img);
    void subtractColor(int r, int g, int b);
    void flatten();
    void threshhold(int thresh);
    void scaleDown(int w, int h);
    int hammingDist(Image img);
    vector<int> detectFace();
}

Image::Image(vector<vector<vector<int>>> img) {
  mat = img;
  width = img.size();
  height = img[0].size();
  depth = img[0][0].size();
  valid(width,vector<bool>(depth,true));
}

Image::Image(vector<vector<vector<int>>> img, vector<vector<bool>> val) {
  mat = img;
  width = img.size();
  height = img[0].size();
  depth = img[0][0].size();
  valid = val;
}

Image::~Image() {
}

void Image::colorFilter(int r, int g, int b, double tolerance) {
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      double dist = sqrt((mat[i][j][0]-r)*(mat[i][j][0]-r)+(mat[i][j][1]-g)*(mat[i][j][1]-g)+(mat[i][j][2]-b)*(mat[i][j][2]-b));
      if (dist>tolerance) {
        valid[width][height] = false;
      }
    }
  }
}

void Image::erode(int r) {
  assert(r%2==1);
  int dim = r/2;
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      if (valid[i][j]) {
        for (int k = -dim; k<=dim; k++) {
          for (int l = -dim; l<=dim; l++) {
            if (!(i+k>=0&&i+k<width&&j+l>=0&&j+l<height&&valid[i+k][j+l])) 
              valid[i][j] = false;
          }
        }
      }
    }
  }
}

void Image::dilate(int r) {
  assert(r%2==1);
  int dim = r/2;
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      if (valid[i][j]) {
        for (int k = -dim; k<=dim; k++) {
          for (int l = -dim; l<=dim; l++) {
            if (i+k>=0&&i+k<width&&j+l>=0&&j+l<height)
              valid[i+k][j+l] = true;
          }
        }
      }
    }
  }
}

vector<int> Image::largestConnComp(Image img) { //implement with check for if no conncomps
}

void Image::subtractColor(int r, int g, int b) {
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      mat[i][j][0]-=r;
      mat[i][j][1]-=g;
      mat[i][j][2]-=b;
    }
  }
}

void Image::flatten() {
  vector<vector<vector<int>>> temp(width,vector<vector<int>>(height,vector<int>(1,0)));
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      for (int k = 0; k<depth; k++) {
        temp[i][j][0]+=mat[i][j][k];
      }
      temp[i][j][0] = temp[i][j][0]/depth; 
    }
  }
  mat = temp;
  depth = 1;
}

void Image::threshhold(int thresh) {
  assert(depth==1);
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      if (mat[i][j][0]<thresh)
        valid[i][j] = false;
    }
  }
}

void Image::scaleDown(int w, int h) {
  assert(w<=width);
  assert(h<=height);
  assert(depth==1);
  vector<vector<vector<int>>> temp(w,vector<vector<int>>(h,vector<int>(depth,0)));
  vector<vector<bool>> tempValid(w,vector<int>(h,true));
  int wratio = width/w;
  int hratio = height/h;
  for (int i = 0; i<w; i++) {
    for (int j = 0; j<h; j++) {
      for (int k = 0; k<wratio; k++) {
        int count = 0;
        for (int l = 0; l<hratio; l++) {
          if (i*wratio+k>=0&&i*wratio+k<width&&j*hratio+l>=0&&j*hratio+l<height&&valid[i*wratio+k][j*hratio+l]) {
            temp[i][j][0]+=mat[i*wratio+k][j*hratio+l][0];
            count++;
          }
        }
        temp[i][j] = temp[i][j]/count;
        if (count<wratio*hratio/2)
          tempValid[i][j] = false;
      }
    }
  }
  mat = temp;
  valid = tempValid;
  width = w;
  height = h;
}

int Image::hammingDist(Image img) {
  assert(img.width==width);
  assert(img.height==height);
  assert(depth==1);
  int count = 0;
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      if ((img.mat[i][j][0]>0)^(valid[i][j]))
        count++;
    }
  }
  return count;
}

vector<int> Image::detectFace() {
  int rskin = 0;
  int gskin = 0;
  int bskin = 0;
  double tolerance = 50;
  int rerode = 19;
  int rdilate = 19;
  int thresh = 100;
  int w = 16;
  int h = 16;
  Image temp(vector<vector<vector<int>>>(w,vector<vector<int>>(h,vector<int>(1,0))));
  int lim = 100;
  colorFilter(rskin,bskin,gskin,tolerance);
  erode(rerode);
  dilate(rdilate);
  Image newimg(vector<vector<vector<int>>>(w,vector<vector<int>>(h,vector<int>(1,0))));
  vector<int> dims = largestConnComp(newimg);
  newimg.subtractColor(rskin,bskin,gskin);
  newimg.flatten();
  newimg.threshhold(thresh);
  newimg.scaleDown(w,h);
  if (newimg.hammingDist(temp)<=lim) 
    return dims;
  return vector<int>(4,0);
}