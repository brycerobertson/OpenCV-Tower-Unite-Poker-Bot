#include "window.h"
#include "detection.h"

Window window;
Detection detection;

int key = 0;

int frame_rate_calc = 1;
int freq = cv::getTickFrequency();

int main(int argc, char** argv)	{
	cv::VideoCapture camera(0, cv::CAP_ANY);
	camera.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
	camera.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

	

	

	while (key != 27) {

		double t1 = cv::getTickCount();

		window.create_window();
		detection.detect_cards();
		
		double t2 = cv::getTickCount();


		double time1 = (t2 - t1) / freq;
		frame_rate_calc = 1 / time1;

		cv::putText(colour_img, std::to_string(frame_rate_calc), { 0,50 }, cv::FONT_HERSHEY_SIMPLEX, 1, { 0,255,0 }, 2);
		window.draw_window();

		key = cv::waitKey(1);
	}

	

	return 0;
}
