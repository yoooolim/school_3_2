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

float inverse_normal_cdf(float p, float cdf[256], float tolerance = 0.00002)
{
	float low_z = 0;//normal_cdf(-10)�� 0�� ����
	float low_p = 0;
	float hi_z = 256;//normal_cdf(10)�� 1�� ����
	float hi_p = 1;
	float mid_z = 0, mid_p = 0;

	//ǥ�� ���� ������ �ƴ϶�� ǥ�� ���Ժ����� ��ȯ�ؾ� �Ѵ�.
	while (hi_z - low_z > tolerance) {
		float check = hi_z - low_z;
		mid_z = (low_z + hi_z) / 2.0;//�߰� ��
		mid_p = cdf[int(mid_z+0.5)];//�߰� ���� �������� ���� ���
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

int main(void) {

	system("color F0");
	hwnd = GetForegroundWindow();
	hdc = GetWindowDC(hwnd);

	int y_axis = 300;

	int w = 512, h = 512; // ������ ���μ��ΰ� ����  �ȼ��� �ִ� ����
	int wh = w*h;

	int w_o = 256, h_o = 256;
	int wh_o = w_o*h_o;

	int sum = 0;

	char fname[] = "barbara(512x512).raw";//���� ���� ��
	char fname2[] = "Output.txt";//�� ���� ��
	char fnameo[] = "gAirplane256_256.raw";

	FILE *f, *g, *option;

	unsigned char **pp;
	unsigned char **pp_o;
	int i = 0, j = 0;
	int range = 256;//RGB:0~255->256
	pp = (unsigned char**)malloc(sizeof(unsigned char*)*h);
	pp_o = (unsigned char**)malloc(sizeof(unsigned char*)*h_o);

	float oi_rgb[256] = { 0.0 };
	float os_rgb[256] = { 0.0 };
	float oi_cdf[256] = { 0.0 };

	float i_rgb[256] = { 0.0 };
	float s_rgb[256] = { 0.0 };
	float new_gray_level[256] = { 0.0 };
	float ori_cdf[256] = { 0.0 };
	float s_cdf[256] = { 0.0 };

	float after_rgb[256] = { 0.0 };
	float after_s_rgb[256] = { 0.0 };
	float after_cdf[256] = { 0.0 };

	float after_o_rgb[256] = { 0.0 };
	float after_os_rgb[256] = { 0.0 };
	float after_o_cdf[256] = { 0.0 };

	float inverse_pdf[256] = { 0.0 };
	float inverse_cdf[256] = { 0.0 };

	for (i = 0; i < h; i++) { pp[i] = (unsigned char*)malloc(sizeof(unsigned char)*w); }
	for (i = 0; i < h_o; i++) { pp_o[i] = (unsigned char*)malloc(sizeof(unsigned char)*w_o); }


	//���� ����

	if ((f = fopen(fname, "rb")) == NULL) exit(1);//���� ���� rb�� ���� ������ �� ������ ���� ��� exit

	if ((g = fopen(fname2, "wb")) == NULL) exit(2);//�� ���� wb�� ���� ������ �� ������ ������� exit

	if ((option = fopen(fnameo, "rb")) == NULL) exit(3);//���ؼ� �ٲ� ���� rb�� ���� ������ �� ������ ���� ��� exit
//////////////////////////////////////////////////////////////////////////	
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
			pp[i][j] = (unsigned char)new_gray_level[pp[i][j]];
		}
	}

//equalization���� pdf & cdf ���
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			after_rgb[int(pp[i][j])]++;
		}
	}

	for (i = 0; i < 256; i++) {
		after_rgb[i] /= wh;
		after_s_rgb[i] = after_rgb[i] * 4096;
		for (j = 0; j < i; j++) after_cdf[i] += after_rgb[j];
	}

	DrawHistogram(after_s_rgb, 500, y_axis - 100, range);

	for (int i = 0; i < range; i++) {
		float value = after_cdf[i];
		Draw(value * 100, 500, y_axis - 100, i, BLUE);
	}
/////////////////////////////////////////////////////////////////////////////////

	//option ������ �о pp_o�� ����
	for (i = 0; i < h_o; i++) fread(pp_o[i], sizeof(unsigned char), w_o, option);

	//oi_rgb�� pp_o�� �󵵼��� ����
	for (i = 0; i < h_o; i++) {
		for (j = 0; j < w_o; j++) {
			oi_rgb[int(pp_o[i][j])]++;
		}
	}

	/*
	1. option ������ rgb ��հ� ���ϱ�
	2. �󵵼��� ��Ÿ�� oi_rgb�� Ȯ���� ��ȯ
	3. oi_rgb�� �츮 ������ �� Ȯ���ϱ� ���ؼ� os_rgb�� ����(os_rgb=oi_rgb*2^(12))
	4. oi_rgb Ȯ���� cdf�� oi_cdf�� ����
	*/
	sum = 0;
	for (i = 0; i < 256; i++) {
		sum += oi_rgb[i];
		new_gray_level[i] = round((((float)sum) * 255.0) / float(wh_o));
		oi_rgb[i] /= wh_o;
		os_rgb[i] = oi_rgb[i] * 4096; // ���� ���� ��ȯ (Ȯ�� * 2^(12))
		for (j = 0; j < i; j++) oi_cdf[i] += oi_rgb[j];
	}

//option pdf & cdf ���
	DrawHistogram(os_rgb, 30, y_axis + 300, range);//pdf�� 4096���ؼ� ���

	for (i = 0; i < range; i++) {
		float value = oi_cdf[i];
		Draw(value * 100, 30, y_axis + 300, i, BLUE);
	}//cdf�� 100���ؼ� �Ķ������� ���

///////////////////////////////////////////////////////////////////////////////////////

	float value = 0;
	sum = 0;

	for (i = 0; i < range; i++) {
		value = inverse_normal_cdf(i/256.0, oi_cdf, 0.00002);
		Draw(value*100.0/256.0, 30, y_axis -100, i, RED);
		for (j = 0; j < range; j++) { 
			if (j == int(value)) { 
				inverse_pdf[j] += i_rgb[i]; 
				break;
			}
		}
		inverse_pdf[i] *= wh;
		sum += inverse_pdf[i];
		inverse_cdf[i]= round((((float)sum) * 255.0) / float(wh));
	}

	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			pp[i][j] = (unsigned char)inverse_cdf[pp[i][j]];
		}

		fwrite(pp[i], sizeof(unsigned char), w, g);
	}

	for (i = 0; i < 256; i++) inverse_pdf[i] = inverse_pdf[i] * 4096 / wh;

	DrawHistogram(inverse_pdf, 500, y_axis + 300, range);

	for (i = 0; i < h; i++)	free(pp[i]);//���� �Ҵ� ����
	for (i = 0; i < h_o; i++)free(pp_o[i]);//���� �Ҵ� ����

	free(pp);
	free(pp_o);

	fclose(f);
	fclose(g);
	fclose(option);

	return 0;

}