#include <opencv2/opencv.hpp>

using namespace cv;

int main() {
  VideoCapture capture(0);
  if (!capture.isOpened()) return 0;

  while (true) {
    Mat frame;
    capture >> frame;
    if (frame.empty()) break;

    imshow("test", frame);
    char c = waitKey(33);
    if (c == 27) break;
  }

  capture.release();
  destroyAllWindows();
  return 0;
}