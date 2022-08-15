#include "detection.h"

std::vector<Card> cards;

void Detection::filter_contours(std::vector<std::vector<cv::Point>> contours, std::vector<std::vector<cv::Point>> &sorted_contours) {
	sorted_contours.clear();
	std::vector<cv::Point> approx;

	for (size_t i = 0; i < contours.size(); i++) {
		double area = cv::contourArea(contours[i]);
		cv::approxPolyDP(contours[i], approx, cv::arcLength(contours[i], true) * 0.01, true);
		if (approx.size() == 4 && area > 8500) {
			sorted_contours.push_back(contours[i]);
		}
		
	}
	
};

auto Detection::flatten_card(cv::Mat image, std::vector<cv::Point> approx, Card &card) {
	
	int maxWidth = 200;
	int maxHeight = 300;
	std::vector< cv::Point2f> src_corners(4);
	std::vector< cv::Point2f> midpoints(4);
	std::vector< cv::Point2f> dst_corners(4);

	src_corners[0] = approx[0];
	src_corners[1] = approx[3];
	src_corners[2] = approx[2];
	src_corners[3] = approx[1];

	midpoints[0] = (src_corners[0] + src_corners[1]) / 2;
	midpoints[1] = (src_corners[1] + src_corners[2]) / 2;
	midpoints[2] = (src_corners[2] + src_corners[3]) / 2;
	midpoints[3] = (src_corners[3] + src_corners[0]) / 2;

	dst_corners[0].x = 0;
	dst_corners[0].y = 0;
	dst_corners[1].x = maxWidth - 1;
	dst_corners[1].y = 0;
	dst_corners[2].x = maxWidth - 1;
	dst_corners[2].y = maxHeight - 1;
	dst_corners[3].x = 0;
	dst_corners[3].y = maxHeight - 1;

	cv::Mat Matrix = cv::getPerspectiveTransform(src_corners, dst_corners);

	cv::warpPerspective(image, card.warp, Matrix, cv::Size(maxWidth, maxHeight),cv::INTER_LINEAR);

};

void Detection::process_contours(std::vector<std::vector<cv::Point>> sorted_contours) {
	//AIDS THAT I MADE BUT DIDNT NEED
	/*std::vector<std::vector<cv::Point>> approx(sorted_contours.size(), std::vector<cv::Point>(4));

	for (size_t i = 0; i < sorted_contours.size(); i++) {
		cv::approxPolyDP(sorted_contours[i], approx[i], cv::arcLength(sorted_contours[i], true) * 0.01, true);
	} 

	for (int i = 0; i < sorted_contours.size(); i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << approx[i][j] << " ";
		}
		std::cout<<std::endl;
	}
	*/

	std::sort(sorted_contours.begin(), sorted_contours.end(),
		[](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
			return a.at(1).x < b.at(1).x;
		});

	cards.clear();
	for (size_t i = 0; i < sorted_contours.size(); i++) {
		Card current_card;
		

		current_card.contour= sorted_contours[i];
		std::vector<cv::Point> approx;

		//find corner points of the card
		cv::approxPolyDP(sorted_contours[i], approx, cv::arcLength(sorted_contours[i], true) * 0.01, true);
		current_card.corner_points = approx;
		std::cout << approx << " card " << i << std::endl;

		//find width and height of cards boudning rectangle
		cv::Rect card_rect = cv::boundingRect(sorted_contours[i]);
		current_card.height = card_rect.height;

		//find centre point of card using average of x and y points
		std::vector<int> centre_point = { (approx[1].x + approx[3].x) / 2,(approx[1].y + approx[3].y) / 2 };
		current_card.centre = centre_point;

		flatten_card(greyscale_img_threshhold, approx, current_card);

		cards.push_back(current_card);
	}
	
};

void Detection::detect_cards() {
	cv::threshold(greyscale_img, greyscale_img_threshhold, 230, 255, cv::THRESH_TOZERO);

	cv::findContours(greyscale_img_threshhold, all_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	filter_contours(all_contours, filtered_contours);

	process_contours(filtered_contours);

	cv::drawContours(colour_img, filtered_contours, -1, cv::Scalar(255, 0, 255), 3);
};