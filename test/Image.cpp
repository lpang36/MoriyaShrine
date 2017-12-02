#include "Image.h"

using namespace std;

Image::Image() {
}

Image::Image(std::vector< std::vector< std::vector<int> > > img) {
  mat = img;
  width = img.size();
  height = img[0].size();
  depth = img[0][0].size();
  valid = std::vector< std::vector<bool> >(width,std::vector<bool>(height,true));
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
  assert(depth==3);
  double tuple[3];
  tuple[0] = log(g+1);
  tuple[1] = log(r+1)-log(g+1);
  tuple[2] = log(b+1)-(log(r+1)+log(g+1))/2;
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      double sample[3];
      sample[0] = log(mat[i][j][1]+1);
      sample[1] = log(mat[i][j][0]+1)-sample[0];
      sample[2] = log(mat[i][j][2]+1)-(sample[0]+sample[1])/2;
      double dist = sqrt((sample[0]-tuple[0])*(sample[0]-tuple[0])/(tuple[0]*tuple[0])+(sample[1]-tuple[1])*(sample[1]-tuple[1])/(tuple[1]*tuple[1])+(sample[2]-tuple[2])*(sample[2]-tuple[2])/(tuple[2]*tuple[2]));
      if (dist>tolerance) {
        valid[i][j] = false;
      }
    }
  }
}

void Image::erode(int r) {
  assert(r%2==1);
  int dim = r/2;
  std::vector< std::vector<bool> > temp = valid;
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      if (valid[i][j]) {
        for (int k = -dim; k<=dim; k++) {
          for (int l = -dim; l<=dim; l++) {
            if (!(i+k>=0&&i+k<width&&j+l>=0&&j+l<height&&valid[i+k][j+l])) 
              temp[i][j] = false;
          }
        }
      }
    }
  }
  valid = temp;
}

void Image::dilate(int r) {
  assert(r%2==1);
  int dim = r/2;
  std::vector< std::vector<bool> > temp = valid;
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      if (valid[i][j]) {
        for (int k = -dim; k<=dim; k++) {
          for (int l = -dim; l<=dim; l++) {
            if (i+k>=0&&i+k<width&&j+l>=0&&j+l<height)
              temp[i+k][j+l] = true;
          }
        }
      }
    }
  }
  valid = temp;
}

//finds the bounding box for the largest connected component of true pixels
//returns a vector in the form [smallest x value, smallest y value, width, height]
std::vector<int> Image::largestConnComp(Image& img, int maxSize = INT_MAX) {
  std::vector< std::vector<bool> > checked(width,std::vector<bool>(height,false));
  std::vector<int> wvals(0);
  std::vector<int> hvals(0);
  int wpoint = -1;
  int hpoint = -1;
  int max = 0;
  //finds size of largest connected component and keeps track of one pixel in that component
  //uses flood fill algorithm
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      if (!checked[i][j]&&valid[i][j]) {
        wvals.clear();
        hvals.clear();
        int count = 1;
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
        if (count>max&&count<=maxSize) {
          max = count;
          wpoint = i;
          hpoint = j;
        }
      }
    }
  }
  if (wpoint==-1&&hpoint==-1)
    return std::vector<int>(0);
  checked = std::vector <std::vector<bool> >(width,std::vector<bool>(height,false));
  wvals.clear();
  hvals.clear();
  int wmin = width;
  int wmax = 0;
  int hmin = height;
  int hmax = 0;
  wvals.push_back(wpoint);
  hvals.push_back(hpoint);
  checked[wpoint][hpoint] = true;
  int count = 1;
  //finds bounds on largest connected component starting with tracked pixel from above
  //again uses flood fill algorithm
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
  //writes all pixels and filter values within bounds to an image, passed by reference
  img.width = wmax-wmin+1;
  img.height = hmax-hmin+1;
  img.depth = depth;
  img.mat = std::vector< std::vector< std::vector<int> > >(img.width, std::vector< std::vector<int> >(img.height,std::vector<int>(depth)));
  img.valid = std::vector< std::vector<bool> >(img.width,std::vector<bool>(img.height,false));
  for (int i = 0; i<img.width; i++) {
    for (int j = 0; j<img.height; j++) {
      for (int k = 0; k<depth; k++) {
        img.mat[i][j][k] = mat[wmin+i][hmin+j][k];
      }
      img.valid[i][j] = valid[wmin+i][hmin+j];
    }
  }
  std::vector<int> out(0);
  out.push_back(wmin);
  out.push_back(hmin);
  out.push_back(img.width);
  out.push_back(img.height);
  return out;
}

