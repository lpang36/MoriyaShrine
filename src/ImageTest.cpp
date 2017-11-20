#include "Image.h"
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <assert.h>
#include <iostream>

using namespace std;
using namespace cv;

class ImageTest {
  public:
    void displayFilter(Image i);
    void displayBW(Image i);
    void test(string filename);
};

void ImageTest::displayBW(Image img) {
  assert(img.depth==1);
  int data[img.width][img.height];
  for (int i = 0; i<img.width; i++) {
    for (int j = 0; j<img.height; j++) {
      data[i][j] = img.mat[i][j][0];
    }
  }
  cv::Mat A(img.width,img.height,CV_8UC1,&data);
  cv::imshow("window",A);
  waitKey(0);
}

void ImageTest::displayFilter(Image img) {
  int data[img.width][img.height];
  for (int i = 0; i<img.width; i++) {
    for (int j = 0; j<img.height; j++) {
      data[i][j] = img.valid[i][j] ? 255 : 0;
    }
  }
  cv::Mat A(img.width,img.height,CV_8UC1,&data);
  cv::imshow("window",A);
  waitKey(0);
}

void ImageTest::test (string filename) {
  cv::Mat img = cv::imread("/home/lpang/Documents/GitHub/LaserTurret/test/"+filename,CV_LOAD_IMAGE_COLOR);
  unsigned char *input = (unsigned char*)(img.data);
  std::vector< std::vector< std::vector<int> > > mat(img.cols,std::vector< std::vector<int> >(img.rows,std::vector<int>(2)));
  for (int i = 0; i<img.rows; i++) {
    for (int j = 0; j<img.cols; j++) {
      mat[j][i][2] = input[img.step*j+i];
      mat[j][i][1] = input[img.step*j+i+1];
      mat[j][i][0] = input[img.step*j+i+2];
    }
  }
  Image i(mat);
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
  i.colorFilter(rskin,bskin,gskin,tolerance);
  displayFilter(i);
  i.erode(rerode);
  displayFilter(i);
  i.dilate(rdilate);
  displayFilter(i);
  Image newimg(std::vector< std::vector< std::vector<int> > >(w,std::vector< std::vector<int> >(h,std::vector<int>(1,0))));
  std::vector<int> dims = i.largestConnComp(newimg);
  for (int i = 0; i<dims.size(); i++)
    std::cout << dims[i] << " ";
  newimg.subtractColor(rskin,bskin,gskin);
  newimg.flatten();
  displayBW(i);
  newimg.threshhold(thresh);
  displayFilter(i);
  newimg.scaleDown(w,h);
  displayBW(i);
  if (newimg.hammingDist(temp)<=lim) 
    std::cout << "face detected" << endl;
}

int main() {
  ImageTest i;
  i.test("test.jpg");
}