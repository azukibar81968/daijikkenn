/***************************************************/
/* myfunc.cpp 雛形 Ver. 1.0                        */
/* 説明: 実験時に学生が編集するソースファイル      */
/* 作成日: 20140912  作成者: 舟洞  編集者:         */
/***************************************************/
/***************************************************
ヘッダファイル
***************************************************/
#include "stdafx.h"
#include <string>
#include <algorithm>
#include <map>
#include "img.h"
#include <Windows.h>
/**************************************************/

/***************************************************
#Define
***************************************************/
#define IMG_XSIZE 640
#define IMG_YSIZE 480
#define LABEL_THD 220
#define MAX_OBJ 512
/**************************************************/



/***************************************************
変数定義
***************************************************/
extern BYTE *img_disp;
extern BYTE *img_src;
extern BYTE *img_work;
extern BYTE *img_work2;
extern BYTE *img_work3;
/**************************************************/

struct coin_list{//コインの個数リストの構造体
	cv::Point coin_point;
	double hit_1 = 0;
	double hit_5 = 0;
	double hit_10 = 0;
	double hit_50 = 0;
	double hit_100 = 0;
	double hit_500 = 0;
	int value = 0;
};


/***************************************************
プロトタイプ宣言: ユーザ定義関数の宣言を記述
***************************************************/
/* サンプルプログラム（キー入力があるまで画像表示） */
void disp_image_while_kbhit(BYTE *img);
/* 以下に自分で作成した関数の宣言を記述*/

/*ロボット制御部分*/
void m_home();//ホームポジションに移動する関数
void m_grip();//ハンドを閉じる関数
void m_ungrip();//ハンドを開く関数
void m_move_position(double x, double y, double z, double a, double b);//指定した姿勢に移動する関数
void m_move_position_2(double x, double y, double z, double a, double b);//指定した姿勢に移動する関数
void m_position_define(int n, double x, double y, double z, double a, double b, int g);//指定した姿勢をポジションリストに登録する関数
void m_position_define_2(int n, double x, double y, double z, double a, double b, int g);//指定した姿勢をポジションリストに登録する関数
void m_ms(int n);//指定したポジション番号へ直線移動
int is_grip();//ハンドの状態を返す関数
void m_move_straight(double x, double y, double z, double a, double b);//指定した座標に直線移動する関数
void m_move_circle2(double cx, double cy, double z, double r);//中心(cx,cy,z) 半径r の円を描くように移動する
void shake();//姿勢a,bをガチャガチャ変える

void check_where_coin();//お釣り用の硬貨の座標チェック
/*画像処理部分*/
void get_cog(BYTE *img, int label, double *gx, double *dy);//重心を取得する関数
void get_size(BYTE *img, int label, double *size_x, double *size_y);//サイズを取得する関数
void get_direction(BYTE *img, int label, double gx, double gy, double* theta);//方向を取得する関数

int get_block_list(BYTE *img, int thd, double param[][3], char* mode);//imgからブロックのデータを取得する関数

void to_color(BYTE *img, BYTE* img_result, int thd_B, int thd_G, int thd_R);//画像を色でラベリングする関数
void get_coin(BYTE img, int coin[6], std::string mode);//コインを判別して配列を返す関数
int get_size(BYTE *img, int label);//与えられたラベル番号の面積を返す関数
double get_sat(BYTE *img, BYTE *img_hsv, int label);//与えられたラベル番号の彩度を返す関数
double get_val(BYTE *img, BYTE *img_hsv, int label);//与えられたラベル番号の明度を返す関数
double get_hue(BYTE *img, BYTE *img_hsv, int label);//与えられたラベル番号の色彩を返す関数
BYTE get_sep_area(BYTE* img);//境界で切り出した画像を返す関数(未完了)
void warpPers(BYTE *img);
void compress_img(BYTE *img, BYTE *img_dst);//グレー画像の白い部分強調処理
void filtering(cv::Mat img, cv::Mat img_sample, cv::Mat img_dst);//フィルタリングで画像検出

int get_coin_enhanced(cv::Mat img, coin_list result_list[MAX_OBJ], cv::Point offset, int template_num);//貨幣のリストを作る
int match_coin(cv::Mat img, cv::Mat img_temp, cv::Point* result_list, double* score, double thd);//テンプレートマッチングをかける
cv::Point cut_image(cv::Mat img, cv::Mat* img_dst);//イメージを切り出す
int how_match(cv::Mat img);//移っている貨幣の総額を返す 最終的にこれを呼べばOK

void get_side_of_tray(cv::Mat image, int label, double *x, double *y);
double get_direction_2(BYTE *img, int label, double* theta);//方向を取得する関数,傾きをついでにreturn
double get_direction_from_corner(cv::Point2f dst_pt[]);
double rad2deg(double rad);//弧度法を入力すると弧度法で返す
void tray_where_to_grip(cv::Mat *image, int label, double *x, double *y);//トレイで掴みやすそうなとこを探す
void get_image_2(cv::Mat *image);//透視変換済みの画像を撮影
void rot_90(cv::Mat &img);//90ド回転
int find_marker_and_get_price_prototype();//ARマーカ風のオブジェクトで値段計算
int find_marker_and_get_price();//ARマーカ風のオブジェクトで値段計算
int find_marker_and_get_price_and_throw();//ARマーカ風のオブジェクトで値段計算,でもって商品をバケツに突っ込む
double average_of_Mat(cv::Mat *img);//白黒画像の平均を取る
int ar_read(cv::Mat *img);//ARの切り抜き画像を読んで数値化
bool is_marker(vector<vector<double>> grid);//マーカーか判断
cv::Mat labeL_num_extraction(cv::Mat *labeled, int label_num);//指定ラベル番号のみのこった二値化画像を抽出
int ar_to_price(vector<vector<double>> grid_double);//ar_read()の内部で数値化に使う
/*連携部分*/
void exchange_ctor(double cx, double cy, double* rx, double* ry);//画像座標系からロボット座標系に変換
void do_zeroin();//実行すると座標の調整を行う
void eject_block();//実行するとブロックの取り除き動作を行う
void grip_and_shake_tray();//トレイを掴み，ゆらして置く
void grip_tray_and_getcoin();//受け取ったお金を回収
void return_change(map<long long int, long long int> coin);//お釣りをトレイに
/**************************************************/
std::map<long long int, long long int> accounting(long long int received, long long int total_price);//返すべきお釣りの内訳をしらべる

//レジとかのがめんのやつ～
class Cashregister_display{
public:
	Cashregister_display(){
		//std::cout << "unko" << std::endl;
		display_image = cv::Mat::zeros(500, 800, CV_8UC3);

		received = -1;
		total_price = -1;
		change = -1;
		update();
	}
	void add_total_price(long long int price){
		display_image = cv::Mat::zeros(500, 800, CV_8UC3);
		if (total_price < 0&&price>0){
			total_price = 0;
		}

		total_price += price;
		update();
	}
	void set_received(int re){
		received = re;
		update();
	}
	void set_change(int ch){
		change = ch;
		update();
	}
private:
	void update(){
		display_image = cv::Mat::zeros(500, 800, CV_8UC3);
		std::string tmp;
		if (total_price >= 0){
			tmp = "";
			for (int i = 5; i > log10(total_price); i--){
				tmp.push_back(' ');
			}
			putText(display_image, "total price:"+tmp+ std::to_string(total_price), cv::Point(20, 80), 0, 2, cv::Scalar(0, 255, 99), 6, CV_AA);
		}
		if (received >= 0){
			tmp = "";
			for (int i = 5; i > log10(received); i--){
				tmp.push_back(' ');
			}
			putText(display_image, "received  :"+tmp + std::to_string(received), cv::Point(20, 160), 0, 2, cv::Scalar(0, 255, 99), 6, CV_AA);
		}
		if (change >= 0){
			tmp = "";
			for (int i = 5; i > log10(change); i--){
				tmp.push_back(' ');
			}
			putText(display_image, "change   :"+tmp  + std::to_string(change), cv::Point(20, 240), 0 ,2,cv::Scalar(0, 255, 99), 6, CV_AA);
		}
		if (total_price < 0 && received < 0 && change < 0){
			putText(display_image, "Welcome !!", cv::Point(220,240 ), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 99), 6, CV_AA);
			putText(display_image, "Please put Product here", cv::Point(100, 300), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 255, 99), 6, CV_AA);
		}
		cv::waitKey(2);
		cv::imshow("H10店へようこそ", display_image);
		cv::waitKey(2);
	}
	cv::Mat display_image;
	long long int received, total_price, change;

};



//ポインタをｸﾞﾛｰｰｰﾊﾞﾙ宣言，他の関数内から呼び出すため インスタンスはmainfuncのはじめで作成
Cashregister_display *cash_disp;

/***************************************************
mainfunc: メイン処理（実際の処理を記述）
***************************************************/
void mainfunc(HDC *hDC) {
	//カメラの解像度設定，必須
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 960);
	//check_where_coin();
	//レジ画面の実体化
	cash_disp = new Cashregister_display();

	//ユーザーが品物を置いてエンターキーを押すのを待つ
	cv::waitKey();

	//価格の認識 , 価格の表示
	int kakaku = find_marker_and_get_price_and_throw();
	
	std::cout << kakaku << std::endl;
	cv::waitKey();
	grip_and_shake_tray();
	disp_image(img_work, "硬貨");
	
	//ユーザーがトレーに代金を置いてエンターキーを押すのを待つ
	cv::waitKey();

	//貨幣総額の認識

	cv::Mat img_payment;
	bool loop_flg = FALSE;
	do{
		loop_flg = FALSE;
		get_image_2(&img_payment);
		get_image_2(&img_payment);
		get_image_2(&img_payment);
		if (img_payment.data == NULL){
			printf("判定する画像の取得に失敗しました\n");
			loop_flg = TRUE;
		}
	} while (loop_flg);

	int payment =  how_match(*img_work);
	cash_disp->set_received(payment);
	cash_disp->set_change(payment - kakaku);
	grip_tray_and_getcoin();
	
	//品物を出す
	//なんかいい感じの関数

	//お釣りを出す
	std::map<long long int, long long int> money = accounting(payment, kakaku);
	return_change(money);//実動作

	/*終了しないようにする関数*/
	std::string hoge;
	do
	{
		hoge = "";
		std::cin >> hoge;
	} while (hoge != "f");

}
/**************************************************/



/***************************************************
以下自分で定義した関数（ユーザ定義関数）の処理を記述
***************************************************/
/* サンプルプログラム（キー入力があるまで画像表示） */
void disp_image_while_kbhit(BYTE *img) {
	through_on();		// スルーモードをオン
	cv::waitKey();		// キー入力があるまで待機
	capture(img);		// キャプチャ画像をimgに保存
	through_off();		// スルーモードをオフ
	return;				// 終了
}
/* 以下に自分で作成した関数の宣言を記述*/

