#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>


using namespace cv;
using namespace std;


int main(void)
{
	// 원본 영상 입력
	Mat origin = imread("document6.png", IMREAD_COLOR);
	if (origin.empty())
	{
		cerr << "Original image load failed!" << endl;
		return -1;
	}

	// 색 변환, 크기 조정
	Mat image;
	float Width = origin.cols, reWidth = 500;
	float Height = origin.rows, reHeight = (Height / Width) * reWidth;
	resize(origin, image, Size(reWidth, reHeight));
	Mat resultImage = image.clone();
	cvtColor(image, image, COLOR_BGR2GRAY);
	imshow("origin", image);

	// 모폴로지, 가우시안
	morphologyEx(image, image, MORPH_GRADIENT, Mat());
	//imshow("1 morphologyEx", image);
	GaussianBlur(image, image, Size(), 0.7);
	//imshow("2 Gaussian", image);

	// 이진화, 팽창
	adaptiveThreshold(image, image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 5, 5);
	//imshow("3 threshold", image);


	Mat imageForOR = image.clone();

	// 내부 노이즈 제거(플러드필)
	floodFill(image, Point(0, 0), Scalar(255));
	//imshow("4 floodfill", image);

	bitwise_not(image, image);
	bitwise_or(imageForOR, image, image);
	//imshow("5 bitwise OR", image);


	// 외부 노이즈 제거
	Mat labels, stats, centroids;
	connectedComponentsWithStats(image, labels, stats, centroids);

	int maxNum = 1, maxVal = 0;
	for (int i = 1; i < stats.rows; i++) {
		int* area = stats.ptr<int>(i);
		if (maxVal <= area[4]) {
			maxVal = area[4];
			maxNum = i;
		}
	}

	for (int i = 0; i < labels.rows; i++) {
		int* labelPixel = labels.ptr<int>(i);
		uchar* imagePixel = image.ptr<uchar>(i);
		for (int j = 0; j < labels.cols; j++) {
			if (labelPixel[j] != maxNum) {
				imagePixel[j] = 0;
			}
		}
	}


	Mat blackBG(image.rows, image.cols, image.type());
	vector<Point2f> h_corner;

	Mat harris;
	Mat harris_norm = Mat::zeros(image.size(), CV_32FC1);
	cornerHarris(image, harris, 3, 9, 0.04);

	normalize(harris, harris_norm, 0, 255, NORM_MINMAX, CV_32FC1);
	for (int j = 0; j < harris_norm.rows; j += 1) {
		for (int i = 0; i < harris_norm.cols; i += 1) {
			if ((int)harris_norm.at<float>(j, i) > 120) {
				circle(image, Point(i, j), 7, Scalar(128), 0, 4, 0);
				h_corner.push_back(Point(i, j));
			}
		}
	}
	cout << h_corner.size() << endl;
	//imshow("harris", image);


	TermCriteria criteria(TermCriteria::COUNT + TermCriteria::EPS, 10, 1.0);
	vector<Point2f> corner;
	vector<Point2f> corners;
	Mat h_labels;

	Mat hcorner(h_corner);
	cout << hcorner << endl << endl;
	hcorner.reshape(0,1);
	cout << hcorner << endl;

	kmeans(hcorner, 4, h_labels, TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 1.0),
			4, KMEANS_PP_CENTERS, corners);
	cout << corners << endl;



	if (corners.size() != 4) {
		cerr << "there are not 4 corners" << endl;
		return -1;
	}
	for (int i = 0; i < corners.size(); i++) {
		circle(image, corners[i], 8, Scalar());
	}
	//imshow("7 goodFeaturesToTrack", image);


	//코너 인덱싱
	//Point2f idxPoint[4];
	vector<int> distance;
	for (int i = 0; i < 4; i++) {		// 원점에서부터의 거리 계산
		distance.push_back(sqrt(pow(0 - corners[i].x, 2) + pow(0 - corners[i].y, 2)));
	}
	
	// 1) leftUp, rightDown
	Point2f idxPoint[4];
	int minIdx = min_element(distance.begin(), distance.end()) - distance.begin();
	int maxIdx = max_element(distance.begin(), distance.end()) - distance.begin();
	idxPoint[0] = corners[minIdx];		// 최소 거리는 좌 상단, 최대 거리는 우 하단으로 간주하여 정렬
	idxPoint[1] = corners[maxIdx];
	if (minIdx < maxIdx) {				// 뒤에 위치한 벡터 포인트부터 먼저 제거
		corners.erase(corners.begin() + maxIdx);
		corners.erase(corners.begin() + minIdx);
	}
	else {
		corners.erase(corners.begin() + minIdx);
		corners.erase(corners.begin() + maxIdx);
	}
	// 2) rightUp, leftDown
	if (corners[0].y < corners[1].y) {	// 남은 두 좌표 중 y값이 작은 좌표를 먼저 정렬
		idxPoint[2] = corners[0];
		idxPoint[3] = corners[1];
	}
	else {
	idxPoint[2] = corners[1];
	idxPoint[3] = corners[0];
	}


	// 투시 변환
	Mat res;
	Point2f dstPoint[4] = { {0,0}, {reWidth, reHeight}, {reWidth, 0}, {0, reHeight} };
	Mat pers = getPerspectiveTransform(idxPoint, dstPoint);
	warpPerspective(resultImage, res, pers, Size(reWidth, reHeight));

	imshow("8 result", res);

	waitKey();

}

