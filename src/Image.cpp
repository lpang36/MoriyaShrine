#include "Image.h"

using namespace std;

Image::Image(std::vector< std::vector< std::vector<int> > > img) {
  mat = img;
  width = img.size();
  height = img[0].size();
  depth = img[0][0].size();
  std::vector< std::vector<bool> > valid(width,std::vector<bool>(height,true));
}

Image::Image(std::vector< std::vector< std::vector<int> > > img, std::vector< std::vector<bool> > val) {
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

std::vector<int> Image::largestConnComp(Image img) { //implement with check for if no conncomps
  std::vector< std::vector<bool> > checked(width,std::vector<bool>(height,false));
  std::vector<int> wvals(0);
  std::vector<int> hvals(0);
  int wpoint;
  int hpoint;
  int max = 0;
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      if (!checked[i][j]) {
        int count = 0;
        wvals.push_back(i);
        hvals.push_back(j);
        while (wvals.size()!=0&&hvals.size()!=0) {
          int k = wvals.back();
          wvals.pop_back();
          int l = hvals.back();
          hvals.pop_back();
          assert(wvals.size()==hvals.size());
          if (k+1>=0&&k+1<width&&l+1>=0&&l+1<height&&!checked[k+1][l+1]&&valid[k+1][l+1]) {
            checked[k+1][l+1] = true;
            wvals.push_back(k+1);
            hvals.push_back(l+1);
            count++;
          }
          if (k+1>=0&&k+1<width&&l-1>=0&&l-1<height&&!checked[k+1][l-1]&&valid[k+1][l-1]) {
            checked[k+1][l-1] = true;
            wvals.push_back(k+1);
            hvals.push_back(l-1);
            count++;
          }
          if (k-1>=0&&k-1<width&&l+1>=0&&l+1<height&&!checked[k-1][l+1]&&valid[k-1][l+1]) {
            checked[k-1][l+1] = true;
            wvals.push_back(k-1);
            hvals.push_back(l+1);
            count++;
          }
          if (k-1>=0&&k-1<width&&l-1>=0&&l-1<height&&!checked[k-1][l-1]&&valid[k-1][l-1]) {
            checked[k-1][l-1] = true;
            wvals.push_back(k-1);
            hvals.push_back(l-1);
            count++;
          }
        }
        if (count>max) {
          max = count;
          wpoint = i;
          hpoint = j;
        }
      }
    }
  }
  wvals.clear();
  hvals.clear();
  int wmin = width;
  int wmax = 0;
  int hmin = height;
  int hmax = 0;
  wvals.push_back(wpoint);
  hvals.push_back(hpoint);
  while (wvals.size()!=0&&hvals.size()!=0) {
    int k = wvals.back();
    wvals.pop_back();
    int l = hvals.back();
    hvals.pop_back();
    assert(wvals.size()==hvals.size());
    if (k<wmin)
      wmin = k;
    if (k>wmax)
      wmax = k;
    if (l<hmin)
      hmin = l;
    if (l>hmax)
      hmax = l;
    if (k+1>=0&&k+1<width&&l+1>=0&&l+1<height&&!checked[k+1][l+1]&&valid[k+1][l+1]) {
      wvals.push_back(k+1);
      hvals.push_back(l+1);
    }
    if (k+1>=0&&k+1<width&&l-1>=0&&l-1<height&&!checked[k+1][l-1]&&valid[k+1][l-1]) {
      wvals.push_back(k+1);
      hvals.push_back(l-1);
    }
    if (k-1>=0&&k-1<width&&l+1>=0&&l+1<height&&!checked[k-1][l+1]&&valid[k-1][l+1]) {
      wvals.push_back(k-1);
      hvals.push_back(l+1);
    }
    if (k-1>=0&&k-1<width&&l-1>=0&&l-1<height&&!checked[k-1][l-1]&&valid[k-1][l-1]) {
      wvals.push_back(k-1);
      hvals.push_back(l-1);
    }
  }
  img.width = wmax-wmin+1;
  img.height = hmax-hmin+1;
  img.depth = depth;
  std::vector< std::vector< std::vector<int> > > tempMat(img.width, std::vector< std::vector<int> >(img.height,std::vector<int>(depth,true)));
  std::vector< std::vector<bool> > tempValid(img.width,std::vector<bool>(img.height,false));
  img.mat = tempMat;
  img.valid = tempValid;
  for (int i = 0; i<img.width; i++) {
    for (int j = 0; j<img.width; j++) {
      img.mat[i][j] = mat[wmin+i][hmin+j];
      img.valid[i][j] = valid[wmin+i][hmin+j];
    }
  }
  std::vector<int> out(0);
  out.push_back(wmin);
  out.push_back(wmax);
  out.push_back(img.width);
  out.push_back(img.height);
  return out;
}

void Image::subtractColor(int r, int g, int b) {
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      mat[i][j][0]-=r;
      mat[i][j][1]-=g;
      mat[i][j][2]-=b;
      if (mat[i][j][0]<0)
        mat[i][j][0] = 0;
      if (mat[i][j][1]<0)
        mat[i][j][1] = 0;
      if (mat[i][j][2]<0)
        mat[i][j][2] = 0;
    }
  }
}

void Image::flatten() {
  std::vector< std::vector< std::vector<int> > > temp(width,std::vector< std::vector<int> >(height,std::vector<int>(1,0)));
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
  std::vector< std::vector< std::vector<int> > > temp(w,std::vector< std::vector<int> >(h,std::vector<int>(depth,0)));
  std::vector< std::vector<bool> > tempValid(w,std::vector<bool>(h,true));
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
        temp[i][j][0] = temp[i][j][0]/count;
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

std::vector<int> Image::detectFace() {
  int rskin = 0;
  int gskin = 0;
  int bskin = 0;
  double tolerance = 50;
  int rerode = 19;
  int rdilate = 19;
  int thresh = 100;
  int w = 16;
  int h = 16;
  int lim = 100;
  Image temp(std::vector< std::vector< std::vector<int> > >(w,std::vector< std::vector<int> >(h,std::vector<int>(1,0))));
  colorFilter(rskin,bskin,gskin,tolerance);
  erode(rerode);
  dilate(rdilate);
  Image newimg(std::vector< std::vector< std::vector<int> > >(w,std::vector< std::vector<int> >(h,std::vector<int>(1,0))));
  std::vector<int> dims = largestConnComp(newimg);
  newimg.subtractColor(rskin,bskin,gskin);
  newimg.flatten();
  newimg.threshhold(thresh);
  newimg.scaleDown(w,h);
  if (newimg.hammingDist(temp)<=lim) 
    return dims;
  return std::vector<int>(4,0);
}
