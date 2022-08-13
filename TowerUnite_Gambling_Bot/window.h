#pragma once
#include <opencv2/opencv.hpp>
#include <Windows.h>

#include <iostream>

extern cv::Mat colour_img;
extern cv::Mat greyscale_img;
extern cv::Mat greyscale_img_threshhold;

class Window{
private:

	HWND hWnd;
	int key = 0;

public:

	void create_window();

	void copy_window();

	void draw_window();
};









