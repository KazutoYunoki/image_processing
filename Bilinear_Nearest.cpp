#include<iostream>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<math.h>
#define PI 3.141592

using namespace std;
using namespace cv;

void image_view_create(Mat_<unsigned char> image, string window_name); //画像を描画するための関数
Mat_<unsigned char> nealestNeighbor_zoomOut(Mat_<unsigned char> image, double magnification); //最近傍法で拡大・縮小する関数
Mat_<unsigned char> nealestNeighbor_rotate(Mat_<unsigned char> image, double angle);//最近傍法で回転させる関数
Mat_<unsigned char> nealestNeighbor(Mat_<unsigned char> image, double magnification = 0, double angle = 0);//最近傍法で拡大・縮小・回転をまとめた関数
Mat_<unsigned char> bilinearInterpolation_zoomOut(Mat_<unsigned char> image, double magnification);//バイリニア補完法で拡大・縮小する関数
Mat_<unsigned char> bilinearInterpolation_rotate(Mat_<unsigned char> image, double angle);//バイリニア補完法で回転させる関数
Mat_<unsigned char> bilinearInterpolation(Mat_<unsigned char> image, double magnification = 0, double angle = 0);//バイリニア補完法で拡大・縮小・回転をまとめた関数

int main()
{
	//input image
	Mat_<unsigned char> wings_piano = imread("D:\\Users\\kazuto yunoki\\Desktop\\image_engineering\\image\\Wings_Of_Piano2.jpg", 0);
	Mat_<unsigned char> egoist_image = imread("D:\\Users\\kazuto yunoki\\Desktop\\image_engineering\\image\\egoist_255.jpg", 0);
	Mat_<unsigned char> hibiya = imread("D:\\Users\\kazuto yunoki\\Desktop\\image_engineering\\image\\hibiya.jpg", 0);
	Mat_<unsigned char> checker = imread("D:\\Users\\kazuto yunoki\\Desktop\\image_engineering\\image\\checker.jpg", 0);
	
	Mat_ < unsigned char> square_4 = imread("D:\\Users\\kazuto yunoki\\Desktop\\image_engineering\\image\\4_square.jpg", 0);
	
	image_view_create(hibiya, "original");

	//nealestNeighbor(引数：画像、倍率、角度)
	//Mat_<unsigned char> result_image = nealestNeighbor(hibiya, 2, 30);

	//result_image = nealestNeighbor(, 1, -30);

	//bilinearInterpolation(引数：画像、倍率、角度)
	Mat_<unsigned char> result_image = bilinearInterpolation(hibiya, 2, 30);

	//result_image = bilinearInterpolation(result_image, 1, -30);
	//display image

	//image_view_create(egoist_image,"original");
 	image_view_create(result_image,"zoom2_rotate30");

	return 0;
}
void image_view_create(Mat_<unsigned char> image, string window_name)
{
	namedWindow(window_name, 1);
	imshow(window_name, image);
	waitKey(0);
	//imwrite(image_name, image);
	destroyWindow(window_name);
}
Mat_<unsigned char> nealestNeighbor_rotate(Mat_<unsigned char> image, double angle){

	Mat_<unsigned char> result_image = Mat::zeros(image.rows, image.cols, CV_8UC3);

	double rotate_x, rotate_y; 

	//radの計算
	double rad = angle*PI / 180;

	//画像の幅、高さの中心
	int image_width_half = image.rows / 2;
	int image_height_half = image.cols / 2;

	for (int y = 0; y < image.cols; y++){
		for (int x = 0; x < image.rows; x++){

			//中心(画像の真ん中を中心とした回転)
			rotate_x = (x - image_width_half)*cos(rad) - (y - image_height_half)*sin(rad) + image_width_half;
			rotate_y = (x - image_width_half)*sin(rad) + (y - image_height_half)*cos(rad) + image_height_half;
			
			/*原点(左上を中心とした回転)
			rotate_x = x*cos(rad) - y*sin(rad);
			rotate_y = x*sin(rad) + y*cos(rad);
			*/
			//最近傍取得
			rotate_x = round(rotate_x);
			rotate_y = round(rotate_y);
			
			//画像のサイズを超えた場合の処理(黒)
			if (rotate_x > image.rows - 1 || rotate_y > image.cols - 1){
				result_image(x, y) = 0;
				continue;
			}
			
			if (rotate_x < 0 || rotate_y < 0){
				result_image(x, y) = 0;
				continue;
			}
			result_image(x, y) = image(rotate_x, rotate_y);
		}
	}
	return result_image;
}
Mat_<unsigned char> nealestNeighbor_zoomOut(Mat_<unsigned char> image, double magnification){

	Mat_<unsigned char> result_image = Mat::zeros(round(image.rows * magnification),round(image.cols * magnification), CV_8UC3);
	int tempx, tempy;

	for (int y = 0; y < image.cols*magnification; y++){
		for (int x = 0; x < image.rows * magnification; x++){

			//最近傍取得
			tempx = round(x / magnification);
			tempy = round(y / magnification);
			
			//元画像のサイズを超える場合の処理
			if (tempx > image.rows-1){
				tempx = image.rows-1;
			}
			if (tempy > image.cols-1){
				tempy = image.cols-1;
			}
			
			result_image(x, y) = image(tempx, tempy); 
		}
		cout << endl;
	}
	return result_image;
}
Mat_<unsigned char> nealestNeighbor(Mat_<unsigned char> image, double magnification, double angle){

	Mat_<unsigned char> result_image = nealestNeighbor_zoomOut(image, magnification);

	result_image = nealestNeighbor_rotate(result_image, angle);

	return result_image;
}
Mat_<unsigned char> bilinearInterpolation_zoomOut(Mat_<unsigned char> image,double magnification){

	Mat_<unsigned char> result_image = Mat::zeros(round(image.rows * magnification), round(image.cols * magnification), CV_8UC3);

	double tempx, tempy, dist_x, dist_y;
	int floor_x, floor_y;

	for (double y = 0; y < image.cols*magnification; y++){
		for (double x = 0; x < image.rows * magnification; x++){

			//元画像の座標(小数点を含む)
			tempx = (x / magnification);
			tempy = (y / magnification);


			//元画像の座標(切り捨て・・・元画像の左上)
			floor_x = floor(tempx);
			floor_y = floor(tempy);
			
			//サイズ外の処理
			if (floor_x > image.rows - 1){
				floor_x = image.rows - 1;
				tempx = image.rows - 1;
			}
			else if (floor_x + 1 > image.rows - 1){
				floor_x = image.rows - 2;
				tempx = image.rows - 2;
			}

			if (floor_y > image.cols - 1){
				floor_y = image.cols - 1;
				tempy = image.cols - 1;
			}
			else if(floor_y + 1 > image.cols - 1){
				floor_y = image.cols - 2;
				tempy = image.cols - 2;

			}

			dist_x = abs(tempx - (double)floor_x);
			dist_y = abs(tempy - (double)floor_y);

			//cout << dist_x << ", " << dist_y << endl;
	
			result_image(x, y) = (1.0 - dist_x)*(1.0 - dist_y)*image(floor_x, floor_y)
						       + dist_x*(1.0 - dist_y)*image(floor_x + 1, floor_y)
				               + (1.0 - dist_x)*dist_y*image(floor_x, floor_y + 1)
				               + dist_x*dist_y*image(floor_x + 1, floor_y + 1);
		}
	}
	//cout << result_image;
	return result_image;
}
Mat_<unsigned char> bilinearInterpolation_rotate(Mat_<unsigned char> image, double angle)
{
	Mat_<unsigned char> result_image = Mat::zeros(image.rows, image.cols, CV_8UC3);

	double rotate_x, rotate_y, dist_x, dist_y;;
	int floor_x, floor_y;

	double rad = angle*PI / 180;

	int image_width_half = image.rows / 2;
	int image_height_half = image.cols / 2;

	for (int y = 0; y < image.cols; y++){
		for (int x = 0; x < image.rows; x++){

			//中心(画像の真ん中を中心とした回転)
			rotate_x = (x - image_width_half)*cos(rad) - (y - image_height_half)*sin(rad) + image_width_half;
			rotate_y = (x - image_width_half)*sin(rad) + (y - image_height_half)*cos(rad) + image_height_half;

			/*原点(左上を中心とした回転)
			rotate_x = x*cos(rad) - y*sin(rad);
			rotate_y = x*sin(rad) + y*cos(rad);
			*/
			//最近傍取得
			floor_x = floor(rotate_x);
			floor_y = floor(rotate_y);

			//画像のサイズを超えた場合の処理(黒)
			if (rotate_x > image.rows - 1 || rotate_y > image.cols - 1){
				result_image(x, y) = 0;
				continue;
			}

			if (rotate_x < 0 || rotate_y < 0){
				result_image(x, y) = 0;
				continue;
			}
			
			dist_x = abs(rotate_x - (double)floor_x);
			dist_y = abs(rotate_y - (double)floor_y);

			result_image(x, y) = (1.0 - dist_x)*(1.0 - dist_y)*image(floor_x, floor_y)
				+ dist_x*(1.0 - dist_y)*image(floor_x +1, floor_y)
				+ (1.0 - dist_x)*dist_y*image(floor_x, floor_y + 1)
				+ dist_x*dist_y*image(floor_x + 1, floor_y + 1);

		}
	}
	return result_image;
}
Mat_<unsigned char> bilinearInterpolation(Mat_<unsigned char> image, double magnification, double angle)
{
	Mat_<unsigned char> result_image = bilinearInterpolation_zoomOut(image, magnification);

	result_image = bilinearInterpolation_rotate(result_image, angle);

	return result_image;
}