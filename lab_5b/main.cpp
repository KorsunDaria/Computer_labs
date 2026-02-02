// g++ -std=c++11 cinematic_sepia_simple.cpp -o cinematic_sepia_simple
// `pkg-config --cflags --libs opencv4`

#include <ctime>
#include <iomanip>
#include <iostream>
#include <opencv2/opencv.hpp>

// Функция для получения текущего времени в секундах (double)
double get_current_time() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) / 1e9;
}

cv::Mat applyCinematicSepiaSimple(const cv::Mat& input) {
  cv::Mat output;

  cv::Mat kernel =
      (cv::Mat_<float>(3, 3) << 0.393, 0.349, 0.272,  // Более теплые тона
       0.769, 0.686, 0.534,   // Увеличенная насыщенность
       0.189, 0.168, 0.131);  // Глубокие тени

  cv::transform(input, output, kernel);

  // Простое повышение контраста вместо сложного виньетирования
  output.convertTo(output, -1, 1.15, 8);

  return output;
}

// Функция для добавления информации о FPS на изображение
void drawFPSInfo(cv::Mat& image, double fps, const std::string& window_name) {
  std::string fps_text = "FPS: " + std::to_string(fps).substr(0, 5);
  std::string name_text = window_name;

  // Настройки текста
  int font_face = cv::FONT_HERSHEY_SIMPLEX;
  double font_scale = 0.7;
  double small_font_scale = 0.5;
  int thickness = 2;

  // Цвет FPS в зависимости от производительности
  cv::Scalar fps_color;
  if (fps > 25)
    fps_color = cv::Scalar(0, 255, 0);  // Зеленый - отлично
  else if (fps > 15)
    fps_color = cv::Scalar(0, 255, 255);  // Желтый - хорошо
  else
    fps_color = cv::Scalar(0, 0, 255);  // Красный - плохо

  // Рисуем полупрозрачный фон для лучшей читаемости
  cv::Rect bg_rect(10, 10, 250, 70);
  cv::Mat overlay = image.clone();
  cv::rectangle(overlay, bg_rect, cv::Scalar(0, 0, 0), -1);
  cv::addWeighted(overlay, 0.6, image, 0.4, 0, image);

  // Рисуем текст
  cv::putText(image, name_text, cv::Point(15, 30), font_face, small_font_scale,
              cv::Scalar(255, 255, 255), 1);
  cv::putText(image, fps_text, cv::Point(15, 55), font_face, font_scale,
              fps_color, thickness);
  cv::putText(image, "Press Q to quit", cv::Point(image.cols - 150, 30),
              cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
}

int main() {
  // Открываем камеру по умолчанию
  cv::VideoCapture cap(0);

  if (!cap.isOpened()) {
    std::cerr << "Ошибка: не удалось открыть камеру!" << std::endl;
    return -1;
  }

  std::cout << "=== CINEMATIC SEPIA FILTER (SIMPLE) ===" << std::endl;
  std::cout << "Для выхода нажмите 'q' в окне с изображением" << std::endl;

  cv::Mat frame, processed_frame;

  // Переменные для измерения производительности - ОТДЕЛЬНО для каждого окна
  double start_time = get_current_time();

  // Для оригинального изображения
  double last_fps_time_original = start_time;
  int frame_count_original = 0;
  double fps_original = 0.0;

  // Для обработанного изображения
  double last_fps_time_processed = start_time;
  int frame_count_processed = 0;
  double fps_processed = 0.0;

  int total_frame_count = 0;

  // Переменные для измерения времени выполнения разных этапов
  double total_capture_time = 0.0;
  double total_processing_time = 0.0;
  double total_display_time = 0.0;

  while (true) {
    // Захват кадра
    double capture_start = get_current_time();
    cap >> frame;

    if (frame.empty()) {
      std::cerr << "Ошибка: пустой кадр!" << std::endl;
      break;
    }
    double capture_end = get_current_time();
    double capture_time = capture_end - capture_start;
    total_capture_time += capture_time;

    // Обработка изображения
    double processing_start = get_current_time();
    processed_frame = applyCinematicSepiaSimple(frame);
    double processing_end = get_current_time();
    double processing_time = processing_end - processing_start;
    total_processing_time += processing_time;

    // Добавляем информацию о FPS на КАЖДОЕ изображение
    drawFPSInfo(frame, fps_original, "Original Image");
    drawFPSInfo(processed_frame, fps_processed, "Sepia Filter");

    // Отображение
    double display_start = get_current_time();
    cv::imshow("Original Image", frame);
    cv::imshow("Sepia Filter", processed_frame);
    double display_end = get_current_time();
    double display_time = display_end - display_start;
    total_display_time += display_time;

    // 📊 ОБНОВЛЕНИЕ СЧЕТЧИКОВ FPS ДЛЯ КАЖДОГО ОКНА

    // Для оригинального изображения
    frame_count_original++;
    double current_time = get_current_time();
    double elapsed_time_original = current_time - last_fps_time_original;

    if (elapsed_time_original >= 1.0) {
      fps_original = frame_count_original / elapsed_time_original;
      frame_count_original = 0;
      last_fps_time_original = current_time;
    }

    // Для обработанного изображения
    frame_count_processed++;
    double elapsed_time_processed = current_time - last_fps_time_processed;

    if (elapsed_time_processed >= 1.0) {
      fps_processed = frame_count_processed / elapsed_time_processed;
      frame_count_processed = 0;
      last_fps_time_processed = current_time;

      // Вывод статистики в консоль
      std::cout << "\rOriginal FPS: " << std::fixed << std::setprecision(2)
                << fps_original << " | Sepia FPS: " << fps_processed;
      std::cout.flush();
    }

    total_frame_count++;

    // Обработка клавиш
    char key = static_cast<char>(cv::waitKey(1));
    if (key == 'q' || key == 'Q') {
      break;
    }
  }

  // Итоговая статистика
  double end_time = get_current_time();
  double total_time = end_time - start_time;

  std::cout << "\n\n=== ИТОГОВАЯ СТАТИСТИКА ===" << std::endl;
  std::cout << "Общее время работы: " << total_time << " секунд" << std::endl;
  std::cout << "Обработано кадров: " << total_frame_count << std::endl;
  std::cout << "Средний FPS оригинал: " << total_frame_count / total_time
            << std::endl;
  std::cout << "Средний FPS сепия: " << total_frame_count / total_time
            << std::endl;

  std::cout << "\nРаспределение времени обработки:" << std::endl;
  std::cout << "Захват кадра: " << std::fixed << std::setprecision(3)
            << (total_capture_time / total_time) * 100 << "%" << std::endl;
  std::cout << "Обработка изображения: "
            << (total_processing_time / total_time) * 100 << "%" << std::endl;
  std::cout << "Отображение: " << (total_display_time / total_time) * 100 << "%"
            << std::endl;
  std::cout << "Прочее: "
            << ((total_time - total_capture_time - total_processing_time -
                 total_display_time) /
                total_time) *
                   100
            << "%" << std::endl;

  std::cout << "\nАбсолютное время на кадр:" << std::endl;
  std::cout << "Захват: " << (total_capture_time / total_frame_count) * 1000
            << " мс" << std::endl;
  std::cout << "Обработка: "
            << (total_processing_time / total_frame_count) * 1000 << " мс"
            << std::endl;
  std::cout << "Отображение: "
            << (total_display_time / total_frame_count) * 1000 << " мс"
            << std::endl;

  cap.release();
  cv::destroyAllWindows();

  return 0;
}