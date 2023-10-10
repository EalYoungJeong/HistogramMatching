#include<iostream>
#include<fstream>
#pragma warning(disable:4996)//fopen ��� ����
using namespace std;

int main()
{
	char RawFileName[7][50] = { 
				"barbara(512x512).raw","Couple(512x512).raw",
				"gAirplane256_256.raw", "gAirplane350_555.raw", "gBaboon256_256.raw", 
				"gBarbara512_512.raw", "gLenna256_256.raw" };//raw ���� �̸��� �迭

	//////////////////////�̹��� ����///////////////////
	FILE* fp_img = fopen(RawFileName[0], "rb");//input image ����		input raw ���� ����!!
	FILE* fp_timg = fopen(RawFileName[1], "rb");//target image ����		target raw ���� ����!!
	//////////////////////////////////////////////////////

	//ifstream inimg, timg;//input image, target image file stream
	FILE* fp_oimg = fopen("Output.raw", "wb");//output image ����
	ofstream inputpdf, inputcdf, targetpdf, targetcdf, outputpdf, outputcdf;//�� pdf, cdf�� �����ϴ� txt ���� ��Ʈ��
	//�� txt ���� ����
	inputpdf.open("ip.txt");
	inputcdf.open("ic.txt");
	targetpdf.open("tp.txt");
	targetcdf.open("tc.txt");
	outputpdf.open("outputpdf.txt");
	outputcdf.open("outputcdf.txt");
	unsigned char *img;//input �̹����� unsigned char type���� �����ϱ� ���� ����
	unsigned char *t_img;//target �̹����� unsigned char type���� �����ϱ� ���� ����
	double p[256] = { 0 };//input �̹����� pdf
	double c[256] = { 0 };//input �̹����� cdf
	double tp[256] = { 0 };//target �̹����� pdf
	double tc[256] = { 0 };//target �̹����� cdf
	//each index means gray scale
	int idx;//to make gray scale to index
	double sum = 0, tsum = 0;//to get cdf

	//size of image file
	int width = 512;//width of input image,						raw ���� �ٲ��ٶ����� �� ����!!
	int height = 512;//height of input imageraw,				raw ���� �ٲ��ٶ����� �� ����!!
	int t_width = 512;//width of target image					raw ���� �ٲ��ٶ����� �� ����!!
	int t_height = 512;//height of target image					raw ���� �ٲ��ٶ����� �� ����!!

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
	for (int i = 0; i < 256; i++)//�� pdf, cdf �� txt���Ͽ� ���
	{
		inputpdf << i << "\t" << p[i] << endl;
		inputcdf << i << "\t" << c[i] << endl;
		targetpdf << i << "\t" << tp[i] << endl;
		targetcdf << i << "\t" << tc[i] << endl;
	}
	double temp;//temp = input image�� cdf�� �ӽ� ����� ����
	double distance;//�ٻ�ġ�� ���� ����
	double min_distance=1;//�ٻ�ġ �� ����
	int min_idx;//�ٻ��� target cdf�� �ε���
	unsigned char *o_img = new unsigned char[width*height];//output raw������ ����� ���� �迭

	//p(x)dx = p(y)dy, ���α׷��ֿ����� �������� ���� �ƴ� �̻����� ���� �����ϹǷ� �ٻ�ġ�� ���Ͽ� ��Ī
	for (int i = 0; i < width*height; i++)
	{
		temp = c[img[i]];//p(x)dy, temp = T(r), img[i] = r, x
		for (int j = 0; j < 256; j++)
		{
			distance = fabs(temp - tc[j]);
			if (distance < min_distance)//���� �ٻ��� ���� p(y)dy ���� �׋��� y�� �� ����, G-1[T(r)]
			{
				min_distance = distance;//���� �ٻ��� �Ÿ�
				min_idx = j;//���� �ٻ��� �Ÿ��� �ִ� cdf�� �ε���
			}
		}
		o_img[i] = min_idx;//���� y�� = z = G-1(T(r))
		min_distance = 1;
	}
	double op[256] = { 0 };//output image pdf
	double oc[256] = { 0 };
	double osum = 0;//cdf�� ���ϱ� ���� ����
	for (int i = 0; i < width*height; i++)//output image�� pdf ���ϱ�
	{
		idx = o_img[i];
		op[idx]++;
	}
	for (int i = 0; i < 256; i++)//txt���Ͽ� output pdf �ڷ� ���
	{
		op[i] /= width * height;
		outputpdf << i << "\t" << op[i] << endl;
	}
	for (int i = 0; i < 256; i++)//output cdf �����
	{
		osum += op[i];
		oc[i] = osum;
		outputcdf << i << "\t" << oc[i] << endl;
	}
	fwrite(o_img, sizeof(unsigned char), width*height, fp_oimg);//output.raw ���� �ۼ�
	//���� ���� �ݱ�
	fclose(fp_img);
	fclose(fp_timg);
	fclose(fp_oimg);
	//�����Ҵ��� �迭�� ����
	delete[] o_img;
	delete[] img;
	delete[] t_img;
}