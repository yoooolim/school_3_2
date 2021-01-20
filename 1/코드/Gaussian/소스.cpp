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

//ġȯ ������ ���� �˾Ƴ� cdf����� ���Ͽ���.
float normal_cdf(float x, float rate, float mu = 0, float sigma = 1) {
	return (1.0+erf((x/rate -mu)/sqrt(2)/sigma))/2.0;
}

// ���� ���� ���� ���� �Լ��� ���Լ��� ���� �˻��� ����ؼ� ���Լ� �ٻ��Ͽ���.
float inverse_normal_cdf(float p, float mu = 0, float sigma = 1, float tolerance = 0.00001)
{
	float low_z = -10.0;//normal_cdf(-10)�� 0�� ����
	float low_p = 0;
	float hi_z = 10.0;//normal_cdf(10)�� 1�� ����
	float hi_p = 1;
	float mid_z = 0, mid_p = 0;

	//ǥ�� ���� ������ �ƴ϶�� ǥ�� ���Ժ����� ��ȯ�ؾ� �Ѵ�.
	if (mu != 0 || sigma != 1) return mu + sigma*inverse_normal_cdf(p,0,1, tolerance);
	while (hi_z - low_z > tolerance) {
		mid_z = (low_z + hi_z)/2.0;//�߰� ��
		mid_p = normal_cdf(mid_z,1.0,mu,sigma);//�߰� ���� �������� ���� ���
		if (mid_p < p) {//�߰� ���� �ʹ� �۴ٸ� �� ū ������ �˻�
			low_z = mid_z;
			low_p = mid_p;
		}
		else if (mid_p > p) {//�߰� ���� �ʹ� ũ�ٸ� �� ���� ������ �˻�
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

//Gaussian���� ��ȯ�ϴ� ���� �̿��Ͽ���.
float normal_pdf(int x, float rate, float  mu = 0, float sigma = 1) {
	double pi = M_PI;//pi��

	double sqrt_two_pi = sqrt(2 * pi);
	double _exp = exp(-pow(((double)x / rate - mu), 2) / (2 * pow(sigma, 2)));
	return _exp / (sqrt_two_pi * sigma);//��� ���� float���·� ����
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

	int range = 400; // 0~400 ���� ���� ����
	int rcnt = 10000; // rcnt times ���� ���� �߻�
	float avg = 1; // ���
	float sigma = 2; // ǥ������

	srand(time(NULL));
	int random_variable[401] = { 0, }; // ���� ���� count
	int Gaussian_random_variable[401] = { 0, }; // ����þ� ���� ī��Ʈ
	float norm_Gaussian_random_variable[401] = { 0, };
	float gaussianCDF[401] = { 0, }; // ���� ����þ� ���� CDF
	float gaussianCDF_true[401] = { 0, }; // ���� ���� ����þ� ���� CDF

	//�������� ������ �̾Ƽ� �ش��ϴ� ������ random_variable �󵵼� �ϳ��� �ø���
	for (int i = 0; i < rcnt; i++) {
		int num = rand() % (range);
		num -= range / 2;
		for (int j = -199; j < 200; j++) {
			if (j == num)
				random_variable[j + 200] += 1;
		}
	}

	float num = 0;
	float tolerance = 0.00001;//���ѷ����� ���� �ʵ��� ���� �����Ͽ���.
	int check = 0;//�� ���Ҵ��� Ȯ���Ͽ���.
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

	DrawHistogram(random_variable, 30, y_axis - 100, range);  // ���� ���� ���� ���
	DrawHistogram(Gaussian_random_variable, 500, y_axis, range); // ����þ� ���� ���� ���

	//�̷����� g_pdf�� ���ϴ� �ҽ��ڵ�
	for (int i = 0; i < range; i++) {
		float value = normal_pdf(i - 1 * range / 2, range / 20, avg, sigma);
		Draw(value * 250, 500, y_axis, i, RED);
	}

	//�̷����� g_cdf�� ���ϴ� �ҽ��ڵ�
	for (int i = 0; i < range; i++) {

		float value = normal_cdf((float)(i-1*range/2), 20.0, avg, sigma);
		gaussianCDF_true[i] = value;
		Draw(value*100, 500, y_axis + 300, i, RED);
	}

	float diff[400] = { 0.0 , }; // (�̷����� g_cdf�� ��) - (���� �ۼ��� gaussian pdf�� ���� ���� cdf ��)
	float ab_diff[400] = { 0.0, };
	float max = 0.0;
	//k-s test
	for (int i = 0; i < range; i++) {
		diff[i] = gaussianCDF_true[i] - gaussianCDF[i];

		//���� ���ϱ�
		if (diff[i] < 0)
			ab_diff[i] = -diff[i];
		else ab_diff[i] = diff[i];

		//������ �ִ� ���ϱ�
		if (diff[i] > max)
			max = diff[i];
	}

	//����ġ ����
	float baseline = 0;
	float num1 = 0.0;

	if (baseline == 0.1) num1 = 1.073;
	else if (baseline == 0.05) num1 = 1.224;
	else if (baseline == 0.025) num1 = 1.358;
	else if (baseline == 0.01) num1 = 1.517;
	else if (baseline == 0.005) num1 = 1.628;
	else num1 = 1.858;

	if (max > num1*sqrt((range + range) / (range*range))) printf("PASS");

	DrawHistogram(gaussianCDF, 30, y_axis + 100, range);//����þ� cdf ���
	DrawHistogram(gaussianCDF_true, 30, y_axis + 100, range); // true ����þ� cdf ���� ���

	return 0;
}