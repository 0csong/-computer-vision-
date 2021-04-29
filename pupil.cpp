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
	int g1, g2, sum1, sum2, temp = 0, T;//g1 g2로 영역을 나눈후 각각의 합
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


		if ((T - temp < 3) && (T - temp > -3)) {//ex) T1과 temp0이 if문을 들어가게됨 그러므로 출력은 현재인 T가 나와야함
			break;
		}
		temp = T;
	} while (1);

	return T; //temp 반환시 if문 통과보다 하나 작아진값이 나오게 됨으로 T반환
}

void AverageConv(BYTE* Img, BYTE* Out, int W, int H) // 박스평활화
{
	double Kernel[3][3] = {0.11111, 0.11111, 0.11111,
										0.11111, 0.11111, 0.11111,
										0.11111, 0.11111, 0.11111	};
	double SumProduct = 0.0;
	for (int i = 1; i < H-1; i++) { // Y좌표 (행)
		for (int j = 1; j < W-1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i+m)*W + (j+n)] * Kernel[m+1][n+1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) // 가우시안평활화
{
	double Kernel[3][3] = {0.0625, 0.125, 0.0625,
										0.125, 0.25, 0.125,
										0.0625, 0.125, 0.0625 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
										-1.0, 0.0, 1.0,
										-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										0.0, 0.0, 0.0,
										1.0, 1.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
										-2.0, 0.0, 2.0,
										-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 1020  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -2.0, -1.0,
										0.0, 0.0, 0.0,
										1.0, 2.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 8.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 2040  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 9.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
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
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S/2];
}

BYTE MaxPooling(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S-1];
}

BYTE MinPooling(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[0];
}
//*5주차
void MedianFiltering(BYTE* Image, BYTE* Output, int W, int H, int Win) {
	int Length = Win;  // 마스크의 한 변의 길이
	int Margin = Length / 2;//마스크 한변의 길이를 2로 나누어 마진을 만든다. mapping을 위한 것이다
	int WSize = Length * Length;//window사이즈를 한변의 길이의 제곱으로 만든다
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize);// Wsize크기의 temp를 선언한다
	int i, j, m, n;//int형인 i,j,m,n을 선언한다
	for (i = Margin; i < H - Margin; i++) {//// Y좌표 (행)
		for (j = Margin; j < W - Margin; j++) {// X좌표 (열)
			for (m = -Margin; m <= Margin; m++) {// Kernel 중심기준 세로방향 이동(행)
				for (n = -Margin; n <= Margin; n++) {// Kernel 중심기준 가로방향 이동(열)
					temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m) * W + j + n];//temp 좌표에 image좌표의 밝기값을 담아서 저장한다
				}
			}
			Output[i * W + j] = Median(temp, WSize);//temp와 WSize를 전달하여 temp에 저장된 밝기값의 중간을 output에 넣는다
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


// GlassFire 알고리즘을 이용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int height, int width)//이진영상을 받음
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;


	// 스택으로 사용할 메모리 할당
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];

	int arr_size = height * width;

	// 라벨링된 픽셀을 저장하기 위해 메모리 할당

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // 메모리 초기화

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
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
						//관심 픽셀이 영상경계를 벗어나면 처리 안함
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // 현재 라벨로 마크
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

	float grayGap = 255.0f / (float)curColor;//curColor는 총 component개수 여기서는 25개
	//얼만큼 색의 변화를 줄것인지 나타내는 graygap
	 
	// 가장 면적이 넓은 영역을 찾아내기 위함 
	for (i = 1; i <= curColor; i++)
	{
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage 배열 클리어~
	for (k = 0; k < width * height; k++) CutImage[k] = 255;

	// coloring에 저장된 라벨링 결과중 (Out_Area에 저장된) 영역이 가장 큰 것만 CutImage에 저장
	for (k = 0; k < width * height; k++)
	{
		//if (BlobArea[coloring[k]]>500) CutImage[k] = 0;//500이상이 되는 요소만 검출(특정면적이상만 출력)
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // 가장 큰 것만 저장(size fillitering) 외곽을 0으로 처리(어둡게)
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap);//만약 라벨링해서 10개 있다하면 255를 10으로 나누어 각자 25.5씩증가시켜 구분
	}
