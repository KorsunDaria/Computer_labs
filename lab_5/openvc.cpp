#include <time.h>

#include <iomanip>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
  VideoCapture video(0);
  if (!video.isOpened()) return 0;

  int frames = 0;
  double total_capture_time = 0;
  double total_processing_time = 0;
  double total_display_time = 0;
  double total_wait_time = 0;

  struct timespec start, end;
  struct timespec program_start, program_end;

  clock_gettime(CLOCK_MONOTONIC_RAW, &program_start);

  while (true) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    Mat frame;
    video >> frame;
    if (frame.empty()) break;

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    double capture_time = (end.tv_sec - start.tv_sec) +
                          0.000000001 * (end.tv_nsec - start.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    Mat edges;
    cvtColor(frame, edges, COLOR_BGR2GRAY);
    edges.convertTo(edges, -1, 1.5, 10);
    Canny(edges, edges, 50, 90, 3);

    Mat blured_backgroung;
    GaussianBlur(frame, blured_backgroung, Size(21, 21), 10);
    blured_backgroung = blured_backgroung * 0.3;
    blured_backgroung.setTo(Scalar(255, 255, 255), edges);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    double processing_time = (end.tv_sec - start.tv_sec) +
                             0.000000001 * (end.tv_nsec - start.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    imshow("Filter video", blured_backgroung);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    double display_time = (end.tv_sec - start.tv_sec) +
                          0.000000001 * (end.tv_nsec - start.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    int key = waitKey(5);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    double wait_time = (end.tv_sec - start.tv_sec) +
                       0.000000001 * (end.tv_nsec - start.tv_nsec);

    total_capture_time += capture_time;
    total_processing_time += processing_time;
    total_display_time += display_time;
    total_wait_time += wait_time;
    frames++;

    if (key == 27) break;
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &program_end);

  double total_program_time =
      (program_end.tv_sec - program_start.tv_sec) +
      0.000000001 * (program_end.tv_nsec - program_start.tv_nsec);

  double real_fps = frames / total_program_time;

  cout << "Capture time: " << fixed << setprecision(2)
       << total_capture_time * 1000.0 << " ms (" << fixed << setprecision(1)
       << (total_capture_time * 100.0 / total_program_time) << "%)" << endl;
  cout << "Processing time: " << fixed << setprecision(2)
       << total_processing_time * 1000.0 << " ms (" << fixed << setprecision(1)
       << (total_processing_time * 100.0 / total_program_time) << "%)" << endl;
  cout << "Display time: " << fixed << setprecision(2)
       << total_display_time * 1000.0 << " ms (" << fixed << setprecision(1)
       << (total_display_time * 100.0 / total_program_time) << "%)" << endl;
  cout << "Wait time: " << fixed << setprecision(2) << total_wait_time * 1000.0
       << " ms (" << fixed << setprecision(1)
       << (total_wait_time * 100.0 / total_program_time) << "%)" << endl;
  cout << "FPS: " << fixed << setprecision(2) << real_fps << endl;

  video.release();
  destroyAllWindows();
  return 0;
}