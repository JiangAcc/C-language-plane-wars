#define _CRT_SECURE_NO_WARNINGS 1
#pragma once
#include<stdio.h>
#include<easyx.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>
enum My
{
	//设置窗口大小
	WIDTH = 480,
	HEIGTH = 700,
	//子弹数量
	BULLELT_NUM = 30,
	ENEMY_NUM = 20,//敌机数量

	BIG,//大飞机
	SMALL//小飞机
};

//飞机、子弹结构体
struct Plane
{
	int x;
	int y;
	int frame;//当前帧
	bool live;//判断是否存活
	int width;//宽度
	int height;//高度
	int HP;//血量
	int type;//飞机类型
	bool destroy;
}plane, bull[BULLELT_NUM], enemy[ENEMY_NUM], enemybull[BULLELT_NUM];
//分数
int score = 0;//当前得分
int max_score=0;//最高分
//转换为字符串
char planeScore[10];
char planeHP[3];
char maxScore[10];

//加载地图
IMAGE img_Map;
IMAGE img_gameover[2];
//保存玩家图片
IMAGE img_Movre[2][2];
//保存子弹
IMAGE img_bullelt[2];
//保存敌机
IMAGE img_enemy[4][4];

void drawImg(int x, int y, IMAGE* src)
{
	// 变量初始化
	DWORD* pwin = GetImageBuffer();			//窗口缓冲区指针
	DWORD* psrc = GetImageBuffer(src);		//图片缓冲区指针
	int win_w = getwidth();				//窗口宽高
	int win_h = getheight();
	int src_w = src->getwidth();				//图片宽高
	int src_h = src->getheight();

	// 计算贴图的实际长宽
	int real_w = (x + src_w > win_w) ? win_w - x : src_w;			// 处理超出右边界
	int real_h = (y + src_h > win_h) ? win_h - y : src_h;			// 处理超出下边界
	if (x < 0) { psrc += -x;			real_w -= -x;	x = 0; }	// 处理超出左边界
	if (y < 0) { psrc += (src_w * -y);	real_h -= -y;	y = 0; }	// 处理超出上边界


	// 修正贴图起始位置
	pwin += (win_w * y + x);

	// 实现透明贴图
	for (int iy = 0; iy < real_h; iy++)
	{
		for (int ix = 0; ix < real_w; ix++)
		{
			byte a = (byte)(psrc[ix] >> 24);//计算透明通道的值[0,256) 0为完全透明 255为完全不透明
			if (a > 100)
			{
				pwin[ix] = psrc[ix];
			}
		}
		//换到下一行
		pwin += win_w;
		psrc += src_w;
	}
}

//初始化敌机血量
void EnemyHP(int i)
{
	int flag = rand() % 10;
	if (flag <= 2 && flag >= 0)
	{
		//大敌机
		enemy[i].type = BIG;
		enemy[i].HP = 4;
		enemy[i].width = 84;
		enemy[i].height = 128;
	}
	else
	{
		//小敌机
		enemy[i].type = SMALL;
		enemy[i].HP = 2;
		enemy[i].width = 57;
		enemy[i].height = 43;
	}
}

//定时器函数
bool Timer(int id, DWORD ms)
{
	static DWORD t[100];
	if (clock() - t[id] > ms)
	{
		t[id] = clock();
		return true;
	}
	return false;
}
//加载图片
void LoadImage()
{
	//加载地图图片
	loadimage(&img_Map, "background.png");

	loadimage(&img_gameover[0], "gameover.png");
	loadimage(&img_gameover[1], "gameover.png");

	//加载玩家图片
	loadimage(&img_Movre[0][0], "me1.png");
	loadimage(&img_Movre[0][1], "me2.png");
	//加载被击毁图片
	loadimage(&img_Movre[1][0], "me_destroy_1.png");
	loadimage(&img_Movre[1][1], "me_destroy_2.png");


	//加载子弹图片
	loadimage(&img_bullelt[0], "bullet2.png");
	loadimage(&img_bullelt[1], "bullet1.png");

	//加载敌机图片
	//小敌机
	loadimage(&img_enemy[0][0], "enemy1.png");
	loadimage(&img_enemy[0][1], "enemy2.png");
	//大敌机
	loadimage(&img_enemy[1][0], "enemy3_n2.png");
	loadimage(&img_enemy[1][1], "enemy3_n1.png");

	//小敌机被击毁
	loadimage(&img_enemy[2][0], "enemy1_down1.png");
	loadimage(&img_enemy[2][1], "enemy1_down2.png");

	//大敌机被击毁
	loadimage(&img_enemy[3][0], "enemy3_down3.png");
	loadimage(&img_enemy[3][1], "enemy3_down4.png");
}

