#include "Image.h"
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace cv;

class ImageTest {
  public:
    void displayFilter(Image img);
    void displayBW(Image img, bool zoom);
    void displayBox(Image img, std::vector<int> dims);
    void displayColor(Image img, int c);
    void test(const char* filename);
};

void ImageTest::displayColor(Image img, int c) {
  assert(img.depth==3);
  unsigned char data[img.width*img.height];
  for (int j = 0; j<img.height; j++) {
    for (int i = 0; i<img.width; i++) {
      data[j*img.width+i] = img.mat[i][j][c];
    }
  }
  cv::Mat A(img.height,img.width,CV_8UC1,&data);
  cv::imshow("window",A);
  cv::waitKey(0);
}

void ImageTest::displayBW(Image img, bool zoom = false) {
  assert(img.depth==1);
  unsigned char data[img.height*img.width];
  for (int j = 0; j<img.height; j++) {
    for (int i = 0; i<img.width; i++) {
      data[j*img.width+i] = img.mat[i][j][0];
    }
  }
  cv::Mat A(img.height,img.width,CV_8UC1,&data);
  if (zoom) {
    cv::namedWindow("zoomed",WINDOW_NORMAL);
    cv::imshow("zoomed",A);
    cv::resizeWindow("zoomed",600,600);
  }
  else
    cv::imshow("window",A);
  cv::waitKey(0);
}

void ImageTest::displayBox(Image img, std::vector<int> dims) {
  assert(img.depth==1);
  unsigned char data[img.height*img.width];
  for (int j = 0; j<img.height; j++) {
    for (int i = 0; i<img.width; i++) {
      data[j*img.width+i] = img.mat[i][j][0];
    }
  }
  cv::Mat A(img.height,img.width,CV_8UC1,&data);
  cv::namedWindow("window");
  cv::rectangle(
    A,
    cv::Point(dims[0],dims[1]),
    cv::Point(dims[0]+dims[2],dims[1]+dims[3]),
    cv::Scalar(255,255,255)
  );
  cv::imshow("window",A);
  cv::waitKey(0);
}

void ImageTest::displayFilter(Image img) {
  unsigned char data[img.height*img.width];
  for (int j = 0; j<img.height; j++) {
    for (int i = 0; i<img.width; i++) {
      data[j*img.width+i] = img.valid[i][j] ? 255 : 0;
    }
  }
  cv::Mat A(img.height,img.width,CV_8UC1,&data);
  cv::imshow("window",A);
  cv::waitKey(0);
}

