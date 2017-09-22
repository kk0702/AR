#pragma once

#ifndef _DEBUG
#pragma comment(lib, "../lib/ARS.lib")
#pragma comment(lib, "../lib/WML.lib")
#else
#pragma comment(lib, "../lib/ARSd.lib")
#pragma comment(lib, "../lib/WMLd.lib")
#endif



#define _CRT_SECURE_NO_WARNINGS

#define CLASSNAME "ARSTEST"
#define APPNAME "ARSTEST"




class Touchable : public Mesh{
private:
   float vx,vy;	//物体の速さ
   bool get_overlapping_center(Texture* hitArea, int *gx, int *gy, unsigned int threshold);//重なり重心を求めるメンバ関数
   void state_action(bool condition);
public:   
   static enum {OUT_TOUCH,IN_TOUCH};
   int state;//状態

   Touchable(ARSG* _g, wchar_t fln[]) : Mesh(_g,fln), vx(0.2f),vy(-0.2f),state(IN_TOUCH){ }
   Touchable(void) : vx(0.2f),vy(-0.2f),state(OUT_TOUCH){ }
   void react(Texture *hitArea);
   void move(void);
   

};