/*--------------------------------------------------
処理:	画像から移っている貨幣の総額を返す関数
戻り値: 見つけた貨幣の数
引数:   入力画像<img>

備考:H10/temp/ にテンプレート画像を入れておく必要がある
--------------------------------------------------*/
int how_match(cv::Mat img){
	cv::Mat img_disp;
	img.copyTo(img_disp);
	cv::Mat img_cut;
	cv::Point pos_cut;
	pos_cut = cut_image(img, &img_cut);
	if (pos_cut == cv::Point(0, 0)) img_cut = img;
	coin_list result_list[MAX_OBJ];
	//disp_image(&img, "");
	//wait_while_kbhit();
	int use_template_num = 10;
	int total_value = 0;

	int coin_maisu = get_coin_enhanced(img_cut, result_list, pos_cut, use_template_num);

	for (int i = 0; i < coin_maisu; i++){

		/*コイン判定 孔を検出 検出点を重心とする5*5の矩形を参照する*/
		cv::Mat rect_img;
		cv::Point top(result_list[i].coin_point.x - 7, result_list[i].coin_point.y - 7);
		cv::Rect cut_rect(top, cv::Size(15, 15));
		rect_img = img(cut_rect);
		cv::cvtColor(rect_img, rect_img, CV_BGR2HSV);
		int max_sat = -1;
		int min_sat = 256;
		int sum_val = 0;

		for (int x = 0; x < rect_img.cols; x++){
			for (int y = 0; y < rect_img.rows; y++){
				if (max_sat < rgb((&rect_img), y, x, 1)) max_sat = rgb((&rect_img), y, x, 1);//取り急ぎbgr関数でアクセス
				if (min_sat > rgb((&rect_img), y, x, 1)) min_sat = rgb((&rect_img), y, x, 1);//取り急ぎbgr関数でアクセス
				sum_val += rgb((&rect_img), y, x, 2);//取り急ぎbgr関数でアクセス
			}
		}
		sum_val /= 255;
		bool hole_flg = FALSE;
		if ((max_sat - min_sat) > 75) hole_flg = TRUE;

		/*貨幣分類開始*/
		/*有孔貨幣*/
		if (hole_flg == TRUE){
			if (result_list[i].hit_5 + result_list[i].hit_10 > result_list[i].hit_50 + result_list[i].hit_100){//5円玉
				result_list[i].value = 5;
			}
			else{//50円玉
				result_list[i].value = 50;
			}
		}
		else{/*孔のない貨幣  どうしようね*/

			if (result_list[i].hit_500 > use_template_num / 3 * 2){
				result_list[i].value = 500;
			}
			else if (result_list[i].hit_5 + result_list[i].hit_10 > result_list[i].hit_50 + result_list[i].hit_100 && max_sat > 50){
				result_list[i].value = 10;
			}

			else{//100円玉，1円玉区別
				cv::Mat rect_img_100en;
				cv::Point top(result_list[i].coin_point.x - 15, result_list[i].coin_point.y - 15);
				cv::Rect cut_rect(top, cv::Size(30, 30));
				rect_img_100en = img(cut_rect);
				cv::cvtColor(rect_img_100en, rect_img_100en, CV_BGR2HSV);
				int max_sat = -1;
				int min_sat = 256;
				int min_val = 999;

				for (int x = 0; x < rect_img_100en.cols; x++){
					for (int y = 0; y < rect_img_100en.rows; y++){
						if (min_val > rgb((&rect_img_100en), y, x, 2)) min_val = rgb((&rect_img_100en), y, x, 2);//取り急ぎbgr関数でアクセス
					}
				}


				if (result_list[i].hit_1 > result_list[i].hit_100){//分類できねええええええ
					result_list[i].value = 1;
				}
				else{
					result_list[i].value = 100;
				}
			}

		}
		int x = result_list[i].coin_point.x;
		int y = result_list[i].coin_point.y;
		printf("(%d,%d): num = %d\n", x, y, i + 1);
		printf("\t1円玉:\t\t%1.0lf\n", result_list[i].hit_1);
		printf("\t5円玉:\t\t%1.0lf\n", result_list[i].hit_5);
		printf("\t10円玉:\t\t%1.0lf\n", result_list[i].hit_10);
		printf("\t50円玉:\t\t%1.0lf\n", result_list[i].hit_50);
		printf("\t100円玉:\t%1.0lf\n", result_list[i].hit_100);
		printf("\t500円玉:\t%1.0lf\n", result_list[i].hit_500);
		printf("\tvalue = %d\n", result_list[i].value);
		printf("sat_range = %d\n", max_sat - min_sat);
		printf("sat = %d\n", max_sat);
		printf("ava_value = %d\n", sum_val);
		printf("\n");

		cv::putText(img_disp, std::to_string(i)+ "-" + std::to_string(result_list[i].value), result_list[i].coin_point - cv::Point(35, 0), 2, 1, cv::Scalar(0, 255, 0));
		//if (i < 8){
		//	cv::line(img_disp, result_list[i].coin_point, cv::Point(200, 100 + 1100 * i / coin_maisu), cv::Scalar(int(255 * i / coin_maisu), 255 - int(255 * i / coin_maisu), 0), 1);
		//	cv::putText(img_disp, std::to_string(result_list[i].value), cv::Point(120, 100 + 1100 * i / coin_maisu), 2, 2, cv::Scalar(0, 255, 0));
		//}
		//else{
		//	cv::line(img_disp, result_list[i].coin_point, cv::Point(1100, 100 + 1100 * (i - 7) / coin_maisu), cv::Scalar(int(255 * i / coin_maisu), 255 - int(255 * i / coin_maisu), 0), 1);
		//	cv::putText(img_disp, std::to_string(result_list[i].value), cv::Point(1150, 100 + 1100 * (i - 7) / coin_maisu), 2, 2, cv::Scalar(0, 255, 0));
		//}

		total_value += result_list[i].value;

	}

	return total_value;
}
/*--------------------------------------------------
処理:	画像から貨幣のパターンマッチングを返す関数
戻り値: 見つけた貨幣の数
引数:   入力画像<img>, 個数リスト格納用変数<int*>
--------------------------------------------------*/
int get_coin_enhanced(cv::Mat img, coin_list result_list[MAX_OBJ],cv::Point offset,int template_num){
	const int coin_size = 15;//コインのサイズ，同様のコインとみなすノルムの閾値
	const double thd = 0.5;//明らかな非コインの閾値
	const std::string coin_list[6] = { "1", "5", "10", "50", "100", "500" };//fileopenに使用
	const int use_temp_num = template_num;//テンプレートの枚数
	int progress = 0;
	int progress_border = 0;

	double score_buf[MAX_OBJ];//貨幣の適合度の仮置き変数
	int count = 0;//現在見つけた貨幣の数

	for (int coin_num = 0; coin_num < 6; coin_num++){//coin_num円玉について
		std::string temp_path_1 = "temp/" + coin_list[coin_num] + "/";
		std::string temp_path_2(".bmp");

		for (int n = 1; n <= use_temp_num; n++){//n番目のテンプレートをつかって
			int temp_num = 0;
			std::string temp_path = temp_path_1 + std::to_string(n) + temp_path_2;//テンプレートへのパスを作成

			cv::Mat img_temp = cv::imread(temp_path);//テンプレートを作成
			if (img_temp.data == NULL) continue;
			cv::Point pos_buf[MAX_OBJ];
			temp_num = match_coin(img, img_temp, pos_buf, score_buf, 0.57);//テンプレートにマッチしたコインがある座標取得

			/*result_Listへデータの格納*/
			for (int i = 0; i < temp_num; i++){//tempで見つけた各座標が
				/*物体として区別*/
				bool check_flg = false;
				int result_list_address = 0;
				for (int j = 0; j < count; j++){//現在のcoinlistに存在するか
					if (cv::norm(pos_buf[i] + offset - result_list[j].coin_point) < coin_size){//coinbufとcoinlistの座標比較，近接していれば
						check_flg = true;
						result_list_address = j;
					}
				}
				if (check_flg == false){//未発見のコインであれば
					result_list_address = count;
					result_list[result_list_address].coin_point = pos_buf[i] + offset;
					count++;
				}
				/*result_list書き換え*/
				switch (coin_num){
				case 0:
					result_list[result_list_address].hit_1 += 1;
					break;
				case 1:
					result_list[result_list_address].hit_5 += 1;
					break;
				case 2:
					result_list[result_list_address].hit_10 += 1;
					break;
				case 3:
					result_list[result_list_address].hit_50 += 1;
					break;
				case 4:
					result_list[result_list_address].hit_100 += 1;
					break;
				case 5:
					result_list[result_list_address].hit_500 += 1;
					break;
				}


			}
			progress += 1;
			if (progress_border < float(progress * 100 / (6 * use_temp_num))){
				printf("受け取り額を計算していますよ   %1.0f ％ ...", float(progress * 100 / (6 * use_temp_num)));
				//printf("   using: %d-%d  hit:%d",coin_num, n, temp_num);
				printf("\n");
				progress_border += 10;
			}
		}
	}
	//printf("探索が終了しました\n");

	return count;
}

/*--------------------------------------------------
処理:	元画像とテンプレートから貨幣の位置のリスト返す関数
戻り値: そのテンプレートで見つけたコインの数
引数:   入力画像<img>,テンプレート画像<img_temp>, 貨幣の位置リスト<result_list>, 適合度<score> スレッショルド<thd>
--------------------------------------------------*/
int match_coin(cv::Mat img_source, cv::Mat img_temp, cv::Point* result_list, double* score, double thd){
	cv::Mat result;
	cv::Mat img;
	img_source.copyTo(img);
	int count = 0;
	//disp_image(&img_source, "");
	for (int i = 0; i < 30; i++){//i番目のオブジェクト探索
		// テンプレートマッチング
		cv::matchTemplate(img, img_temp, result, CV_TM_CCOEFF_NORMED);

		// 最大のスコアの場所を探す
		cv::Point max_pos;
		max_pos.x = 0;
		max_pos.y = 0;
		double max_val = 0;
		cv::minMaxLoc(result, NULL, &max_val, NULL, &max_pos);

		// 一定スコア以下の場合は処理終了
		if (max_val < thd) break;

		//元画像にマスク
		cv::Rect result_rect(0, 0, img_temp.cols - 18, img_temp.rows - 18);
		result_rect.x = max_pos.x + 10;
		result_rect.y = max_pos.y + 10;
		cv::rectangle(img, result_rect, cv::Scalar(0, 255, 0), -1);
		//std::cout << "(" << max_pos.x << ", " << max_pos.y << "), score=" << max_val << std::endl;
		//disp_image(&img, "");
		//wait_while_kbhit();


		// 結果として格納
		max_pos.x += img_temp.cols / 2;
		max_pos.y += img_temp.rows / 2;
		result_list[i] = max_pos;
		score[i] = max_val;

		

		count++;
	}

	return count;
}

