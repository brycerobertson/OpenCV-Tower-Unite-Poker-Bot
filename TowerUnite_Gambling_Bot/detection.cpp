#include "detection.h"


void Detection::filter_contours(std::vector<std::vector<cv::Point>> contours) {
	filtered_contours.clear();
	for (size_t i = 0; i < contours.size(); i++) {
		double area = cv::contourArea(contours[i]);
		if (area > 10000) {
			filtered_contours.push_back(contours.at(i));
		}
	}
};

void Detection::detect_cards()	{
	cv::threshold(greyscale_img, greyscale_img_threshhold, 230, 255, cv::THRESH_TOZERO);

	cv::findContours(greyscale_img_threshhold, all_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	filter_contours(all_contours);

	cv::drawContours(colour_img, filtered_contours, -1, cv::Scalar(255, 0, 255), 2);
};