//初始化游戏数据
void GameInit()
{
	//加载图片
	LoadImage();
	//玩家开局坐标
	plane.x = WIDTH / 2 - 40;
	plane.y = HEIGTH - 120;
	plane.HP = 5;//玩家血量
	plane.width = 60;
	plane.height = 74;
	//开局状态
	plane.live = true;//存活

	for (int i = 0; i < BULLELT_NUM; i++)
	{
		//初始化玩家子弹
		bull[i].x = 0;
		bull[i].y = 0;
		bull[i].live = false;
	}

	for (int i = 0; i < BULLELT_NUM; i++)
	{
		//初始化敌机子弹
		enemybull[i].x = 0;
		enemybull[i].y = 0;
		enemybull[i].live = false;
	}

	//初始化敌机
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		enemy[i].live = false;
		enemy[i].destroy = false;
		EnemyHP(i);
	}

}

//绘制图片
void GameDraw()
{
	//贴背景图
	putimage(0, 0, &img_Map);
	//绘制玩家
	if (plane.live)
	{
		//存活
		drawImg(plane.x, plane.y, &img_Movre[0][plane.frame]);
		plane.frame = (plane.frame + 1) % 2;
	}
	else
	{
		//被击毁
		drawImg(plane.x, plane.y, &img_Movre[1][1]);
	}
	//绘制子弹
	for (int i = 0; i < BULLELT_NUM; i++)
	{
		if (bull[i].live)
		{
			drawImg(bull[i].x, bull[i].y, &img_bullelt[0]);
		}
	}

	//绘制敌机子弹
	for (int i = 0; i < BULLELT_NUM; i++)
	{
		if (enemybull[i].live)
		{
			drawImg(enemybull[i].x, enemybull[i].y, &img_bullelt[0]);
		}
	}

	//绘制敌机
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].live)
		{
			if (enemy[i].type == BIG)
			{
				//绘制大飞机
				drawImg(enemy[i].x, enemy[i].y, &img_enemy[1][0]);
			}
			else
			{
				//绘制小飞机
				drawImg(enemy[i].x, enemy[i].y, &img_enemy[0][0]);
			}
		}
		//被击毁
		if (enemy[i].destroy)
		{
			if (enemy[i].type == SMALL)
			{
				//小敌机
				drawImg(enemy[i].x, enemy[i].y, &img_enemy[2][0]);
			}
			else
			{
				//大敌机
				drawImg(enemy[i].x, enemy[i].y, &img_enemy[3][0]);
			}
		}
	}

	//玩家阵亡
	if (!plane.live)
	{
		drawImg(getwidth()/5, getheight()/2, &img_gameover[0]);
	}
}

//玩家子弹发射位置
void CreateBullelt()
{
	for (int i = 0; i < BULLELT_NUM; i++)
	{
		if (!bull[i].live)
		{
			bull[i].x = plane.x + 27;
			bull[i].y = plane.y;
			bull[i].live = true;
			break;
		}
	}
}

//玩家子弹创建和移动
void Bullelt_Move(int speed)
{
	//创建子弹
	//玩家存在可创建
	if (plane.live)
	{
		if (GetAsyncKeyState(VK_SPACE) && Timer(3, 100))//空格
		{
			CreateBullelt();
		}
		for (int i = 0; i < BULLELT_NUM; i++)
		{
			if (bull[i].live)
			{
				bull[i].y -= speed;
				if (bull[i].y < 0)
				{
					bull[i].live = false;
				}
			}
		}
	}
}

