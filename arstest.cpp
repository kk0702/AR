#pragma once


#include <tchar.h>
#include <windows.h>
#define D3D_DEBUG_INFO
#include <stdlib.h>
#include <math.h>
#include <d3dx9.h>
#include <XAudio2.h>
#include <vector>

#include "../include/WindowManager.h"
#include "../include/ars.h"
#include "arstest.h"


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------


void subtract_mask(Texture* result, Texture* bg, Texture* src, DWORD border);
const unsigned int sizex = 640; 
const unsigned int sizey = 480;

UINT MainLoop(WindowManager *winmgr)
{
	//ShowDebugWindow();

	//for debug(1/2)
	Window window2;
	winmgr->RegisterWindow(&window2);//

	ARSG arsgd(window2.hWnd, sizex, sizey, true);
	Texture debug(&arsgd, sizex, sizey);
	debug.SetDrawMode(true);
	
	Window window;
	winmgr->RegisterWindow(&window);

	ARSG g(window.hWnd, sizex, sizey, true);
	g.SetBackgroundColor(255,0,0,0);

	Light light(&g);	
	g.Register(&light);

	ARSD d;
	d.Init();
	d.AttachCam(0);
	d.StartGraph();
	
	Texture hitArea(&g,sizex,sizey);//あたり判定のもとの領域を入れるテクスチャー
	Texture stored (&g,sizex,sizey);
	Texture source (&g,sizex,sizey);
	source.SetDrawMode(TRUE);
	g.Register(&source);

	Touchable ball(&g, L"ball.x");	
	ball.SetScale(2.0f, 2.0f, 2.0f);
	ball.SetPosition(0.0f, 6.0f, 0.0f,GL_ABSOLUTE);		
	g.Register(&ball);

	InputHandler *keyIn = window.GetInputHandler();
	
	while(!d.GetCamImage(&stored));

	while (!winmgr->WaitingForTermination()){
		if (keyIn->GetKeyTrig('A'))
			d.GetCamImage(&stored);
		d.GetCamImage(&source);
		if (keyIn->GetKeyTrig('Q')) break;
			//hitAreaを切り抜き
		subtract_mask(&hitArea,&stored,&source,0x20202020);	
		
		ball.react(&hitArea);
		ball.move();

		//for debug(2/2)
		debug = hitArea;
		arsgd.Draw(&debug);
		g.Draw();
		
	}
	d.StopGraph();
	return 0;
}

inline void subtract_mask(Texture* result, Texture* backgrnd, Texture* src, DWORD border)
{
	ARSC::diff(result,backgrnd,src,border);
	ARSC::monochrome(result,result);
	ARSC::thresholding(result,result,border);
}

//重なりを調べる関数
inline bool Touchable::get_overlapping_center(Texture* hitArea, int *pGx, int *pGy, unsigned int threshold)
{	
	static Texture txtr;
	ARSG* g = GetARSG(); 
	txtr.Init(g,sizex,sizey);

	unsigned int pixel_count;
	//Touvhable(自分）を２次元に射影してtxtrに出力(上書き）
	g->Draw(this,&txtr);
	//領域のand演算
	ARSC::and(&txtr, &txtr, hitArea, 0x10101010);

	ARSC::getCG(pGx, pGy, &pixel_count, &txtr);	
	return pixel_count > threshold;
}
//重なり接触による応答
//動作触ったときの（状態の動きを定義）
inline void Touchable::react(Texture* _hitArea)
{//Touchable　の座標（オブジェクト座標）をスクリーン座標に変換
	int gx,gy;
	bool overlapping = get_overlapping_center(_hitArea, &gx, &gy,100);

	VECTOR2D c;		
	GetARSG()->Convert3Dto2D(&c, GetPosition());

	switch (state) {
		case OUT_TOUCH:
			if (overlapping) {
				vx = (c.x - gx) * 0.05f;
				vy = -(c.y - gy) * 0.05f;
				state = IN_TOUCH;
			}
			break;
		case IN_TOUCH:
			if (!overlapping)
				state = OUT_TOUCH;
			break;
		default:
			break;
	}
}

//移動の定義
inline void Touchable::move()
{
	VECTOR2D c;
	GetARSG()->Convert3Dto2D(&c, GetPosition());
		
	//枠の反射
	if (c.x < 0 || c.x > sizex)	vx *= -1.0f;
	if (c.y > sizey-50 && vy<0)	vy *= -1.0f;

	//自由落下または停止
	if (c.y > sizey-50 && vy<0.03f) 
		vy = 0;
	else
		vy -= 0.03f;

	//空気抵抗
	vx *= 0.8f;
	vy *= 0.8f;

   SetPosition(vx, vy, 0.0f, GL_RELATIVE);
}


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WindowManager program(hInstance, &MainLoop);
#ifdef DEBUG
    MessageBox(NULL,L"OK?",TEXT(APPNAME), NULL);
#endif
    return 0;
}
