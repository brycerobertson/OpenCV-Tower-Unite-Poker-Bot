#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <numeric>

#include "window.h"

struct Card
{
	int width = 0;
	int height = 0;
	std::vector<std::vector<cv::Point>> contour;
	std::vector<int> centre;
	std::vector<cv::Point> corner_points;
	cv::Mat warp;

};

extern std::vector<Card> cards;

class Detection {

private:
	std::vector<std::vector<cv::Point>>  all_contours;
	std::vector<std::vector<cv::Point>>  filtered_contours;
	std::vector<std::vector<cv::Point>>  processed_cards;

	cv::Mat warped_image;

	void filter_contours(std::vector<std::vector<cv::Point>> contours, std::vector<std::vector<cv::Point>> &sorted_contours);

	void flatten_card(cv::Mat image, std::vector<cv::Point> approx, Card &card);
	
public:
	void detect_cards();

	void process_contours(std::vector<std::vector<cv::Point>> sorted_contours, Card& card);
};