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

//define parameters
int CAMERA_WIDTH = 640;
int CAMERA_HEIGHT = 480;
int EXTEND_DIMS = 5;
const double RGB_LEARNING_RATE = 0.1;
const double IMG_LEARNING_RATE = 0.1;
const int R_SKIN_INIT = 255;
const int G_SKIN_INIT = 217;
const int B_SKIN_INIT = 139;
const int R_LASER = 255;
const int G_LASER = 255;
const int B_LASER = 255;
const char* LOG_FILE_NAME = "logfile.txt";
const char* OUTPUT_FILE_NAME = "output.txt";
const char* STD_FILE_NAME = "std.txt";

//empty default constructor
Image::Image() {
}

//constructor with image vector
Image::Image(std::vector< std::vector< std::vector<int> > > img) {
  mat = img;
  width = img.size();
  height = img[0].size();
  depth = img[0][0].size();
  valid = std::vector< std::vector<bool> >(width,std::vector<bool>(height,true));
}

//constructor with image and filter vectors
Image::Image(std::vector< std::vector< std::vector<int> > > img, std::vector< std::vector<bool> > val) {
  mat = img;
  width = img.size();
  height = img[0].size();
  depth = img[0][0].size();
  valid = val;
}

//empty default destructor
Image::~Image() {
}

//filter based on color
//if color is not sufficiently close, filter for pixel is set to false
void Image::colorFilter(int r, int g, int b, double tolerance) {
  assert(depth==3);
  double tuple[3];
  tuple[0] = log(g+1);
  tuple[1] = log(r+1)-log(g+1);
  tuple[2] = log(b+1)-(log(r+1)+log(g+1))/2;
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      double sample[3];
      //logarithmic method based on work of Darrell et al. (1998)
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

//erosion operation
//if not all pixels within an area of rxr are true, that pixel is set to false
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

//dilation operation
//for every true pixel, set all pixels within an area of rxr to true
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

//subtracts an rgb value from all pixels
void Image::subtractColor(int r, int g, int b) {
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      mat[i][j][0] = (mat[i][j][0]-r)*((mat[i][j][0]-r>0)*2-1);
      mat[i][j][1] = (mat[i][j][1]-g)*((mat[i][j][1]-g>0)*2-1);
      mat[i][j][2] = (mat[i][j][2]-b)*((mat[i][j][2]-b>0)*2-1);
    }
  }
}

//average color of image within some bounds
//returns vector in format [average r, average g, average b]
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

//converts to grayscale by averaging r, g, b
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

//threshholds grayscale image
void Image::threshhold(int thresh) {
  assert(depth==1);
  for (int i = 0; i<width; i++) {
    for (int j = 0; j<height; j++) {
      if (mat[i][j][0]<thresh)
        valid[i][j] = false;
    }
  }
}

//scales image down to given size
//basic averaging algorithm
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

//computes sum of absolute differences between each pixel in two images
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

//detects face in image and returns its bounds
//returns a vector in the form [smallest x value, smallest y value, width, height]
//based on work of Darrell et al. (1998)
std::vector<int> Image::detectFace(Image& standard, int rskin, int gskin, int bskin, int& loss, std::vector<int>& params, const double IMG_LEARNING_RATE, ofstream* logfile, string time) {
  //various parameters
  double tolerance = 10;
  int rerode = 3;
  int rdilate = 5;
  int thresh = 50;
  int w = 16;
  int h = 16;
  int lim = 200;
  //filters image based on similarity to skin color
  colorFilter(rskin,bskin,gskin,tolerance);
  *logfile << time << "Applied color filter to image." << endl;
  //image opening (erosion followed by dilation)
  //generally eliminates noise
  erode(rerode);
  dilate(rdilate);
  Image newimg = Image();
  //finds the largest connected component
  //hopefully, this is where the face is
  std::vector<int> dims = largestConnComp(newimg);
  if (dims.size()>0) {
    *logfile << time << "Found possible face candidate." << endl;
    //creates a mask of the face by subtracting average face color
    params = averageColor(dims);
    newimg.subtractColor(rskin,gskin,bskin);
    //convert to grayscale
    newimg.flatten();
    //threshhold to eliminate noise
    newimg.threshhold(thresh);
    //scale down to compare to standard face mask
    newimg.scaleDown(w,h);
    *logfile << time << "Created face mask from image." << endl;
    //compare to standard face mask
    loss = newimg.hammingDist(standard);
    *logfile << time << "Compared image mask to standard face mask." << endl;
    //if face is close enough to standard mask, return bounds
    if (loss<=lim) {
      //update standard mask 
      //weighted average between standard mask and mask in current image
      double alpha = loss*IMG_LEARNING_RATE;
      for (int i = 0; i<w; i++) {
        for (int j = 0; j<h; j++) {
          standard.mat[i][j][0] = (int)(alpha*newimg.mat[i][j][0]+(1-alpha)*standard.mat[i][j][0]);
        }
      }
      return dims;
    }
    else
      *logfile << time << "Warning: Face candidate not sufficiently similar to standard face mask." << endl;
  }
  //return empty vector
  *logfile << time << "Warning: No face candidate found." << endl;
  return std::vector<int>(0);
}

