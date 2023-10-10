#include<iostream>
#include<fstream>
#pragma warning(disable:4996)//fopen 경고 무시
using namespace std;

int main()
{
	char RawFileName[7][50] = { 
				"barbara(512x512).raw","Couple(512x512).raw",
				"gAirplane256_256.raw", "gAirplane350_555.raw", "gBaboon256_256.raw", 
				"gBarbara512_512.raw", "gLenna256_256.raw" };//raw 파일 이름들 배열

	//////////////////////이미지 변경///////////////////
	FILE* fp_img = fopen(RawFileName[0], "rb");//input image 오픈		input raw 파일 변경!!
	FILE* fp_timg = fopen(RawFileName[1], "rb");//target image 오픈		target raw 파일 변경!!
	//////////////////////////////////////////////////////

	//ifstream inimg, timg;//input image, target image file stream
	FILE* fp_oimg = fopen("Output.raw", "wb");//output image 오픈
	ofstream inputpdf, inputcdf, targetpdf, targetcdf, outputpdf, outputcdf;//각 pdf, cdf를 저정하는 txt 파일 스트림
	//각 txt 파일 오픈
	inputpdf.open("ip.txt");
	inputcdf.open("ic.txt");
	targetpdf.open("tp.txt");
	targetcdf.open("tc.txt");
	outputpdf.open("outputpdf.txt");
	outputcdf.open("outputcdf.txt");
	unsigned char *img;//input 이미지를 unsigned char type으로 저장하기 위한 변수
	unsigned char *t_img;//target 이미지를 unsigned char type으로 저장하기 위한 변수
	double p[256] = { 0 };//input 이미지의 pdf
	double c[256] = { 0 };//input 이미지의 cdf
	double tp[256] = { 0 };//target 이미지의 pdf
	double tc[256] = { 0 };//target 이미지의 cdf
	//each index means gray scale
	int idx;//to make gray scale to index
	double sum = 0, tsum = 0;//to get cdf

	//size of image file
	int width = 512;//width of input image,						raw 파일 바꿔줄때마다 값 수정!!
	int height = 512;//height of input imageraw,				raw 파일 바꿔줄때마다 값 수정!!
	int t_width = 512;//width of target image					raw 파일 바꿔줄때마다 값 수정!!
	int t_height = 512;//height of target image					raw 파일 바꿔줄때마다 값 수정!!

	//make image file to char type array
	img = new unsigned char[width * height];//dynamic allocation with image size
	fread(img, sizeof(unsigned char), width*height, fp_img);
	t_img = new unsigned char[t_width * t_height];//dynamie allocation with image size 
	fread(t_img, sizeof(unsigned char), t_width*t_height, fp_timg);


	//make input image pdf
	for (int i = 0; i < width * height; i++)
	{
		idx = img[i];//make gray scale to index
		p[idx]++;
	}
	//make target image pdf
	for (int i = 0; i < t_width*t_height; i++)
	{
		idx = t_img[i];//make gray scale to index
		tp[idx]++;
	}
	//make cdf
	for (int i = 0; i < 256; i++)
	{
		p[i] /= width * height;//n(k)/n
		tp[i] /= t_width * t_height;//n(k)/n
		//make input image cdf
		sum += p[i];
		c[i] = sum;
		//make target image cdf
		tsum += tp[i];
		tc[i] += tsum;
	}
	for (int i = 0; i < 256; i++)//각 pdf, cdf 값 txt파일에 기록
	{
		inputpdf << i << "\t" << p[i] << endl;
		inputcdf << i << "\t" << c[i] << endl;
		targetpdf << i << "\t" << tp[i] << endl;
		targetcdf << i << "\t" << tc[i] << endl;
	}
	double temp;//temp = input image의 cdf가 임시 저장될 변수
	double distance;//근사치를 구할 변수
	double min_distance=1;//근사치 비교 변수
	int min_idx;//근사한 target cdf의 인덱스
	unsigned char *o_img = new unsigned char[width*height];//output raw파일을 만들기 위한 배열

	//p(x)dx = p(y)dy, 프로그래밍에서는 연속적인 값이 아닌 이산적인 값이 존재하므로 근사치를 구하여 매칭
	for (int i = 0; i < width*height; i++)
	{
		temp = c[img[i]];//p(x)dy, temp = T(r), img[i] = r, x
		for (int j = 0; j < 256; j++)
		{
			distance = fabs(temp - tc[j]);
			if (distance < min_distance)//가장 근사한 값의 p(y)dy 값과 그떄의 y의 값 구함, G-1[T(r)]
			{
				min_distance = distance;//가장 근사한 거리
				min_idx = j;//가장 근사한 거리에 있는 cdf의 인덱스
			}
		}
		o_img[i] = min_idx;//구한 y값 = z = G-1(T(r))
		min_distance = 1;
	}
	double op[256] = { 0 };//output image pdf
	double oc[256] = { 0 };
	double osum = 0;//cdf를 구하기 위한 변수
	for (int i = 0; i < width*height; i++)//output image의 pdf 구하기
	{
		idx = o_img[i];
		op[idx]++;
	}
	for (int i = 0; i < 256; i++)//txt파일에 output pdf 자료 기록
	{
		op[i] /= width * height;
		outputpdf << i << "\t" << op[i] << endl;
	}
	for (int i = 0; i < 256; i++)//output cdf 만들기
	{
		osum += op[i];
		oc[i] = osum;
		outputcdf << i << "\t" << oc[i] << endl;
	}
	fwrite(o_img, sizeof(unsigned char), width*height, fp_oimg);//output.raw 파일 작성
	//열린 파일 닫기
	fclose(fp_img);
	fclose(fp_timg);
	fclose(fp_oimg);
	//동적할당한 배열들 해제
	delete[] o_img;
	delete[] img;
	delete[] t_img;
}