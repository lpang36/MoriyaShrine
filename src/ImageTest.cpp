#include "Image.h"
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <assert.h>
#include <iostream>

using namespace std;
using namespace cv;

class ImageTest {
  public:
    void displayFilter(Image img);
    void displayBW(Image img, bool zoom);
    void displayBox(Image img, std::vector<int> dims);
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
  double tolerance = 16.5;
  int rerode = 9;
  int rdilate = 9;
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
  //displayBW(temp,true);
  i.colorFilter(rskin,bskin,gskin,tolerance);
  displayFilter(i);
  i.erode(rerode);
  displayFilter(i);
  i.dilate(rdilate);
  displayFilter(i);
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