//gets current local time as string
string getCurrentTime() {
  time_t rawtime;
  char buffer[25];
  time(&rawtime);
  struct tm* timeinfo = localtime(&rawtime);
  strftime(buffer,25,"%c",timeinfo);
  return string(buffer)+": ";
}

int main(const int argc, const char* const argv[]) {
  //various parameters
  int r = R_SKIN_INIT;
  int g = G_SKIN_INIT;
  int b = B_SKIN_INIT;
  ifstream file;
  ofstream logfile;
  ofstream output;
  file.open(STD_FILE_NAME);
  logfile.open(LOG_FILE_NAME,std::ios::app);
  if (argc!=3) {
    logfile << getCurrentTime() << "Error: Insufficient command line arguments." << endl;
    return -1;
  }
  int choice = atoi(argv[1]);
  const char* IMAGE_FILE_NAME = argv[2];
  //read standard face mask from file
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
  logfile << getCurrentTime() << "Read image file." << endl;
  Image standard(standardMat);
  int count = 0;
  //read jpg file using the stb_image library
  int bpp;
  uint8_t* rgb_image = stbi_load(IMAGE_FILE_NAME, &CAMERA_WIDTH, &CAMERA_HEIGHT, &bpp, 3);
  std::vector< std::vector< std::vector<int> > >mat(CAMERA_WIDTH,std::vector< std::vector<int> >(CAMERA_HEIGHT,std::vector<int>(3,0)));
  for (int i = 0; i<CAMERA_HEIGHT; i++) {
    for (int j = 0; j<CAMERA_WIDTH; j++) {
      for (int k = 0; k<3; k++) { 
        mat[j][i][k] = rgb_image[(i*CAMERA_WIDTH+j)*3+k];
      }
    }
  }
  stbi_image_free(rgb_image);
  logfile << getCurrentTime() << "Created image object." << endl;
  Image i(mat);
  std::vector<int> dims(4,0);
  //face detection if command line argument is 0
  if (choice==0) {
    logfile << getCurrentTime() << "Starting face detection." << endl;
    EXTEND_DIMS = 5;
    int loss = 0;
    std::vector<int> params(3,0);
    //detect face
    dims = i.detectFace(standard,r,g,b,loss,params,IMG_LEARNING_RATE,&logfile,getCurrentTime());
    //if face detection is successful
    if (dims.size()>0) {
      EXTEND_DIMS = 20;
      //update average face colour
      //weighted average between standard color and average color in image
      double alpha = RGB_LEARNING_RATE*log(255-loss)/log(255);
      r = params[0]*alpha+r*(1-alpha);
      g = params[1]*alpha+g*(1-alpha);
      b = params[2]*alpha+b*(1-alpha);
      logfile << getCurrentTime() << "Adjusted skin color filter to (" << r << ", " << g << ", " << b << ")." << endl;
      logfile << getCurrentTime() << "Face detected in frame " << count << " with upper-left corner (" << dims[0] << ", " << dims[1] << ") and dimensions " << dims[2] << "x" << dims[3] << "." << endl;
    }
    else 
      logfile << getCurrentTime() << "Warning: No face detected in frame " << count << "." << endl;
  }
  //laser pointer detection if command line argument is 1
  else if (choice==1) {
    logfile << getCurrentTime() << "Starting laser pointer detection." << endl;
    EXTEND_DIMS = 5;
    Image bw = i;
    //convert to grayscale
    bw.flatten();
    //threshhold to find bright point (i.e. the laser pointer, hopefully)
    bw.threshhold(235);
    while (dims.size()>0) {
      //find the largest (probably only) connected component
      Image newimg = Image();
      dims = bw.largestConnComp(newimg,50);
      logfile << getCurrentTime() << "Found possible laser pointer candidate." << endl;
      //detect dominant red color
      int redCount = 0;
      for (int j = dims[0]; j<dims[0]+dims[2]; j++) {
        for (int k = dims[1]; k<dims[1]+dims[3]; k++) {
          if (i.mat[j][k][0]>=i.mat[j][k][1]&&i.mat[j][k][0]>=i.mat[j][k][2])
            redCount++;
        }
      }
      //if red is dominant and area is small, pointer detected
      if ((redCount+0.0)/(dims[2]*dims[3])>0.8&&dims[2]<10&&dims[3]<10)
        break;
      //otherwise, set current connected component to false
      else {
        logfile << getCurrentTime() << "Candidate was not a laser pointer. Retrying." << endl;
        for (int j = dims[0]; j<dims[0]+dims[2]; j++) {
          for (int k = dims[1]; k<dims[1]+dims[3]; k++) {
            bw.valid[j][k] = false;
          }
        }
      }
    }
    //if laser pointer detection successful
    if (dims.size()>0)
      logfile << getCurrentTime() << "Laser pointer detected in frame " << count << " at (" << (dims[0]+dims[2]/2) << ", " << (dims[1]+dims[3]/2) << ")." << endl;
    else
      logfile << getCurrentTime() << "Warning: No laser pointer detected in frame " << count << "." << endl;
  }
  //averaging over entire image
  else {
    logfile << getCurrentTime() << "Averaging over entire image." << endl;
    EXTEND_DIMS = 0;
    dims[0] = 0;
    dims[1] = 0;
    dims[2] = CAMERA_WIDTH;
    dims[3] = CAMERA_HEIGHT;
  }
  //dimensions of an area surrounding target detection
  //whether face or laser pointer
  logfile << getCurrentTime() << "Computing average color around target." << endl;
  std::vector<int> extended(4,0);
  extended[0] = max(dims[0]-EXTEND_DIMS,0);
  extended[1] = max(dims[1]-EXTEND_DIMS,0);
  extended[2] = min(dims[2]+2*EXTEND_DIMS,CAMERA_WIDTH-extended[0]);
  extended[3] = min(dims[3]+2*EXTEND_DIMS,CAMERA_HEIGHT-extended[1]);
  //compute average color in area
  std::vector<int> avg = i.averageColor(extended,choice!=0);
  //scale down from 0-255 to 0-100
  for (int i = 0; i<avg.size(); i++) 
    avg[i] = avg[i]*100/255;
  //calibrations
  avg[1] = (int)(0.75*avg[1]);
  if (choice==1)
    avg[0] = (int)(0.5*avg[0]);
  //color adjustments for emphasis
  if (avg[0]>=avg[1]&&avg[0]>=avg[2])
    avg[0] = min(2*avg[0],100);
  if (avg[1]>=avg[0]&&avg[1]>=avg[2])
    avg[1] = min(2*avg[1],100);
  if (avg[2]>=avg[0]&&avg[2]>=avg[1])
    avg[2] = min(2*avg[2],100);
  logfile << getCurrentTime() << "Wrote RGB color (" << avg[0] << ", " << avg[1] << ", " << avg[2] << ") to LED." << endl;
  //write to output file for led control
  output.open(OUTPUT_FILE_NAME);
  output << avg[0] << endl << avg[1] << endl << avg[2] << endl;
  output.close();
  //write updated standard face mask to file
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
  logfile << getCurrentTime() << "Updated standard image template." << endl;
  logfile.close();
}