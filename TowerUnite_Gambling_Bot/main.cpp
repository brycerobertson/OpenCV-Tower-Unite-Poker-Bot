#include "window.h"
#include "detection.h"

Window window;
Detection detection;

int key = 0;



int main(int argc, char** argv)	{
	cv::VideoCapture camera(0, cv::CAP_ANY);
	camera.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
	camera.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
	while (key != 27) {
		window.create_window();
		window.copy_window(camera);
		detection.detect_cards();
		window.draw_window();
		key = cv::waitKey(1);
	}
	
	return 0;
}