// 라벨링 후 가장 넓은 영역에 대해서만 뽑아내는 코드 포함
	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}

void BinaryImageEdgeDetection(BYTE* Bin, BYTE* Out, int W, int H) {
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Bin[i * W + j] == 0)
			{//전경화소라면
				if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 && Bin[i * W + j - 1] == 0 && Bin[i * W + j + 1] == 0))//전경화소주변이 0이 아니라면 즉,경계라면
					Out[i * W + j] = 255;//하얗게 칠하기
			}
		}
	}
}
void DrawRectOutline(BYTE* Img, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) {
	//Img: 가로/세로 라인을 그릴 이미지배열, W: 영상 가로사이즈, H: 영상 세로사이즈, 
    //LU_X: 사각형의 좌측상단 X좌표, LU_Y: 사각형의 좌측상단 Y좌표, RD_X: 사각형의 우측하단 X좌표, RD_Y: 사각형의 우측하단 Y좌표.
	for (int j = LU_X; j < RD_X; j++) {//하단가로
		Img[RD_Y * W + j] = 255;
	}
	for (int j = LU_X; j < RD_X; j++) {//상단가로
		Img[LU_Y * W + j] = 255;
	}
	for (int i = RD_Y; i <= LU_Y; i++) {//오른쪽 세로
		Img[i * W + RD_X] = 255;
	}
	for (int i = RD_Y; i <= LU_Y; i++) {//왼쪽세로
		Img[i * W + LU_X] = 255;
	}
}
void DrawCrossLine(BYTE* Img, int W, int H, int Cx, int Cy) {
// Img: 가로/세로 라인을 그릴 이미지배열, W: 영상 가로사이즈, H: 영상 세로사이즈,
// Cx: 가로/세로 라인이 교차되는 지점의 X좌표 ,Cy: 가로/세로 라인이 교차되는 지점의 Y좌표
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (i == Cy || j == Cx) {
				Img[i * W + j] = 255;
			}
		}
	}

}
void Translation(BYTE *Img,BYTE* Output,int Tx,int Ty,int H,int W) {
	Ty *= -1;//bmp파일은 뒤집혀있기에
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if ((i + Ty < H && i + Ty >= 0) && (j + Tx < W && j + Ty >= 0))
				Output[(i + Ty) * W + (j + Tx)] = Img[i * W + j];
		}
	} 
}
void VerticalFlip(BYTE* Img, int W, int H) {//위아래 뒤집음
	for (int i = 0; i < H / 2; i++) {//Y좌표
		for (int j = 0; j < W; j++) {//x좌표
			swap(&Img[i * W + j], &Img[(H - 1-i) * W + j]);
		}
	}
}
void HorizontalFlip(BYTE* Img, int W, int H) { //좌우 뒤집음
	for (int i = 0; i < W / 2; i++) {//x좌표
		for (int j = 0; j < H; j++) {//Y좌표
			swap(&Img[j * W + i], &Img[j * W + (W-1-i)]);
		}
	}
}
void Scaling(BYTE* Image, BYTE* Output, int W, int H, double SF_x, double SF_y) {
	int Tmpx, Tmpy;
	for (int i = 0; i < H; i++) {//스켈링한 위치 i,j
		for (int j = 0; j < W; j++) {
			Tmpx = (int)(j / SF_x);//Tmpx,y는 원본 위치를 뜻함 
			Tmpy = (int)(i / SF_y);//그렇기에 나눠줌
			if (Tmpy < H && Tmpx < W)
				Output[i * W + j] = Image[Tmpy * W + Tmpx];
		}
	}
}//이것은 역방향 코드임

