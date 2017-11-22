#include "Image.h"
#include <vector>
#include <ostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <string>

using namespace std;

void turnMotor(double theta, double rho) {
}

string getCurrentTime() {
  time_t rawtime;
  char buffer[30];
  time(&rawtime);
  struct tm* timeinfo = localtime(&rawtime);
  strftime(buffer,30,"%c",timeinfo);
  return string(buffer,30);
}

int main() {
  const int CAMERA_WIDTH = 480;
  const int CAMERA_HEIGHT = 360;
  const double CAMERA_FOV_WIDTH = 120;
  const double CAMERA_FOV_HEIGHT = 90;
  ifstream file;
  ofstream log;
  file.open("std.txt");
  log.open("log.txt",std::ios::app);
  int w, h;
  file >> w;
  file >> h;
  std::vector< std::vector< std::vector<int> > >standard(w,std::vector< std::vector<int> >(h,std::vector<int>(1,0)));
  for (int i = 0; i<w; i++) {
    for (int j = 0; j<h; j++) {
      file >> standard[i][j][0];
    }
  }
  int count = 0;
  log << getCurrentTime << "Starting face detection." << endl;
  while(true) {
    //get image from camera somehow
    std::vector< std::vector< std::vector<int> > >mat(CAMERA_WIDTH,std::vector< std::vector<int> >(CAMERA_HEIGHT,std::vector<int>(3,0)));
    Image i(mat);
    std::vector<int> dims = i.detectFace(standard);
    if (dims.size()>0) {
      log << getCurrentTime << "Face detected in frame " << count << " with upper-left corner (" << dims[0] << ", " << dims[1] << ") and dimensions " << dims[2] << "x" << dims[3] << "." << endl;
      int aimWidth = dims[0]+dims[2]/2;
      int aimHeight = dims[1]+(int)(dims[3]*0.75); //small downward adjustment to prevent shining in eyes
      double theta = CAMERA_FOV_WIDTH*(aimWidth-CAMERA_WIDTH/2.0)/CAMERA_WIDTH;
      double rho = CAMERA_FOV_HEIGHT*(aimHeight-CAMERA_HEIGHT/2.0)/CAMERA_HEIGHT;
      turnMotor(theta,rho);
      log << getCurrentTime << "Motor turned by " << theta << " degrees in the x direction and " << rho << " degrees in the y direction." << endl;
    }
    else {
      log << getCurrentTime << "No face detected in frame " << count << "." << endl;
    }
    count++;
  }
  log << getCurrentTime << "Shutting down." << endl;
}