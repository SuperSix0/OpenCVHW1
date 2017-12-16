#include <io.h>
#include <time.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;
using namespace std;
void getAllFiles(string path, vector<string>& pic, vector<string>& video)
{
	long File = 0;
	struct _finddata_t fileinfo;
	string p = path;
	if ((File = _findfirst((p + "\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (strstr(fileinfo.name, ".bmp") != NULL
				|| strstr(fileinfo.name, ".dib") != NULL
				|| strstr(fileinfo.name, ".jpeg") != NULL
				|| strstr(fileinfo.name, ".jpg") != NULL
				|| strstr(fileinfo.name, ".jpe") != NULL
				|| strstr(fileinfo.name, ".jp2") != NULL
				|| strstr(fileinfo.name, ".png") != NULL
				|| strstr(fileinfo.name, ".pbm") != NULL
				|| strstr(fileinfo.name, ".pgm") != NULL
				|| strstr(fileinfo.name, ".ppm") != NULL
				|| strstr(fileinfo.name, ".sr") != NULL
				|| strstr(fileinfo.name, ".ras") != NULL
				|| strstr(fileinfo.name, ".tiff") != NULL
				|| strstr(fileinfo.name, ".tif") != NULL)		//寻找符合格式的图片文件
				pic.push_back(p + "\\" + fileinfo.name);
			else if (strstr(fileinfo.name, ".avi") != NULL
				|| strstr(fileinfo.name, ".rmvb") != NULL
				|| strstr(fileinfo.name, ".rm") != NULL
				|| strstr(fileinfo.name, ".3pg") != NULL
				|| strstr(fileinfo.name, ".mp4") != NULL)		//寻找符合格式的视频文件
				video.push_back(p + "\\" + fileinfo.name);
		} while (_findnext(File,&fileinfo) == 0);
		_findclose(File);
	}
}
int main(int argc,char** argv)
{
	clock_t start, finish;
	vector <string> pic, video;

	start = clock();

	for (int i = 1; i < argc; i++)
		getAllFiles(*(argv + i), pic, video);					//将输入的多个路径中的图片、视频文件存入pic、video

	for (int i = 0; i < pic.size(); i++)
		cout << pic[i] << endl;
	for (int i = 0; i < video.size(); i++)
		cout << video[i] << endl;

	VideoWriter writer;
	VideoCapture capture;
	Size videoSize(640, 480);									//设置输出视频的大小为640*480
	double rate = 25;											//设置输出视频的帧速率为25帧/秒

	writer.open("output.avi", CV_FOURCC('D', 'I', 'V', 'X'), rate, videoSize);//设置输出视频的文件名、编码格式、帧速率、高宽
	
	double font_scale = 1;
	int thickness = 2, baseline;
	Size text_size = getTextSize("SuperSix", CV_FONT_HERSHEY_SCRIPT_SIMPLEX, font_scale, thickness, &baseline);//获取字幕的大小

	Mat frame,preframe,tempframe;
	Mat temp1, temp2, temp3;
	
	if (pic.size() > 0)
	{
		frame = imread(pic[0]);
		resize(frame, frame, videoSize);						//调整图片的大小
		tempframe = frame.clone();
		putText(tempframe, "SuperSix", Point(tempframe.cols / 2 - text_size.width / 2, tempframe.rows - text_size.height / 2), CV_FONT_HERSHEY_SCRIPT_SIMPLEX, font_scale, Scalar(255, 255, 255), thickness, 8, false);//在图片底部加字幕
		for (int i = 0; i < 50; i++)							//输出50帧此图片，即持续2秒钟
			writer.write(tempframe);
	}

	for (int i = 1; i < pic.size(); i++)
	{
		preframe = frame.clone();								//存储前一张图片的数据，用于生成切换效果
		frame = imread(pic[i]);									//读入下一张图片
		resize(frame, frame, videoSize);						//调整图片的大小
		switch (i % 5)
		{
			case 0:												//渐入渐出效果
				for (int j = 0; j < 100; j++)					//持续100帧即4秒
				{
					tempframe = preframe * (1 - 0.01 * j) + frame * (0.01 * j);//根据帧数调整两张图片的百分比
					putText(tempframe, "SuperSix", Point(tempframe.cols / 2 - text_size.width / 2, tempframe.rows - text_size.height / 2), CV_FONT_HERSHEY_SCRIPT_SIMPLEX, font_scale, Scalar(255, 255, 255), thickness, 8, false);//在图片底部加字幕
					writer.write(tempframe);
				}
				break;
			case 1:												//向上飞出效果
				for (int j = 0; j < frame.rows; j++)			//每帧向上移1行（像素），即一秒25行（像素）
				{
					Mat temp;
					temp.push_back(preframe.rowRange(j, preframe.rows));//将前一张图的下部分和下一张图的上部分拼接，生成一个新的矩阵
					temp.push_back(frame.rowRange(0, j));
					tempframe = temp.clone();
					putText(tempframe, "SuperSix", Point(tempframe.cols / 2 - text_size.width / 2, tempframe.rows - text_size.height / 2), CV_FONT_HERSHEY_SCRIPT_SIMPLEX, font_scale, Scalar(255, 255, 255), thickness, 8, false);//在图片底部加字幕
					writer.write(tempframe);
				}
				break;
			case 2:												//向下飞出效果
				for (int j = 0; j < frame.rows; j++)			//每帧向下移1行（像素），即一秒25行（像素）
				{
					Mat temp;
					temp.push_back(frame.rowRange(frame.rows - j, frame.rows));//将下一张图的下部分和前一张图的上部分拼接，生成一个新的矩阵
					temp.push_back(preframe.rowRange(0, preframe.rows - j));
					tempframe = temp.clone();
					putText(tempframe, "SuperSix", Point(tempframe.cols / 2 - text_size.width / 2, tempframe.rows - text_size.height / 2), CV_FONT_HERSHEY_SCRIPT_SIMPLEX, font_scale, Scalar(255, 255, 255), thickness, 8, false);//在图片底部加字幕
					writer.write(tempframe);
				}
				break;
			case 3:												//向左飞出效果
				transpose(frame, temp1);						//由于push_back是将多行压入，所以为了实现列拼接，先对矩阵转置，行拼接后再转置
				transpose(preframe, temp2);
				for (int j = 0; j < temp1.rows; j++)
				{
					Mat temp;
					temp.push_back(temp2.rowRange(j, temp2.rows));//将前一张图的右部分和下一张图的左部分拼接，生成一个新的矩阵
					temp.push_back(temp1.rowRange(0, j));
					transpose(temp, temp3);
					tempframe = temp3.clone();
					putText(tempframe, "SuperSix", Point(tempframe.cols / 2 - text_size.width / 2, tempframe.rows - text_size.height / 2), CV_FONT_HERSHEY_SCRIPT_SIMPLEX, font_scale, Scalar(255, 255, 255), thickness, 8, false);//在图片底部加字幕
					writer.write(tempframe);
				}
				break;
			case 4:												//向右飞出效果
				transpose(frame, temp1);
				transpose(preframe, temp2);
				for (int j = 0; j < temp1.rows; j++)
				{
					Mat temp;
					temp.push_back(temp1.rowRange(temp1.rows - j, temp1.rows));//将下一张图的右部分和前一张图的左部分拼接，生成一个新的矩阵
					temp.push_back(temp2.rowRange(0, temp2.rows - j));
					transpose(temp, temp3);
					tempframe = temp3.clone();
					putText(tempframe, "SuperSix", Point(tempframe.cols / 2 - text_size.width / 2, tempframe.rows - text_size.height / 2), CV_FONT_HERSHEY_SCRIPT_SIMPLEX, font_scale, Scalar(255, 255, 255), thickness, 8, false);//在图片底部加字幕
					writer.write(tempframe);
				}
				break;
		default:
			break;
		}
		
		for (int j = 0; j < 50; j++)							//图片持续50帧，即2秒
		{
			tempframe = frame.clone();
			putText(tempframe, "SuperSix", Point(tempframe.cols / 2 - text_size.width / 2, tempframe.rows - text_size.height / 2), CV_FONT_HERSHEY_SCRIPT_SIMPLEX, font_scale, Scalar(255, 255, 255), thickness, 8, false);//在图片底部加字幕
			writer.write(tempframe);
		}
			
	}

	for (int i = 0; i < video.size(); i++)
	{
		capture.open(video[i]);
		int k = 0;
		long totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);//获取视频的总帧数
		double fps = capture.get(CV_CAP_PROP_FPS), j = 0;			//获取视频的帧速率

		while (capture.read(frame) == true)							//读入一帧
		{
			resize(frame, frame, videoSize);
			putText(tempframe, "SuperSix", Point(tempframe.cols / 2 - text_size.width / 2, tempframe.rows - text_size.height / 2), CV_FONT_HERSHEY_SCRIPT_SIMPLEX, font_scale, Scalar(255, 255, 255), thickness, 8, false);//在底部加字幕
			j += rate / fps;				//根据视频的速率，计算读入的帧应该在输出的帧速率为25帧/秒的视频中对应几帧
			while (j >= 1)
			{
				writer.write(frame);
				j--;
			}
			k++;
			cout << video[i] << " : " << k << " / " << totalFrameNumber << endl;//显示当前处理的视频和正在处理第几帧以及此视频的总帧数，用来观察进度
		}
		capture.release();
	}
	writer.release();

	finish = clock();
	cout << "It takse " << (finish - start) / 1000.0 << " s." << endl;//计算读入、生成所用时间
	return 0;
}