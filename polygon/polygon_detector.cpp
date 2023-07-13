#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;


int main(void)
{
	Mat Origin = imread("ex3.png", IMREAD_COLOR);
	imshow("Origin", Origin);

	if (Origin.empty())
	{
		cerr << "image load failed!" << endl;
		return -1;
	}
	// 색공간 변환
	Mat image;
	cvtColor(Origin, image, COLOR_BGR2GRAY);

	// 미디언 필터
	medianBlur(image, image, 3);
	imshow("median", image);

	// 적응형 이진화
	adaptiveThreshold(image, image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 15, 5);
	imshow("adaptive threshold", image);

	// 팽창 연산
	dilate(image, image, Mat(), Point(-1,-1), 2);
	morphologyEx(image, image, MORPH_OPEN, Mat());
	imshow("dilate", image);


	// 외곽선 정보 추출
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(image, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE);
	

	// 외곽선 그리기
	Mat BlackBG(Origin.rows, Origin.cols, Origin.type());
	drawContours(BlackBG, contours, -1, Scalar::all(255), 1, LINE_8);
	imshow("drawContours", BlackBG);


	// 근사화, 도형 검출
	vector<vector<Point>> approx(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(contours[i], approx[i], arcLength(contours[i], true) * 0.025, true);

		if (contourArea(approx[i]) < 500 || approx[i].size() < 3 || hierarchy[i][3] == -1) {
			continue; // 해당 면적보다 작거나, 외곽점 개수가 작은 경우, 최상위 객체인 경우 제외
		}
		for (int j = 0; j < approx[i].size(); j++){
			circle(BlackBG, Point(approx[i][j]), 1, Scalar::all(255), 2, LINE_8);
		}

		String shape;
		if (approx[i].size() == 3 || approx[i].size() == 4 || approx[i].size() == 5 || approx[i].size() == 6)
		{
			switch (approx[i].size())
			{
			case 3: 
				shape = "triangle";
				break;
			case 4:
				shape = "rectangle";
				break;
			case 5:
				shape = "pentagon";
				break;
			case 6:
				shape = "hexagon";
				break;
			default:
				break;
			}
			putText(BlackBG, shape, approx[i][0], FONT_HERSHEY_PLAIN, 1, Scalar::all(255));
			putText(Origin, shape, approx[i][0], FONT_HERSHEY_PLAIN, 1, Scalar(0,255,255));
		}
		// 다각형, 원 처리
		else {
			vector<vector<Point>> approx_temp(contours.size());
			approxPolyDP(contours[i], approx_temp[i], arcLength(contours[i], true) * 0.01, true);

			for (int m = 0; m < approx_temp[i].size(); m++) {
				circle(BlackBG, Point(approx_temp[i][m]), 1, Scalar::all(255), 2, LINE_8);
			}

			if (approx_temp[i].size() >= 10) {
				putText(BlackBG, "circle", approx_temp[i][0], FONT_HERSHEY_PLAIN, 1, Scalar::all(128));
				putText(Origin, "circle", approx_temp[i][0], FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 255));
			}
		};
	};

	imshow("approxpolyDP", BlackBG);
	imshow("result", Origin);
	waitKey();
}