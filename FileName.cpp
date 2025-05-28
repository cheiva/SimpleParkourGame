
#include "FileName.h"
using namespace std;

#define WIN_WIDTH 1012
#define WIN_HEIGHT 396
#define OBSTACLE_COUNT 10
int initspeed=1;
int herospeed = 1;
int win_score = 10;
int alresdyUP = 0;
int noEND = 0;
IMAGE imgBgs[3];
int bgX[3];//设置背景X坐标
int bgSpeed[3] = { 1,2,4 };
void initSpeed() {
	bgSpeed[0] = 1;
	bgSpeed[1] = 2;
	bgSpeed[2] = 4;
	herospeed = 1;
	herospeed += initspeed;
	bgSpeed[2] *= initspeed;
	bgSpeed[0] = bgSpeed[2] / 4;
	bgSpeed[1] = bgSpeed[2] / 2;
}
IMAGE imgHeros[12];
int heroX;
int heroY;
int heroIndex;



typedef enum
{
	TORTOISE,//0
	LION,
	HEART,
	HOOK1,
	HOOK2,
	HOOK3,
	HOOK4,
	OBSTACLE_TYPE_COUNT//取边界作用

}obstacle_type;

typedef enum
{
	START,
	DIFFICUILT,
	RANK,
	HLEP,
	SET,
	CONTINUE,
	MODE
}staus;
vector<vector<IMAGE>>obstacleImgs;//储存障碍物图片的数组
typedef struct obstacle
{
	obstacle_type type;//障碍物类型
	int imgIndex;//当前时刻显示图片的序号
	int x, y;//障碍物坐标
	int power;
	int speed;
	bool exist;
	bool hited;//判断本次是否已经发生了碰撞，防止多次计算一个障碍物
	bool passed;
}obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];

int lastObsIndex;

IMAGE imgHeroDown[2];//有两张图片下蹲
bool heroDown;//判断玩家是否处于下蹲状态

IMAGE imgSZ[10];

