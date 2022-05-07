#include <iostream>
#include <iomanip>

#include <string.h>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#define  Num  33   //���ұ�ȡֵ�ռ���32*32*32��
#define  Invalid_NumberRows 6   //a.cube = 12
//#define  _CRT_SECURE_NO_WARNINGS

int cubeIndex(float r, float g, float b)
{
	return (int( r + g * Num + b * Num * Num ));
}

float mixvalue(float a, float b, float c)
{
	return (a + (b - a) * (c - floor(c)));
}



int main()
{
	//step1 ��ȡ*.cube�ļ�

	char* text, *point_ext;
	FILE* pf = fopen("..\\3DLUT\\f.cube", "rb+");
	if (pf == NULL)
	{
		printf("error!");
		return -1;
	}

	fseek(pf, 0, SEEK_END);
	long lSize = ftell(pf);

	text = new char[lSize];
	rewind(pf);

	fread(text, sizeof(char), lSize, pf);
	char* char_w = new char[8];
	double LUTD3[Num*Num*Num*3] = { 0.0F };
	int t(0), i(0), itN(0), invat(0);

	// �趨�ڼ���Ϊ��Ч������
	while (itN < Invalid_NumberRows)
	{
		if (text[invat] == '\n')
			itN++;
		invat++;
	}

	//��λ����Ч���ݵ���ʼλ��
	point_ext = text + invat;

	for (i = 0; i < lSize-invat; i++)
	{
		if ((point_ext[i] == '\n') || (point_ext[i] == char(32)))
		{
			memcpy(char_w, point_ext + i - 8, 8);
			LUTD3[t++] = atof(char_w);
			//printf("%.6f\n", LUTD3[t-1]);
			memset(char_w, 0, 8);
		}
	}

	// step2 ��ȡͼ��

	Mat ORI = imread("..\\IMg\\cat.jpg");  //1234.bmp/cat.jpg/detailIM.jpg
	namedWindow("ԭͼ��",CV_WINDOW_NORMAL);
	imshow("ԭͼ��", ORI);

	// step3 3D_LUT arith

	Mat ORI_normfloat = Mat(ORI.size(), CV_32FC3);
	Mat NewIM = ORI.clone();

	for (int i=0; i<ORI.rows; i++)
	{
		for (int j=0; j<ORI.cols; j++)
		{
			ORI_normfloat.at<Vec3f>(i,j)[0] = (float)(ORI.at<Vec3b>(i, j)[0]) / 255.0f;
			ORI_normfloat.at<Vec3f>(i,j)[1] = (float)(ORI.at<Vec3b>(i, j)[1]) / 255.0f;
			ORI_normfloat.at<Vec3f>(i,j)[2] = (float)(ORI.at<Vec3b>(i, j)[2]) / 255.0f;
		}
	}

	float r, g, b, toR, toG, toB;
	unsigned char rH, gH, bH, rL, gL, bL;
	int indexH, indexL;


	for (int i = 0; i < ORI.rows; i++)
	{
		for (int j = 0; j < ORI.cols; j++)
		{
			r = ORI_normfloat.at<Vec3f>(i, j)[0] * (Num-1);
			g = ORI_normfloat.at<Vec3f>(i, j)[1] * (Num-1);
			b = ORI_normfloat.at<Vec3f>(i, j)[2] * (Num-1);

			rH = ceil(r);
			gH = ceil(g);
			bH = ceil(b);

			rL = floor(r);
			gL = floor(g);
			bL = floor(b);

			indexH = cubeIndex(rH, gH, bH);
			indexL = cubeIndex(rL, gL, bL);

			toR = mixvalue(LUTD3[indexL * 3], LUTD3[indexH * 3], r);
			toG = mixvalue(LUTD3[indexL * 3 + 1], LUTD3[indexH * 3 + 1], g);
			toB = mixvalue(LUTD3[indexL * 3 + 2], LUTD3[indexH * 3 + 2], b);

			NewIM.at<Vec3b>(i, j)[0] = uchar(toR * 255);
			NewIM.at<Vec3b>(i, j)[1] = uchar(toG * 255);
			NewIM.at<Vec3b>(i, j)[2] = uchar(toB * 255);
		}
	}
	
	namedWindow("����ͼ��", CV_WINDOW_NORMAL);
	imshow("����ͼ��", NewIM);

	 
	//delete[] text;
	delete[] char_w;


	waitKey(0);
	return 0;
}