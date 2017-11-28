#include "Image.h"
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include <ostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <string>
#include <math.h>

using namespace std;

const int CAMERA_WIDTH = 640;
const int CAMERA_HEIGHT = 480;
const double RGB_LEARNING_RATE = 0.1;
const double IMG_LEARNING_RATE = 0.1;
const int R_SKIN_INIT = 255;
const int G_SKIN_INIT = 227;
const int B_SKIN_INIT = 159;
const int R_LASER = 255;
const int G_LASER = 255;
const int B_LASER = 255;
const char* LOG_FILE_NAME = "logfile.txt";
const char* OUTPUT_FILE_NAME = "output.txt";
const char* STD_FILE_NAME = "std.txt";

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

std::vector<int> Image::largestConnComp(Image& img) {
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
  checked = std::vector <std::vector<bool> >(width,std::vector<bool>(height,false));
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
      checked[k+1][l+1] = true;
      wvals.push_back(k+1);
      hvals.push_back(l+1);
    }
    if (k+1>=0&&k+1<width&&l-1>=0&&l-1<height&&!checked[k+1][l-1]&&valid[k+1][l-1]) {
      checked[k+1][l-1] = true;
      wvals.push_back(k+1);
      hvals.push_back(l-1);
    }
    if (k-1>=0&&k-1<width&&l+1>=0&&l+1<height&&!checked[k-1][l+1]&&valid[k-1][l+1]) {
      checked[k-1][l+1] = true;
      wvals.push_back(k-1);
      hvals.push_back(l+1);
    }
    if (k-1>=0&&k-1<width&&l-1>=0&&l-1<height&&!checked[k-1][l-1]&&valid[k-1][l-1]) {
      checked[k-1][l-1] = true;
      wvals.push_back(k-1);
      hvals.push_back(l-1);
    }
  }
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

string getCurrentTime() {
  time_t rawtime;
  char buffer[25];
  time(&rawtime);
  struct tm* timeinfo = localtime(&rawtime);
  strftime(buffer,25,"%c",timeinfo);
  return string(buffer)+": ";
}

int main(const int argc, const char* const argv[]) {
  int r = R_SKIN_INIT;
  int g = G_SKIN_INIT;
  int b = B_SKIN_INIT;
  int choice = atoi(argv[1]);
  const char* IMAGE_FILE_NAME = argv[2];
  ifstream file;
  ofstream logfile;
  ofstream output;
  file.open(STD_FILE_NAME);
  logfile.open(LOG_FILE_NAME,std::ios::app);
  int w, h;
  file >> w;
  file >> h;
  std::vector< std::vector< std::vector<int> > >standardMat(w,std::vector< std::vector<int> >(h,std::vector<int>(1,0)));
  for (int i = 0; i<w; i++) {
    for (int j = 0; j<h; j++) {
      file >> standardMat[i][j][0];
    }
  }
  file.close();
  Image standard(standardMat);
  int count = 0;
  logfile << getCurrentTime() << "Starting face detection." << endl;
  while(true) {
    int width, height, bpp;
    uint8_t* rgb_image = stbi_load(IMAGE_FILE_NAME, &width, &height, &bpp, 3);
    std::vector< std::vector< std::vector<int> > >mat(CAMERA_WIDTH,std::vector< std::vector<int> >(CAMERA_HEIGHT,std::vector<int>(3,0)));
    for (int i = 0; i<CAMERA_WIDTH; i++) {
      for (int j = 0; j<CAMERA_HEIGHT; j++) {
        for (int k = 0; k<3; k++) { 
          mat[i][j][k] = (int)(rgb_image[(i*CAMERA_HEIGHT+j)*3+k]);
        }
      }
    }
    stbi_image_free(rgb_image);
    logfile << getCurrentTime() << "Read frame." << endl;
    Image i(mat);
    std::vector<int> dims(0,4);
    if (choice==0) {
      int loss = 0;
      std::vector<int> params(3,0);
      dims = i.detectFace(standard,r,g,b,loss,params,IMG_LEARNING_RATE);
      if (dims.size()>0) {
        double alpha = RGB_LEARNING_RATE*log(255-loss)/log(255);
        r = params[0]*alpha+r*(1-alpha);
        g = params[1]*alpha+g*(1-alpha);
        b = params[2]*alpha+b*(1-alpha);
        logfile << getCurrentTime() << "Adjusted skin color filter to (" << r << ", " << g << ", " << b << ")." << endl;
        logfile << getCurrentTime() << "Face detected in frame " << count << " with upper-left corner (" << dims[0] << ", " << dims[1] << ") and dimensions " << dims[2] << "x" << dims[3] << "." << endl;
      }
      else 
        logfile << getCurrentTime() << "No face detected in frame " << count << "." << endl;
    }
    else {
      Image bw = i;
      bw.flatten();
      bw.threshhold(200);
      Image newimg = Image();
      dims = bw.largestConnComp(newimg);
      if (dims.size()>0)
        logfile << getCurrentTime() << "Laser pointer detected in frame " << count << " at (" << (dims[0]+dims[2]/2) << ", " << (dims[1]+dims[3]/2) << ")." << endl;
      else
        logfile << getCurrentTime() << "No laser pointer detected in frame " << count << "." << endl;
    }
    std::vector<int> extended(4,0);
    extended[0] = max(dims[0]-20,0);
    extended[1] = max(dims[1]-20,0);
    extended[2] = min(dims[2]+40,CAMERA_WIDTH-extended[0]);
    extended[3] = min(dims[3]+40,CAMERA_HEIGHT-extended[1]);
    std::vector<int> avg = i.averageColor(extended,choice!=0);
    for (int i = 0; i<avg.size(); i++) 
      avg[i] = avg[i]*100/255;
    output.open(OUTPUT_FILE_NAME);
    output << avg[0] << endl << avg[1] << endl << avg[2] << endl;
    output.close();
    count++;
  }
  ofstream stdwrite;
  stdwrite.open(STD_FILE_NAME);
  stdwrite << standard.width << " " << standard.height << endl;
  for (int i = 0; i<standard.width; i++) {
    for (int j = 0; j<standard.height; j++) {
      stdwrite << standard.mat[i][j][0] << " ";
    }
    stdwrite << endl;
  }       
  stdwrite.close();
  logfile << getCurrentTime() << "Wrote standard image template." << endl;
  logfile << getCurrentTime() << "Shutting down." << endl;
  logfile.close();
}