//敌机子弹发射位置
void Create_Bullelt_ENEMY()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].live)
		{
			if (!enemybull[i].live)
			{
				enemybull[i].live = true;
				//小飞机
				if (enemy[i].type == SMALL)
				{
					enemybull[i].x = enemy[i].x + 25;
					enemybull[i].y = enemy[i].y + 43;
				}
				//大飞机发射位置
				else
				{
					enemybull[i].x = enemy[i].x + 42;
					enemybull[i].y = enemy[i].y + 120;
				}
			}
		}
	}
}

//敌机子弹移动
void Bullelt_Enemy(int speed)
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemybull[i].live)
		{
			enemybull[i].y += speed;
			//子弹飞出地图
			if (enemybull[i].y > HEIGTH)
			{
				enemybull[i].live = false;
			}
		}
	}
}

//获取键盘控制玩家移动
void PlayeMove(int speed)
{
	//使用windows函数获取键盘输入，十分流畅
	if (plane.live)
	{
		if (GetAsyncKeyState('W'))//上
		{
			//处理边界
			if (plane.y > 0)
				plane.y -= speed;
		}
		if (GetAsyncKeyState('S'))//下
		{
			//减去飞机的长度
			if (plane.y < HEIGTH - 100)
				plane.y += speed;
		}
		if (GetAsyncKeyState('A'))//左
		{
			if (plane.x + 50 > 0)
				plane.x -= speed;
		}
		if (GetAsyncKeyState('D'))//右
		{
			if (plane.x < WIDTH - 30)
				plane.x += speed;
		}
	}
}

//敌机移动
void Enemy_Mocve(int speed)
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		//敌机存活
		if (enemy[i].live)
		{
			//出地图重置
			enemy[i].y += speed;

			if (enemy[i].y > HEIGTH)
			{
				enemy[i].live = false;
			}
		}
	}
}

//生成敌机
void CreateEnemy()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (!enemy[i].live)
		{
			enemy[i].live = true;
			enemy[i].destroy = false;
			//产生的坐标
			enemy[i].x = rand() % (WIDTH - (enemy[i].width / 2));
			enemy[i].y = -40;
			EnemyHP(i);
			break;
		}
	}
}

//玩家射击飞机
void playPlance()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		//敌机不存在什么也不做
		if (!enemy[i].live)
			continue;
		for (int j = 0; j < BULLELT_NUM; j++)
		{
			//子弹不存在什么也不做
			if (!bull[j].live)
				continue;
			//子弹命中
			if ((bull[j].x > enemy[i].x) && (bull[j].x < enemy[i].x + enemy[i].width)
				&& (bull[j].y > enemy[i].y) && (bull[j].y < enemy[i].y + enemy[i].height))
			{
				//子弹消失
				bull[j].live = false;
				//掉血
				enemy[i].HP--;
			}
		}
		if (enemy[i].HP <= 0)
		{
			enemy[i].live = false;
			enemy[i].destroy = true;
			//计分
			if (enemy[i].type == BIG)
				score += 5;
			else
				score += 2;
		}
	}
}
void change(int score, char str[]);
int record();
//敌机子弹命中玩家
void playEnmey()
{
	//玩家存在
	if (plane.live)
	{
		for (int i = 0; i < BULLELT_NUM; i++)
		{
			//子弹存在
			if (enemybull[i].live)
			{
				//子弹命中玩家
				if ((enemybull[i].x > plane.x) && (enemybull[i].x < plane.x + plane.width)
					&& (enemybull[i].y > plane.y) && (enemybull[i].y < plane.y + plane.height))
				{
					enemybull[i].live = false;
					plane.HP--;
				}
			}
		}
		//玩家被击毁
		if (plane.HP <= 0)
		{
			plane.live = false;
		}
		//////////////////////////////////////////////////////////////////////////////////////
	}
	else
	{
		GameDraw();
	}
}