//游戏的初始化
void FileName::init()
{
	//创建游戏窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	color1 = color2 = color3 = color4 = RGB(247, 255, 7);
	//加载背景资源
	char name[64];
	for (int i = 0; i < 3; i++)
	{
		sprintf(name, "res/bg%03d.png", i + 1);//"  "里是插入三重图片的路径
		loadimage(&imgBgs[i], name);
		bgX[i] = 0;
	}
	//加载Hero奔跑的图片帧素材
	for (int i = 0; i < 12; i++)
	{
		sprintf(name, "res/hero%d.png", i + 1);
		loadimage(&imgHeros[i], name);
	}
	//设置玩家初始位置
	heroX = WIN_WIDTH * 0.5 - imgHeros[0].getwidth() * 0.5;
	heroY = 345 - imgHeros[0].getheight();
	heroIndex = 0;

	heroJump = false;
	jumpHeightMax = 345 - imgHeros[0].getheight() - 120;
	heroJumpOff = -4;

	update = true;


	//加载小乌龟

	IMAGE imgTort;
	loadimage(&imgTort, "res/t1.png");
	vector<IMAGE>imgTortArray;
	imgTortArray.push_back(imgTort);
	obstacleImgs.push_back(imgTortArray);


	//加载狮子
	IMAGE imgLion;
	vector<IMAGE>imgLionArray;
	for (int i = 0; i < 6; i++)
	{
		sprintf(name, "res/p%d.png", i + 1);
		loadimage(&imgLion, name);
		imgLionArray.push_back(imgLion);
	}
	obstacleImgs.push_back(imgLionArray);


	//加载爱心
	IMAGE imgHeart;
	loadimage(&imgHeart, "res/t2.png");
	vector<IMAGE>imgHeartArray;
	imgHeartArray.push_back(imgHeart);
	obstacleImgs.push_back(imgHeartArray);



	//初始化障碍物池子
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		obstacles[i].exist = false;
	}


	//加载下蹲素材
	loadimage(&imgHeroDown[0], "res/d1.png");
	loadimage(&imgHeroDown[1], "res/d2.png");
	heroDown = false;//初始化玩家下蹲

	//障碍物加载
	IMAGE imgH;
	for (int i = 0; i < 4; i++)
	{
		vector<IMAGE>imgHookArray;//循环内可以减少数据占用费
		sprintf(name, "res/h%d.png", i + 1);
		loadimage(&imgH, name, 63, 260, true);
		imgHookArray.push_back(imgH);
		obstacleImgs.push_back(imgHookArray);
	}
	heroBlood = 100;//满血
	//预加载音效
	Tool::preLoadSound("res/hit.mp3");//普通碰撞音效
	Tool::preLoadSound("res/getHeart.mp3");//回血音效

	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);//BGM
	lastObsIndex = -1;
	score = 0;

	for (int i = 0; i < 10; i++) {
		sprintf(name, "res/sz/%d.png", i);
		loadimage(&imgSZ[i], name);
	}
}
void  FileName::creatObstacle()//创建障碍物函数定义
{
	int i;
	for (i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist == false)
		{
			break;
		}
	}

	if (i >= OBSTACLE_COUNT)
	{
		return;
	}
	obstacles[i].exist = true;
	obstacles[i].hited = false;//表示本次的碰撞状态已经被计算了
	obstacles[i].imgIndex = 0;
	obstacles[i].type = (obstacle_type)(rand() % 4);
	if (lastObsIndex >= 0 &&
		obstacles[lastObsIndex].type >= HOOK1 &&
		obstacles[lastObsIndex].type <= HOOK4 &&
		obstacles[i].type == LION &&
		obstacles[lastObsIndex].x > (WIN_WIDTH - 500)
		) {
		obstacles[i].type = TORTOISE;
	}
	lastObsIndex = i;
	if (obstacles[i].type == HOOK1)
	{
		obstacles[i].type = (obstacle_type)((int)(obstacles[i].type) + rand() % 4);//如果保持障碍物类型为枚举，再分配需要的强转int再强转枚举
		//obstacles[i].type += rand() % 4;//直接改掉了原来的数据类型从而直接计算
	}
	//obstacles[i].type=(obstacle_type)(rand()%OBSTACLE_TYPE_COUNT);
	obstacles[i].x = WIN_WIDTH;
	obstacles[i].y = 345 + 5 - obstacleImgs[obstacles[i].type][0].getheight();
	if (obstacles[i].type == HEART){
		obstacles[i].speed = 0;
		obstacles[i].power = -15;
	}
	if (obstacles[i].type == TORTOISE)
	{
		obstacles[i].speed = 0;
		obstacles[i].power = 5;
	}
	else if (obstacles[i].type == LION)
	{
		obstacles[i].speed = 4;
		obstacles[i].power = 20;
	}
	//柱子障碍物的判定-四个柱子一致，一起判定了
	else if (obstacles[i].type >= HOOK1 && obstacles[i].type <= HOOK4)
	{
		obstacles[i].speed = 0;
		obstacles[i].power = 20;
		obstacles[i].y = 0;
	}
	obstacles[i].passed = false;
}
void FileName::checkHit() {
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist && obstacles[i].hited == false) {//碰撞物存在且没有计算过

			int a1x, a1y, a2x, a2y;//左上的xy和右下角的xy
			int off = 30;
			if (!heroDown) {//非下蹲（奔跑、跳跃）
				a1x = heroX + off;
				a1y = heroY + off;
				a2x = heroX + imgHeros[heroIndex].getwidth() - off;
				a2y = heroY + imgHeros[heroIndex].getheight();
			}
			else {
				a1x = heroX + off;
				a1y = 345 - imgHeroDown[heroIndex].getheight();
				a2x = heroX + imgHeroDown[heroIndex].getwidth() - off;
				a2y = 345;
			}
			IMAGE img = obstacleImgs[obstacles[i].type][obstacles[i].imgIndex];//当前障碍物，单独定义了指针，提高代码的可读性。
			int b1x = obstacles[i].x + off;
			int b1y = obstacles[i].y + off;
			int b2x = obstacles[i].x + img.getwidth() - off;
			int b2y = obstacles[i].y + img.getheight() - 10;//怕小乌龟碰撞取30就几乎没有体积了
			if (Tool::rectIntersect(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y))
			{
				if (obstacles[i].type == HEART &&( heroBlood == 100||heroBlood-obstacles[i].power>=100)  )
					heroBlood = 100;
				else
				heroBlood -= obstacles[i].power;//扣血
				printf("血量剩余%d\n", heroBlood);
				if (obstacles[i].type != HEART)
				{
					Tool::playSound("res/hit.mp3");
				}
				else {
					Tool::playSound("res/getHeart.mp3");
				}
				obstacles[i].hited = true;
			}		

		}
		if (obstacles[i].hited && obstacles[i].type == HEART && obstacles[i].exist)
			obstacles[i].exist = false;

	}


};
void FileName::fly()
{
	for (int i = 0; i < 3; i++)
	{
		bgX[i] -= bgSpeed[i];
		if (bgX[i] < -WIN_WIDTH)
		{
			bgX[i] = 0;
		}
	}
	//实现跳跃
	if (heroJump == true)
	{
		if (heroY < jumpHeightMax)
		{
			heroJumpOff = 4+herospeed;
		}
		heroY += heroJumpOff;
		if (heroY > 345 - imgHeros[0].getheight())
		{
			heroJump = false;
			heroJumpOff = -4-herospeed;
		}
	}
	else if (heroDown == true)
	{
		static int count = 0;
		int delays[2] = { 10-2*initspeed,32-2*initspeed};//第一张下蹲的帧数停止为4，第二张为10
		count++;//解决时间过短问题
		if (count >= delays[heroIndex])
		{
			count = 0;
			heroIndex++;
			if (heroIndex >= 2)
			{
				heroIndex = 0;
				heroDown = false;//只有两张图片，可以增加图片
			}
		}


	}
	else
	{
		heroIndex = (heroIndex + 1) % 12;
	}


	//创建障碍物
	static int frameCount = 0;
	static int enemyFre = 50;
	frameCount++;
	if (frameCount > enemyFre)
	{
		frameCount = 0;

		enemyFre = (55 - 5 * initspeed) + rand() % 50;
		creatObstacle();
	}
	//更新障碍物的坐标
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist)
		{

			obstacles[i].x -= obstacles[i].speed + bgSpeed[2];
			if (obstacles[i].x < -obstacleImgs[obstacles[i].type][0].getwidth() * 2)
			{
				obstacles[i].exist = false;
			}
			int len = obstacleImgs[obstacles[i].type].size();
			obstacles[i].imgIndex = (obstacles[i].imgIndex + 1) % len;

		}
	}
	//障碍物碰撞检测
	checkHit();
}