void Rotation(BYTE* Image, BYTE* Output, int W, int H, int Angle) {
	int tmpX, tmpY;//원래 영상좌표
	double Radian = Angle * 3.141592 / 180.0;//원주율
	for (int i = 0; i < H; i++) {//변환후 영상 y
		for (int j = 0; j < W; j++) {//변환후 영상 x
			tmpX = (int)(cos(Radian) * j + sin(Radian) * i);//회전행렬의 역행렬임
			tmpY = (int)(-sin(Radian) * j + cos(Radian) * i);
			if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0))
				Output[i * W + j] = Image[tmpY * W + tmpX];//
		}
	}
}//마찬가지로 역방향사상를 이용
int main()
{
	BITMAPFILEHEADER hf; // 14바이트
	BITMAPINFOHEADER hInfo; // 40바이트
	RGBQUAD hRGB[256]; // 1024바이트
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
	BYTE* Temp = (BYTE*)malloc(ImgSize); // 임시배열
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);//영상의 원점은 좌측상단이다 하지만 bmp는 위아래가 뒤집혀있다.
	
	int Histo[256] = { 0 };
	int AHisto[256] = { 0 };
	int H = hInfo.biHeight;
	int W = hInfo.biWidth;
	int LU_X = W, LU_Y = 0, RD_X = 0, RD_Y = H;
	int sumx = 0, sumy = 0, Cx = 0, Cy = 0, cnt = 0;
	int Tx = 50, Ty = 30;
	//2주차
	/*for (int i = 0; i < ImgSize; i++)
		Output[i] = Image[i];

	fp = fopen("output1.bmp", "wb"); //원본
	
	for (int i = 0; i < ImgSize; i++)
		Output[i] = Image[i] + 50;
	fp = fopen("output2.bmp", "wb"); //밝기 50증가
	
	for (int i = 0; i < ImgSize; i++)
		Output[i] = 255 - Image[i];
	fp = fopen("output3.bmp", "wb");//반전 */
	
	//*3주차
	/*ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	ObtainAHistogram(Histo, AHisto);
	HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);*/
	/*int Thres = GozalezBinThresh(Histo);
	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);*/
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);

	//*4주차
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
	
	
	//*6주차
	/*Binarization(Image, Temp, hInfo.biWidth, hInfo.biHeight, 50);//까만색으로 눈동자,눈썹검출
	InverseImage(Temp, Temp, W, H);//눈동자 눈썹 하얗게
	m_BlobColoring(Temp, hInfo.biHeight, hInfo.biWidth);//여기서부터 특정영역1개만 어둡게 처리
	for (int i = 0; i < ImgSize; i++)
		Output[i] = Image[i];
	BinaryImageEdgeDetection(Temp, Output, W, H);//특징외곽그리기

	//사각형 좌표구하기
	for (int i = 0; i < H; i++) {//전경화소 싹다 구해서 변수를 각각 갱신시키기
		for (int j = 0; j < W; j++) {
			if (Temp[i * W + j] == 0) { //전경화소라면
				if (i >= LU_Y) LU_Y = i; //사각형의 좌측상단 y좌표, 큰놈갱신
				if (j <= LU_X) LU_X = j;//사각형의 좌측상단 x좌표 , 작은놈갱신
				if (i <= RD_Y) RD_Y = i;//사각형의 우측하단 y좌표 , 작은놈갱신
				if (j >= RD_X) RD_X = j;//사각형의 우측하단 x좌표 , 큰놈갱신
				cnt++;//픽셀개수
				sumx = sumx + j;
				sumy = sumy + i;
			}
		}
	}
	
	//무게중심
	Cx = sumx / cnt;
	Cy = sumy / cnt;
	
	printf("좌측상단 x,y: %d %d", LU_X, LU_Y);
	printf("우측하단 x,y:%d %d", RD_X, RD_Y);
	DrawRectOutline(Output, W, H, LU_X, LU_Y, RD_X, RD_Y);//외접사각형 그리기
	DrawCrossLine(Output, W, H, Cx, Cy); //무게중심구해서 십자가그리기
	*/

	//*7주차
	//영상의 원점은 좌측상단이다 하지만 bmp는 위아래가 뒤집혀있다.
	//HorizontalFlip(Image, W, H);
	//VerticalFlip(Image, W, H);
	//Translation(Image, Output, Tx, Ty, H, W);
	//Scaling(Image, Output, W, H, 0.7, 0.7);
	Rotation(Image, Output, W, H, -30);//영상의 원점을 중심으로 회전
	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "output_lenna.bmp");


	free(Image);
	free(Output);
	free(Temp);
	return 0;
}