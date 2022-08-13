#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "window.h"


class Detection {

private:
	std::vector<std::vector<cv::Point>>  all_contours;
	std::vector<std::vector<cv::Point>>  filtered_contours;

	void filter_contours(std::vector<std::vector<cv::Point>> contours);
public:
	void detect_cards();
};