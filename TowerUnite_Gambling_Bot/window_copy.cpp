#include "window_copy.h"

BITMAPINFOHEADER createBitmapHeader(int width, int height) {

	//specify image format by using bitmapinfoheader
	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0; //because no compression
	bi.biXPelsPerMeter = 0; //we
	bi.biYPelsPerMeter = 0; //we
	bi.biClrUsed = 0; //we ^^
	bi.biClrImportant = 0; //still we

	return bi;
};

cv::Mat getMat(HWND hWnd) {

	cv::Mat mat;

	HDC deviceContext = GetDC(hWnd);
	HDC memoryDeviceContext = CreateCompatibleDC(deviceContext);
	SetStretchBltMode(memoryDeviceContext, COLORONCOLOR);

	RECT windowRect;
	GetClientRect(hWnd, &windowRect);	//get client coordinates relative to itself excluding title bar

	int height = windowRect.bottom;
	int width = windowRect.right;

	mat.create(height, width, CV_8UC4); // 8 bit unsigned ints 4 Channels -> RGBA

	HBITMAP bitmap = CreateCompatibleBitmap(deviceContext, width, height);
	BITMAPINFOHEADER bi = createBitmapHeader(width, height);

	SelectObject(memoryDeviceContext, bitmap);

	//copy window image into bitmap
	StretchBlt(memoryDeviceContext, 0, 0, width, height, deviceContext, 0, 0, width, height, SRCCOPY);
	//transform data & store into mat.data
	GetDIBits(memoryDeviceContext, bitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	//clean up
	DeleteObject(bitmap);
	DeleteDC(memoryDeviceContext);
	ReleaseDC(hWnd, deviceContext);

	return mat;
};


void window_copy::create_window() {

	LPCWSTR window_title = L"Tower Unite  ";		//set title of window to search for
	hWnd = FindWindow(NULL, window_title);			//get HWND of window title
	cv::namedWindow("output", cv::WINDOW_AUTOSIZE); //create window named "output"
}

void window_copy::copy_window()	{
	while (key != 27) {

		HWND temp = GetForegroundWindow();
		if (temp != hWnd) {
			Sleep(10);
			continue;
		}

		cv::Mat target = getMat(hWnd);																//store hWnd mat to new Mat
		cv::Mat background;																			//create Mat to copy target to
		target.copyTo(background);																//copy target Mat to background Mat
		cv::cvtColor(target, target, cv::COLOR_BGR2HSV);									//convert target image to another colour space	

		cv::imshow("output", background);													//show our image inside the created window.
		key = cv::waitKey(30);																	//wait for any keystroke in the window
	};
};