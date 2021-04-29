#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>
void InverseImage(BYTE* Img, BYTE *Out, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = 255 - Img[i];
	}
}
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)
{
	int ImgSize = W * H;
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
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0)
		{
			Out[i] = 255;
		}
		else 	Out[i] = (BYTE)(Img[i] * Val);
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

void ObtainAHistogram(int* Histo, int* AHisto)
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
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);
	}
}
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)
{
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

int GozalezBinThresh(int* Histo)
{
	BYTE Low = 0, High = 0;
	double m1 = 0.0, m2 = 0.0;
	int g1, g2, sum1, sum2, temp = 0, T;//g1 g2�� ������ ������ ������ ��
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

void AverageConv(BYTE* Img, BYTE* Out, int W, int H) // �ڽ���Ȱȭ
{
	double Kernel[3][3] = {0.11111, 0.11111, 0.11111,
										0.11111, 0.11111, 0.11111,
										0.11111, 0.11111, 0.11111	};
	double SumProduct = 0.0;
	for (int i = 1; i < H-1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W-1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i+m)*W + (j+n)] * Kernel[m+1][n+1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) // ����þ���Ȱȭ
{
	double Kernel[3][3] = {0.0625, 0.125, 0.0625,
										0.125, 0.25, 0.125,
										0.0625, 0.125, 0.0625 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
										-1.0, 0.0, 1.0,
										-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										0.0, 0.0, 0.0,
										1.0, 1.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
										-2.0, 0.0, 2.0,
										-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 1020  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -2.0, -1.0,
										0.0, 0.0, 0.0,
										1.0, 2.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 8.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 2040  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 9.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			//Out[i * W + j] = abs((long)SumProduct) / 8;
			if (SumProduct > 255.0) Out[i * W + j] = 255;
			else if (SumProduct < 0.0) Out[i * W + j] = 0;
			else Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, 
	RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName)
{
	FILE * fp = fopen(FileName, "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W*H, fp);
	fclose(fp);
}

void swap(BYTE* a, BYTE* b) 
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

BYTE Median(BYTE* arr, int size)
{
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S/2];
}

BYTE MaxPooling(BYTE* arr, int size)
{
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S-1];
}

BYTE MinPooling(BYTE* arr, int size)
{
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[0];
}
//*5����
void MedianFiltering(BYTE* Image, BYTE* Output, int W, int H, int Win) {
	int Length = Win;  // ����ũ�� �� ���� ����
	int Margin = Length / 2;//����ũ �Ѻ��� ���̸� 2�� ������ ������ �����. mapping�� ���� ���̴�
	int WSize = Length * Length;//window����� �Ѻ��� ������ �������� �����
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize);// Wsizeũ���� temp�� �����Ѵ�
	int i, j, m, n;//int���� i,j,m,n�� �����Ѵ�
	for (i = Margin; i < H - Margin; i++) {//// Y��ǥ (��)
		for (j = Margin; j < W - Margin; j++) {// X��ǥ (��)
			for (m = -Margin; m <= Margin; m++) {// Kernel �߽ɱ��� ���ι��� �̵�(��)
				for (n = -Margin; n <= Margin; n++) {// Kernel �߽ɱ��� ���ι��� �̵�(��)
					temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m) * W + j + n];//temp ��ǥ�� image��ǥ�� ��Ⱚ�� ��Ƽ� �����Ѵ�
				}
			}
			Output[i * W + j] = Median(temp, WSize);//temp�� WSize�� �����Ͽ� temp�� ����� ��Ⱚ�� �߰��� output�� �ִ´�
		}
	}
}
int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top)
{
	if (*top >= arr_size) return(-1);
	(*top)++;
	stackx[*top] = vx;
	stacky[*top] = vy;
	return(1);
}

int pop(short* stackx, short* stacky, short* vx, short* vy, int* top)
{
	if (*top == 0) return(-1);
	*vx = stackx[*top];
	*vy = stacky[*top];
	(*top)--;
	return(1);
}


