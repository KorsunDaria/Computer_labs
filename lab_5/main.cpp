// #include <time.h>

// #include <iomanip>
// #include <iostream>
// #include <opencv2/opencv.hpp>

// using namespace cv;
// using namespace std;

// int main() {
//   VideoCapture video(0);
//   if (!video.isOpened()) {
//     cout << "Error: Cannot open camera!" << endl;
//     return 0;
//   }

//   // Создаем объекты для вычитания фона
//   Ptr<BackgroundSubtractor> backSub =
//       createBackgroundSubtractorMOG2(500, 16, false);
//   Mat foregroundMask, background;

//   int frames = 0;
//   double total_capture_time = 0;
//   double total_processing_time = 0;
//   double total_display_time = 0;
//   double total_wait_time = 0;

//   struct timespec start, end;
//   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
//   double program_start = start.tv_sec + 0.000000001 * start.tv_nsec;

//   while (true) {
//     // 1. Получение кадра с камеры
//     clock_gettime(CLOCK_MONOTONIC_RAW, &start);

//     Mat frame;
//     video >> frame;
//     if (frame.empty()) break;

//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double capture_time = (end.tv_sec - start.tv_sec) +
//                           0.000000001 * (end.tv_nsec - start.tv_nsec);

//     // 2. Обработка кадра - обнаружение движущихся объектов
//     clock_gettime(CLOCK_MONOTONIC_RAW, &start);

//     // Применяем вычитание фона
//     backSub->apply(frame, foregroundMask, 0.01);

//     // Уменьшаем шум с помощью морфологических операций
//     Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));
//     morphologyEx(foregroundMask, foregroundMask, MORPH_OPEN, kernel);
//     morphologyEx(foregroundMask, foregroundMask, MORPH_CLOSE, kernel);

//     // Размываем маску для объединения близких контуров
//     GaussianBlur(foregroundMask, foregroundMask, Size(15, 15), 0);

//     // Бинаризуем с порогом
//     threshold(foregroundMask, foregroundMask, 50, 255, THRESH_BINARY);

//     // Находим контуры на маске
//     vector<vector<Point>> contours;
//     findContours(foregroundMask, contours, RETR_EXTERNAL,
//     CHAIN_APPROX_SIMPLE);

//     // Создаем копию кадра для отрисовки результатов
//     Mat result = frame.clone();

//     // Объединяем близкие контуры и рисуем bounding boxes
//     vector<Rect> boundingRects;
//     for (size_t i = 0; i < contours.size(); i++) {
//       double area = contourArea(contours[i]);
//       if (area > 1000) {  // Увеличили минимальную площадь
//         Rect rect = boundingRect(contours[i]);
//         boundingRects.push_back(rect);
//       }
//     }

//     // Объединяем близкие прямоугольники
//     vector<Rect> mergedRects;
//     for (size_t i = 0; i < boundingRects.size(); i++) {
//       bool merged = false;
//       for (size_t j = 0; j < mergedRects.size(); j++) {
//         // Проверяем пересечение или близость прямоугольников
//         Rect intersection = boundingRects[i] & mergedRects[j];
//         if (intersection.area() > 0 ||
//             norm(Point(boundingRects[i].x + boundingRects[i].width / 2,
//                        boundingRects[i].y + boundingRects[i].height / 2) -
//                  Point(mergedRects[j].x + mergedRects[j].width / 2,
//                        mergedRects[j].y + mergedRects[j].height / 2)) < 100)
//                        {
//           // Объединяем прямоугольники
//           mergedRects[j] = mergedRects[j] | boundingRects[i];
//           merged = true;
//           break;
//         }
//       }
//       if (!merged) {
//         mergedRects.push_back(boundingRects[i]);
//       }
//     }

//     // Рисуем объединенные bounding boxes
//     int objectCount = 0;
//     for (size_t i = 0; i < mergedRects.size(); i++) {
//       if (mergedRects[i].area() >
//           1500) {  // Фильтр по площади объединенного объекта
//         rectangle(result, mergedRects[i], Scalar(0, 255, 0), 3);

//         // Добавляем номер объекта
//         string objText = "Obj " + to_string(++objectCount);
//         putText(result, objText, Point(mergedRects[i].x, mergedRects[i].y -
//         10),
//                 FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);

//         // Добавляем площадь
//         string areaText = "Area: " + to_string(mergedRects[i].area());
//         putText(result, areaText,
//                 Point(mergedRects[i].x,
//                       mergedRects[i].y + mergedRects[i].height + 20),
//                 FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
//       }
//     }