/*--------------------------------------------------
処理:	画像をトレーの範囲をすべて含むように適当に切り出す
戻り値: 切り取りの基準座標 cv::Point
引数:   画像<img>，返り画像<img_dst>
--------------------------------------------------*/
cv::Point cut_image(cv::Mat img, cv::Mat* img_dst){
	cv::Mat img_gray;
	cv::Mat img_hsv;
	img.copyTo(img_gray);
	cv::cvtColor(img, img_hsv, CV_BGR2HSV);

	to_gray(&img_gray);
	cv::threshold(img_gray, img_gray, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	erosion(&img_gray, 10);
	dilation(&img_gray, 10);
	dilation(&img_gray, 10);
	erosion(&img_gray, 10);

	cv::Mat img_labelled;
	int label_count = labeling(&img_gray, &img_labelled);

	//hue 50~80,val 60~110をひとまずトレーのボーダーとする
	for (int i = 1; i <= label_count; i++){
		double hue = get_hue(&img_labelled, &img_hsv, i);
		double val = get_val(&img_labelled, &img_hsv, i);
		int size = get_size(&img_labelled, i);
		//printf("hue = %lf, val = %lf, size = %d\n", hue, val, size);

		if (hue < 80 || hue > 110 || size < 160000 || size > 180000) continue;
		
		/*包含矩形取得*/
		int max_y = -1;
		int max_x = -1; 
		int min_y = 9999;
		int min_x = 9999;


		for (int x = 0; x < img_labelled.cols; x++){
			for (int y = 0; y < img_labelled.rows; y++){
				if (gray((&img_labelled),y,x)==i){
					if (max_y < y) max_y = y;
					if (max_x < x) max_x = x;
					if (min_y > y) min_y = y;
					if (min_x > x) min_x = x;
				}
			}
		}
		cv::Point bottom(min_x, min_y);
		cv::Point top(max_x, max_y);
		cv::Rect cut_rect(bottom, top);
		*img_dst = img(cut_rect);
		return bottom;
		break;
	}
	return cv::Point(0, 0);
}


/*--------------------------------------------------
処理:	ホームポジションに移動する関数
戻り値: void
引数:   なし
--------------------------------------------------*/
void m_home(){
	printf("Move Home\n");
	rsputs("MP 160,0,478,0,180");
	return;
}

/*--------------------------------------------------
処理:	グリップを閉じる関数
戻り値: void 
引数:   なし
--------------------------------------------------*/
void m_grip(){
	printf("GC\n");
	rsputs("GC");
	return;
}

/*--------------------------------------------------
処理:	グリップを開く関数
戻り値: void
引数:   なし
--------------------------------------------------*/
void m_ungrip(){
	printf("GO\n");
	rsputs("GO");
	return;
}

/*--------------------------------------------------
処理:	指定した姿勢と位置に移動する関数
戻り値: void
引数:   位置<x>,<y>,<z>, 姿勢 <a>,<b> 
--------------------------------------------------*/
void m_move_position(double x, double y, double z, double a, double b){
	std::string command("MP ");
	command += std::to_string((int)x);
	command += ",";
	command += std::to_string((int)y);
	command += ",";
	command += std::to_string((int)z);
	command += ",";
	command += std::to_string((int)a);
	command += ",";
	command += std::to_string((int)b);

	printf((char*)command.c_str());
	printf("\n");
	rsputs((char*)command.c_str());
	wait_done();
	return;
}

/*--------------------------------------------------
処理:	指定した姿勢と位置に移動する関数(コマンドが小数点2つくらいまでだすだけで機能に変わりはないです)
戻り値: void
引数:   位置<x>,<y>,<z>, 姿勢 <a>,<b> 
--------------------------------------------------*/
void m_move_position_2(double x, double y, double z, double a, double b){
	std::cout << "(" << x << "," << y << "," << z << "," << a << "," << b << ")に移動します" << endl;
	char buf[BUF_SIZE];

	sprintf_s(buf, BUF_SIZE, "MP %+5.2lf,%+5.2lf,%+5.2lf,%+5.2lf,%+5.2lf", x, y, z, a, b);
	cout << buf << endl;
	rsputs(buf);
	wait_done();

}
/*--------------------------------------------------
処理:	指定した姿勢をポジションリストに登録する関数
戻り値: void
引数:   ポジションリスト登録先 <n> 位置<x>,<y>,<z>, 姿勢 <a>,<b> グリップ<g>（g =1:close  g=0:open）
--------------------------------------------------*/
void m_position_define(int n, double x, double y, double z, double a, double b, int g){
	std::string command("PD ");
	command += std::to_string((int)n);
	command += ",";
	command += std::to_string((int)x);
	command += ",";
	command += std::to_string((int)y);
	command += ",";
	command += std::to_string((int)z);
	command += ",";
	command += std::to_string((int)a);
	command += ",";
	command += std::to_string((int)b);
	command += ",R,A,";

	if (g == 0) command += "O";
	else if (g == 1)command += "C";
	else {
		printf("error: 未定義のグリップ状態");
		
	}

	printf((char*)command.c_str());
	printf("\n");
	rsputs((char*)command.c_str());

	return;
}


/*--------------------------------------------------
処理:	指定した姿勢をポジションリストに登録する関数(コマンドが小数点2つくらいまでだすだけで機能に変わりはないです)
戻り値: void
引数:   ポジションリスト登録先 <n> 位置<x>,<y>,<z>, 姿勢 <a>,<b> グリップ<g>（g =1:close  g=0:open）
--------------------------------------------------*/
void m_position_define_2(int n, double x, double y, double z, double a, double b, int g){
	std::cout << "番号" << n << "=(" << x << "," << y << "," << z << "," << a << "," << b << " " << g << ")を登録します" << endl;

	char buf[BUF_SIZE];
	char grip;
	if (g == 1){
		grip = 'C';
	}
	else{
		grip = 'O';
	}
	sprintf_s(buf, BUF_SIZE, "PD %d,%+5.2lf,%+5.2lf,%+5.2lf,%+5.2lf,%+5.2lf,R,A,%c", n, x, y, z, a, b, grip);
	cout << buf << endl;
	rsputs(buf);
	wait_done();
}


/*--------------------------------------------------
処理:	指定したポジション番号へ直線移動
戻り値: void
引数:   ポジションリスト参照先 <n>
--------------------------------------------------*/
void m_ms(int n){
	std::string command("MS ");
	command += std::to_string(n);

	printf((char*)command.c_str());
	printf("\n called ->");
	rsputs((char*)command.c_str());
}

/*--------------------------------------------------
処理:	ハンドの状態を返す関数
戻り値: int (0: open 1: close)
引数:   無し
--------------------------------------------------*/
int is_grip(){//ハンドの状態を返す関数
	double dpos[5];
	char cpos[3];
	get_position(dpos, cpos);
	if (cpos[POS_OC] == 'O') return 0;
	else return 1;
}

/*--------------------------------------------------
処理:	指定した座標に直線移動する関数
戻り値: void
引数:   位置<x>,<y>,<z>, 姿勢 <a>,<b>
--------------------------------------------------*/
void m_move_straight(double x, double y, double z, double a, double b){
	int grp_flag = is_grip();

	m_position_define_2(1, x, y, z, a, b, grp_flag);
	m_ms(1);

}

/*--------------------------------------------------
処理:	中心(cx,cy,z) 半径r の円を描くように移動する
戻り値: void
引数:   中心座標<cx>,<cy>,<z>, 半径 <r>
--------------------------------------------------*/
void m_move_circle2(double cx, double cy, double z, double r){
	double x[4];
	double y[4];


	/*まずは円周上の代表3点を取得*/
	for (int i = 0; i < 4; i++){
		x[i] = cx + (r * cos(M_PI * 3 / 4 * i));
		y[i] = cy + (r * sin(M_PI * 3 / 4 * i));
	}

	/*代表点を登録*/
	m_position_define(1, x[0], y[0], z, 0, 180, is_grip());
	m_position_define(2, x[1], y[1], z, 0, 180, is_grip());
	m_position_define(3, x[2], y[2], z, 0, 180, is_grip());
	m_position_define(3, x[3], y[3], z, 0, 180, is_grip());

	/*代表点から演習運動させる*/
	printf("MR 1,2,3\n");
	rsputs("MR 1,2,3");
	printf("MR 3,4,1\n");
	rsputs("MR 3,4,1");

}


/*--------------------------------------------------
処理:	labelの図形の重心の座標を取得する関数
戻り値: void
引数:   ラベリングされた画像＜*img＞，ラベル番号<label>，重心格納用変数＜(double) *gx＞＜(double)*gy＞
--------------------------------------------------*/
void get_cog(BYTE *img, int label, double *gx, double *gy){
	long long int x_sum = 0;
	long long int y_sum = 0;
	int size_count = 0;

	for (int x_axis = 0; x_axis < img->cols; x_axis++){
		for (int y_axis = 0; y_axis < img->rows; y_axis++){
			if (gray(img, y_axis, x_axis) == label){
				x_sum += x_axis;//重み取得
				y_sum += y_axis;//重み取得
				
				size_count++;
			}
		}
	}
	
	if (size_count == 0){
		printf("対応するラベル付けがされていません label = %d", label);
	}
	*gx = (double)x_sum / (double)size_count;
	*gy = (double)y_sum / (double)size_count;

	return;
}


/*--------------------------------------------------
処理:	ラベリングされた図形のx軸のサイズ，y軸のサイズを求める関数
戻り値: void
引数:   ラベリングされた画像＜*img＞，ラベル番号<label>，サイズ格納用変数＜(double) *size_x＞＜(double)*size_y＞
--------------------------------------------------*/
void get_size(BYTE *img, int label, double *size_x, double *size_y){
	int x_size_min = 999;
	int x_size_max = -1;
	int y_size_min = 999;
	int y_size_max = 01;

	for (int x_axis = 0; x_axis < img->cols; x_axis++){
		for (int y_axis = 0; y_axis < img->rows; y_axis++){
			if (gray(img, x_axis, y_axis) == label){
				if (x_axis > x_size_max){//サイズ取得：xの最大値
					x_size_max = x_axis;
				}
				if (y_axis > y_size_max){//サイズ取得：yの最大値
					y_size_max = y_axis;
				}
				if (x_axis < x_size_min){//サイズ取得：xの最小値
					x_size_min = x_axis;
				}
				if (y_axis < y_size_min){//サイズ取得：yの最小値
					y_size_min = y_axis;
				}
			}
		}
	}

	*size_x = x_size_max - x_size_min;
	*size_x = y_size_max - y_size_min;

}


/*--------------------------------------------------
処理:	ラベリングされた図形の方向を求める関数
返り値：void
引数:   ラベリングされた画像＜*img＞，ラベル番号<label>，重心（get_cog()で取得）＜*gx＞＜*gy＞,
重心格納用変数＜theta＞
--------------------------------------------------*/
void get_direction(BYTE *img, int label, double gx, double gy, double *theta){
	double S11 = 0;
	double S12 = 0;
	double S22 = 0;
	double lambda = 0;
	double e_x = 0;
	double e_y = 0;

	for (int x = 0; x < img->cols; x++){
		for (int y = 0; y < img->rows; y++){
			if (gray(img, y, x) == label){
				S11 += (x - gx)*(x - gx);
				S12 += (x - gx)*(y - gy);
				S22 += (y - gy)*(y - gy);
			}
		}
	}

	lambda = S11 + S22 + sqrt(S11*S11 + S22*S22 - 2*S11*S22 + 4 * S12*S12);

	e_x = S12;
	e_y = lambda - S12;

	*theta = atan2(e_y, e_x);
}



/*--------------------------------------------------
処理: 	ロボットの動作速度を変更する関数
返り値：void
引数:   指定したい値n(0<=n<=22出ない値を入力する場合clampされます，22<n<=30にしたいときは関数仕様を変更すること)
重心格納用変数＜theta＞
--------------------------------------------------*/
void speed_change(int n){
	char buf[BUF_SIZE];

	if (0 <= n&&n <= 22){


		sprintf_s(buf, BUF_SIZE, "SP %d", n);
		cout << buf << endl;
		rsputs(buf);
		wait_done();
	}
	else{
		std::cout << "指定されたスピードは範囲外です" << std::endl;
	}
}

/*--------------------------------------------------
処理:	得られた画像からブロックの座標と傾きの一覧を取得する関数
返り値：(int)ブロックの数
引数:   生の画像＜*img＞，スレッショルド<thd> ,ブロックパラメータ <param[gx,gy,theta]> 表示モード<mode> : v = ヒストグラム，画像表示
--------------------------------------------------*/
int get_block_list(BYTE *img, int thd, double param[][3], char* mode){
	to_gray(img);//モノクロにする
	from_gray(img);//モノクロイメージの作成
	if (mode == "v")	disp_histogram(img, "");//ヒストグラム作成
	binarize(img, thd);//二値化処理

	/*膨張・縮小処理*/
	for (int i = 0; i < 6 ; i++){
		erosion(img, i * 1);
		dilation(img, i * 1);
		dilation(img, i * 1);
		erosion(img, i * 1);
	}

	int label_count = labeling(img, img_work);//ブロックの数を取得

	if (mode == "v")	disp_labelled_image(img_work, "");//ラベル付け済画像表示

	/*仮置き変数宣言*/
	double gx = 0;
	double gy = 0;
	double direction = 0;

	/*ラベル付けされたオブジェクトの解析*/
	for (int j = 0; j < label_count; j++){
		get_cog(img_work, j + 1, &gx, &gy);
		get_direction(img_work, j + 1, gx, gy, &direction);
		printf("label %d : cog = (%lf,%lf)  theta = %lf\n", j + 1, gx, gy, direction / M_PI * 180);
		param[j][0] = gx;
		param[j][1] = gy;
		param[j][2] = direction / M_PI * 180;
	}

	return label_count;//オブジェクトの数を返す

}


/*--------------------------------------------------
処理:	画像座標系をロボット座標系に変換する
返り値：void
引数:   画像座標系 <cx> <cy> 格納用ロボット座標系<rx> <ry>
--------------------------------------------------*/
void exchange_ctor(double cx, double cy, double* rx, double* ry){
	/**rx = 1.0773 * cy - 188.77;
	*ry = 1.1383 * cx - 324;
	*/

	*rx = 0.4792*cy + 124.66;
	*ry=0.3989*cx - 257.16;
	
	return;

}


/*--------------------------------------------------
処理:	画像座標系とロボット座標系の対応付けデータを測定する（ガバあり）
返り値：void
引数:   なし
--------------------------------------------------*/

void do_zeroin(){
	/*カメラからの情報を解析*/
	int thd = 200;
	double block_list[64][3];//[0]: x座標, [1]: y座標, [2]: 角度
	double arm_pos[64][2]; //[0]: x座標 [1]: y座標 [2]:手首の回転

	int count_block = 0;
	int total_count_block = 0;

	double dpos[5];
	char cpos[3];


	for (int count = 0; count < 4;count++){

		capture(img_src);//画像取得
		capture(img_src);//画像取得
		capture(img_src);//画像取得


		/*画像の座標を解析*/
		count_block = get_block_list(img_src, thd, block_list, "v");
		total_count_block += count_block;//標本にしたブロックの総数を記録
		printf("\n*******************************************\n");
		for (int i = 0; i < count_block; i++){
			printf("block %d :x = %4.2lf, y = %4.2lf, direction = %5.2lf\n", i + 1, block_list[i][0], block_list[i][1], block_list[i][2]);
		}
		printf("*******************************************\n");


		/*ロボットの姿勢を解析*/

		printf("\n*******************************************\n");
		for (int j = 0; j < count_block; j++){
			wait_while_kbhit();//アームを操作したらキーを押す

			get_position(dpos, cpos);
			arm_pos[j][0] = dpos[0];
			arm_pos[j][1] = dpos[1];
			arm_pos[j][2] = dpos[3];
			printf("arm_pos %d :x = %4.2lf, y = %4.2lf, direction = %5.2lf\n", j + 1, arm_pos[j][0], arm_pos[j][1], arm_pos[j][2]);
		
		}
		printf("*******************************************\n");

		m_home();
		printf("\n**remove arm**\nPush any keys to Enter\n\n");
		wait_while_kbhit();//次の周へ

	}


	for (int k = 0; k < total_count_block; k++){
		printf("\n\n==============<  result: (img x , armpos x)  >==============\n");
		printf("%lf,%lf\n", block_list[k][0], arm_pos[k][0]);
		printf("\n\n");
	}

	for (int k = 0; k < total_count_block; k++){
		printf("\n\n==============<  result: (img y , armpos y)  >==============\n");
		printf("%lf,%lf\n", block_list[k][1], arm_pos[k][1]);
		printf("\n\n");
	}

	wait_while_kbhit();
	wait_while_kbhit();
	wait_while_kbhit();

	return;

}
/**************************************************/

/*--------------------------------------------------
処理:	画像座標系とロボット座標系の対応付けデータを測定する(自動化)
返り値：void
引数:   なし
--------------------------------------------------*/

void do_zeroin_refined(){
	int thd = 190;
	double put_positoin_list[10][2];//[0]: x座標, [1]: y座標
	double block_list_buf[64][3];//[0]: x座標, [1]: y座標, [2]: 角度

	double arm_pos[64][2]; //[0]: x座標 [1]: y座標 [2]:手首の回転
	double block_list[64][3];//[0]: x座標, [1]: y座標, [2]: 角度

	double dpos[5];
	char cpos[3];

	for (int count = 0; count < 10; count++){
		/*ブロックをつかませる*/
		m_ungrip();
		wait_while_kbhit(); //ブロックを手で持たせに行った後キーボードを押す
		m_grip();

		/*ブロックを置きに行く*/
		m_position_define(1, put_positoin_list[count][0], put_positoin_list[count][1], 50, 0, 180, is_grip());
		m_ms(1);
		m_ungrip();

		/*ポーズを記録*/
		get_position(dpos, cpos);
		arm_pos[count][0] = dpos[0];
		arm_pos[count][1] = dpos[1];

		/*腕をホームに戻す*/
		m_position_define(2, put_positoin_list[count][0], put_positoin_list[count][1], 200, 0, 180, is_grip());
		m_ms(2);
		m_home();

		/*画像データ解析*/
		capture(img_src);//念のため3回キャプチャする
		capture(img_src);
		capture(img_src);
		get_block_list(img_src, thd, block_list_buf, "v");
		for (int i = 0; i < 2; i++){
			block_list[count][i] = block_list_buf[0][i];
		}
	}

	printf("\n\n********<result X>*******\n");
	for (int j = 0; j < 10; j++){
		printf(" &lf , %lf \n", arm_pos[j][0], block_list[j][0]);
	}
	printf("\n\n********<result X>*******\n");
	for (int j = 0; j < 10; j++){
		printf(" &lf , %lf \n", arm_pos[j][1], block_list[j][1]);
	}

}

/*--------------------------------------------------
処理:	ブロックを取り除く関数
返り値：void
引数:   なし
--------------------------------------------------*/

void eject_block(){

	double block_data[64][3];
	double arm_posList[64][6];
	int block_number = 0;
	int thd = 220;

	/*データのセット*/
	capture(img_src);
	block_number = get_block_list(img_src, thd, block_data, "v");
	for (int i = 0; i < block_number; i++){
		exchange_ctor(block_data[i][0], block_data[i][1], &arm_posList[i][0], &arm_posList[i][1]);//x,y座標
		arm_posList[i][3] = 400;//z座標
		arm_posList[i][4] = block_data[i][2] - 90;//手首のひねり角度
		arm_posList[i][5] = 180;//手首の振り角度
		arm_posList[i][6] = is_grip();//グリップO/C
	}

	/*アームの稼働*/
	m_position_define(50, 200, 200, 100, 0, 180, 1);//ブロックを捨てるポイントを定義
	for (int j = 0; j < block_number; j++){
		/*ブロックの上に移動*/
		m_position_define(1, arm_posList[j][0], arm_posList[j][1], arm_posList[j][2], arm_posList[j][3], arm_posList[j][4], 1);
		m_ms(1);
		/*グリップを開く*/
		m_ungrip();
		/*アームを下げる*/
		m_position_define(2, arm_posList[j][0], arm_posList[j][1], 170, arm_posList[j][3], arm_posList[j][4], 1);
		m_ms(2);
		/*グリップを閉じてブロックをつかむ*/
		m_grip();
		/*アームを上げる*/
		m_position_define(3, arm_posList[j][0], arm_posList[j][1], arm_posList[j][2], arm_posList[j][3], arm_posList[j][4], 0);
		m_ms(3);
		/*ブロックを捨てる*/
		m_ms(50);
	}

}


/*--------------------------------------------------
処理:	色で二値化し,ラベル処理をした画像を返す
返り値：void
引数:   生の画像 <*img>,生の画像のサイズ <size_raws><size_cols>, ラベリング画像 <*img_result>, ターゲット <thd_B><thd_G><thd_R>
--------------------------------------------------*/
void to_color(BYTE *img,BYTE* img_result, int thd_B, int thd_G, int thd_R){
	
	int range = 10;//色の許容範囲

	cv::Mat color_only_img = cv::Mat::zeros(img->rows,img->cols, CV_8UC1);
	cv::Mat labeled_img;

	auto s_min = cv::Scalar(thd_B - range, thd_G + range, thd_R + range);
	auto s_max = cv::Scalar(thd_B + range, thd_G - range, thd_R - range);


	cv::inRange(*img, s_min, s_max, color_only_img);
	color_only_img.copyTo(*img_result);

	labeling(&color_only_img,&labeled_img);
	disp_labeled_image(&labeled_img,"");



}

/*--------------------------------------------------
処理:	与えられた配列に貨幣の個数リストを格納する関数
返り値：void
引数:   画像データ<img_raw>,格納用配列 <coin_list> ,動作モード<mode> "v"->コインパラメータ出力
--------------------------------------------------*/
void get_coin(BYTE img,int coin[6],std::string mode ){

	/*宣言*/
	cv::Mat img_labelled;//ラベル済み画像
	cv::Mat img_cvt;
	cv::Mat img_hsv;
	cv::cvtColor(img, img_hsv, CV_BGR2HSV);

	BOOL hole_flg = FALSE;
	BOOL sat_flg = FALSE;
	BOOL big_flg = FALSE;
	BOOL sml_flg = FALSE;

	/*画像下処理*/
	//---------------グレー化-------------------------------
	cv::cvtColor(img, img_cvt, CV_BGR2HSV);
	for (int rows = 0; rows < img_cvt.rows; rows++){
		for (int cols = 0; cols < img_cvt.cols; cols++){
			rgb((&img_cvt), rows, cols, 0) = rgb((&img_cvt), rows, cols, 1);
			rgb((&img_cvt), rows, cols, 2) = rgb((&img_cvt), rows, cols, 1);
		}
	}
	img_cvt = ~img_cvt;

	//disp_image((&img_cvt), "");
	wait_while_kbhit();



	binarize((&img_cvt), 180);//二値化
	erosion((&img_cvt), 1);//ノイズ除去
	dilation((&img_cvt), 2);//ノイズ除去
	erosion((&img_cvt), 1);//ノイズ除去
	to_gray(&img_cvt);
	int c_num = labeling(&img_cvt, &img_labelled);//ラベリング

	/*確認用，もう使わないかな*/
	//disp_image((&img_cvt), "");
	wait_while_kbhit();

	disp_labeled_image(&img_labelled, "");
	wait_while_kbhit();



	/*各オブジェクト探索開始*/
	for (int i = 1; i <= c_num; i++){
		/*フラグ初期化*/
		hole_flg = FALSE;
		sat_flg = FALSE;
		big_flg = FALSE;
		sml_flg = FALSE;

		/*穴が開いているか？*/
		double center_x = 0;
		double center_y = 0;
		get_cog(&img_labelled, i, &center_x, &center_y);
		if ((img_labelled.data)[(((int)center_y)*img_labelled.step + ((int)center_x)*img_labelled.elemSize())] == 0){
			hole_flg = TRUE;//穴が開いてる！！！！
		}

		/*色がついているか？*/
		double sat = get_sat(&img_labelled, &img_hsv, i);
		if (sat > 3.2){
			sat_flg = TRUE;
		}


		/*大きいか？*/
		int size = get_size(&img_labelled, i);
		if (size > 830){
			big_flg = TRUE;
		}

		/*小さいか？*/
		int size2 = get_size(&img_labelled, i);
		if (size2 < 660){
			sml_flg = TRUE;
		}
	


		/*パラメタ出力*/
		if (mode == "v"){
			printf("label = %d\t", i);
			printf("size = %d\t", size);
			printf("sat = %lf\t", sat);
			printf("\n");
		}

		/*判断*/
		if (hole_flg){
			if (sat_flg){
				if (mode == "v") printf("ラベル%dは5円玉です\n", i);
				coin[4]++;
			}
			else{
				if (mode == "v") printf("ラベル%dは50円玉です\n", i);
				coin[2]++;
			}
		}
		else{
			if (sat_flg){
				if (mode == "v") printf("ラベル%dは10円玉です\n", i);
				coin[3]++;
			}
			else{
				if (big_flg){
					if (mode == "v") printf("ラベル%dは500円玉です\n", i);
					coin[0]++;
				}
				else{
					if (sml_flg){
						if (mode == "v") printf("ラベル%dは1円玉です\n", i);
						coin[5]++;
					}
					else{
						if (mode == "v") printf("ラベル%dは100円玉です\n", i);
						coin[1]++;
					}
				}
			}
		}

	}

	/*確認用，もう使わないかな*/
	//disp_labelled_image(&img_labelled, "");
	//wait_while_kbhit();

}


/*--------------------------------------------------
処理:	指定ラベルの面積を求める関数
返り値：int
引数:   ラベル済み画像データ<img>,ラベル番号<label>
--------------------------------------------------*/
int get_size(BYTE *img, int label){
	int size = 0;

	for (int x = 0; x < img->cols; x++){
		for (int y = 0; y < img->rows; y++){
			if (gray(img, y, x) == label) {
				size++;
			}
		}
	}
	
	return size;
}

/*--------------------------------------------------
処理:	指定ラベルの彩度を求める関数
返り値：double
引数:   ラベル済み画像データ<img>,hsv画像データ<img_hsv>ラベル番号<label>
--------------------------------------------------*/
double get_sat(BYTE *img, BYTE *img_hsv ,int label){
	double sat = 0;

	for (int x = 0; x < img->cols; x++){
		for (int y = 0; y < img->rows; y++){
			if (gray(img, y, x) == label) {
				sat += rgb(img_hsv,y,x,1);//取り急ぎbgr関数でアクセス
			}
		}
	}

	sat = sat / get_size(img, label);//satの平均をとる

	return sat;

}


/*--------------------------------------------------
処理:	指定ラベルの明度を求める関数
返り値：double
引数:   ラベル済み画像データ<img>,hsv画像データ<img_hsv>ラベル番号<label>
--------------------------------------------------*/
double get_val(BYTE *img, BYTE *img_hsv, int label){
	double val = 0;

	for (int x = 0; x < img->cols; x++){
		for (int y = 0; y < img->rows; y++){
			if (gray(img, y, x) == label) {
				val += rgb(img_hsv, y, x, 2);//取り急ぎbgr関数でアクセス
			}
		}
	}

	val = val / get_size(img, label);//satの平均をとる

	return val;

}

/*--------------------------------------------------
処理:	指定ラベルの色彩を求める関数
返り値：double
引数:   ラベル済み画像データ<img>,hsv画像データ<img_hsv>ラベル番号<label>
--------------------------------------------------*/
double get_hue(BYTE *img, BYTE *img_hsv, int label){
	double hue = 0;

	for (int x = 0; x < img->cols; x++){
		for (int y = 0; y < img->rows; y++){
			if (gray(img, y, x) == label) {
				hue += rgb(img_hsv, y, x, 0);//取り急ぎbgr関数でアクセス
			}
		}
	}

	hue = hue / get_size(img, label);//hueの平均をとる

	return hue;

}


/*--------------------------------------------------
処理:	画像を真上から撮ったように変形する
返り値：なし
引数:   変形したい画像のポインタ(上書き)
--------------------------------------------------*/
void warpPers(BYTE *img){

	//1280 960
	// 変換前の画像での座標
	const cv::Point2f src_pt[] = {
		cv::Point2f(115, 0),
		cv::Point2f(1170, 0),
		cv::Point2f(0, 960),
		cv::Point2f(1280, 960) };

	// 変換後の画像での座標
	const cv::Point2f dst_pt[] = {
		cv::Point2f(0, 0),
		cv::Point2f(1280, 0),
		cv::Point2f(0, 960),
		cv::Point2f(1280, 960)
	};

	/*640 480
	// 変換前の画像での座標
	const cv::Point2f src_pt[] = {
	cv::Point2f(65, 0),
	cv::Point2f(595, 0),
	cv::Point2f(0, 480),
	cv::Point2f(640, 480) };
	// 変換後の画像での座標
	const cv::Point2f dst_pt[] = {
	cv::Point2f(0, 0),
	cv::Point2f(640, 0),
	cv::Point2f(0, 480),
	cv::Point2f(640, 480)
	};*/

	//変換行列の計算
	const cv::Mat homography_matrix = cv::getPerspectiveTransform(src_pt, dst_pt);

	//もし何回も画像の変形を行う必要があるなら，ここまでの行列の計算のための処理をmainの最初なりでやることで，
	//homography_matrixをはじめに初期化したあとグローバル的に使いまわしたほうが処理回数が減っていいかも...?


	cv::warpPerspective(*img, *img, homography_matrix, img->size());

	//このとき，
	//robotX = 0.8088*imageY + 161.89, robotY = 0.6933*imageX - 222.32,
	//これがこの画像の変形を使った場合の座標変換になります  
}


/*--------------------------------------------------
処理:	画像の変形(ロボット用)
返り値：なし
引数:   変形したい画像のポインタ(上書き)
--------------------------------------------------*/
BYTE get_sep_area(BYTE* img){
	//---------------グレー化-------------------------------
	cv::cvtColor(*img, *img, CV_BGR2HSV);
	for (int rows = 0; rows < img->rows; rows++){
		for (int cols = 0; cols < img->cols; cols++){
			rgb(img, rows, cols, 0) = rgb(img, rows, cols, 1);
			rgb(img, rows, cols, 2) = rgb(img, rows, cols, 1);
		}
	}

	//---------------GoussianBlurによる平滑化---------------
	cv::Mat img_gauss;
	cv::GaussianBlur(*img, img_gauss, cv::Size(5, 5), 0, 0);
	disp_gray_image(&img_gauss, "");
	wait_while_kbhit();

	//---------------Canny変換------------------------------
	cv::Mat img_canny;
	cv::Canny(img_gauss, img_canny, 100, 200);
	disp_gray_image(&img_canny, "");
	wait_while_kbhit();

	return img_canny;

	//--------------Hough変換-------------------------------
	cv::Mat img_hough;
	cv::HoughLines(img_canny, img_hough, 1, CV_PI / 256, 20);
	//tyuu dann tyuu
}


/*--------------------------------------------------
処理:トレイ(とか)の横の座標を取得()
返り値：なし
引数:  画像image，トレイのラベル番号label,座標格納用変数x,y)
--------------------------------------------------*/
void get_side_of_tray(cv::Mat image,int label,double *x, double *y){
	double gx, gy, th,inclition;
	get_cog(&image, label, &gx, &gy);
	inclition=get_direction_2(&image, label,  &th);
	std::cout<<rad2deg(th)<<" " << inclition << std::endl;



}
/*--------------------------------------------------
処理:トレイの横の座標を取得，ガタガタやる
返り値：なし
引数:  なし
--------------------------------------------------*/
void grip_and_shake_tray(){
	double gx, gy, th, deg;
	int tray_label = -1;
	cv::Mat image;

	get_image_2(&image);
	std::cout << "test" << std::endl;
	//image = cv::imread("sozai/sozai1.png");
	to_gray(&image);
	cv::threshold(image, image, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	
	erosion(&image, 2);
	dilation(&image, 2);
	
	cv::Mat lab;
	int label_num = labeling(&image, &lab);
	disp_labeled_image(&lab, "画像処理表示");
	for (int i = 1; i <= label_num; i++){
		if (160000 < get_size(&lab, i) && get_size(&lab, i) < 174000){
			tray_label = i;
			break;
		}
	}
	if (tray_label == -1){
		std::cout << "トレイ検出失敗" << std::endl;
		return;
	}
	else{
		get_cog(&lab, tray_label, &gx, &gy);
		get_direction_2(&lab, tray_label, &th);
		deg = rad2deg(th);

		cv::waitKey();
		std::cout << get_size(&lab, 1) << std::endl;
		/*
		while (!((-10 <= deg&&deg <= 10) || (deg <= -80 || 80 <= deg))){
		std::cout << "向きがやばいので回転させます" << std::endl;
		double side_cx, side_cy,side_gx,side_gy;
		char mode;//U:画像上へアーム移動,D:画像下へアーム移動,R:画像右へアーム移動L:画像左へアーム移動
		if (-45 >= deg && 45 <= deg){
		if (gx < 960){
		if (-45 >= deg){
		mode = 'D';

		}
		else{
		mode = 'U';

		}
		}
		else{
		if (45 <= deg){
		mode = 'D';

		}
		else{
		mode = 'U';

		}
		}
		}
		else{
		if (deg > 0){
		mode = 'L';

		}
		else{
		mode = 'R';

		}
		}
		grip();
		exchange_ctor(side_cx, side_cy, &side_gx, &side_gy);
		m_move_position(side_gx, side_gy, 210, 0, 180);
		if (mode == 'D'){
		m_move_straight(side_gx+50, side_gy, 210, 0, 180);
		}
		else if (mode == 'U'){
		m_move_straight(side_gx-50, side_gy, 210, 0, 180);
		}
		else if (mode == 'R'){
		m_move_straight(side_gx, side_gy+50, 210, 0, 180);
		}
		else if (mode == 'L'){
		m_move_straight(side_gx, side_gy-50, 210, 0, 180);
		}
		m_home();
		cv::waitKey();
		//	get_image_2(&image);
		image = cv::imread("sozai/sozai1.png");

		to_gray(&image);
		cv::threshold(image, image, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

		int label_num = labeling(&image, &lab);
		for (int i = 1; i <= label_num; i++){
		if (168000 < get_size(&lab, i) && get_size(&lab, i) < 174000){
		tray_label = i;
		break;
		}
		}
		if (tray_label == -1){
		std::cout << "トレイ検出失敗" << std::endl;
		return;
		}
		get_cog(&lab, tray_label, &gx, &gy);
		get_direction_2(&lab, tray_label, &th);
		deg = rad2deg(th);
		cv::waitKey();
		}*/

		ungrip();
		std::cout << tray_label << "番をつかみます" << std::endl;
		double grip_cx, grip_cy, grip_rx, grip_ry;
		tray_where_to_grip(&lab, tray_label, &grip_cx, &grip_cy);
		//cv::circle(*img_src, cv::Point(grip_cx, grip_cy), 30, cv::Scalar(200, 0, 0), -1, CV_AA);
		exchange_ctor(grip_cx, grip_cy, &grip_rx, &grip_ry);
		std::cout << grip_cx << " " << grip_cy << " " << grip_rx << " " << grip_ry << std::endl;

		exchange_ctor(grip_cx, grip_cy, &grip_rx, &grip_ry);
		std::cout << "ここへGO " << grip_rx << " " << grip_ry << std::endl;

		cv::waitKey();
		m_move_position_2(grip_rx - 10, grip_ry, 210, 0, 180);
	
		m_move_position_2(grip_rx - 10, grip_ry, 210, 0, 180);
		m_move_position_2(grip_rx - 20, grip_ry, 210, 0, 160);
		m_move_position_2(grip_rx - 20, grip_ry, 200, 0, 140);
		m_move_position_2(grip_rx - 20, grip_ry, 195, 0, 140);
		/*
		m_move_position_2(grip_rx-50, grip_ry, 195, 0, 120);
		m_move_straight(grip_rx - 50, grip_ry, 175, 0, 120);
		*/
		rsputs("GP 30,63,6");
		grip();
		wait_done();
		m_move_position_2(180, 0, 480, 0, 140);
		rsputs("GP 63,63,3");
		wait_done();
		std::cout << "shake" << std::endl;
		shake();

		m_move_position_2(350, 0, 500, 0, 140);
		cv::waitKey();
		get_image(img_work);

		m_move_position_2(280, 0, 200, 0, 140);
		ungrip();
		m_move_position_2(310, 0, 210, 0, 160);
		m_move_position_2(350, 0, 210, 0, 180);
		m_home();
		wait_done();
		ungrip();






	}


}


/*--------------------------------------------------
処理:トレイの横の座標を取得，受け取ったお金を回収
返り値：なし
引数:  なし
--------------------------------------------------*/
void grip_tray_and_getcoin(){
	double gx, gy, th, deg;
	int tray_label = -1;
	cv::Mat image;

	get_image_2(&image);
	std::cout << "test" << std::endl;
	//image = cv::imread("sozai/sozai1.png");
	to_gray(&image);
	cv::threshold(image, image, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	
	erosion(&image, 2);
	dilation(&image, 4);
	erosion(&image, 2);
	cv::Mat lab;
	int label_num = labeling(&image, &lab);
	disp_labeled_image(&lab, "画像処理表示");
	for (int i = 1; i <= label_num; i++){
		if (160000 < get_size(&lab, i) && get_size(&lab, i) < 174000){
			tray_label = i;
			break;
		}
	}
	if (tray_label == -1){
		std::cout << "トレイ検出失敗" << std::endl;
		return;
	}
	else{
		get_cog(&lab, tray_label, &gx, &gy);
		get_direction_2(&lab, tray_label, &th);
		deg = rad2deg(th);

		cv::waitKey();
		std::cout << get_size(&lab, 1) << std::endl;
		/*
		while (!((-10 <= deg&&deg <= 10) || (deg <= -80 || 80 <= deg))){
		std::cout << "向きがやばいので回転させます" << std::endl;
		double side_cx, side_cy,side_gx,side_gy;
		char mode;//U:画像上へアーム移動,D:画像下へアーム移動,R:画像右へアーム移動L:画像左へアーム移動
		if (-45 >= deg && 45 <= deg){
		if (gx < 960){
		if (-45 >= deg){
		mode = 'D';

		}
		else{
		mode = 'U';

		}
		}
		else{
		if (45 <= deg){
		mode = 'D';

		}
		else{
		mode = 'U';

		}
		}
		}
		else{
		if (deg > 0){
		mode = 'L';

		}
		else{
		mode = 'R';

		}
		}
		grip();
		exchange_ctor(side_cx, side_cy, &side_gx, &side_gy);
		m_move_position(side_gx, side_gy, 210, 0, 180);
		if (mode == 'D'){
		m_move_straight(side_gx+50, side_gy, 210, 0, 180);
		}
		else if (mode == 'U'){
		m_move_straight(side_gx-50, side_gy, 210, 0, 180);
		}
		else if (mode == 'R'){
		m_move_straight(side_gx, side_gy+50, 210, 0, 180);
		}
		else if (mode == 'L'){
		m_move_straight(side_gx, side_gy-50, 210, 0, 180);
		}
		m_home();
		cv::waitKey();
		//	get_image_2(&image);
		image = cv::imread("sozai/sozai1.png");

		to_gray(&image);
		cv::threshold(image, image, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

		int label_num = labeling(&image, &lab);
		for (int i = 1; i <= label_num; i++){
		if (168000 < get_size(&lab, i) && get_size(&lab, i) < 174000){
		tray_label = i;
		break;
		}
		}
		if (tray_label == -1){
		std::cout << "トレイ検出失敗" << std::endl;
		return;
		}
		get_cog(&lab, tray_label, &gx, &gy);
		get_direction_2(&lab, tray_label, &th);
		deg = rad2deg(th);
		cv::waitKey();
		}*/

		ungrip();
		std::cout << tray_label << "番をつかみます" << std::endl;
		double grip_cx, grip_cy, grip_rx, grip_ry;
		tray_where_to_grip(&lab, tray_label, &grip_cx, &grip_cy);
		//cv::circle(*img_src, cv::Point(grip_cx, grip_cy), 30, cv::Scalar(200, 0, 0), -1, CV_AA);
		exchange_ctor(grip_cx, grip_cy, &grip_rx, &grip_ry);
		std::cout << grip_cx << " " << grip_cy << " " << grip_rx << " " << grip_ry << std::endl;

		exchange_ctor(grip_cx, grip_cy, &grip_rx, &grip_ry);
		std::cout << "ここへGO " << grip_rx << " " << grip_ry << std::endl;

		cv::waitKey();
		m_move_position_2(grip_rx - 10, grip_ry, 210, 0, 180);

		m_move_position_2(grip_rx - 10, grip_ry, 210, 0, 180);
		m_move_position_2(grip_rx - 20, grip_ry, 210, 0, 160);
		m_move_position_2(grip_rx - 20, grip_ry, 200, 0, 140);
		m_move_position_2(grip_rx - 20, grip_ry, 195, 0, 140);
		/*
		m_move_position_2(grip_rx-50, grip_ry, 195, 0, 120);
		m_move_straight(grip_rx - 50, grip_ry, 175, 0, 120);
		*/
		rsputs("GP 30,63,6");
		grip();
		wait_done();
		m_move_position_2(180, 0, 480, 0, 140);
		rsputs("GP 63,63,3");
		wait_done();
		
		m_move_position_2(70, 70, 500, 30, 140);
		m_move_straight(70, 70, 500, -140, 130);
		
		m_move_position_2(70, 70, 500, 0, 140);
		m_move_position_2(280, 0, 200, 0, 140);
		ungrip();
		m_move_position_2(310, 0, 210, 0, 160);
		m_move_position_2(350, 0, 210, 0, 180);
		m_home();
		wait_done();
		ungrip();






	}


}


/*--------------------------------------------------
処理:   トレイの掴むべき場所を探す
戻り値: void
引数:  ラベリング済み画像image,トレイのラベル番号label,格納変数x,y
--------------------------------------------------*/
void tray_where_to_grip(cv::Mat *image, int label, double *x, double *y){
	int sum;
	double x_min, x_max;
	double gx,gy;
	get_cog(image, label, &gx, &gy);
	for (int i = 0; i < image->rows; i++){
		sum = 0;
		x_min = image->cols;
		x_max = 0;
		for (int j = 0; j < image->cols; j++){
			if ((int)gray(image, i, j) == label){
				x_min = std::min((double)j, x_min);
				x_max = std::max((double)j, x_max);

				sum++;
			}
		}
		if (sum >= 400){
			*x = (x_min + x_max) / 2;
			*y = i;
			return;
		}
	}
}

void shake(){
	double dpos[5];
	char cpos[3];
	get_position(dpos, cpos);
	speed_change(20);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3] + 35, dpos[4] - 10);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3] - 35, dpos[4] - 10);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3] + 35, dpos[4] - 10);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3] - 35, dpos[4] - 10);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3]+15, dpos[4] + 25);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2] + 100, dpos[3]+8, dpos[4] - 30);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);

	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3] + 35, dpos[4] - 10);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3] - 35, dpos[4] - 10);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3] + 35, dpos[4] - 10);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3] - 35, dpos[4] - 10);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3] + 13, dpos[4] + 25);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2] + 100, dpos[3] - 8, dpos[4] - 30);
	Sleep(440);
	m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3], dpos[4]);
	speed_change(15);
	
