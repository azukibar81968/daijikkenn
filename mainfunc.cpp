
  
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
#include <map>
#include <set>
#include "img.h"
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
void m_position_define(int n, double x, double y, double z, double a, double b, int g);//指定した姿勢をポジションリストに登録する関数
void m_ms(int n);//指定したポジション番号へ直線移動
int is_grip();//ハンドの状態を返す関数
void m_move_straight(double x, double y, double z, double a, double b);//指定した座標に直線移動する関数
void m_move_circle2(double cx, double cy, double z, double r);//中心(cx,cy,z) 半径r の円を描くように移動する

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

void warpPers(BYTE *img);

/*連携部分*/
void exchange_ctor(double cx, double cy, double* rx, double* ry);//画像座標系からロボット座標系に変換
void do_zeroin();//実行すると座標の調整を行う
void eject_block();//実行するとブロックの取り除き動作を行う


/*お釣り計算*/
std::map<long long int, long long int> accounting(long long int received, long long int total_price);

/**************************************************/



/***************************************************
mainfunc: メイン処理（実際の処理を記述）
***************************************************/
void mainfunc(HDC *hDC) {

	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 960);
	

	std::map<long long int, long long int>mp;
	mp = accounting(4000, 2800);
	for (auto x : mp){
		std::cout << x.first << " " << x.second << std::endl;
	}

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
	return;
}

/*--------------------------------------------------
処理:	指定した姿勢をポジションリストに登録する関数
戻り値: void
引数:   ポジションリスト登録先 <n> 位置<x>,<y>,<z>, 姿勢 <a>,<b> グリップ<g>（g =1:open  g=0:close）
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

	if (g == 1) command += "O";
	else if (g == 0)command += "C";
	else {
		printf("error: 未定義のグリップ状態");
		
	}

	printf((char*)command.c_str());
	printf("\n");
	rsputs((char*)command.c_str());

	return;
}

/*--------------------------------------------------
処理:	指定したポジション番号へ直線移動
戻り値: void
引数:   ポジションリスト参照先 <n>
--------------------------------------------------*/
void m_ms(int n){
	std::string command("MO ");
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

	if (cpos[2] == (const char)"O") return 0;
	else return 1;
}

/*--------------------------------------------------
処理:	指定した座標に直線移動する関数
戻り値: void
引数:   位置<x>,<y>,<z>, 姿勢 <a>,<b>
--------------------------------------------------*/
void m_move_straight(double x, double y, double z, double a, double b){
	int grp_flag = is_grip();

	m_position_define(1, x, y, z, a, b, grp_flag);
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
	int x_sum = 0;
	int y_sum = 0;
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
	*gx = x_sum / size_count;
	*gy = y_sum / size_count;

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
	
	*rx = 0.4044*cy + 161.89;
	*ry=0.34665*cx - 222.32;

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
	/*定義*/
	cv::Mat img_gray;//グレー化画像
	to_gray(&img, &img_gray);//グレー化

	cv::Mat img_labelled;//ラベル済み画像

	cv::Mat img_hsv;//hsv変換画像
	cv::cvtColor(img, img_hsv, CV_BGR2HSV);

	BOOL hole_flg = FALSE;
	BOOL sat_flg = FALSE;
	BOOL big_flg = FALSE;
	BOOL sml_flg = FALSE;
	/*画像下処理*/
	binarize(&img_gray, LABEL_THD);//二値化
	erosion(&img_gray, 1);//ノイズ除去
	dilation(&img_gray, 2);//ノイズ除去
	erosion(&img_gray, 1);//ノイズ除去
	int c_num = labeling(&img_gray, &img_labelled);//ラベリング

	/*確認用，もう使わないかな*/
	//disp_labeled_image(&img_labelled, "");
	//wait_while_kbhit();



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
		cv::Point2f(130, 0),
		cv::Point2f(1190, 0),
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