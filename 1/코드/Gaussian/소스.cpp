#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#pragma warning(disable : 4996)

using namespace std;

HWND hwnd;
HDC hdc;

enum color {
	BLUE, RED, WHITE, GREEN
};

void Draw(float val, int x_origin, int y_origin, int curx, color c) {
	if (c == BLUE) {
		SetPixel(hdc, x_origin + curx, y_origin - val, RGB(0, 0, 255));
	}
	else if (c == RED) {
		SetPixel(hdc, x_origin + curx, y_origin - val, RGB(255, 0, 0));
	}
	else if (c == GREEN) {
		SetPixel(hdc, x_origin + curx, y_origin - val, RGB(0, 255, 0));
	}
	else {
		SetPixel(hdc, x_origin + curx, y_origin - val, RGB(255, 255, 255));
	}
}

void DrawHistogram(int histogram[400], int x_origin, int y_origin, int cnt) {
	MoveToEx(hdc, x_origin, y_origin, 0);
	LineTo(hdc, x_origin + cnt, y_origin);

	MoveToEx(hdc, x_origin, 100, 0);
	LineTo(hdc, x_origin, y_origin);

	for (int CurX = 0; CurX < cnt; CurX++) {
		for (int CurY = 0; CurY < histogram[CurX]; CurY++) {
			MoveToEx(hdc, x_origin + CurX, y_origin, 0);
			LineTo(hdc, x_origin + CurX, y_origin - histogram[CurX] / 2);
		}
	}
}

void nomalization(int histogram[400], float norm_histogram[400], int range, int rcnt) {
	for (int i = 0; i < range; i++) {
		norm_histogram[i] = (float)histogram[i] / 100;
	}
}

void DrawHistogram(float histogram[400], int x_origin, int y_origin, int cnt) {
	MoveToEx(hdc, x_origin, y_origin, 0);
	LineTo(hdc, x_origin + cnt, y_origin);

	MoveToEx(hdc, x_origin, y_origin, 0);
	LineTo(hdc, x_origin, y_origin - 100);

	for (int CurX = 0; CurX < cnt; CurX++) {
		for (int CurY = 0; CurY < histogram[CurX]; CurY++) {
			MoveToEx(hdc, x_origin + CurX, y_origin, 0);
			LineTo(hdc, x_origin + CurX, y_origin - histogram[CurX]);
		}
	}
}

int Getfactorial(int n) {
	if (n == 0) {
		return 1;
	}

	int fac = n;
	for (int i = n - 1; i >= 1; i--) {
		fac *= i;
	}

	return fac;
}

float getERF(float x) {
	return 0;
}

//치환 적분을 통해 알아낸 cdf방식을 취하였다.
float normal_cdf(float x, float rate, float mu = 0, float sigma = 1) {
	return (1.0+erf((x/rate -mu)/sqrt(2)/sigma))/2.0;
}

// 정규 분포 누적 분포 함수의 역함수로 이진 검색을 사용해서 역함수 근사하였다.
float inverse_normal_cdf(float p, float mu = 0, float sigma = 1, float tolerance = 0.00001)
{
	float low_z = -10.0;//normal_cdf(-10)는 0에 근접
	float low_p = 0;
	float hi_z = 10.0;//normal_cdf(10)는 1에 근접
	float hi_p = 1;
	float mid_z = 0, mid_p = 0;

	//표준 정규 분포가 아니라면 표준 정규분포로 변환해야 한다.
	if (mu != 0 || sigma != 1) return mu + sigma*inverse_normal_cdf(p,0,1, tolerance);
	while (hi_z - low_z > tolerance) {
		mid_z = (low_z + hi_z)/2.0;//중간 값
		mid_p = normal_cdf(mid_z,1.0,mu,sigma);//중간 값의 누적분포 값을 계산
		if (mid_p < p) {//중간 값이 너무 작다면 더 큰 값드을 검색
			low_z = mid_z;
			low_p = mid_p;
		}
		else if (mid_p > p) {//중간 값이 너무 크다면 더 작은 값들을 검색
			hi_z = mid_z;
			hi_p = mid_p;
		}
		else break;
	}
	return mid_z;
}

double gaussDistributeRand(int value, int range, float average = 0, float sigma = 1) {
	return 0;
}

//Gaussian으로 변환하는 식을 이용하였다.
float normal_pdf(int x, float rate, float  mu = 0, float sigma = 1) {
	double pi = M_PI;//pi값

	double sqrt_two_pi = sqrt(2 * pi);
	double _exp = exp(-pow(((double)x / rate - mu), 2) / (2 * pow(sigma, 2)));
	return _exp / (sqrt_two_pi * sigma);//계산 값을 float형태로 전달
}

