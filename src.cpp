#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
void InverseImage(BYTE* Img, BYTE *Out, int W, int H)//������Ű�� �Լ�
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = 255 - Img[i];
	}
}
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)//(���� ��ȭ�ϰ� ������ŭ))//��� �����Լ�
{
	int ImgSize = W * 
		H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] + Val > 255)
		{
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0)
		{
			Out[i] = 0;
		}
		else 	Out[i] =Img[i] + Val;
	}
}
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val)//����Լ�
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0)
		{
			Out[i] = 255;
		}
		else 	Out[i] = (BYTE)(Img[i] * Val);// ����Ÿ���� �ٽ� ����Ʈ Ÿ������
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)//������׷� ���ϴ� �Լ�
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++; // Ư�� ȭ�Ұ��� ������ img�� ���� histo�� �ְ� �������Ѽ� histo�� �� ȭ�Ҽ��� �� ����� ��Ÿ��
	}
}

void ObtainAHistogram(int* Histo, int* AHisto)//����������׷�
{
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] += Histo[j];
		}
	}
}

void HistogramStretching(BYTE * Img, BYTE * Out, int * Histo, int W, int H)
{
	int ImgSize = W * H;
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {//���� ������
		if (Histo[i] != 0) {//low�� ���� ���ʿ� ��ġ
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {//���� ū��
		if (Histo[i] != 0) {//high�� ���� �����ʿ� ��ġ
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);//���ںи� byte�� ������ ���������°� �������� ������
	}
}
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)
{//��Ȱȭ 
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = NormSum[Img[i]];
	}
}

void Binarization(BYTE * Img, BYTE * Out, int W, int H, BYTE Threshold)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

int GozalezBinThresh(int *Histo)
{
	BYTE Low = 0, High = 0;
	double m1 = 0.0, m2 = 0.0;
	int g1, g2, sum1, sum2, temp=0, T;//g1 g2�� ������ ������ ������ ��
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}

	T = (int)((High + Low) / 2);

	do {
		sum1 = 0;
		g1 = 0;
		sum2 = 0;
		g2 = 0;

		for (int i = Low; i <= T; i++) {
			sum1 += i * Histo[i];
			g1 += Histo[i];
		}
		m1 = sum1 / (double)g1;

		for (int i = T + 1; i <= High; i++) {
			sum2 += i * Histo[i];
			g2 += Histo[i];
		}
		m2 = sum2 / (double)g2;

		T = (int)((m1 + m2) / 2.0);
		

		if ((T - temp < 3) && (T - temp > -3)) {//ex) T1�� temp0�� if���� ���Ե� �׷��Ƿ� ����� ������ T�� ���;���
			break;
		}
		temp = T; 
	} while (1);

	return T; //temp ��ȯ�� if�� ������� �ϳ� �۾������� ������ ������ T��ȯ
}
int main()
{
	BITMAPFILEHEADER hf; // 14����Ʈ
	BITMAPINFOHEADER hInfo; // 40����Ʈ
	RGBQUAD hRGB[256]; // 1024����Ʈ
	FILE* fp;
	fp = fopen("coin.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE * Image = (BYTE *)malloc(ImgSize);
	BYTE * Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	int Histo[256] = { 0 };//0���� 255���� ȭ�Ұ��� �ֱ⿡ 256�迭 
	int AHisto[256] = { 0 };

	ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	/*ObtainAHistogram(Histo, AHisto);
	HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);*/
	int Thres = GozalezBinThresh(Histo);
	printf("%d", Thres);
	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
	


	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);

	fp = fopen("output.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	free(Image);
	free(Output);
	return 0;
}