m_move_position_2(dpos[0], dpos[1], dpos[2], dpos[3] , dpos[4]);

}

/*--------------------------------------------------
処理:	ラベリングされた図形の方向を求める関数
返り値：傾きdouble
引数:   ラベリングされた画像＜*img＞，ラベル番号<label>，重心（get_cog()で取得）＜*gx＞＜*gy＞,
重心格納用変数＜theta＞
--------------------------------------------------*/
double get_direction_2(BYTE *img, int label, double *theta){
	double gx, gy;
	get_cog(img, label, &gx, &gy);

	double s11 = 0, s12 = 0, s22 = 0;
	for (int i = 0; i < img->rows; i++){
		for (int j = 0; j < img->cols; j++){
			if ((int)gray(img, i, j) == label){
				s11 += (j - gx)*(j - gx);
				s12 += (j - gx)*(i - gy);
				s22 += (i - gy)*(i - gy);
			}
		}
	}
	double lambda = (s11 + s22 + sqrt(s11*s11 + s22*s22 - 2 * s11*s22 + 4 * s12*s12)) / 2;
	double result = std::atan2(lambda - s11, s12);
	if (rad2deg(result) > 90){
		result -= M_PI;
	}
	*theta = result;
	//std::cout << (lambda - s11) << " " << (s12) << " " << (lambda - s11 )/( s12)<< std::endl;
	return (lambda - s11) / (s12);
}