void Image::subtractColor(int r, int g, int b) {
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      mat[i][j][0] = (mat[i][j][0]-r)*((mat[i][j][0]-r>0)*2-1);
      mat[i][j][1] = (mat[i][j][1]-g)*((mat[i][j][1]-g>0)*2-1);
      mat[i][j][2] = (mat[i][j][2]-b)*((mat[i][j][2]-b>0)*2-1);
    }
  }
}

std::vector<int> Image::averageColor(std::vector<int> bounds, bool val = true) {
  std::vector<int> avg(depth,0);
  int count = 0;
  for (int i = bounds[0]; i<bounds[0]+bounds[2]; i++) {
    for (int j = bounds[1]; j<bounds[1]+bounds[3]; j++) {
      if (val) {
        if (valid[i][j]) {
          count++;
          for (int k = 0; k<depth; k++) { 
            avg[k]+=mat[i][j][k];
          }
        }
      }
      else {
        if (!valid[i][j]) {
          count++;
          for (int k = 0; k<depth; k++) { 
            avg[k]+=mat[i][j][k];
          }
        }
      }
    }
  }
  if (count>0) {
    for (int i = 0; i<depth; i++) {
      avg[i] = avg[i]/count;
    }
  }
  return avg;
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
  assert(depth==1);
  std::vector< std::vector< std::vector<int> > > temp(w,std::vector< std::vector<int> >(h,std::vector<int>(depth,0)));
  std::vector< std::vector<bool> > tempValid(w,std::vector<bool>(h,true));
  int wratio = width/w;
  int hratio = height/h;
  for (int i = 0; i<w; i++) {
    for (int j = 0; j<h; j++) {
      int count = 0;
      for (int k = 0; k<wratio; k++) {
        for (int l = 0; l<hratio; l++) {
          if (i*wratio+k>=0&&i*wratio+k<width&&j*hratio+l>=0&&j*hratio+l<height&&valid[i*wratio+k][j*hratio+l]) {
            temp[i][j][0]+=mat[i*wratio+k][j*hratio+l][0];
            count++;
          }
        }
      }
      if (count!=0)
        temp[i][j][0] = temp[i][j][0]/count;
      if (count<wratio*hratio/2)
        tempValid[i][j] = false;
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

std::vector<int> Image::detectFace(Image& standard, int rskin, int gskin, int bskin, int& loss, std::vector<int>& params, const double IMG_LEARNING_RATE) {
  double tolerance = 16.5;
  int rerode = 9;
  int rdilate = 9;
  int thresh = 50;
  int w = 16;
  int h = 16;
  int lim = 240;
  colorFilter(rskin,bskin,gskin,tolerance);
  erode(rerode);
  dilate(rdilate);
  Image newimg = Image();
  std::vector<int> dims = largestConnComp(newimg);
  params = averageColor(dims);
  newimg.subtractColor(rskin,gskin,bskin);
  newimg.flatten();
  newimg.threshhold(thresh);
  newimg.scaleDown(w,h);
  loss = newimg.hammingDist(standard);
  double alpha = loss*IMG_LEARNING_RATE;
  for (int i = 0; i<w; i++) {
    for (int j = 0; j<h; j++) {
      standard.mat[i][j][0] = (int)(alpha*newimg.mat[i][j][0]+(1-alpha)*standard.mat[i][j][0]);
    }
  }
  if (loss<=lim) 
    return dims;
  return std::vector<int>(0);
}