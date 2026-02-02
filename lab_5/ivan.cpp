#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[]) {
  cv::VideoCapture camera(0);
  if (!camera.isOpened()) {
    std::cerr << "Could not open camera" << std::endl;
    return 0;
  }
  cv::namedWindow("image", cv::WINDOW_NORMAL);
  cv::Mat frame;
  int frame_count = 0;
  double total_time = 0.0;
  double input_time = 0.0;
  double processing_time = 0.0;
  double display_time = 0.0;
  double tick_frequency = (double)cv::getTickFrequency();  // 1e9
  double start_time = (double)cv::getTickCount();
  while (true) {
    double t0 = (double)cv::getTickCount();
    // Input
    camera >> frame;
    double t1 = (double)cv::getTickCount();
    if (frame.empty()) {
      break;
    }
    input_time += (t1 - t0) / tick_frequency;
    // Processing
    cv::flip(frame, frame, 1);

    frame *= 1.2;
    cv::Mat gray, edges;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(11, 11), 3);
    cv::Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
    cv::Sobel(gray, grad_x, CV_16S, 1, 0, 3);
    cv::Sobel(gray, grad_y, CV_16S, 0, 1, 3);
    cv::convertScaleAbs(grad_x, abs_grad_x);
    cv::convertScaleAbs(grad_y, abs_grad_y);
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edges);
    cv::bitwise_not(edges, edges);
    double t2 = (double)cv::getTickCount();
    processing_time += (t2 - t1) / tick_frequency;
    // Display
    cv::imshow("image", edges);
    double t3 = (double)cv::getTickCount();
    char c = (char)cv::waitKey(20);
    if (c == 27) {  // ESC key
      break;
    }
    display_time += (t3 - t2) / tick_frequency;

    frame_count++;
  }
  total_time = ((double)cv::getTickCount() - start_time) / tick_frequency;

  double fps = frame_count / total_time;
  double input_ratio = input_time / total_time * 100.0;
  double processing_ratio = processing_time / total_time * 100.0;
  double display_ratio = display_time / total_time * 100.0;
  double stall_time =
      total_time - (input_time + processing_time + display_time);
  double stall_ratio = stall_time / total_time * 100.0;

  std::cout << "Frame count: " << frame_count << std::endl;
  std::cout << "Total time: " << total_time << " sec" << std::endl;
  std::cout << "FPS: " << fps << std::endl;
  std::cout << "Input time: " << input_time << " sec (" << input_ratio << " %)"
            << std::endl;
  std::cout << "Processing time: " << processing_time << " sec ("
            << processing_ratio << " %)" << std::endl;
  std::cout << "Display time: " << display_time << " sec (" << display_ratio
            << " %)" << std::endl;
  std::cout << "Stall time: " << stall_time << " sec (" << stall_ratio << " %)"
            << std::endl;
  return 0;
}