//     // Добавляем информационный текст
//     string infoText = "Objects: " + to_string(objectCount);
//     putText(result, infoText, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.8,
//             Scalar(0, 0, 255), 2);

//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double processing_time = (end.tv_sec - start.tv_sec) +
//                              0.000000001 * (end.tv_nsec - start.tv_nsec);

//     // 3. Показ результатов
//     clock_gettime(CLOCK_MONOTONIC_RAW, &start);

//     imshow("Original", frame);
//     imshow("Foreground Mask", foregroundMask);
//     imshow("Motion Detection", result);

//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double display_time = (end.tv_sec - start.tv_sec) +
//                           0.000000001 * (end.tv_nsec - start.tv_nsec);

//     // 4. Ожидание
//     clock_gettime(CLOCK_MONOTONIC_RAW, &start);

//     int key = waitKey(30);

//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double wait_time = (end.tv_sec - start.tv_sec) +
//                        0.000000001 * (end.tv_nsec - start.tv_nsec);

//     // Суммируем время (в секундах)
//     total_capture_time += capture_time;
//     total_processing_time += processing_time;
//     total_display_time += display_time;
//     total_wait_time += wait_time;
//     frames++;

//     if (key == 27) break;
//   }

//   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//   double program_end = end.tv_sec + 0.000000001 * end.tv_nsec;
//   double total_program_time = program_end - program_start;

//   // Расчет FPS
//   double real_fps = (total_program_time > 0) ? frames / total_program_time :
//   0;

//   // Вывод результатов производительности
//   cout << "=== BREAKDOWN BY OPERATION ===" << endl;
//   cout << "Capture time: " << fixed << setprecision(2)
//        << total_capture_time * 1000.0 << " ms (" << fixed << setprecision(1)
//        << (total_capture_time * 100.0 / total_program_time) << "%)" << endl;
//   cout << "Processing time: " << fixed << setprecision(2)
//        << total_processing_time * 1000.0 << " ms (" << fixed <<
//        setprecision(1)
//        << (total_processing_time * 100.0 / total_program_time) << "%)" <<
//        endl;
//   cout << "Display time: " << fixed << setprecision(2)
//        << total_display_time * 1000.0 << " ms (" << fixed << setprecision(1)
//        << (total_display_time * 100.0 / total_program_time) << "%)" << endl;
//   cout << "Wait time: " << fixed << setprecision(2) << total_wait_time *
//   1000.0
//        << " ms (" << fixed << setprecision(1)
//        << (total_wait_time * 100.0 / total_program_time) << "%)" << endl;
//   cout << "FPS: " << fixed << setprecision(2) << real_fps << endl;

//   video.release();
//   destroyAllWindows();
//   return 0;
// }

#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  // Открываем камеру (C++ стиль)
  cv::VideoCapture capture(0);
  if (!capture.isOpened()) {
    std::cerr << "Не удалось открыть камеру" << std::endl;
    return -1;
  }

  cv::namedWindow("Negative", cv::WINDOW_AUTOSIZE);

  cv::Mat frame, neg;
  int frame_count = 0;

  // Для измерения производительности
  auto start_time = std::chrono::steady_clock::now();
  double total_processing_time = 0;

  while (true) {
    // Захватываем кадр
    capture >> frame;
    if (frame.empty()) break;

    // Начало обработки
    auto processing_start = std::chrono::steady_clock::now();

    // Создаем негатив (простой способ)
    cv::bitwise_not(frame, neg);

    // Или альтернативно вручную:
    // neg = 255 - frame;

    // Конец обработки
    auto processing_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> processing_elapsed =
        processing_end - processing_start;
    total_processing_time += processing_elapsed.count();

    // Показываем результат
    cv::imshow("Negative", neg);
    frame_count++;

    // Обработка клавиш
    char c = (char)cv::waitKey(1);
    if (c == 27) break;  // Esc
  }

  // Расчет статистики
  auto end_time = std::chrono::steady_clock::now();
  std::chrono::duration<double> total_elapsed = end_time - start_time;

  double fps = frame_count / total_elapsed.count();
  double cpu_usage_percent =
      (total_processing_time / total_elapsed.count()) * 100.0;

  std::cout << "=== РЕЗУЛЬТАТЫ ===" << std::endl;
  std::cout << "Обработано кадров: " << frame_count << std::endl;
  std::cout << "Общее время: " << total_elapsed.count() << " сек" << std::endl;
  std::cout << "FPS: " << fps << std::endl;
  std::cout << "Загрузка CPU на обработку: " << cpu_usage_percent << "%"
            << std::endl;

  // Автоматическое освобождение ресурсов (не нужно release вручную)
  return 0;
}