// GlassFire �˰����� �̿��� �󺧸� �Լ�
void m_BlobColoring(BYTE* CutImage, int height, int width)//���������� ����
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;


	// �������� ����� �޸� �Ҵ�
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];

	int arr_size = height * width;

	// �󺧸��� �ȼ��� �����ϱ� ���� �޸� �Ҵ�

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // �޸� �ʱ�ȭ

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// �̹� �湮�� ���̰ų� �ȼ����� 255�� �ƴ϶�� ó�� ����
			if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
			r = i; c = j; top = 0; area = 1;
			curColor++;

			while (1)
			{
			GRASSFIRE:
				for (m = r - 1; m <= r + 1; m++)
				{
					index = m * width;
					for (n = c - 1; n <= c + 1; n++)
					{
						//���� �ȼ��� �����踦 ����� ó�� ����
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // ���� �󺧷� ��ũ
							if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
							r = m; c = n; area++;
							goto GRASSFIRE;
						}
					}
				}
				if (pop(stackx, stacky, &r, &c, &top) == -1) break;
			}
			if (curColor < 1000) BlobArea[curColor] = area;
		}
	}

	float grayGap = 255.0f / (float)curColor;//curColor�� �� component���� ���⼭�� 25��
	//��ŭ ���� ��ȭ�� �ٰ����� ��Ÿ���� graygap
	 
	// ���� ������ ���� ������ ã�Ƴ��� ���� 
	for (i = 1; i <= curColor; i++)
	{
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage �迭 Ŭ����~
	for (k = 0; k < width * height; k++) CutImage[k] = 255;

	// coloring�� ����� �󺧸� ����� (Out_Area�� �����) ������ ���� ū �͸� CutImage�� ����
	for (k = 0; k < width * height; k++)
	{
		//if (BlobArea[coloring[k]]>500) CutImage[k] = 0;//500�̻��� �Ǵ� ��Ҹ� ����(Ư�������̻� ���)
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // ���� ū �͸� ����(size fillitering) �ܰ��� 0���� ó��(��Ӱ�)
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap);//���� �󺧸��ؼ� 10�� �ִ��ϸ� 255�� 10���� ������ ���� 25.5���������� ����
	}
// �󺧸� �� ���� ���� ������ ���ؼ��� �̾Ƴ��� �ڵ� ����
	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}

void BinaryImageEdgeDetection(BYTE* Bin, BYTE* Out, int W, int H) {
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Bin[i * W + j] == 0)
			{//����ȭ�Ҷ��
				if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 && Bin[i * W + j - 1] == 0 && Bin[i * W + j + 1] == 0))//����ȭ���ֺ��� 0�� �ƴ϶�� ��,�����
					Out[i * W + j] = 255;//�Ͼ�� ĥ�ϱ�
			}
		}
	}
}
void DrawRectOutline(BYTE* Img, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) {
	//Img: ����/���� ������ �׸� �̹����迭, W: ���� ���λ�����, H: ���� ���λ�����, 
    //LU_X: �簢���� ������� X��ǥ, LU_Y: �簢���� ������� Y��ǥ, RD_X: �簢���� �����ϴ� X��ǥ, RD_Y: �簢���� �����ϴ� Y��ǥ.
	for (int j = LU_X; j < RD_X; j++) {//�ϴܰ���
		Img[RD_Y * W + j] = 255;
	}
	for (int j = LU_X; j < RD_X; j++) {//��ܰ���
		Img[LU_Y * W + j] = 255;
	}
	for (int i = RD_Y; i <= LU_Y; i++) {//������ ����
		Img[i * W + RD_X] = 255;
	}
	for (int i = RD_Y; i <= LU_Y; i++) {//���ʼ���
		Img[i * W + LU_X] = 255;
	}
}
void DrawCrossLine(BYTE* Img, int W, int H, int Cx, int Cy) {
// Img: ����/���� ������ �׸� �̹����迭, W: ���� ���λ�����, H: ���� ���λ�����,
// Cx: ����/���� ������ �����Ǵ� ������ X��ǥ ,Cy: ����/���� ������ �����Ǵ� ������ Y��ǥ
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (i == Cy || j == Cx) {
				Img[i * W + j] = 255;
			}
		}
	}

}
void Translation(BYTE *Img,BYTE* Output,int Tx,int Ty,int H,int W) {
	Ty *= -1;//bmp������ �������ֱ⿡
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if ((i + Ty < H && i + Ty >= 0) && (j + Tx < W && j + Ty >= 0))
				Output[(i + Ty) * W + (j + Tx)] = Img[i * W + j];
		}
	} 
}
void VerticalFlip(BYTE* Img, int W, int H) {//���Ʒ� ������
	for (int i = 0; i < H / 2; i++) {//Y��ǥ
		for (int j = 0; j < W; j++) {//x��ǥ
			swap(&Img[i * W + j], &Img[(H - 1-i) * W + j]);
		}
	}
}
void HorizontalFlip(BYTE* Img, int W, int H) { //�¿� ������
	for (int i = 0; i < W / 2; i++) {//x��ǥ
		for (int j = 0; j < H; j++) {//Y��ǥ
			swap(&Img[j * W + i], &Img[j * W + (W-1-i)]);
		}
	}
}
void Scaling(BYTE* Image, BYTE* Output, int W, int H, double SF_x, double SF_y) {
	int Tmpx, Tmpy;
	for (int i = 0; i < H; i++) {//���̸��� ��ġ i,j
		for (int j = 0; j < W; j++) {
			Tmpx = (int)(j / SF_x);//Tmpx,y�� ���� ��ġ�� ���� 
			Tmpy = (int)(i / SF_y);//�׷��⿡ ������
			if (Tmpy < H && Tmpx < W)
				Output[i * W + j] = Image[Tmpy * W + Tmpx];
		}
	}
}//�̰��� ������ �ڵ���

