#pragma once
#include <opencv2/opencv.hpp>
#include <Windows.h>

#include <iostream>


class window_copy{
private:

	HWND hWnd;
	int key = 0;

public:

	cv::Mat getMat(HWND hWnd) {
		cv::Mat mat;
		return mat;
	}


	void create_window();

	void copy_window();
};