//渲染游戏背景
void FileName::updateBg()
{
	Tool::putimagePNG2(bgX[0], 0, &imgBgs[0]);
	Tool::putimagePNG2(bgX[1], 119, &imgBgs[1]);
	Tool::putimagePNG2(bgX[2], 330, &imgBgs[2]);

}

void FileName::jump()
{
	heroJump = true;
	update = true;
}
void FileName::down()
{
	heroDown = true;
	update = true;
	heroIndex = 0;
}
//处理用户的按键输入
void FileName::keyEvent()
{
	ExMessage msg;
	char ch;
	if (_kbhit())//如果有按键输入，函数返回true
	{
		ch = _getch();//不需要按下回车即可直接读取
		if (ch == ' ')
		{
			jump();
			//heroJump=true;
		}
		else if (ch == 'a')
		{
			down();
		}
		else if (ch ==27)
		{
			staus=CONTINUE;
		}
	}
}

//渲染小乌龟
void FileName::updateEnemy()
{

	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist)
		{
			Tool::putimagePNG2(obstacles[i].x, obstacles[i].y, WIN_WIDTH,
				&obstacleImgs[obstacles[i].type][obstacles[i].imgIndex]);//第一个数是第几行第几种障碍物，第二个数是第几张图片
		}
	}

}
void FileName::updateHero()//对玩家状态进行更新
{
	if (!heroDown)
	{
		Tool::putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);//在非下蹲状态下进行更新
	}
	else
	{
		int y = 345 - imgHeroDown[heroIndex].getheight();
		Tool::putimagePNG2(heroX, y, &imgHeroDown[heroIndex]);
	}

}
void FileName::updateBloodBar() {//画血条,用了一个封装接口
	Tool::drawBloodBar(10, 10, 200, 10, 2, BLUE, DARKGRAY, RED, heroBlood / 100.0);
}

void FileName::checkOver() {//结束界面
	if (heroBlood <= 0) {
		loadimage(0, "res/over.png");
		//结束本局
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");
		//开始下一局
		rank.push_back(score);
		WriteRank();
		/*if (initspeed < 3)
		{
			initspeed++;
		}*/
		staus = SET;
		initspeed = 1;
		initSpeed();
		heroBlood = 100;
		score = 0;
		mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	}
}

