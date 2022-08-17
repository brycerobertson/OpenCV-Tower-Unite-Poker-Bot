#include "detection.h"

std::vector<Card> cards;
extern std::vector<Rank> train_ranks;
extern std::vector<Rank> train_suits;


void Detection::filter_contours(std::vector<std::vector<cv::Point>> contours, std::vector<std::vector<cv::Point>> &sorted_contours) {
	sorted_contours.clear();
	std::vector<cv::Point> approx;

	for (size_t i = 0; i < contours.size(); i++) {
		double area = cv::contourArea(contours[i]);
		cv::approxPolyDP(contours[i], approx, cv::arcLength(contours[i], true) * 0.01, true);
		if (approx.size() == 4 && area > 10000) {
			sorted_contours.push_back(contours[i]);
		}
		
	}
	
};

void Detection::filter_rank_suit_contours(std::vector<std::vector<cv::Point>> contours, std::vector<std::vector<cv::Point>>& sorted_contours) {
	sorted_contours.clear();
	std::vector<cv::Point> approx;

	for (size_t i = 0; i < contours.size(); i++) {
		double area = cv::contourArea(contours[i]);
		cv::approxPolyDP(contours[i], approx, cv::arcLength(contours[i], true) * 0.01, true);
		if (approx.size() >4 && area < 20000) {
			sorted_contours.push_back(contours[i]);
		}

	}

};

auto Detection::flatten_card(cv::Mat image, std::vector<cv::Point> approx, Card &card) {
	
	int maxWidth = 250;
	int maxHeight = 375;
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

	cv::warpPerspective(image, card.warp, Matrix, cv::Size(maxWidth, maxHeight),cv::INTER_LANCZOS4);

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
		cv::approxPolyDP(sorted_contours[i], approx, cv::arcLength(sorted_contours[i], true)*0.02, true);
		current_card.corner_points = approx;
		/*std::cout << approx << " card " << i << std::endl;*/

		//find width and height of cards boudning rectangle
		cv::Rect card_rect = cv::boundingRect(sorted_contours[i]);
		current_card.rect.height = card_rect.height;
		current_card.rect.width = card_rect.width;

		//find centre point of card using average of x and y points
		std::vector<int> centre_point = { (approx[1].x + approx[3].x) / 2,(approx[1].y + approx[3].y) / 2 };
		current_card.centre = centre_point;

		flatten_card(colour_img, approx, current_card);

		cv::Mat cardCornerZoom;
		cv::Mat cardCornerZoomThresh;

		//grab corner of card, including rank & suit and scale it up by 2x
		cv::Mat cardCorner = current_card.warp(cv::Range(0,  CORNER_HEIGHT), cv::Range(0, CORNER_WIDTH));
		cv::resize(cardCorner, cardCornerZoom, cv::Size(), 4, 4, cv::INTER_CUBIC);

		

		//greyscale corner seciton after scaling
		cv::cvtColor(cardCornerZoom, cardCornerZoomThresh, cv::COLOR_BGRA2GRAY);

		//threshold greyscale image for contour finding
		cv::threshold(cardCornerZoomThresh, cardCornerZoomThresh, 200, 255, cv::THRESH_BINARY_INV);

		cv::Mat rank = cardCornerZoomThresh(cv::Range(1, 235), cv::Range(1, 115));
		cv::Mat suit = cardCornerZoomThresh(cv::Range(235, 379), cv::Range(1, 120));
		

		std::vector<std::vector<cv::Point>> rankContours;
		std::vector<std::vector<cv::Point>> sortedRankContours;
		std::vector<std::vector<cv::Point>> suitContours;
		std::vector<std::vector<cv::Point>> sortedSuitContours;

		//find contours of card rank
		cv::findContours(rank, rankContours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

		if (rankContours.size() > 0) {
			filter_rank_suit_contours(rankContours, sortedRankContours);

			/*std::sort(sortedRankContours.begin(), sortedRankContours.end(),
				[](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
					return cv::contourArea(a) > cv::contourArea(b);
				});*/

			if (sortedRankContours.size() > 0) {
				cv::Rect rank_rect = cv::boundingRect(sortedRankContours[0]);
				current_card.rank_image = rank(cv::Range(rank_rect.y, (rank_rect.y + rank_rect.height)), cv::Range(rank_rect.x, (rank_rect.x + rank_rect.width)));
				/*cv::drawContours(cardCornerZoom(cv::Range(1, 235), cv::Range(1, 120)), sortedRankContours, -1, { 255,0,255 }, 2);
				cv::rectangle(cardCornerZoom(cv::Range(1, 235), cv::Range(1, 120)), rank_rect, { 255,0,255 }, 2);
				cv::imshow("rank", cardCornerZoom(cv::Range(1, 235), cv::Range(1, 120)));*/
			}
		}

		//find contours of card suit
		cv::findContours(suit, suitContours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

		if (rankContours.size() > 0) {
			filter_rank_suit_contours(suitContours, sortedSuitContours);

			/*std::sort(sortedRankContours.begin(), sortedRankContours.end(),
				[](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
					return cv::contourArea(a) > cv::contourArea(b);
				});*/

			if (sortedSuitContours.size() > 0) {
				cv::Rect suit_rect = cv::boundingRect(sortedSuitContours[0]);
				current_card.suit_image = suit(cv::Range(suit_rect.y, (suit_rect.y + suit_rect.height)), cv::Range(suit_rect.x, (suit_rect.x + suit_rect.width)));
				/*cv::drawContours(cardCornerZoom(cv::Range(235, 379), cv::Range(1, 120)), sortedSuitContours, 0, { 255,0,255 }, 2);
				cv::rectangle(cardCornerZoom(cv::Range(235, 379), cv::Range(1, 120)), suit_rect, { 255,0,255 }, 2);
				cv::imshow("suit", cardCornerZoom(cv::Range(235, 379), cv::Range(1, 120)));*/
				//cv::imshow("suit", current_card.suit_image);
			}
		}
		



		cards.push_back(current_card);

	}
	
};

void Detection::detect_cards() {
	

	cv::threshold(greyscale_img, greyscale_img_threshold,230, 255, cv::THRESH_TOZERO);

	cv::findContours(greyscale_img_threshold, all_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	filter_contours(all_contours, filtered_contours);

	if(filtered_contours.size() > 0) {
		process_contours(filtered_contours);
		cv::drawContours(colour_img, filtered_contours, -1, cv::Scalar(255, 0, 255), 3);
	}

	
};