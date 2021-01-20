#define _CRT_SECURE_NO_DEPRECATE
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <stdlib.h>

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

int main(void) {

	system("color F0");
	hwnd = GetForegroundWindow();
	hdc = GetWindowDC(hwnd);

	int y_axis = 300;

	int w = 256, h = 256; // ������ ���μ��ΰ� ����  �ȼ��� �ִ� ����
	int wh = w*h;
	int sum = 0;

	char fname[] = "gLenna256_256.raw";//���� ���� ��
	char fname2[] = "Output.txt";//�� ���� ��

	FILE *f, *g;

	unsigned char **pp;
	int i = 0, j = 0;
	int range = 256;//RGB:0~255->256
	pp = (unsigned char**)malloc(sizeof(unsigned char*)*h);

	float i_rgb[256] = { 0 };
	float s_rgb[256] = { 0 };
	float new_gray_level[256] = { 0.0 };
	float ori_cdf[256] = { 0.0 };
	float s_cdf[256] = { 0.0 };

	float after_rgb[256] = { 0.0 };
	float after_s_rgb[256] = { 0.0 };

	for (i = 0; i < h; i++) { pp[i] = (unsigned char*)malloc(sizeof(unsigned char)*w); }


	//���� ����

	if ((f = fopen(fname, "rb")) == NULL) exit(1);//���� ���� rb�� ���� ������ �� ������ ���� ��� exit

	if ((g = fopen(fname2, "wb")) == NULL) exit(1);//�� ���� wb�� ���� ������ �� ������ ������� exit

	//f ������ �о pp�� ����
	for (i = 0; i < h; i++) fread(pp[i], sizeof(unsigned char), w, f);

	//i_rgb�� pp�� �󵵼��� ����
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			i_rgb[int(pp[i][j])]++;
		}
	}

	/*
	1. ������ rgb ��հ� ���ϱ�
	2. �󵵼��� ��Ÿ�� i_rgb�� Ȯ���� ��ȯ
	3. i_rgb�� �츮 ������ �� Ȯ���ϱ� ���ؼ� s_rgb�� ����(s_rgb=i_rgb*2^(12))
	4. i_rgb Ȯ���� cdf�� ori_cdf�� ����
	*/
	for (i = 0; i < 256; i++) {
		sum += i_rgb[i];
		new_gray_level[i] = round((((float)sum) * 255.0) / float(wh));
		i_rgb[i] /= wh;
		s_rgb[i] = i_rgb[i] * 4096; // ���� ���� ��ȯ (Ȯ�� * 2^(12))
		for (j = 0; j < i; j++) ori_cdf[i] += i_rgb[j];
	}

//	DrawHistogram(new_gray_level, 500, y_axis + 300, range);

//���� pdf & cdf ���
	DrawHistogram(s_rgb, 30, y_axis - 100, range);//pdf�� 4096���ؼ� ���

	for (int i = 0; i < range; i++) {
		float value = ori_cdf[i];
		Draw(value * 100, 30, y_axis - 100, i, BLUE);
	}//cdf�� 100���ؼ� �Ķ������� ���

	/*
	1. pp�� round�� new_gray_level�� ����
	2. equalization�� ������ Output.txt�� ����
	*/
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			pp[i][j]= (unsigned char)new_gray_level[pp[i][j]];
		}
		fwrite(pp[i], sizeof(unsigned char), w, g);
	}

//equalization���� pdf & cdf ���
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			after_rgb[int(pp[i][j])]++;
		}
	}

	for (int i = 0; i < 256; i++) {
		after_rgb[i] /= wh;
		after_s_rgb[i] = after_rgb[i] * 4096;
	}

	DrawHistogram(after_s_rgb, 500, y_axis - 100, range);

	for (i = 0; i < h; i++)	free(pp[i]);//���� �Ҵ� ����

	free(pp);
	fclose(f);
	fclose(g);

	return 0;

}