void Rotation(BYTE* Image, BYTE* Output, int W, int H, int Angle) {
	int tmpX, tmpY;//���� ������ǥ
	double Radian = Angle * 3.141592 / 180.0;//������
	for (int i = 0; i < H; i++) {//��ȯ�� ���� y
		for (int j = 0; j < W; j++) {//��ȯ�� ���� x
			tmpX = (int)(cos(Radian) * j + sin(Radian) * i);//ȸ������� �������
			tmpY = (int)(-sin(Radian) * j + cos(Radian) * i);
			if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0))
				Output[i * W + j] = Image[tmpY * W + tmpX];//
		}
	}
}//���������� �������� �̿�
int main()
{
	BITMAPFILEHEADER hf; // 14����Ʈ
	BITMAPINFOHEADER hInfo; // 40����Ʈ
	RGBQUAD hRGB[256]; // 1024����Ʈ
	FILE* fp;
	fp = fopen("LENNA.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE* Image = (BYTE*)malloc(ImgSize);
	BYTE* Temp = (BYTE*)malloc(ImgSize); // �ӽù迭
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);//������ ������ ��������̴� ������ bmp�� ���Ʒ��� �������ִ�.
	
	int Histo[256] = { 0 };
	int AHisto[256] = { 0 };
	int H = hInfo.biHeight;
	int W = hInfo.biWidth;
	int LU_X = W, LU_Y = 0, RD_X = 0, RD_Y = H;
	int sumx = 0, sumy = 0, Cx = 0, Cy = 0, cnt = 0;
	int Tx = 50, Ty = 30;
	//2����
	/*for (int i = 0; i < ImgSize; i++)
		Output[i] = Image[i];

	fp = fopen("output1.bmp", "wb"); //����
	
	for (int i = 0; i < ImgSize; i++)
		Output[i] = Image[i] + 50;
	fp = fopen("output2.bmp", "wb"); //��� 50����
	
	for (int i = 0; i < ImgSize; i++)
		Output[i] = 255 - Image[i];
	fp = fopen("output3.bmp", "wb");//���� */
	
	//*3����
	/*ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	ObtainAHistogram(Histo, AHisto);
	HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);*/
	/*int Thres = GozalezBinThresh(Histo);
	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);*/
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);

	//*4����
	/*AverageConv(Image, Output, hInfo.biWidth, hInfo.biHeight);*/
	//GaussAvrConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	/*Sobel_X_Conv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
	Sobel_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	for (int i = 0; i < ImgSize; i++) {
		if (Temp[i] > Output[i]) 	Output[i] = Temp[i];
	}
	Binarization(Output, Output, hInfo.biWidth, hInfo.biHeight, 40);*/
	/*GaussAvrConv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
	Laplace_Conv_DC(Temp, Output, hInfo.biWidth, hInfo.biHeight);*/
	
	
	//*6����
	/*Binarization(Image, Temp, hInfo.biWidth, hInfo.biHeight, 50);//������� ������,�������
	InverseImage(Temp, Temp, W, H);//������ ���� �Ͼ��
	m_BlobColoring(Temp, hInfo.biHeight, hInfo.biWidth);//���⼭���� Ư������1���� ��Ӱ� ó��
	for (int i = 0; i < ImgSize; i++)
		Output[i] = Image[i];
	BinaryImageEdgeDetection(Temp, Output, W, H);//Ư¡�ܰ��׸���

	//�簢�� ��ǥ���ϱ�
	for (int i = 0; i < H; i++) {//����ȭ�� �ϴ� ���ؼ� ������ ���� ���Ž�Ű��
		for (int j = 0; j < W; j++) {
			if (Temp[i * W + j] == 0) { //����ȭ�Ҷ��
				if (i >= LU_Y) LU_Y = i; //�簢���� ������� y��ǥ, ū�𰻽�
				if (j <= LU_X) LU_X = j;//�簢���� ������� x��ǥ , �����𰻽�
				if (i <= RD_Y) RD_Y = i;//�簢���� �����ϴ� y��ǥ , �����𰻽�
				if (j >= RD_X) RD_X = j;//�簢���� �����ϴ� x��ǥ , ū�𰻽�
				cnt++;//�ȼ�����
				sumx = sumx + j;
				sumy = sumy + i;
			}
		}
	}
	
	//�����߽�
	Cx = sumx / cnt;
	Cy = sumy / cnt;
	
	printf("������� x,y: %d %d", LU_X, LU_Y);
	printf("�����ϴ� x,y:%d %d", RD_X, RD_Y);
	DrawRectOutline(Output, W, H, LU_X, LU_Y, RD_X, RD_Y);//�����簢�� �׸���
	DrawCrossLine(Output, W, H, Cx, Cy); //�����߽ɱ��ؼ� ���ڰ��׸���
	*/

	//*7����
	//������ ������ ��������̴� ������ bmp�� ���Ʒ��� �������ִ�.
	//HorizontalFlip(Image, W, H);
	//VerticalFlip(Image, W, H);
	//Translation(Image, Output, Tx, Ty, H, W);
	//Scaling(Image, Output, W, H, 0.7, 0.7);
	Rotation(Image, Output, W, H, -30);//������ ������ �߽����� ȸ��
	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "output_lenna.bmp");


	free(Image);
	free(Output);
	free(Temp);
	return 0;
}