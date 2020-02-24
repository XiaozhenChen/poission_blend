#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <iostream>
#include <time.h>

using namespace cv;
using namespace std;

#define elif else if
#define ATD at<double>


Mat poission_blending(Mat& src, Mat& dst, Rect ROI, int posX, int posY);
Mat getA(int hieght, int width);
int getLabel(int i, int j, int height, int width);
Mat getB1(Mat& src, Mat& dst, int posX, int posY, Rect rectSrc);
Mat getB2(Mat& src, Mat& dst, int posX, int posY, Rect rectSrc);
Mat getLaplacian();

// calculate horizontal gradient, img(i, j + 1) - img(i, j)
Mat
getGradientXp(Mat& img) {
	int height = img.rows;
	int width = img.cols;
	Mat cat = repeat(img, 1, 2);

	Rect roi = Rect(1, 0, width, height);
	Mat roimat = cat(roi);

	cout << "roimat - img" << endl;

	cout << roimat - img << endl;
	return roimat - img;
}

// calculate vertical gradient, img(i + 1, j) - img(i, j)
Mat
getGradientYp(Mat& img) {
	int height = img.rows;
	int width = img.cols;
	Mat cat = repeat(img, 2, 1);

	Rect roi = Rect(0, 1, width, height);
	Mat roimat = cat(roi);

	cout << "roimat - img" << endl;
	cout << roimat - img << endl;

	return roimat - img;
}

// calculate horizontal gradient, img(i, j - 1) - img(i, j)
Mat
getGradientXn(Mat& img) {
	int height = img.rows;
	int width = img.cols;
	Mat cat = repeat(img, 1, 2);

	Rect roi = Rect(width - 1, 0, width, height);
	Mat roimat = cat(roi);

	cout << "roimat - img" << endl;
	cout << roimat - img << endl;

	return roimat - img;
}

// calculate vertical gradient, img(i - 1, j) - img(i, j)
Mat
getGradientYn(Mat& img) {
	int height = img.rows;
	int width = img.cols;
	Mat cat = repeat(img, 2, 1);

	Rect roi = Rect(0, height - 1, width, height);
	Mat roimat = cat(roi);

	cout << "roimat - img" << endl;
	cout << roimat - img << endl;
	return roimat - img;
}

Mat getResult(Mat& A, Mat& B, Rect& ROI)
{
	Mat result;
	solve(A, B, result);
	result = result.reshape(0, ROI.height);
	cout << "result" << endl;
	cout << result << endl;

	return  result;
}

int main(int argc, char** argv)
{
	long start, end;
	start = clock();

	Mat src, dst;
	Mat srcImg = imread("tree.jpg");
	Mat dstImg = imread("oceam.jpg");

	srcImg.convertTo(src, CV_64FC3);
	srcImg.convertTo(dst, CV_64FC3);

	Rect rectSrc = Rect(200, 200, 4, 4);
	Mat result = poission_blending(src, dst, rectSrc, 200, 200);
	result.convertTo(result, CV_8UC1);
	Rect rc2 = Rect(184, 220, 55, 90);
	Mat roimat = dst(rc2);
	result.copyTo(roimat);

	end = clock();
	cout << "used time: " << ((double)(end - start)) / CLOCKS_PER_SEC << " second" << endl;
	imshow("roi", result);
	imshow("result", dst);

	waitKey(0);
	return 0;

}

Mat poission_blending(Mat& src, Mat& dst, Rect ROI, int posX, int posY)
{
	int roiHeight = ROI.height;
	int roiWidth  = ROI.width;

	//Ax = b
	Mat A = getA(roiHeight, roiWidth);

	//do poission blending to each channel
	vector<Mat> rgb1;
	split(src, rgb1);
	vector<Mat> rgb2;
	split(dst, rgb2);

	vector<Mat> result;
	Mat merged, res, Br, Bg, Bb;

	// For calculating B, you can use either getB1() or getB2()
	Br = getB2(rgb1[0], rgb2[0], posX, posY, ROI);
	//Br = getB2(rgb1[0], rgb2[0], posX, posY, ROI);
	res = getResult(A, Br, ROI);
	result.push_back(res);
	cout << "R channel finished..." << endl;
	Bg = getB2(rgb1[1], rgb2[1], posX, posY, ROI);
	//Bg = getB2(rgb1[1], rgb2[1], posX, posY, ROI);
	res = getResult(A, Bg, ROI);
	result.push_back(res);
	cout << "G channel finished..." << endl;
	Bb = getB2(rgb1[2], rgb2[2], posX, posY, ROI);
	//Bb = getB2(rgb1[2], rgb2[2], posX, posY, ROI);
	res = getResult(A, Bb, ROI);
	result.push_back(res);
	cout << "B channel finished..." << endl;

	// merge the 3 gray images into a 3-channel image 
	merge(result, merged);
	return merged;


	return A;
}

