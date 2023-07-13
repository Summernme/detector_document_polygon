//#include "opencv2/opencv.hpp"
//#include <iostream>
//#include <algorithm>
//
//
//using namespace cv;
//using namespace std;
//
//
//int main(void)
//{
//	// 원본 영상 입력
//	Mat origin = imread("document1.png", IMREAD_COLOR);
//
//	if (origin.empty())
//	{
//		cerr << "Original image load failed!" << endl;
//		return -1;
//	}
//
//	// 색 변환, 크기 조정
//	Mat image;
//	float Width = origin.cols, reWidth = 500;
//	float Height = origin.rows, reHeight = (Height / Width) * reWidth;
//	resize(origin, image, Size(reWidth, reHeight));
//	Mat resultImage = image.clone();
//	cvtColor(image, image, COLOR_BGR2GRAY);
//
//
//	// 모폴로지, 가우시안
//	morphologyEx(image, image, MORPH_GRADIENT, Mat());
//	//imshow("1 morphologyEx", image);
//	GaussianBlur(image, image, Size(), 0.7);
//	//imshow("2 Gaussian", image);
//
//	// 이진화, 팽창
//	adaptiveThreshold(image, image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 25, 5);
//	//imshow("3 threshold", image);
//	waitKey();
//
//
//	Mat imageForOR = image.clone();
//
//	// 내부 노이즈 제거(플러드필)
//	floodFill(image, Point(0, 0), Scalar(255));
//	//imshow("4 floodfill", image);
//
//	bitwise_not(image, image);
//	bitwise_or(imageForOR, image, image);
//	//imshow("5 bitwise OR", image);
//
//
//	// 외부 노이즈 제거
//	Mat labels, stats, centroids;
//	connectedComponentsWithStats(image, labels, stats, centroids);
//
//	int maxNum = 1, maxVal = 0;
//	for (int i = 1; i < stats.rows; i++) {
//		int* area = stats.ptr<int>(i);
//		if (maxVal <= area[4]) {
//			maxVal = area[4];
//			maxNum = i;
//		}
//	}
//
//	for (int i = 0; i < labels.rows; i++) {
//		int* labelPixel = labels.ptr<int>(i);
//		uchar* imagePixel = image.ptr<uchar>(i);
//		for (int j = 0; j < labels.cols; j++) {
//			if (labelPixel[j] != maxNum) {
//				imagePixel[j] = 0;
//			}
//		}
//	}
//	//imshow("6 object", image);
//
//
//	Mat blackBG(image.rows, image.cols, image.type());
//	vector<Point2f> corner;
//	goodFeaturesToTrack(image, corner, 4, 0.01, 10);
//	if (corner.size() != 4) {
//		cerr << "there are not 4 corners" << endl;
//		return -1;
//	}
//	for (int i = 0; i < corner.size(); i++) {
//		circle(image, corner[i], 8, Scalar(128));
//	}
//	imshow("goodFeaturesToTrack", image);
//
//	if (corner.size() != 4) {
//		cerr << "there are not 4 corners" << endl;
//		return -1;
//	}
//	for (int i = 0; i < corner.size(); i++) {
//		circle(image, corner[i], 8, Scalar(128));
//	}
//	//imshow("7 goodFeaturesToTrack", image);
//
//
//	// 코너 인덱싱
//	Point2f idxPoint[4];
//	vector<int> distance;
//	for (int i = 0; i < 4; i++) {		// 원점에서부터의 거리 계산
//		distance.push_back(sqrt(pow(0 - corner[i].x, 2) + pow(0 - corner[i].y, 2)));
//	}
//
//	// 1) leftUp, rightDown
//	int minIdx = min_element(distance.begin(), distance.end()) - distance.begin();
//	int maxIdx = max_element(distance.begin(), distance.end()) - distance.begin();
//	idxPoint[0] = corner[minIdx];		// 최소 거리는 좌 상단, 최대 거리는 우 하단으로 간주하여 정렬
//	idxPoint[1] = corner[maxIdx];
//	if (minIdx < maxIdx) {				// 뒤에 위치한 벡터 포인트부터 먼저 제거
//		corner.erase(corner.begin() + maxIdx);
//		corner.erase(corner.begin() + minIdx);
//	}
//	else {
//		corner.erase(corner.begin() + minIdx);
//		corner.erase(corner.begin() + maxIdx);
//	}
//	// 2) rightUp, leftDown
//	if (corner[0].y < corner[1].y) {	// 남은 두 좌표 중 y값이 작은 좌표를 먼저 정렬
//		idxPoint[2] = corner[0];
//		idxPoint[3] = corner[1];
//	}
//	else {
//		idxPoint[2] = corner[1];
//		idxPoint[3] = corner[0];
//	}
//
//
//	// 투시 변환
//	Mat res;
//	Point2f dstPoint[4] = { {0,0}, {reWidth, reHeight}, {reWidth, 0}, {0, reHeight} };
//	Mat pers = getPerspectiveTransform(idxPoint, dstPoint);
//	warpPerspective(resultImage, res, pers, Size(reWidth, reHeight));
//
//	imshow("8 result", res);
//
//	waitKey();
//
//}
//
//