double get_direction_from_corner(cv::Point2f dst_pt[]){
	double gx, gy,theta;
	gx = (dst_pt[0].x + dst_pt[1].x + dst_pt[2].x + dst_pt[3].x) / 4.;

	gy = (dst_pt[0].y + dst_pt[1].y + dst_pt[2].y + dst_pt[3].y) / 4.;

	double s11 = 0, s12 = 0, s22 = 0;
	for (int i = 0; i <4; i++){
		s11 += (dst_pt[i].x - gx)*(dst_pt[i].x - gx);
		s12 += (dst_pt[i].x - gx)*(dst_pt[i].y - gy);
		s22 += (dst_pt[i].y - gy)*(dst_pt[i].y - gy);
	}
	double lambda = (s11 + s22 + sqrt(s11*s11 + s22*s22 - 2 * s11*s22 + 4 * s12*s12)) / 2;
	double result = std::atan2(lambda - s11, s12);
	if (rad2deg(result) > 90){
		result -= M_PI;
	}
	theta = result;
	return theta;
}

/*--------------------------------------------------
処理:	弧度法から度数法を
返り値：度数法のけっか
引数:   弧度法のrad
--------------------------------------------------*/
double rad2deg(double rad){

	return rad * 180 / (M_PI);
}


/*--------------------------------------------------
処理:	画像を撮影ｍ透視変換(warpPers())済みのものを返す
返り値:なし
引数:   画像格納用変数<image>
--------------------------------------------------*/
void get_image_2(cv::Mat *image){
	get_image(image);
	warpPers(image);
}


