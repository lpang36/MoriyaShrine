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

void turnMotor(double theta, double rho) {
}

string getCurrentTime() {
  time_t rawtime;
  char buffer[25];
  time(&rawtime);
  struct tm* timeinfo = localtime(&rawtime);
  strftime(buffer,25,"%c",timeinfo);
  return string(buffer)+": ";
}

int main() {
  const int CAMERA_WIDTH = 480;
  const int CAMERA_HEIGHT = 360;
  const double CAMERA_FOV_WIDTH = 120;
  const double CAMERA_FOV_HEIGHT = 90;
  const double RGB_LEARNING_RATE = 0.1;
  const double IMG_LEARNING_RATE = 0.1;
  const int R_SKIN_INIT = 255;
  const int G_SKIN_INIT = 227;
  const int B_SKIN_INIT = 159;
  int r = R_SKIN_INIT;
  int g = G_SKIN_INIT;
  int b = B_SKIN_INIT;
  ifstream file;
  ofstream logfile;
  file.open("std.txt");
  logfile.open("logfile.txt",std::ios::app);
  int w, h;
  file >> w;
  file >> h;
  std::vector< std::vector< std::vector<int> > >standardMat(w,std::vector< std::vector<int> >(h,std::vector<int>(1,0)));
  for (int i = 0; i<w; i++) {
    for (int j = 0; j<h; j++) {
      file >> standardMat[i][j][0];
    }
  }
  Image standard(standardMat);
  int count = 0;
  logfile << getCurrentTime() << "Starting face detection." << endl;
  while(true) {
    int width, height, bpp;
    uint8_t* rgb_image = stbi_load("frame.jpg", &width, &height, &bpp, 3);
    std::vector< std::vector< std::vector<int> > >mat(CAMERA_WIDTH,std::vector< std::vector<int> >(CAMERA_HEIGHT,std::vector<int>(3,0)));
    for (int i = 0; i<CAMERA_WIDTH; i++) {
      for (int j = 0; j<CAMERA_HEIGHT; j++) {
        for (int k = 0; k<3; k++) { 
          mat[i][j][k] = (int)(rgb_image[i*CAMERA_HEIGHT+j*3+k]);
        }
      }
    }
    stbi_image_free(rgb_image);
    logfile << getCurrentTime() << "Read frame." << endl;
    Image i(mat);
    int loss = 0;
    std::vector<int> params(3,0);
    std::vector<int> dims = i.detectFace(standard,r,g,b,loss,params,IMG_LEARNING_RATE);
    if (dims.size()>0) {
      double alpha = RGB_LEARNING_RATE*log(255-loss)/log(255);
      r = params[0]*alpha+r*(1-alpha);
      g = params[1]*alpha+g*(1-alpha);
      b = params[2]*alpha+b*(1-alpha);
      logfile << getCurrentTime() << "Adjusted skin color filter to (" << r << ", " << g << ", " << b << ")." << endl;
      logfile << getCurrentTime() << "Face detected in frame " << count << " with upper-left corner (" << dims[0] << ", " << dims[1] << ") and dimensions " << dims[2] << "x" << dims[3] << "." << endl;
      int aimWidth = dims[0]+dims[2]/2;
      int aimHeight = dims[1]+(int)(dims[3]*0.75); //small downward adjustment to prevent shining in eyes
      double theta = CAMERA_FOV_WIDTH*(aimWidth-CAMERA_WIDTH/2.0)/CAMERA_WIDTH;
      double rho = CAMERA_FOV_HEIGHT*(aimHeight-CAMERA_HEIGHT/2.0)/CAMERA_HEIGHT;
      turnMotor(theta,rho);
      logfile << getCurrentTime() << "Motor turned by " << theta << " degrees in the x direction and " << rho << " degrees in the y direction." << endl;
    }
    else {
      logfile << getCurrentTime() << "No face detected in frame " << count << "." << endl;
    }
    count++;
  }
  ofstream stdwrite;
  stdwrite.open("std.txt");
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
}