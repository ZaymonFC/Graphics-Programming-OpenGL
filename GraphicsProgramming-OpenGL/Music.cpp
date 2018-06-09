#include "Music.h"
#include <Mmsystem.h>
#include <mciapi.h>


Music::Music(std::string fileName)
{
	mciSendString("open \"*.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
}

auto Music::Play() -> void
{
}