float Gaussian_pdf(int x, float rate, float  mu = 0, float sigma = 1) {
	double pi = M_PI;

	double sqrt_two_pi = sqrt(2 * pi);
	double _exp = exp(-pow(((double)x / rate - mu), 2) / (2 * pow(sigma, 2)));
	return _exp / (sqrt_two_pi * sigma) *250;
}

int main(void)
{
	system("color F0");
	hwnd = GetForegroundWindow();
	hdc = GetWindowDC(hwnd);

	int y_axis = 300;

	int range = 400; // 0~400 범위 랜덤 변수
	int rcnt = 10000; // rcnt times 랜덤 변수 발생
	float avg = 1; // 평균
	float sigma = 2; // 표준편차

	srand(time(NULL));
	int random_variable[401] = { 0, }; // 랜덤 변수 count
	int Gaussian_random_variable[401] = { 0, }; // 가우시안 변수 카운트
	float norm_Gaussian_random_variable[401] = { 0, };
	float gaussianCDF[401] = { 0, }; // 랜덤 가우시안 분포 CDF
	float gaussianCDF_true[401] = { 0, }; // 실제 정답 가우시안 분포 CDF

	//랜덤으로 변수를 뽑아서 해당하는 숫자의 random_variable 빈도수 하나씩 늘리기
	for (int i = 0; i < rcnt; i++) {
		int num = rand() % (range);
		num -= range / 2;
		for (int j = -199; j < 200; j++) {
			if (j == num)
				random_variable[j + 200] += 1;
		}
	}

	float num = 0;
	float tolerance = 0.00001;//무한루프를 돌지 않도록 값을 지정하였다.
	int check = 0;//잘 돌았는지 확인하였다.
	int real_num = 0;

	for (int i = 0; i < range; i++) {
		num = inverse_normal_cdf(i/400.0, avg, sigma,tolerance);
		real_num= (int)(num * 20);
		for (int j = -200; j < 200; j++) {
			if (j == real_num) {
				Gaussian_random_variable[j + 200] += random_variable[i];
				check += 1;
				break;
			}
		}
	}
	Gaussian_random_variable[200] /= 2;

	for (int i = 0; i < range; i++) {
		for (int j = 0; j < i; j++) {
			gaussianCDF[i] += (Gaussian_random_variable[j])*0.01;
		}
		Draw(gaussianCDF[i], 500, y_axis + 300, i, BLUE);
	}

	//nomalization(Gaussian_random_variable, norm_Gaussian_random_variable, range, rcnt);

	DrawHistogram(random_variable, 30, y_axis - 100, range);  // 랜덤 변수 분포 출력
	DrawHistogram(Gaussian_random_variable, 500, y_axis, range); // 가우시안 변수 분포 출력

	//이론적인 g_pdf를 구하는 소스코드
	for (int i = 0; i < range; i++) {
		float value = normal_pdf(i - 1 * range / 2, range / 20, avg, sigma);
		Draw(value * 250, 500, y_axis, i, RED);
	}

	//이론적인 g_cdf를 구하는 소스코드
	for (int i = 0; i < range; i++) {

		float value = normal_cdf((float)(i-1*range/2), 20.0, avg, sigma);
		gaussianCDF_true[i] = value;
		Draw(value*100, 500, y_axis + 300, i, RED);
	}

	float diff[400] = { 0.0 , }; // (이론적인 g_cdf의 값) - (내가 작성한 gaussian pdf를 더해 만든 cdf 값)
	float ab_diff[400] = { 0.0, };
	float max = 0.0;
	//k-s test
	for (int i = 0; i < range; i++) {
		diff[i] = gaussianCDF_true[i] - gaussianCDF[i];

		//절댓값 구하기
		if (diff[i] < 0)
			ab_diff[i] = -diff[i];
		else ab_diff[i] = diff[i];

		//절댓값의 최댓값 구하기
		if (diff[i] > max)
			max = diff[i];
	}

	//기준치 설정
	float baseline = 0;
	float num1 = 0.0;

	if (baseline == 0.1) num1 = 1.073;
	else if (baseline == 0.05) num1 = 1.224;
	else if (baseline == 0.025) num1 = 1.358;
	else if (baseline == 0.01) num1 = 1.517;
	else if (baseline == 0.005) num1 = 1.628;
	else num1 = 1.858;

	if (max > num1*sqrt((range + range) / (range*range))) printf("PASS");

	DrawHistogram(gaussianCDF, 30, y_axis + 100, range);//가우시안 cdf 출력
	DrawHistogram(gaussianCDF_true, 30, y_axis + 100, range); // true 가우시안 cdf 정답 출력

	return 0;
}