/*--------------------------------------------------
処理:	画像を撮影,マーカをひろって価格判断 もういらない(小声)
返り値:合計価格
引数:   なし
--------------------------------------------------*/
int find_marker_and_get_price_prototype(){


	int result=0;
	cv::Mat img = cv::imread("marker/test4.png");
	//get_image_2(&img);
	std::map<int, cv::Mat> mp;
	/*
	mp[10] = cv::imread("marker/10.png",0);
	mp[100] = cv::imread("marker/100.png",0);
	*/
	mp[42] = cv::imread("marker/42.png", 0);
	mp[77] = cv::imread("marker/77.png", 0);
	mp[120] = cv::imread("marker/120.png", 0);
	mp[330] = cv::imread("marker/330.png", 0);
	
	
	
	//disp_image(&img, "");
	cv::waitKey();

	
	cv::waitKey();
	to_gray(&img);
	cv::threshold(img, img, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cv::Mat *labeled=new cv::Mat;
	int label_num = labelling(&img,labeled),size;
	disp_labeled_image(labeled, "");
	cv::waitKey();
	// 変換後の画像での座標
	const cv::Point2f dst_pt[] = {
		cv::Point2f(0, 0),
		cv::Point2f(505, 0),
		cv::Point2f(0, 505),
		cv::Point2f(505, 505)
	};
	cv::Mat tmp,match_result_mat;
	cv::Rect roi(cv::Point(0, 0), cv::Size(500, 500));
	
	
	std::map<int,double> result_of_match;


	for (int i = 1; i <= label_num; i++){
		size = get_size(labeled,i);
		std::cout << i << " " << size << std::endl;
		disp_labeled_image(labeled, "");
		cv::waitKey();
		if (size >= 1000 && size <= 100000){
			std::cout << "マーカくらいのおおきさ?" << std::endl;
			

			cv::Point2f src_pt[4];
			
			for (int j = 0; j < labeled->rows; j++){
				for (int k = 0; k < labeled->cols; k++){
					if ((int)gray(labeled, j, k) == i){
						src_pt[0] = cv::Point2f(k, j+3);
						
					}
				}
			}
			for (int k = labeled->cols-1; k >=0; k--){
				for (int j = 0; j < labeled->rows; j++){
				
					if ((int)gray(labeled, j, k) == i){
						src_pt[1] = cv::Point2f(k-3, j);

					}
				}
			}
			
			
			
			for (int k = 0; k<labeled->cols; k++){
				for (int j = labeled->rows - 1; j >= 0; j--){
			
					if ((int)gray(labeled, j, k) == i){
						src_pt[2] = cv::Point2f(k+3, j);

					}
				}
			}
			for (int j = labeled->rows - 1; j >= 0; j--){
				for (int k = labeled->cols - 1; k >= 0; k--){
					if ((int)gray(labeled, j, k) == i){
						src_pt[3] = cv::Point2f(k , j-3);

					}
				}
			}
			//変換行列の計算
			const cv::Mat homography_matrix = cv::getPerspectiveTransform(src_pt, dst_pt);


			cv::warpPerspective(img, tmp, homography_matrix, img.size());
			tmp = tmp(roi);
			//disp_image(&tmp,"");
			for (auto &y : result_of_match){

				y.second = 0;

			}
			for (auto x : mp){
			

				std::cout << "価格 "<<x.first << std::endl;
				//disp_image(&tmp, "");
				//cv::waitKey();
				cv::matchTemplate(tmp, x.second, match_result_mat, CV_TM_SQDIFF);
				cv::Point max_pt;
				double maxVal;
		
				cv::matchTemplate(tmp, x.second, match_result_mat, 3);
				cv::minMaxLoc(match_result_mat, NULL, &maxVal, NULL, &max_pt);
				result_of_match[x.first] = max(maxVal, result_of_match[x.first]);
				for (auto y:result_of_match){
					std::cout << y.second << " ";

				}std::cout << std::endl;
				rot_90(tmp);
				disp_image(&tmp, "");
				//cv::waitKey();	
				cv::matchTemplate(tmp, x.second, match_result_mat, 3);
				cv::minMaxLoc(match_result_mat, NULL, &maxVal, NULL, &max_pt);
				result_of_match[x.first] = max(maxVal, result_of_match[x.first]);
				for (auto y : result_of_match){
					std::cout << y.second << " ";

				}std::cout << std::endl;
				rot_90(tmp);
				//disp_image(&tmp, "");
				//cv::waitKey();
				cv::matchTemplate(tmp, x.second, match_result_mat, 3);
				cv::minMaxLoc(match_result_mat, NULL, &maxVal, NULL, &max_pt);
				result_of_match[x.first] = max(maxVal, result_of_match[x.first]);
				for (auto y : result_of_match){
					std::cout << y.second << " ";

				}std::cout << std::endl;
				rot_90(tmp);
				//disp_image(&tmp, "");
				cv::waitKey();
				cv::matchTemplate(tmp, x.second, match_result_mat, 3);
				cv::minMaxLoc(match_result_mat, NULL, &maxVal, NULL, &max_pt);
				result_of_match[x.first] = max(maxVal, result_of_match[x.first]);
				for (auto y : result_of_match){
					std::cout << y.second << " ";

				}std::cout << std::endl;
			
		



				std::cout<<std::endl;
			}
			int tmp_result = 0;
			double tmp_match_level=0;
			for (auto x : result_of_match){
				if (x.second >= 0.90){
					if (tmp_match_level < x.second){
						tmp_match_level = x.second,
						tmp_result = x.first;
					}
				}
			}
			cout << tmp_result << endl;
			result += tmp_result;
			//cv::waitKey();
		
		}
		else{

			//std::cout << "こいつはマーカではなさそう..." << std::endl;
		
		}

		std::cout << std::endl;
	}
	return result;
}


/*--------------------------------------------------
処理:	受け取った画像を90ド回転
返り値:回した画像
引数:   回したい画像
--------------------------------------------------*/
void rot_90(cv::Mat &img){
	cv::Rect roi_rect(0, 0, img.cols, img.rows);
	cv::Mat src_roi(img, roi_rect);
	cv::Mat dst_roi(img, roi_rect);

	// (2)With specified three parameters (angle, rotation center, scale)
	//    calculate an affine transformation matrix by cv2DRotationMatrix
	double angle = -90.0, scale = 1.0;
	cv::Point2d center(src_roi.cols*0.5, src_roi.rows*0.5);
	const cv::Mat affine_matrix = getRotationMatrix2D(center, angle, scale);

	// (3)rotate the image by warpAffine taking the affine matrix
	warpAffine(src_roi, img, affine_matrix, dst_roi.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar::all(255));

}




/*--------------------------------------------------
処理:	事前準備用:おつり用硬貨の場所を決めるための関数，アームが移動してくのでそこに硬貨を置く
返り値:なし
引数:   なし
--------------------------------------------------*/
void check_where_coin(){
	speed_change(22);
	m_ungrip();
	m_move_position_2(162, 148, 258.2, 0, 180);
	m_move_position_2(162, 148, 248.6, 0, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(162, 148, 248.6, 90, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(162, 148, 258.6, 90, 180);
	m_home();
	m_move_position_2(162, 208, 258.6, 0, 180);
	m_move_position_2(162, 208, 248.6, 0, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(162, 208, 248.6, 90, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(162, 208, 258.6, 90, 180);
	m_home();
	m_move_position_2(222, 148, 258.6, 0, 180);
	m_move_position_2(222, 148, 248.6, 0, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(222, 148, 248.6, 90, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(222, 148, 258.6, 90, 180);
	m_home();
	m_move_position_2(222, 208, 258.6, 0, 180);
	m_move_position_2(222, 208, 248.6, 0, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(222, 208, 248.6, 90, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(222, 208, 258.6, 90, 180);
	m_home();
	m_move_position_2(282, 148, 258.6, 0, 180);
	m_move_position_2(282, 148, 248.2, 0, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(282, 148, 248.2, 90, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(282, 148, 258.2, 90, 180);
	m_home();
	m_move_position_2(282, 208, 258.2, 0, 180);
	m_move_position_2(282, 208, 248.2, 0, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(282, 208, 248.2, 90, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_position_2(282, 208, 258.2, 90, 180);
	m_home();


}

/*--------------------------------------------------
処理:	お釣りをトレイに乗せる
戻り値: void
引数:   返す硬貨coin
--------------------------------------------------*/
void return_change(map<long long int, long long int> coin){
	/*
	硬貨の種類	直径	厚み	孔径	重さ
	1円	20.0mm	約1.5mm	-	1g
	5円	22.0mm	約1.5mm	5mm	3.75g
	10円	23.5mm	約1.5mm	-	4.5g
	50円	21.0mm	約1.7mm	4mm	4g
	100円	22.6mm	約1.7mm	-	4.8g
	500円	26.5mm	約1.8mm	-	7g
	*/
	/*
	m_move_straight(160, 160, 250, 0, 180);
	m_move_straight(160, 220, 250, 0, 180);
	m_move_straight(220, 160, 250, 0, 180);
	m_move_straight(220, 220, 250, 0, 180);
	m_move_straight(280, 160, 250, 0, 180);
	m_move_straight(280, 220, 250, 0, 180);

	*/
	map<long long int, double> thickness;
	thickness[1] = 1.5;
	thickness[5] = 1.47;
	thickness[10] = 1.5;
	thickness[50] = 1.7;
	thickness[100] = 1.7;
	thickness[500] = 1.8;

	for (int i = 1; i <= coin[1]; i++){
		m_home();
		m_move_position_2(162, 148, 268.2 + (thickness[1] * (5 - (double)i)), 0, 180);
		m_move_straight(162, 148, 248.6 + (thickness[1] * (5 - (double)i)), 0, 180);
		wait_done();
		m_grip();
		m_move_position_2(162, 148, 268.2 + (thickness[1] * (5 - (double)i)), 0, 180);
		m_move_position_2(400, 0, 270, 0, 180);
		ungrip();
	}
	for (int i = 1; i <= coin[5]; i++){
		m_home();
		m_move_position_2(162, 208, 268.2 + (thickness[5] * (1 - (double)i)), 0, 180);
		m_move_straight(162, 208, 248.6 + (thickness[5] * (1 - (double)i)), 0, 180);
		wait_done();
		m_grip();
		m_move_position_2(162, 208, 268.2 + (thickness[5] * (1 - (double)i)), 0, 180);
		m_move_position_2(400, 0, 270, 0, 180);
		ungrip();
	}
	for (int i = 1; i <= coin[10]; i++){
		m_home();
		m_move_position_2(222, 148, 268.2 + (thickness[10] * (5 - (double)i)), 0, 180);
		m_move_straight(222, 148, 248.6 + (thickness[10] * (5 - (double)i)), 0, 180);
		wait_done();
		m_grip();
		m_move_position_2(222, 148, 268.2 + (thickness[10] * (5 - (double)i)), 0, 180);
		m_move_position_2(400, 0, 270, 0, 180);
		ungrip();
	}
	for (int i = 1; i <= coin[50]; i++){
		m_home();
		m_move_position_2(222, 208, 268.2 + (thickness[50] * (1 - (double)i)), 0, 180);
		m_move_straight(222, 208, 248.6 + (thickness[50] * (1 - (double)i)), 0, 180);
		wait_done();
		m_grip();
		m_move_position_2(222, 208, 268.2 + (thickness[50] * (1 - (double)i)), 0, 180);
		m_move_position_2(400, 0, 270, 0, 180);
		ungrip();
	}
	for (int i = 1; i <= coin[100]; i++){
		m_home();
		m_move_position_2(282, 148, 268.2 + (thickness[100] * (5 - (double)i)), 0, 180);
		m_move_straight(282, 148, 248.2 + (thickness[100] * (5 - (double)i)), 0, 180);
		wait_done();
		m_grip();
		m_move_position_2(282, 148, 268.2 + (thickness[100] * (5 - (double)i)), 0, 180);
		m_move_position_2(400, 0, 270, 0, 180);
		ungrip();

	}
	for (int i = 1; i <= coin[500]; i++){
		m_home();
		m_move_position_2(282, 208, 268.2 + (thickness[500] * (1 - (double)i)), 0, 180);
		m_move_straight(282, 208, 248.2 + (thickness[500] * (1 - (double)i)), 0, 180);
		wait_done();
		m_grip();
		m_move_position_2(282, 208, 268.2 + (thickness[500] * (1 - (double)i)), 0, 180);
		m_move_position_2(400, 0, 270, 0, 180);
		ungrip();
	}
	m_home();


}

/*--------------------------------------------------
処理:	合計金額と受け取ったお金からだすべきお釣りの貨幣を計算
返り値： 各貨幣をキーとし，中に枚数が入っている連想配列
引数:    うけとったお金<received>, 合計金額<totalprice>
--------------------------------------------------*/
std::map<long long int, long long int> accounting(long long int received, long long int total_price){
	long long int otsuri = received - total_price;
	//存在しうる貨幣をここで指定
	std::set<long long int>st = { 1, 5, 10, 50, 100, 500 };

	std::map<long long int, long long int> result;
	if (otsuri<0){
		std::cout << "受け取ったお金では足りません!!!" << std::endl;
		return result;
	}
	else{
		//setのなかでおおきい貨幣から順にできるかぎりたくさん枚数を出していく
		std::set<long long int>::reverse_iterator i = st.rbegin();
		while (i != st.rend()){
			result[*i] = otsuri / (*i);
			otsuri %= (*i);
			i++;
		}
		return result;
	}
}


/*--------------------------------------------------
処理:   ARマーカかの判定+数値化
戻り値: マーカの価格
引数:   切り出したマーカの画像image
--------------------------------------------------*/
int ar_read(cv::Mat *image){
	cv::Mat im = *image;
	cv::Mat *img = &im;
	vector<vector<double>> grid(6, vector<double>(6));
	cv::Mat out;
	cv::Rect rect;
	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 6; j++){
			
			rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
			out=(*img)(rect);
			grid[i][j]=average_of_Mat(&out);
		//	cout << grid[i][j] << " ";
		}
		//cout << endl;
	}if (is_marker(grid)){
		//disp_image(img, "");
		cv::waitKey();
		return ar_to_price(grid);
	}
	else{
		rot_90(im);
		for (int i = 0; i < 6; i++){
			for (int j = 0; j < 6; j++){

				rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
				out = (*img)(rect);
				grid[i][j] = average_of_Mat(&out);
			///	cout << grid[i][j] << " ";
			}
		//	cout << endl;
		}if (is_marker(grid)){
			//disp_image(img, "");
			cv::waitKey();
			return ar_to_price(grid);
		}
		else{
			rot_90(im);
			for (int i = 0; i < 6; i++){
				for (int j = 0; j < 6; j++){

					rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
					out = (*img)(rect);
					grid[i][j] = average_of_Mat(&out);
					//cout << grid[i][j] << " ";
				}
			//	cout << endl;
			}if (is_marker(grid)){
				//disp_image(img, "");
				cv::waitKey();
				return ar_to_price(grid);
			}
			else{
				rot_90(im);
				for (int i = 0; i < 6; i++){
					for (int j = 0; j < 6; j++){

						rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
						out = (*img)(rect);
						grid[i][j] = average_of_Mat(&out);
					//	cout << grid[i][j] << " ";
					}
					//cout << endl;
				}if (is_marker(grid)){
					//disp_image(img, "");
					cv::waitKey();
					return ar_to_price(grid);
				}
				
			}

		}
	}
	cv::flip(im, im, 0);
	//disp_image(img, "");
	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 6; j++){

			rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
			out = (*img)(rect);
			grid[i][j] = average_of_Mat(&out);
			//	cout << grid[i][j] << " ";
		}
		//cout << endl;
	}
	//disp_image(image, "");
	if (is_marker(grid)){
		//disp_image(img, "");
		return ar_to_price(grid);
	}
	else{
		rot_90(im);
		for (int i = 0; i < 6; i++){
			for (int j = 0; j < 6; j++){

				rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
				out = (*img)(rect);
				grid[i][j] = average_of_Mat(&out);
				///	cout << grid[i][j] << " ";
			}
			//	cout << endl;
		}
		//disp_image(img, "");
		if (is_marker(grid)){
			//disp_image(image, "");
			return ar_to_price(grid);
		}
		else{
			rot_90(im);
			for (int i = 0; i < 6; i++){
				for (int j = 0; j < 6; j++){

					rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
					out = (*img)(rect);
					grid[i][j] = average_of_Mat(&out);
					//cout << grid[i][j] << " ";
				}
				//	cout << endl;
			}
			
			//disp_image(img, "");
			if (is_marker(grid)){
				//disp_image(image, "");
				return ar_to_price(grid);
			}
			else{
				rot_90(im);
				for (int i = 0; i < 6; i++){
					for (int j = 0; j < 6; j++){

						rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
						out = (*img)(rect);
						grid[i][j] = average_of_Mat(&out);
						//	cout << grid[i][j] << " ";
					}
					//cout << endl;
				}
				//disp_image(img, "");
				if (is_marker(grid)){
					//disp_image(img, "");
					return ar_to_price(grid);
				}
				else{
					rot_90(im);
					for (int i = 0; i < 6; i++){
						for (int j = 0; j < 6; j++){

							rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
							out = (*img)(rect);
							grid[i][j] = average_of_Mat(&out);
							//	cout << grid[i][j] << " ";
						}
						//cout << endl;
					}
					//disp_image(img, "");
					if (is_marker(grid)){
						//disp_image(img, "");
						return ar_to_price(grid);
					}

				}

			}

		}
	}
	//std::cout << "こいつはマーカではなさそう" << std::endl;
	return 0;
}


/*--------------------------------------------------
処理:マーカを見っけて合計の値段を調べ理
戻り値: 合計の値段
引数:   nasi 
--------------------------------------------------*/
int find_marker_and_get_price(){
	

	int result = 0;
	//cv::Mat img


	//get_image_2(&img);

	bool loop_flg;
	cv::Mat img;// = cv::imread("coin_test9.png"/*"marker/AR264.png"*/, 1);
	
	do{
		loop_flg = FALSE;
		get_image_2(&img);
		get_image_2(&img);
		get_image_2(&img);
		if (img.data == NULL){
			printf("判定する画像の取得に失敗しました\n");
			loop_flg = TRUE;
		}
	} while (loop_flg);
	
		//disp_image(&img, "");
	to_gray(&img);
	cv::Mat image_source;
	//disp_image(&img, "");


	cv::threshold(img, img, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cv::Mat labeled_image;
	int label_num = labeling(&img, &labeled_image);
	disp_labeled_image(&labeled_image, "");

	copy_image(&img, &image_source);
	// 変換後の画像での座標
	const cv::Point2f dst_pt[] = {
		cv::Point2f(0, 0),
		cv::Point2f(300, 0),
		cv::Point2f(0, 300),
		cv::Point2f(300, 300)
	};

	cv::Rect roi(cv::Point(0, 0), cv::Size(300, 300));


	std::map<int, double> result_of_match;

	//disp_image(&image_source, "");
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat tmp,label_i_image;
	
	for (int label_i = 1; label_i <= label_num; label_i++){
		int max_level = 0;
		label_i_image = labeL_num_extraction(&labeled_image, label_i);
		//disp_image(&label_i_image, "");
		//cv::waitKey();
	  	cv::findContours(label_i_image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_L1);


		
		for (int i = 0; i < contours.size(); i++) {
			// ある程度の面積が有るものだけに絞る
			double a = contourArea(contours[i], false);
			if (a > 80) {
				//輪郭を直線近似する
				std::vector<cv::Point> approx;
				cv::approxPolyDP(cv::Mat(contours[i]), approx, 0.05 * cv::arcLength(contours[i], true), true);
				// 矩形のみ取得
			//	std::cout << approx.size() << std::endl;
				if (approx.size() == 4) {
					cv::Point2f src_pt[4];
					src_pt[0] = approx[0];
					src_pt[1] = approx[1];
					src_pt[2] = approx[3];
					src_pt[3] = approx[2];
					const cv::Mat homography_matrix = cv::getPerspectiveTransform(src_pt, dst_pt);
					cv::warpPerspective(image_source, tmp, homography_matrix, image_source.size());
					tmp = tmp(roi);
					//disp_image(&tmp, "");
					int tmp_price = ar_read(&tmp);
					std::cout << tmp_price<<" "<<label_i << std::endl;
					cash_disp->add_total_price(tmp_price);
					result += tmp_price;
				}
			}
		}
	}
	//disp_image(&image_source, "");
	return result;
}


/*--------------------------------------------------
処理:マーカを見っけて合計の値段を調べ理
戻り値: 合計の値段
引数:   nasi
--------------------------------------------------*/
int find_marker_and_get_price_and_throw(){


	int result = 0;
	//cv::Mat img


	//get_image_2(&img);

	bool loop_flg;
	cv::Mat img;// = cv::imread("coin_test9.png"/*"marker/AR264.png"*/, 1);

	do{
		loop_flg = FALSE;
		get_image_2(&img);
		get_image_2(&img);
		get_image_2(&img);
		if (img.data == NULL){
			printf("判定する画像の取得に失敗しました\n");
			loop_flg = TRUE;
		}
	} while (loop_flg);
	erosion(&img, 2);
	dilation(&img,2);
	//disp_image(&img, "");
	to_gray(&img);
	cv::Mat image_source;
	//disp_image(&img, "");


	cv::threshold(img, img, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cv::Mat labeled_image;
	int label_num = labeling(&img, &labeled_image);
	disp_labeled_image(&labeled_image, "");

	copy_image(&img, &image_source);
	// 変換後の画像での座標
	const cv::Point2f dst_pt[] = {
		cv::Point2f(0, 0),
		cv::Point2f(300, 0),
		cv::Point2f(0, 300),
		cv::Point2f(300, 300)
	};

	cv::Rect roi(cv::Point(0, 0), cv::Size(300, 300));


	std::map<int, double> result_of_match;

	//disp_image(&image_source, "");
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat tmp, label_i_image;

	for (int label_i = 1; label_i <= label_num; label_i++){
		int max_level = 0;
		label_i_image = labeL_num_extraction(&labeled_image, label_i);
		//disp_image(&label_i_image, "");
		//cv::waitKey();
		cv::findContours(label_i_image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_L1);



		for (int i = 0; i < contours.size(); i++) {
			// ある程度の面積が有るものだけに絞る
			double a = contourArea(contours[i], false);
			if (a > 80) {
				//輪郭を直線近似する
				std::vector<cv::Point> approx;
				cv::approxPolyDP(cv::Mat(contours[i]), approx, 0.05 * cv::arcLength(contours[i], true), true);
				// 矩形のみ取得
				//	std::cout << approx.size() << std::endl;
				if (approx.size() == 4) {
					cv::Point2f src_pt[4];
					src_pt[0] = approx[0];
					src_pt[1] = approx[1];
					src_pt[2] = approx[3];
					src_pt[3] = approx[2];
					const cv::Mat homography_matrix = cv::getPerspectiveTransform(src_pt, dst_pt);
					cv::warpPerspective(image_source, tmp, homography_matrix, image_source.size());
					tmp = tmp(roi);
					//disp_image(&tmp, "");
					int tmp_price = ar_read(&tmp);
					
					std::cout << tmp_price << " " << label_i << std::endl;
					if (tmp_price){
						double cx = (src_pt[0].x + src_pt[1].x + src_pt[2].x + src_pt[3].x) / 4,
							cy=(src_pt[0].y + src_pt[1].y+ src_pt[2].y + src_pt[3].y) / 4, rx, ry;
						exchange_ctor(cx, cy, &rx, &ry);
						m_ungrip();
						m_move_position_2(rx+22, ry, 300,rad2deg(get_direction_from_corner(src_pt)), 180);
						m_move_straight(rx + 22, ry, 220, rad2deg(get_direction_from_corner(src_pt)), 180);
						m_grip();
						m_move_straight(rx+22, ry, 340, get_direction_from_corner(src_pt), 180);
						m_move_position_2(430, -169, 340, 90, 100);
						m_ungrip();
						m_home();
					}
					cash_disp->add_total_price(tmp_price);
					result += tmp_price;
				}
			}
		}
	}
	//disp_image(&image_source, "");
	return result;
}

/*--------------------------------------------------
処理:	ar_read用の関数，数値化の処理の一部のラッピング
戻り値: マーカ価格
引数:   平均値grid
--------------------------------------------------*/
int ar_to_price(vector<vector<double>> grid_double){
	/*
	□ □ □ □ □ □
	□ ■ □  1 ■ □
	□ ■ 2  □  4 □
	□ 8  16 32 64 □
	□ ■128 256□ □
	□ □ □ □ □ □
	*/
	vector<vector<int>> grid(6,vector<int>(6));
	
	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 6; j++){
			grid[i][j] = (grid_double[i][j] < 128) ? 1 : 0;
		}
	}
	int result = 0;
	result += grid[1][3] * 1 
		+ grid[2][2] * 2 + grid[2][4] * 4
		+ grid[3][1] * 8 + grid[3][2] * 16 + grid[3][3] * 32 + grid[3][4] * 64
		+ grid[4][2] * 128 + grid[4][3] * 256;
	return result;

}


/*--------------------------------------------------
処理:   マーカか判定
戻り値: true or false
引数:   平均値grid
--------------------------------------------------*/
bool is_marker(vector<vector<double>> grid){
	/*std::cout << grid[1][1] << " " << grid[4][1] << " " << grid[1][4] << " "
		<< grid[2][1] << " " << grid[2][3] << " " <<
		grid[1][2] << " " << grid[4][4] << std::endl;
	*/
	if (grid[1][1]<140 && grid[4][1]<140 && grid[1][4]<140 && grid[2][1]<140 && grid[2][3]>180 && grid[1][2]>180 && grid[4][4]>180){

		return true;
	}
	else{
		return false;
	}

}

/*--------------------------------------------------
処理:	画像の平均値取得
戻り値: 平均double 
引数:   画像img
--------------------------------------------------*/
double average_of_Mat(cv::Mat *img){
	double result = 0, img_size = img->rows*img->cols;
	for (int i = 0; i < img->rows; i++){
		for (int j = 0; j < img->cols; j++){
			result += double(gray(img, i, j))/img_size;
		}
	}
	return result;
}


/*--------------------------------------------------
処理:	画像の正規化，圧縮を行う関数（圧縮部分は未実装，必要になったら作る）
返り値：なし
引数:   元画像<img>, 変更後画像<img_dst> (同じポインタでも動くよ)
--------------------------------------------------*/
void compress_img(BYTE *img, BYTE *img_dst){
	int gray_max = 0;
	int gray_min = 10000;

	/*最大値，最小値の取得*/
	for (int rows = 0; rows < img->rows; rows++){
		for (int cols = 0; cols < img->cols; cols++){
			if (rgb(img, rows, cols, 0) > gray_max){
				gray_max = rgb(img, rows, cols, 0);
			}
			if (rgb(img, rows, cols, 0) < gray_min){
				gray_min = rgb(img, rows, cols, 0);
			}
		}
	}

	/*正規化*/
	for (int rows = 0; rows < img->rows; rows++){
		for (int cols = 0; cols < img->cols; cols++){
			int normalize = (rgb(img, rows, cols, 0) - gray_min) * 255 / gray_max - gray_min;
			rgb(img_dst, rows, cols, 0) = normalize;
			rgb(img_dst, rows, cols, 1) = normalize;
			rgb(img_dst, rows, cols, 2) = normalize;
		}
	}
}

/*--------------------------------------------------
処理:	フィルタリングから，サンプル画像との一致度分布を出す関数
返り値：なし
引数:   元画像<img>, サンプル画像<img_sample>, 出力画像<img_dst>
--------------------------------------------------*/
void filtering(cv::Mat img, cv::Mat img_sample, cv::Mat img_dst){
	/*変数宣言*/
	int img_y = img.rows;
	int img_x = img.cols;
	int sample_y = img.rows;
	if (sample_y % 2 == 0) sample_y--;//奇数になるように
	int sample_x = img.cols;
	if (sample_x % 2 == 0) sample_x--;//奇数になるように
	int img_size = img_x * img_y;
	int sample_size = sample_x * sample_y;

	int sample_center_y = (sample_y - 1) / 2;
	int sample_center_x = (sample_x - 1) / 2;


	/*画像下処理*/
	cv::GaussianBlur(img_sample, img_sample, cv::Size(5, 5), 0, 0);
	cv::cvtColor(img, img, CV_BGR2HSV);
	cv::cvtColor(img_sample, img_sample, CV_BGR2HSV);

	/*フィルタ開始*/
	for (int rows = 0; rows < img_y; rows++){
		for (int cols = 0; cols < img_x; cols++){
			/*各画素に対して*/
			double filtered = 0;//フィルタ値仮置き用変数
			/*sampleでフィルタリング*/
			for (int s_rows = 0; s_rows < sample_y; s_rows++){
				for (int s_cols = 0; s_cols < sample_x; s_cols++){
					/*マスク用の分岐*/
					if (rgb((&img_sample), s_rows, s_cols, 0) == 0 &&
						rgb((&img_sample), s_rows, s_cols, 1) == 0 &&
						rgb((&img_sample), s_rows, s_cols, 2) == 0){
						continue;//(0,0,0)なら，マスク部分として無視する
					}
					/*元画像で有効な範囲であれば*/
					if (cols - sample_center_x + s_cols >= 0 &&
						rows - sample_center_y + s_rows >= 0 &&
						cols - sample_center_x + s_cols < img_x &&
						rows - sample_center_y + s_rows < img_y){
						/*差をとって*/
						int distance_h = rgb((&img), rows - sample_center_y + s_rows, cols - sample_center_x + s_cols, 0) - rgb((&img_sample), s_rows, s_cols, 0);
						int distance_s = rgb((&img), rows - sample_center_y + s_rows, cols - sample_center_x + s_cols, 1) - rgb((&img_sample), s_rows, s_cols, 1);
						int distance_v = rgb((&img), rows - sample_center_y + s_rows, cols - sample_center_x + s_cols, 2) - rgb((&img_sample), s_rows, s_cols, 2);
						/*正規化関数(0~255)にぶち込む*/
						double distance_sum = ((distance_h + distance_s + distance_v) / 3) -128;
						if (distance_sum < 0) distance_sum = 0;//ReLU関数
						/*返り値をfilteredに足す*/
						filtered += distance_sum;
					}
				}
			}
			/*filteredをimg_sampleの画素数で割る（正規化）*/
			filtered /= sample_size;
			/*img_dstの(rows,cols)に代入*/
			rgb((&img_dst), rows, cols, 0) = filtered;
			rgb((&img_dst), rows, cols, 1) = filtered;
			rgb((&img_dst), rows, cols, 2) = filtered;
		}
	}

	return;
}

cv::Mat labeL_num_extraction(cv::Mat *labeled, int label_num){
	cv::Mat result(cv::Size(labeled->cols, labeled->rows), CV_8U, cv::Scalar::all(0));;


	cv::Mat *result_pointer = &result;
	
	for (int rows = 0; rows < labeled->rows; rows++){
		for (int cols = 0; cols < labeled->cols; cols++){
			if (gray(labeled, rows, cols) == label_num){
				//std::cout << cols << " " << rows << std::endl;
				gray(result_pointer, rows, cols) = 255;
			}
			else{
				gray(result_pointer, rows, cols) = 0;

			}
		}
	}
	return result;

}