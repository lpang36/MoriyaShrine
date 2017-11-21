#include "Image.h"
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <assert.h>
#include <iostream>

using namespace std;
using namespace cv;

class ImageTest {
  public:
    void displayFilter(Image img);
    void displayBW(Image img);
    void displayColor(Image img, int c);
    void test(string filename);
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
  waitKey(0);
  delete &data;
}

void ImageTest::displayBW(Image img) {
  assert(img.depth==1);
  unsigned char data[img.height*img.width];
  for (int j = 0; j<img.height; j++) {
    for (int i = 0; i<img.width; i++) {
      data[j*img.width+i] = img.mat[i][j][0];
    }
  }
  cv::Mat A(img.height,img.width,CV_8UC1,&data);
  cv::imshow("window",A);
  waitKey(0);
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
  waitKey(0);
}

void ImageTest::test (string filename) {
  cv::Mat img = cv::imread("/home/lpang/Documents/GitHub/LaserTurret/test/"+filename,CV_LOAD_IMAGE_COLOR);
  std::vector< std::vector< std::vector<int> > > mat(img.cols,std::vector< std::vector<int> >(img.rows,std::vector<int>(3)));
  for (int i = 0; i<img.rows; i++) {
    for (int j = 0; j<img.cols; j++) {
      cv::Vec3b intensity = img.at<Vec3b>(i,j);
      mat[j][i][2] = intensity.val[0];
      mat[j][i][1] = intensity.val[1];
      mat[j][i][0] = intensity.val[2];
    }
  }
  Image i(mat);
  int rskin = 255;
  int gskin = 227;
  int bskin = 159;
  double tolerance = 1;
  int rerode = 19;
  int rdilate = 19;
  int thresh = 50;
  int w = 16;
  int h = 16;
  int lim = 100;
  Image temp(std::vector< std::vector< std::vector<int> > >(w,std::vector< std::vector<int> >(h,std::vector<int>(1,0))));
  i.colorFilter(rskin,bskin,gskin,tolerance);
  //displayFilter(i);
  i.erode(rerode);
  //displayFilter(i);
  i.dilate(rdilate);
  //displayFilter(i);
  Image newimg = Image();
  std::vector<int> dims = i.largestConnComp(newimg);
  for (int i = 0; i<dims.size(); i++)
    cout << dims[i] << " ";
  cout << endl;
  newimg.subtractColor(rskin,gskin,bskin);
  newimg.flatten();
  //displayBW(newimg);
  newimg.threshhold(thresh);
  //displayFilter(newimg);
  newimg.scaleDown(w,h);
  displayBW(newimg);
  if (newimg.hammingDist(temp)<=lim) 
    cout << "face detected" << endl;
}

int main() {
  ImageTest i;
  i.test("test.jpg");
}