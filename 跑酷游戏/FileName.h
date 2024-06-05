#include<iostream>
#include<stdio.h>
#include<graphics.h>
#include<conio.h>
#include <easyx.h>
#include<vector>
#include <fstream>
#include <algorithm>
#include<random>
#include"tools.h"

class FileName :public Tool
{
public:
	FileName();
	~FileName();
public :
	void init();
	void Set();
	void run();
	void fly();
	void  creatObstacle();
	void checkHit();
	void updateBg();
	void jump();
	void down();
	void keyEvent();
	void updateEnemy();
	void updateHero();//对玩家状态进行更新
	void updateBloodBar();
	void speedUP();
	void checkOver();
	void checkScore();
	void updateScore();
	void checkWin();
	void clickBtn(ExMessage*msg);
	void difficuilt(ExMessage* msg);
	void ReadRank();
	void WriteRank();
	void SelectRank(ExMessage* msg);
	void help(ExMessage* msg);
	void Continue(ExMessage* msg);
	void Mode(ExMessage* msg);
private:
	bool heroJump;//表示玩家正在跳跃
	int jumpHeightMax;
	int heroJumpOff;
	int update;//表示是否需要马上刷新页面
	int heroBlood;//初始化一下血量
	int score;
	int timer = 0;
	COLORREF color1;       //第一个按钮颜色  以此类推
	COLORREF color2;
	COLORREF color3;             
	COLORREF color4;
	int staus = 4;
	std::vector<int> rank;
};