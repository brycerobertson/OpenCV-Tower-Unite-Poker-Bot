#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <numeric>

#include "window.h"

struct Card
{
	cv::Rect rect;
	std::vector<cv::Point> contour;
	std::vector<int> centre;
	std::vector<cv::Point> corner_points;
	cv::Mat warp;
	cv::Mat rank_image;
	cv::Mat suit_image;
	std::string best_rank_match;
	std::string best_suit_match;
	int rank_diff = 0;
	int suit_diff = 0;
};

struct Rank {

};

extern std::vector<Rank> train_ranks;

struct Suit {

};

extern std::vector<Rank> train_suits;


extern std::vector<Card> cards;


class Detection {

private:

	//Adaptive threshold levels
	int BKG_THRESH = 6;
	int CARD_THRESH = 30;

	//Width and height of card corner, where rank and suit are
	int CORNER_WIDTH = 35;
	int CORNER_HEIGHT = 95;

	//Dimensions of rank train images
	int RANK_WIDTH = 70;
	int RANK_HEIGHT = 125;

	//Dimensions of suit train images
	int SUIT_WIDTH = 70;
	int SUIT_HEIGHT = 100;

	int RANK_DIFF_MAX = 2000;
	int SUIT_DIFF_MAX = 700;

	int CARD_MAX_AREA = 120000;
	int CARD_MIN_AREA = 8500;

	std::vector<std::vector<cv::Point>>  all_contours;
	std::vector<std::vector<cv::Point>>  filtered_contours;
	std::vector<std::vector<cv::Point>>  processed_cards;


	cv::Mat warped_image;


	void filter_contours(std::vector<std::vector<cv::Point>> contours, std::vector<std::vector<cv::Point>> &sorted_contours);

	void filter_rank_suit_contours(std::vector<std::vector<cv::Point>> contours, std::vector<std::vector<cv::Point>>& sorted_contours);

	auto flatten_card(cv::Mat image, std::vector<cv::Point> approx, Card &card);
	
public:


	void detect_cards();

	void process_contours(std::vector<std::vector<cv::Point>> sorted_contours);
};