void ImageTest::test (const char* filename) {
  /*
  cv::Mat img = cv::imread(filename,CV_LOAD_IMAGE_COLOR);
  std::vector< std::vector< std::vector<int> > > mat(img.cols,std::vector< std::vector<int> >(img.rows,std::vector<int>(3)));
  for (int i = 0; i<img.rows; i++) {
    for (int j = 0; j<img.cols; j++) {
      cv::Vec3b intensity = img.at<Vec3b>(i,j);
      mat[j][i][2] = intensity.val[0];
      mat[j][i][1] = intensity.val[1];
      mat[j][i][0] = intensity.val[2];
    }
  }
  */
  int width, height, bpp;
    uint8_t* rgb_image = stbi_load(filename, &width, &height, &bpp, 3);
    std::vector< std::vector< std::vector<int> > >mat2(width,std::vector< std::vector<int> >(height,std::vector<int>(3,0)));
    for (int i = 0; i<height; i++) {
      for (int j = 0; j<width; j++) {
        for (int k = 0; k<3; k++) { 
          mat2[j][i][k] = (int)(rgb_image[(i*width+j)*bpp+k]);
        }
      }
    }
  Image i(mat2);
  std::vector<int> dims(4,0);
  double tolerance = 0;
  Image bw = i;
  bw.flatten();
  bw.threshhold(235);
  //displayFilter(i);
  Image newimg = Image();
  while (dims.size()>0) {
      //find the largest (probably only) connected component
      Image newimg = Image();
      dims = bw.largestConnComp(newimg,50);
      //detect dominant red color
      int redCount = 0;
      for (int j = dims[0]; j<dims[0]+dims[2]; j++) {
        for (int k = dims[1]; k<dims[1]+dims[3]; k++) {
          if (i.mat[j][k][0]>=i.mat[j][k][1]&&i.mat[j][k][0]>=i.mat[j][k][2])
            redCount++;
        }
      }
    cout << ((redCount+0.0)/(dims[2]*dims[3])) << endl;
      cout << dims[0] << " " << dims[1] << endl;
      //if red is dominant, pointer detected
      if ((redCount+0.0)/(dims[2]*dims[3])>0.8&&dims[2]<10&&dims[3]<10)
        break;
    else {
        for (int j = dims[0]; j<dims[0]+dims[2]; j++) {
          for (int k = dims[1]; k<dims[1]+dims[3]; k++) {
            bw.valid[j][k] = false;
          }
        }
     }
    }
  displayBox(bw,dims);
  stbi_image_free(rgb_image);
  //displayColor(i,0);
  int rskin = 255;
  int gskin = 217;
  int bskin = 139;
  tolerance = 10;
  int rerode = 3;
  int rdilate = 5;
  int thresh = 50;
  int w = 16;
  int h = 16;
  int lim = 240;
  cv::Mat standard = cv::imread("/home/lpang/Documents/GitHub/LaserTurret/test/standard.jpg",CV_LOAD_IMAGE_COLOR);
  std::vector< std::vector< std::vector<int> > > stdmat(standard.cols,std::vector< std::vector<int> >(standard.rows,std::vector<int>(3)));
  for (int i = 0; i<standard.rows; i++) {
    for (int j = 0; j<standard.cols; j++) {
      cv::Vec3b intensity = standard.at<Vec3b>(i,j);
      stdmat[j][i][2] = intensity.val[0];
      stdmat[j][i][1] = intensity.val[1];
      stdmat[j][i][0] = intensity.val[2];
    }
  }
  Image temp(stdmat);
  temp.subtractColor(rskin,gskin,bskin);
  temp.flatten();
  temp.scaleDown(w,h);
  ofstream file;
  file.open("std.txt");
  file << temp.width << " " << temp.height << endl;
  for (int i = 0; i<temp.width; i++) {
    for (int j = 0; j<temp.height; j++) {
      file << temp.mat[i][j][0] << " ";
    }
    file << endl;
  }       
  file.close();
  int loss = 0;
  std::vector<int> params(3,0);
  int IMG_LEARNING_RATE = 0.1;
  //std::vector<int> dims = i.detectFace(temp,rskin,gskin,bskin,loss,params,IMG_LEARNING_RATE);
  //i.flatten();
  //displayBox(i,dims);
  //displayBW(temp,true);
  i.colorFilter(rskin,bskin,gskin,tolerance);
  displayFilter(i);
  i.erode(rerode);
  displayFilter(i);
  i.dilate(rdilate);
  displayFilter(i);
  //Image newimg = Image();
  dims = i.largestConnComp(newimg,INT_MAX);
  for (int i = 0; i<dims.size(); i++)
    cout << dims[i] << " ";
  cout << endl;
  newimg.subtractColor(rskin,gskin,bskin);
  newimg.flatten();
  //displayBW(newimg);
  newimg.threshhold(thresh);
  //displayFilter(newimg);
  newimg.scaleDown(w,h);
  displayBW(newimg,true);
  cout << newimg.hammingDist(temp) << endl;
  if (newimg.hammingDist(temp)<=lim) { 
    cout << "face detected" << endl;
  }
  i.flatten();
  displayBox(i,dims);
}

int main(const int argc, const char* const argv[]) {
  ImageTest i;
  i.test(argv[1]);
}