Mat getA(int height, int width)
{
	Mat A = Mat::eye(height * width, height * width, CV_64FC1);
	A *= -4;
	Mat M = Mat::zeros(height, width, CV_64FC1);
	Mat temp = Mat::ones(height, width - 2, CV_64FC1);
	Rect roi = Rect(1, 0, width - 2, height);
	Mat roimat = M(roi);
	temp.copyTo(roimat);
	temp = Mat::ones(height - 2, width, CV_64FC1);
	roi = Rect(0, 1, width, height - 2);
	roimat = M(roi);
	temp.copyTo(roimat);
	temp = Mat::ones(height - 2, width - 2, CV_64FC1);
	temp *= 2;
	roi = Rect(1, 1, width - 2, height - 2);
	roimat = M(roi);
	temp.copyTo(roimat);

	cout << height<<" , "<<width << endl;
	cout << "A" << endl;
	cout << A << endl;
	cout << "M" << endl;
	cout << M << endl;
	cout << "roimat" << endl;
	cout << roimat << endl;
	cout << "temp" << endl;
	cout << temp << endl;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int label = getLabel(i, j, height, width);

			if (M.ATD(i, j) == 0) // 边界条件
			{
				if (i == 0)  
					A.ATD(getLabel(i + 1, j, height, width), label) = 1;
				elif(i == height - 1)  
					A.ATD(getLabel(i - 1, j, height, width), label) = 1;
				if (j == 0)  
					A.ATD(getLabel(i, j + 1, height, width), label) = 1;
				elif(j == width - 1)  
					A.ATD(getLabel(i, j - 1, height, width), label) = 1;
			}
			else if (M.ATD(i, j) == 1) // 边界条件
			{
				if (i == 0) {
					A.ATD(getLabel(i + 1, j, height, width), label) = 1;
					A.ATD(getLabel(i, j - 1, height, width), label) = 1;
					A.ATD(getLabel(i, j + 1, height, width), label) = 1;
				}elif(i == height - 1) {
					A.ATD(getLabel(i - 1, j, height, width), label) = 1;
					A.ATD(getLabel(i, j - 1, height, width), label) = 1;
					A.ATD(getLabel(i, j + 1, height, width), label) = 1;
				}
				if (j == 0) {
					A.ATD(getLabel(i, j + 1, height, width), label) = 1;
					A.ATD(getLabel(i - 1, j, height, width), label) = 1;
					A.ATD(getLabel(i + 1, j, height, width), label) = 1;
				}elif(j == width - 1) {
					A.ATD(getLabel(i, j - 1, height, width), label) = 1;
					A.ATD(getLabel(i - 1, j, height, width), label) = 1;
					A.ATD(getLabel(i + 1, j, height, width), label) = 1;
				}
	
			}

			else 
			{
				A.ATD(getLabel(i, j - 1, height, width), label) = 1;
				A.ATD(getLabel(i, j + 1, height, width), label) = 1;
				A.ATD(getLabel(i - 1, j, height, width), label) = 1;
				A.ATD(getLabel(i + 1, j, height, width), label) = 1;
					
			}
		}
	}
	 
	cout << "A" << endl;
	cout << A << endl;

	return A;
}

int getLabel(int i, int j, int height, int width)
{
	return i * width + j;
}

// B:   Ax = b  , B for b    use convolution
Mat
getB1(Mat& img1, Mat& img2, int posX, int posY, Rect ROI) {
	Mat Lap;
	filter2D(img1, Lap, -1, getLaplacian());
	int roiheight = ROI.height;
	int roiwidth = ROI.width;
	Mat B = Mat::zeros(roiheight * roiwidth, 1, CV_64FC1);
	for (int i = 0; i < roiheight; i++) {
		for (int j = 0; j < roiwidth; j++) {
			double temp = 0.0;
			temp += Lap.ATD(i + ROI.y, j + ROI.x);
			if (i == 0)              temp -= img2.ATD(i - 1 + posY, j + posX);
			if (i == roiheight - 1)  temp -= img2.ATD(i + 1 + posY, j + posX);
			if (j == 0)              temp -= img2.ATD(i + posY, j - 1 + posX);
			if (j == roiwidth - 1)   temp -= img2.ATD(i + posY, j + 1 + posX);
			B.ATD(getLabel(i, j, roiheight, roiwidth), 0) = temp;
		}
	}
	return B;
}

// Calculate b
// using getGradient functions.
Mat
getB2(Mat& img1, Mat& img2, int posX, int posY, Rect ROI) {
	Mat grad = getGradientXp(img1) + getGradientYp(img1) + getGradientXn(img1) + getGradientYn(img1);
	
	int roiheight = ROI.height;
	int roiwidth = ROI.width;
	Mat B = Mat::zeros(roiheight * roiwidth, 1, CV_64FC1);
	for (int i = 0; i < roiheight; i++) {
		for (int j = 0; j < roiwidth; j++) {
			double temp = 0.0;
			temp += grad.ATD(i + ROI.y, j + ROI.x);
			if (i == 0)              temp -= img2.ATD(i - 1 + posY, j + posX);
			if (i == roiheight - 1)  temp -= img2.ATD(i + 1 + posY, j + posX);
			if (j == 0)              temp -= img2.ATD(i + posY, j - 1 + posX);
			if (j == roiwidth - 1)   temp -= img2.ATD(i + posY, j + 1 + posX);
			B.ATD(getLabel(i, j, roiheight, roiwidth), 0) = temp;
		}
	}
	return B;
}


Mat getLaplacian()
{
	Mat laplacian = Mat::zeros(3, 3, CV_64FC1);
	laplacian.ATD(0, 1) = 1.0;
	laplacian.ATD(1, 0) = 1.0;
	laplacian.ATD(1, 2) = 1.0;
	laplacian.ATD(2, 1) = 1.0;
	laplacian.ATD(1, 1) = -4.0;
	return laplacian;
}