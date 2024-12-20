#ifndef GIFBASE_H
#define GIFBASE_H
#include "cocos2d.h"
#include "Bitmap.h"

class GifBase : public cocos2d::CCSprite
{
public:
	virtual void updateGif(uint32_t delta) = 0;
	virtual cocos2d::CCTexture2D* createTexture(Bitmap* bm, int index, bool getCache);

	virtual void update(float delta)
	{
        //if delta>1, generally speaking  the reason is the device is stuck
        if(delta > 1)
        {
            return;
        }
		uint32_t ldelta = (uint32_t)(delta * 1000);
		updateGif(ldelta * m_speed_mult);
	};
    
	virtual std::string getGifFrameName(int index)
	{
		return m_gif_fullpath;
	};

	double m_speed_mult = 1.f;

protected:
	std::string m_gif_fullpath;;
};
#endif//GIFBASE_H

