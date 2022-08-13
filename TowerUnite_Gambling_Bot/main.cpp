#include "window.h"
#include "detection.h"

Window window;
Detection detection;

int key = 0;

int main(int argc, char** argv)	{
	
	while (key != 27) {
		window.create_window();
		window.copy_window();
		detection.detect_cards();
		window.draw_window();

		key = cv::waitKey(30);
	}
	
	return 0;
}