void FileName::checkScore() {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist &&
			obstacles[i].passed == false
			&& obstacles[i].hited == false &&
			obstacles[i].x + obstacleImgs[obstacles[i].type][0].getwidth() < heroX) {
			switch (obstacles[i].type)
			{
			case LION: score += 2;
				break;
			case HOOK1:case HOOK2:case HOOK3:case HOOK4:
				score += 3;
				break;
			case TORTOISE: score++;
				break;
			}
			obstacles[i].passed = true;
			printf("score:%d\n", score);
		}
	}

}
void FileName::updateScore() {
	char str[8];
	sprintf(str, "%d", score);
	int x = 20;
	int y = 25;
	for (int i = 0; str[i]; i++) {
		int sz = str[i] - '0';
		Tool::putimagePNG(x, y, &imgSZ[sz]);
		x += imgSZ[sz].getwidth() + 5;
		IMAGE imgESC;
		loadimage(&imgESC, "res/esc.png");
		putimagePNG(0, 0, &imgESC);
	}
}
void FileName::speedUP() {
	if (score - alresdyUP >= 10) {
		initspeed++;
		initSpeed();
		alresdyUP += 10;
	}		if (endless == 1) {
			if (score >= 10 && score % 10 == 0) {
				initspeed++;
				initSpeed();
			}

		}
}
void FileName::checkWin() {//胜利判定
	if (score >= win_score) {
		rank.push_back(score);
		WriteRank();
		FlushBatchDraw();
		mciSendString("play res/win.mp3", 0, 0, 0);
		Sleep(2000);
		loadimage(0, "res/win.jpg");
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");
		initspeed++;
		initSpeed();
		heroBlood = 100;
		score = 0;
		char ch1;
			ch1 = _getch();//不需要按下回车即可直接读取
				if (ch1 == 'z')
				{
					staus = SET;
				}
		mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	} 
}
void FileName::clickBtn(ExMessage*msg)//按钮界面
{
	peekmessage(msg, EX_MOUSE);
	if (msg->x >= 100 && msg->x <= 150 && msg->y >= 110 && msg->y <= 160)   //是否进去按钮可点击范围
	{
		color1 = RGB(250, 234, 211);           //如果是则改变
		if (msg->message == WM_LBUTTONDOWN)  //是否按键按下  同下
		{
			staus = MODE;
		}
	}
	else        //否这变回来
	{
		color1 = RGB(234, 208, 209);
	}
	if (msg->x >= 100 && msg->x <= 150 && msg->y >= 180 && msg->y <= 230)
	{
		color2 = RGB(250, 234, 211);
		if (msg->message == WM_LBUTTONDOWN)
		{
			staus = RANK;
		}
	}
	else
	{
		color2 = RGB(234, 208, 209);
	}
	if (msg->x >= 100 && msg->x <= 150 && msg->y >= 250 && msg->y <= 300)
	{
		color3 = RGB(250, 234, 211);
		if (msg->message == WM_LBUTTONDOWN)
		{
			staus = HLEP;
		
		}
	}
	else
	{
		color3 = RGB(234, 208, 209);
	}
}

