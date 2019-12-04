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
#define IMG_YSIZE 
#define LABEL_THD 220
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

void check_where_coin();
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
BYTE get_sep_area(BYTE* img);//境界で切り出した画像を返す関数
void warpPers(BYTE *img);

void get_side_of_tray(cv::Mat image, int label, double *x, double *y);
double get_direction_2(BYTE *img, int label, double* theta);//方向を取得する関数,傾きをついでにreturn
double rad2deg(double rad);//弧度法を入力すると弧度法で返す
void tray_where_to_grip(cv::Mat *image, int label, double *x, double *y);//トレイで掴みやすそうなとこを探す
void get_image_2(cv::Mat *image);//透視変換済みの画像を撮影
void rot_90(cv::Mat &img);
int find_marker_and_get_price_prototype();//ARマーカ風のオブジェクトで値段計算
int find_marker_and_get_price();//ARマーカ風のオブジェクトで値段計算
double average_of_Mat(cv::Mat *img);
int ar_read(cv::Mat *img);
bool is_marker(vector<vector<double>> grid);
int ar_to_price(vector<vector<double>> grid_double);
/*連携部分*/
void exchange_ctor(double cx, double cy, double* rx, double* ry);//画像座標系からロボット座標系に変換
void do_zeroin();//実行すると座標の調整を行う
void eject_block();//実行するとブロックの取り除き動作を行う
void grip_and_shake_tray();//トレイを掴み，ゆらして置く
void grip_tray_and_getcoin();//受け取ったお金を回収
void return_change(map<long long int, long long int> coin);
/**************************************************/
std::map<long long int, long long int> accounting(long long int received, long long int total_price);


