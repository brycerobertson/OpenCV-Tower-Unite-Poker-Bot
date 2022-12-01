#include "detection.h"


std::vector<Card> cards;

extern std::vector<Rank> train_ranks;

extern std::vector<Suit> train_suits;


void Detection::filter_contours(std::vector<std::vector<cv::Point>>& contours, std::vector<std::vector<cv::Point>>& sorted_contours) {
	sorted_contours.clear();
	std::vector<cv::Point> approx;

	for (size_t i = 0; i < contours.size(); i++) {
		double area = cv::contourArea(contours[i]);
		cv::approxPolyDP(contours[i], approx,10, 1);
		if (approx.size() == 4 && area > 10000) {
			sorted_contours.push_back(contours[i]);
		};
		
	}
	std::sort(sorted_contours.begin(), sorted_contours.end(),
		[](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
			return a.at(1).x < b.at(1).x;
		});
};

void Detection::filter_rank_suit_contours(std::vector<std::vector<cv::Point>>& contours, std::vector<std::vector<cv::Point>>& sorted_contours) {
	sorted_contours.clear();
	std::vector<cv::Point> approx;

	for (size_t i = 0; i < contours.size(); i++) {
		double area = cv::contourArea(contours[i]);
		cv::approxPolyDP(contours[i], approx, 1, true);
		//if (approx.size() >4 && area < 20000) {
			sorted_contours.push_back(approx);
		//}

	}

};

auto Detection::flatten_card(cv::Mat &image, std::vector<cv::Point> &approx, Card &card) {
	
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

void Detection::process_contours(std::vector<std::vector<cv::Point>> contours) {
	//AIDS THAT I MADE BUT DIDNT NEED
	/*std::vector<std::vector<cv::Point>> approx(contours.size(), std::vector<cv::Point>(4));

	for (size_t i = 0; i < contours.size(); i++) {
		cv::approxPolyDP(contours[i], approx[i], cv::arcLength(contours[i], true) * 0.01, true);
	} 

	for (int i = 0; i < contours.size(); i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << approx[i][j] << " ";
		}
		std::cout<<std::endl;
	}
	*/
	cards.clear();

	for (size_t i = 0; i < contours.size(); i++) {
		cards.push_back(Card());

		cards[i].contour= contours[i];
		std::vector<cv::Point> approx;

		//find corner points of the card
		cv::approxPolyDP(contours[i], approx,10, true);
		cards[i].corner_points = approx;
		//std::cout << approx << " card " << i << std::endl;

		//find width and height of cards boudning rectangle
		cv::Rect card_rect = cv::boundingRect(contours[i]);
		cards[i].rect.height = card_rect.height;
		cards[i].rect.width = card_rect.width;

		//std::cout << card.rect.height << "height " << card.rect.width << "width " << " card " << i << std::endl;

		//find centre point of card using average of x and y points
		std::vector<int> centre_point = { (approx[1].x + approx[3].x) / 2,(approx[1].y + approx[3].y) / 2 };
		cards[i].centre = centre_point;

		flatten_card(colour_img, approx, cards[i]);


		cv::Mat cardCornerZoom;
		cv::Mat cardCornerZoomThresh;

		//grab corner of card, including rank & suit and scale it up by 2x
		cv::Mat cardCorner = cards[i].warp(cv::Range(0,  CORNER_HEIGHT), cv::Range(0, CORNER_WIDTH));
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
			//filter_rank_suit_contours(rankContours, sortedRankContours);

			//if (sortedRankContours.size() > 0) {
				cv::Rect rank_rect = cv::boundingRect(rankContours[0]);
				cards[i].rank_image = rank(cv::Range(rank_rect.y, (rank_rect.y + rank_rect.height)), cv::Range(rank_rect.x, (rank_rect.x + rank_rect.width)));
				//cv::drawContours(cardCornerZoom(cv::Range(1, 235), cv::Range(1, 120)), sortedRankContours, -1, { 255,0,255 }, 2);
				//cv::rectangle(cardCornerZoom(cv::Range(1, 235), cv::Range(1, 120)), rank_rect, { 255,0,255 }, 2);
				//cv::imshow("rank", cardCornerZoom(cv::Range(1, 235), cv::Range(1, 120)));
			//}
		}

		//find contours of card suit
		cv::findContours(suit, suitContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		if (rankContours.size() > 0) {
			//filter_rank_suit_contours(suitContours, sortedSuitContours);

			/*std::sort(sortedRankContours.begin(), sortedRankContours.end(),
				[](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
					return cv::contourArea(a) > cv::contourArea(b);
				});*/

			if (suitContours.size() > 0) {
				cv::Rect suit_rect = cv::boundingRect(suitContours[0]);
				cards[i].suit_image = suit(cv::Range(suit_rect.y, (suit_rect.y + suit_rect.height)), cv::Range(suit_rect.x, (suit_rect.x + suit_rect.width)));
				//cv::drawContours(cardCornerZoom(cv::Range(235, 379), cv::Range(1, 120)), sortedSuitContours, 0, { 255,0,255 }, 2);
				//cv::rectangle(cardCornerZoom(cv::Range(235, 379), cv::Range(1, 120)), suit_rect, { 255,0,255 }, 2);
				//cv::imshow("suit 2", cardCornerZoom(cv::Range(235, 379), cv::Range(1, 120)));
				//cv::imshow("suit", card.suit_image);
			}

		}

	}
	
};

void Detection::detect_cards() {


	/*for (size_t j = 0; j < 4; j++) {
		suit_images[j] = cv::imread("C:/Users/bryce/Pictures/opencv/suits/" + suit_names[j] + ".png");
	}
	for (size_t k = 0; k < 5; k++) {
		rank_images[k] = cv::imread("C:/Users/bryce/Pictures/opencv/ranks/" + rank_names[k] + ".png");
	}*/

	cv::threshold(greyscale_img, greyscale_img_threshold, 150, 200, cv::THRESH_TOZERO);

	cv::findContours(greyscale_img_threshold, all_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//cv::drawContours(colour_img, all_contours, -1, cv::Scalar(255, 0, 255), 2.5);

	filter_contours(all_contours, filtered_contours);

	if(filtered_contours.size() > 0) {
		process_contours(filtered_contours);

		//std::cout << cards.size() << std::endl;

		//std::cout << "C:\\Users\\bryce\\Pictures\\opencv\\suits\\" + suit_names[0] + ".png" << std::endl;

		for (size_t i = 0; i < cards.size(); i++) {
			
			cv::imshow(std::to_string(i+1), cards[i].suit_image);

			
		}

		cv::drawContours(colour_img, filtered_contours, -1, cv::Scalar(255, 0, 0), 2.5);
	}

	
};