void FileName::difficuilt(ExMessage* msg)
{
	Tool::Button(50, 110, 100, 50, "简单模式", color1);
	Tool::Button(50, 180, 100, 50, "普通模式", color2);
	Tool::Button(50, 250, 100, 50, "困难模式", color3);
	Tool::Button(50, 320, 100, 50, "随机模式", color4);
	peekmessage(msg, EX_MOUSE|EX_KEY);
	if (msg->vkcode == VK_ESCAPE)  //点击esc退出help界面
	{
		staus = MODE;
		return;
	}
	if (msg->x >= 50 && msg->x <= 150 && msg->y >= 110 && msg->y <= 160)   //是否进去按钮可点击范围
	{ 
		color1 = RGB(250, 234, 211);           //如果是则改变
		if (msg->message == WM_LBUTTONDOWN)  //是否按键按下  同下
		{
			staus = START;
			initspeed = 1;
			initSpeed();

		}
	}
	else        //否这变回来
	{
		color1 = RGB(146, 172, 209);
	}
	if (msg->x >= 50 && msg->x <= 150 && msg->y >= 180 && msg->y <= 230)
	{
		color2 = RGB(250, 234, 211);
		if (msg->message == WM_LBUTTONDOWN)
		{
			staus = START;
			initspeed = 2;
			initSpeed();

		}
	}
	else
	{
		color2 = RGB(146, 172, 209);
	}
	if (msg->x >= 50 && msg->x <= 150 && msg->y >= 250 && msg->y <= 300)
	{
		color3 = RGB(250, 234, 211);
		if (msg->message == WM_LBUTTONDOWN)
		{
			staus = START;
			initspeed = 3;
			initSpeed();

		}
	}
	else
	{
		color3 = RGB(146, 172, 209);
	}
	if (msg->x >= 50 && msg->x <= 150 && msg->y >= 320 && msg->y <= 370)
	{
		color4 = RGB(250, 234, 211);
		if (msg->message == WM_LBUTTONDOWN)
		{
			staus = START;
			random_device rd;

			mt19937 gen(rd());

			// 定义随机数分布（1到8之间的整数）  
			uniform_int_distribution<> dis(1, 8);
			initspeed = dis(gen);
			initSpeed();
		}
	}
	else
	{
		color4 = RGB(234, 208, 209);
	}
}
void FileName::Continue(ExMessage* msg)
{
	Tool::Button(100, 110, 100, 50, "继续游戏", color1);
	Tool::Button(100, 180, 100, 50, "回到菜单", color2);
	peekmessage(msg, EX_MOUSE | EX_KEY);
	if (msg->x >= 100 && msg->x <= 250 && msg->y >= 110 && msg->y <= 160)   //是否进去按钮可点击范围
	{
		color1 = RGB(250, 234, 211);           //如果是则改变
		if (msg->message == WM_LBUTTONDOWN)  //是否按键按下  同下
		{
			staus = START;
		}
	}
	else        //否这变回来
	{
		color1 = RGB(146, 172, 209);
	}
	if (msg->x >= 100 && msg->x <= 250 && msg->y >= 180 && msg->y <= 230)
	{
		color2 = RGB(250, 234, 211);
		if (msg->message == WM_LBUTTONDOWN)
		{
			heroBlood = 100;
			score = 0;
			staus = SET;
		}
	}
	else
	{
		color2 = RGB(146, 172, 209);
	}
}
void FileName::Mode(ExMessage* msg) {
	Tool::Button(150, 110, 100, 50, "无尽模式", color1);
	Tool::Button(150, 180, 100, 50, "挑战模式", color2);
	peekmessage(msg, EX_MOUSE | EX_KEY);
	if (msg->vkcode == VK_ESCAPE)  //点击esc退出help界面
	{
		staus = SET;
		return;
	}
	if (msg->x >= 150 && msg->x <=250 && msg->y >= 110 && msg->y <= 160)   //是否进去按钮可点击范围
	{
		color1 = RGB(250, 234, 211);           //如果是则改变
		if (msg->message == WM_LBUTTONDOWN)  //是否按键按下  同下
		{
			win_score = 99999999;
			noEND = 1;
			alresdyUP = 0;
			staus = DIFFICUILT;
		}
	}
	else        //否这变回来
	{
		color1 = RGB(146, 172, 209);
	}
	if (msg->x >= 150 && msg->x <= 250 && msg->y >= 180 && msg->y <= 230)
	{
		color2 = RGB(250, 234, 211);
		if (msg->message == WM_LBUTTONDOWN)
		{
			cout << "请输入想要达到的分数\n";
			cin >> win_score;
			staus = DIFFICUILT;
		}
	}
	else
	{
		color2 = RGB(146, 172, 209);
	}
}
void FileName::ReadRank()
{
	fstream fs;

	fs.open("rank.txt", ios::in); //文本流输入函数
	int num = 0;
	int size = 0;
	while (fs >> num)
	{
		rank.push_back(num);
		size++;
		if (size >= 14)
		{
			break;
		}
	}
	fs.close();
}
void FileName::WriteRank()
{
	fstream fs;
	fs.open("rank.txt", ios::out);       //文本流输出函数
	sort(rank.rbegin(), rank.rend());
	int size = 0;
	for (auto v : rank)
	{
		fs << v << endl;
		size++;
		if (size == 14)
		{
			break;
		}
	}
	fs.close();
}
void FileName::SelectRank(ExMessage* msg)     //查询排行
{
	peekmessage(msg, EX_MOUSE | EX_KEY);     //点击esc退出帮组界面
	if (msg->vkcode == VK_ESCAPE)
	{
		staus = SET;
		return;
	}
	int x = 0;
	char buf[20] = { 0 };
	sort(rank.rbegin(), rank.rend());
	for (int i = 0; i < rank.size(); i++)  
	{
		setfont(22, 0, "华康标题宋W9(P)");
		sprintf(buf, "第%d名:%d",i+1, rank.at(i));
			setfillcolor(RGB(250, 234, 211));
			fillrectangle(x, i * 50, x + 100, i * 50 + 50);
			setcolor(RGB(0,0,139));
			rectangle(x, i * 50, x + 100, i * 50 + 50);
			setcolor(RGB(134,150,167));
			setbkmode(TRANSPARENT);
			outtextxy(x + (100 - textwidth(buf)) / 2, i * 50 + (50 - textheight(buf)) / 2, buf);
			
	}
}
void FileName::help(ExMessage* msg)    //帮助文档
{
	peekmessage(msg, EX_MOUSE | EX_KEY);
	if (msg->vkcode == VK_ESCAPE)   //点击esc退出help界面
	{
		staus = SET; 
		return;
	}
	IMAGE imgHelp ;
	loadimage(&imgHelp, "res/ui-help.png");
	putimagePNG(0,0,&imgHelp);
}
void FileName::Set()
{
	setfont(22, 0, "华康标题宋W9(P)");
	Tool::Button(100, 110, 100, 50, "模式选择", color1);
	Tool::Button(100, 180, 100, 50, "游戏排行", color2);
	Tool::Button(100, 250, 100, 50, "游戏介绍", color3);
}
void FileName::run()
{
	
	//刷新初始画面
	init();
	ReadRank();
	ExMessage msg;
	BeginBatchDraw();
	
	while (1)
	{

		if (staus != START && staus != CONTINUE)
		{

			cleardevice();
			loadimage(0, "res/start.png");
			switch (staus)
			{
			case 1:
				difficuilt(&msg);
				break;
			case 2:
				SelectRank(&msg);
				break;
			case 3:
				help(&msg);
				break;
			case 4:
				Set();
				clickBtn(&msg);
				break;
			case 5:
				Continue(&msg);
				break;
			case 6: 
				Mode(&msg);
				break;
			}
		}
		if (staus == CONTINUE)
		{
			cleardevice();
			//background
			Tool::putimagePNG2(bgX[0], 0, &imgBgs[0]);
			Tool::putimagePNG2(bgX[1], 119, &imgBgs[1]);
			Tool::putimagePNG2(bgX[2], 330, &imgBgs[2]);
			//Enemy
			for (int i = 0; i < OBSTACLE_COUNT; i++)
			{
				if (obstacles[i].exist)
				{
					Tool::putimagePNG2(obstacles[i].x, obstacles[i].y, WIN_WIDTH,
						&obstacleImgs[obstacles[i].type][obstacles[i].imgIndex]);
				}
			}
			//hero
			if (!heroDown)
			{
				Tool::putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);//在非下蹲状态下进行更新
			}
			else
			{
				int y = 345 - imgHeroDown[heroIndex].getheight();
				Tool::putimagePNG2(heroX, y, &imgHeroDown[heroIndex]);
			}
			//blood
			Tool::drawBloodBar(10, 10, 200, 10, 2, BLUE, DARKGRAY, RED, heroBlood / 100.0);
			//score
			char str[8];
			sprintf(str, "%d", score);
			int x = 20;
			int y = 25;
			for (int i = 0; str[i]; i++) {
				int sz = str[i] - '0';
				Tool::putimagePNG(x, y, &imgSZ[sz]);
				x += imgSZ[sz].getwidth() + 5;
			}

			IMAGE imgStop;
			loadimage(&imgStop, "res/ui-stop.png");
			putimagePNG(0, 0, &imgStop);
			Continue(&msg);
			

		}
		if (staus == START)
		{
			keyEvent();
			
			timer += Tool::getDelay();

			if (timer > 30)
			{
				timer = 0;
				update = true;
			}
			if (update)
			{
				
			
					update = false;
					updateBg();
					updateHero();
					updateEnemy();
					updateBloodBar();
					updateScore();
					checkWin();
					if (noEND == 1) {
						speedUP();
					}
					checkOver();//检查结束
					checkScore();
					fly();
				
			}
		}
		FlushBatchDraw();
	}
	
	EndBatchDraw();
	while (1)
	{

		
	}
	system("pause");

}

FileName::FileName()
{
}

FileName::~FileName()
{
}