/***************************************************
mainfunc: メイン処理（実際の処理を記述）
***************************************************/
void mainfunc(HDC *hDC) {


	/*
	cv::Mat img=	cv::imread("sozai.png",0);
	
	cv::threshold(img, img, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	
	disp_image(&img, "");
	cv::waitKey();
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 960);
	
	cv::waitKey();
	*/
	std::cout << find_marker_and_get_price() << std::endl;
	//grip_and_shake_tray();
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

	disp_image((&img_cvt), "");
	wait_while_kbhit();



	binarize((&img_cvt), 180);//二値化
	erosion((&img_cvt), 1);//ノイズ除去
	dilation((&img_cvt), 2);//ノイズ除去
	erosion((&img_cvt), 1);//ノイズ除去
	to_gray(&img_cvt);
	int c_num = labeling(&img_cvt, &img_labelled);//ラベリング

	/*確認用，もう使わないかな*/
	disp_image((&img_cvt), "");
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
	

	//変換行列の計算
	const cv::Mat homography_matrix = cv::getPerspectiveTransform(src_pt, dst_pt);

	//もし何回も画像の変形を行う必要があるなら，ここまでの行列の計算のための処理をmainの最初なりでやることで，
	//homography_matrixをはじめに初期化したあとグローバル的に使いまわしたほうが処理回数が減っていいかも...?


	cv::warpPerspective(*img, *img, homography_matrix, img->size());

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
	/*
	erosion(&image, 3);
	dilation(&image, 3);
	*/
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
		m_move_position_2(grip_rx - 40, grip_ry, 200, 0, 140);
		m_move_position_2(grip_rx - 38, grip_ry, 195, 0, 140);
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
	/*
	erosion(&image, 3);
	dilation(&image, 3);
	*/
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
		m_move_position_2(grip_rx - 40, grip_ry, 200, 0, 140);
		m_move_position_2(grip_rx - 38, grip_ry, 195, 0, 140);
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
		
		m_move_position_2(160, 160, 250, 0, 140);
		m_move_straight(160, 160, 250, -70, 130);
		
		m_move_straight(160, 160, 250, 0, 140);
		m_move_position_2(280, 0, 200, 0, 140);
		ungrip();
		m_move_position_2(310, 0, 210, 0, 160);
		m_move_position_2(350, 0, 210, 0, 180);
		m_home();
		wait_done();
		ungrip();






	}


}
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
		if (sum >= 300){
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
処理:	画像を撮影,マーカをひろって価格判断
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
	
	
	
	disp_image(&img, "");
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
			disp_image(&tmp,"");
			for (auto &y : result_of_match){

				y.second = 0;

			}
			for (auto x : mp){
			

				std::cout << "価格 "<<x.first << std::endl;
				disp_image(&tmp, "");
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
				disp_image(&tmp, "");
				//cv::waitKey();
				cv::matchTemplate(tmp, x.second, match_result_mat, 3);
				cv::minMaxLoc(match_result_mat, NULL, &maxVal, NULL, &max_pt);
				result_of_match[x.first] = max(maxVal, result_of_match[x.first]);
				for (auto y : result_of_match){
					std::cout << y.second << " ";

				}std::cout << std::endl;
				rot_90(tmp);
				disp_image(&tmp, "");
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

			std::cout << "こいつはマーカではなさそう..." << std::endl;
		
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
	speed_change(13);
	m_move_straight(160, 160, 250.7, 0, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_move_straight(160, 160, 250.7, 90, 180);
	getchar();
	m_grip();
	getchar();
	m_ungrip();
	m_home();
	m_move_straight(160, 220, 250.7, 0, 180);
	m_grip();
	getchar();
	m_ungrip();
	m_move_straight(160, 220, 250.7, 90, 180);
	m_grip();
	getchar();
	m_ungrip();
	m_home();
	m_move_straight(220, 160, 250.7, 0, 180);
	m_grip();
	getchar();
	m_ungrip();
	m_move_straight(220, 160, 250.7, 90, 180);
	m_grip();
	getchar();
	m_ungrip();
	m_home();
	m_move_straight(220, 220, 250.7, 0, 180);
	m_grip();
	getchar();
	m_ungrip();
	m_move_straight(220, 220, 250.7, 90, 180);
	m_grip();
	getchar();
	m_ungrip();
	m_home();
	m_move_straight(280, 160, 250.7, 0, 180);
	m_home();
	m_grip();
	getchar();
	m_move_straight(280, 160, 250.7, 90, 180);
	m_home();
	m_grip();
	getchar();
	m_ungrip();
	m_move_straight(280, 220, 250.7, 0, 180);
	m_home();
	m_grip();
	getchar();
	m_move_straight(280, 220, 250.7, 90, 180);
	m_home();
	m_grip();
	getchar();
	m_ungrip();


}

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
	thickness[5] = 1.5;
	thickness[10] = 1.5;
	thickness[50] = 1.7;
	thickness[100] = 1.7;
	thickness[500] = 1.8;

	for (int i = 1; i <= coin[1]; i++){
		m_home();
		m_move_straight(160, 160, 250+(thickness[1]*(5.5-(double)i)), 0, 180);
		grip();
		m_move_position_2(400, 0, 270, 0, 180);
		ungrip();
	}
	for (int i = 1; i <= coin[5]; i++){
		m_home();

		m_move_straight(160, 220, 250 + (thickness[5] * (5.5 - (double)i)), 0, 180);
		grip();
		m_move_position_2(400, 0, 270, 0, 180);
		ungrip();
	}
	for (int i = 1; i <= coin[10]; i++){
		m_home();
		m_move_straight(220, 160, 250 + (thickness[10] * (5.5 - (double)i)), 0, 180);
		grip();
		m_move_position_2(400, 0, 270, 0, 180);
		ungrip();
	}
	for (int i = 1; i <= coin[50]; i++){
		m_home();
		m_move_straight(220, 220, 250 + (thickness[50] * (5.5 - (double)i)), 0, 180);
		grip();
		m_move_position_2(400, 0, 270, 0, 180);
		ungrip();
	}
	for (int i = 1; i <= coin[100]; i++){
		m_home();
		m_move_straight(280, 160, 250 + (thickness[100] * (5.5 - (double)i)), 0, 180);
		grip();
		m_move_position_2(400, 0, 270, 0, 180);
		ungrip();

	}
	for (int i = 1; i <= coin[500]; i++){
		m_home();
		m_move_straight(280, 220, 250 + (thickness[500] * (5.5 - (double)i)), 0, 180);
		grip();
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
			cout << grid[i][j] << " ";
		}
		cout << endl;
	}if (is_marker(grid)){
		return ar_to_price(grid);
	}
	else{
		rot_90(im);
		for (int i = 0; i < 6; i++){
			for (int j = 0; j < 6; j++){

				rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
				out = (*img)(rect);
				grid[i][j] = average_of_Mat(&out);
				cout << grid[i][j] << " ";
			}
			cout << endl;
		}if (is_marker(grid)){
			return ar_to_price(grid);
		}
		else{
			rot_90(im);
			for (int i = 0; i < 6; i++){
				for (int j = 0; j < 6; j++){

					rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
					out = (*img)(rect);
					grid[i][j] = average_of_Mat(&out);
					cout << grid[i][j] << " ";
				}
				cout << endl;
			}if (is_marker(grid)){
				return ar_to_price(grid);
			}
			else{
				rot_90(im);
				for (int i = 0; i < 6; i++){
					for (int j = 0; j < 6; j++){

						rect = cv::Rect(j*img->cols / 6, i*img->rows / 6, img->cols / 6, img->rows / 6);
						out = (*img)(rect);
						grid[i][j] = average_of_Mat(&out);
						cout << grid[i][j] << " ";
					}
					cout << endl;
				}if (is_marker(grid)){
					return ar_to_price(grid);
				}
				
			}

		}
	}
	std::cout << "こいつはマーカではなさそう" << std::endl;
	return 0;
}
int find_marker_and_get_price(){

	int result = 0;
	cv::Mat img = cv::imread("marker/test3.bmp");
	//get_image_2(&img);
	cv::Mat image_source = img;
	disp_image(&img, "");
	cv::waitKey();


	to_gray(&img);
	cv::threshold(img, img, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	disp_image(&img, "");
	cv::waitKey();

	
	disp_image(&img, "");
	cv::waitKey();
	// 変換後の画像での座標
	const cv::Point2f dst_pt[] = {
		cv::Point2f(0, 0),
		cv::Point2f(505, 0),
		cv::Point2f(0, 505),
		cv::Point2f(505, 505)
	};
	cv::Rect roi(cv::Point(2, 2), cv::Size(502, 502));


	std::map<int, double> result_of_match;


	disp_image(&image_source, "");
	cv::waitKey();
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_L1);
	cv::Mat tmp;
	int max_level = 0;
	for (int i = 0; i < contours.size(); i++) {
		// ある程度の面積が有るものだけに絞る
		double a = contourArea(contours[i], false);
		if (a > 500) {
			//輪郭を直線近似する
			std::vector<cv::Point> approx;
			cv::approxPolyDP(cv::Mat(contours[i]), approx, 0.1 * cv::arcLength(contours[i], true), true);
			// 矩形のみ取得
			std::cout << approx.size() << std::endl;
			if (approx.size() == 4) {
				cv::Point2f src_pt[4];
				src_pt[0] = approx[0];
				src_pt[1] = approx[1];
				src_pt[2] = approx[3];
				src_pt[3] = approx[2];
				const cv::Mat homography_matrix = cv::getPerspectiveTransform(src_pt, dst_pt);
				cv::warpPerspective(image_source, tmp, homography_matrix, image_source.size());
				tmp = tmp(roi);
				disp_image(&tmp, "");
				cv::waitKey();
				int tmp_price = ar_read(&tmp);
				std::cout << tmp_price << std::endl;
				result += tmp_price;
			}
		}
	}

	return result;
}
int ar_to_price(vector<vector<double>> grid_double){
	/*
	□ □ □ □ □ □
	□ ■ □  1 ■ □
	□ □  2 □  4 □
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
bool is_marker(vector<vector<double>> grid){
	if (grid[1][1]<80 && grid[4][1]<80 && grid[1][4]<80 && grid[2][1]>180 && grid[2][3]>180 && grid[1][2]>180 && grid[4][4]>180){
		return true;
	}
	else{
		return false;
	}

}
double average_of_Mat(cv::Mat *img){
	double result = 0, img_size = img->rows*img->cols;
	for (int i = 0; i < img->rows; i++){
		for (int j = 0; j < img->cols; j++){
			result += double(gray(img, i, j))/img_size;
		}
	}
	return result;
}