//敌机玩家碰撞
void collision()
{
	//玩家存活
	if (plane.live)
	{
		for (int i = 0; i < ENEMY_NUM; i++)
		{
			//敌机也存活
			if (enemy[i].live)
			{
				//敌机玩家碰撞
				if ((enemy[i].x > plane.x) && (enemy[i].x < plane.x + plane.width)
					&& (enemy[i].y > plane.y) && (enemy[i].y < plane.y + plane.height)
					|| (plane.x > enemy[i].x + 10) && (plane.x < enemy[i].x + enemy[i].width - 10)
					&& (plane.y > enemy[i].y) && (plane.y < enemy[i].y + enemy[i].height))
				{
					//撞小飞机
					if (enemy[i].type == SMALL)
					{
						enemy[i].destroy = true;
						enemy[i].live = false;
						plane.HP -= 3;
					}
					else
					{
						//撞大飞机
						enemy[i].destroy = true;
						enemy[i].live = false;
						plane.HP -= 10;
					}
					if (plane.HP <= 0)
					{
						plane.live = false;
						////////////////////////////////////////////////
					}
				}
			}
		}
	}
}

//整数转换为字符串
void change(int score,char str[]) {
	int cnt = 0;
	int x = score;
	while (x)
	{
		str[cnt++] = x % 10 + '0';
		x = x / 10;
	}
	for (int i = 0; i < cnt/2; i++)
	{
		char op = str[i];
		str[i] = str[cnt - 1 - i];
		str[cnt - 1 - i] = op;
	}
	str[cnt] = '\0';
}

//文件操作-记录最高分
int record()
{
	FILE* f;
	//若没有文件，先创建一个（针对第一次）
	if ((f = fopen("D:\\飞机大战score.txt", "r")) == NULL)
	{
		//创建
		f = fopen("D:\\飞机大战score.txt", "w");
		//写入分数
		fwrite(&score, sizeof(int), 1, f);
		fclose(f);
		return score;
	}
	else
	{
		fclose(f);
		//读入之间的分数
		f = fopen("D:\\飞机大战score.txt", "r");
		int sc;
		fread(&sc, sizeof(int), 1, f);//读取最高分
		fclose(f);
		//与最高分比较
		if (score > sc)
		{
			sc = score;
			f = fopen("D:\\飞机大战score.txt", "w");
			fwrite(&score, sizeof(int), 1, f);//写入最高分
			fclose(f);
		}
		fclose(f);
		return sc;
	}
}

int main()
{
	srand((unsigned int)time(NULL));
	//创建窗口大小
	initgraph(WIDTH, HEIGTH);
	GameInit();
	GameDraw();

	//双缓冲绘图
	BeginBatchDraw();
	max_score = record();
	while (true)
		{
			GameDraw();
			//将整形转换为字符型
			outtextxy(0, 0, "得分：");
			change(score, planeScore);
			outtextxy(40, 0, planeScore);
			outtextxy(0, 30, "HP：");
			change(plane.HP, planeHP);
			outtextxy(35, 30, planeHP);
			outtextxy(0, 60, "最高分：");
			record();
			change(max_score, maxScore);
			outtextxy(50, 60, maxScore);

			//刷新
			FlushBatchDraw();
			//玩家移动
			if (Timer(0, 2))
				PlayeMove(1);
			//子弹移动
			Bullelt_Move(1);
			//敌机生成
			if (Timer(1, 1500))
				CreateEnemy();

			//敌机移动
			if (Timer(2, 50))
				Enemy_Mocve(1);
			//敌人发射子弹
			if (Timer(4, 500))
				Create_Bullelt_ENEMY();
			if (Timer(5, 50))
				Bullelt_Enemy(12);

			//子弹是否命中飞机
			playPlance();
			playEnmey();
			//飞机碰撞
			collision();
		}
	EndBatchDraw();
	return 0;
}