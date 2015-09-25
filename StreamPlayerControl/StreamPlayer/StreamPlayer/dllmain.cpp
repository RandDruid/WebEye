#include "streamplayer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdexcept>

#define STREAMPLAYER_API extern "C"

FFmpeg::Facade::StreamPlayer player;

STREAMPLAYER_API int32_t __stdcall Initialize(FFmpeg::Facade::StreamPlayerParams params)
{
    try
    {
        player.Initialize(params);
    }
    catch (std::runtime_error &)
    {
        return 1;
    }

    return 0;
}

STREAMPLAYER_API int32_t __stdcall StartPlay(const char* url)
{
    try
    {
        player.StartPlay(url);
    }
    catch (std::runtime_error &)
    {
        return 1;
    }

    return 0;
}

STREAMPLAYER_API int32_t __stdcall StartPlayPiP(const char* url)
{
	try
	{
		player.StartPlayPiP(url);
	}
	catch (std::runtime_error &)
	{
		return 1;
	}

	return 0;
}

STREAMPLAYER_API int32_t __stdcall GetCurrentFrame(uint8_t** bmp_ptr)
{
    try
    {
        player.GetCurrentFrame(bmp_ptr);
    }
    catch (std::runtime_error &)
    {
        return 1;
    }

    return 0;
}

STREAMPLAYER_API int32_t __stdcall GetFrameSize(uint32_t* widthPtr, uint32_t* heightPtr)
{
    try
    {
        player.GetFrameSize(widthPtr, heightPtr);
    }
    catch (std::runtime_error &)
    {
        return 1;
    }

    return 0;
}

STREAMPLAYER_API int32_t __stdcall SetupPiP(int32_t* width, int32_t* top, int32_t* left)
{
	try
	{
		player.SetupPiP(width, top, left);
	}
	catch (std::runtime_error &)
	{
		return 1;
	}

	return 0;
}

STREAMPLAYER_API int32_t __stdcall SetupZoom(int32_t* zoom)
{
	try
	{
		player.SetupZoom(zoom);
	}
	catch (std::runtime_error &)
	{
		return 1;
	}

	return 0;
}

STREAMPLAYER_API int32_t __stdcall SetupCross(int32_t* cross)
{
	try
	{
		player.SetupCross(cross);
	}
	catch (std::runtime_error &)
	{
		return 1;
	}

	return 0;
}

STREAMPLAYER_API int32_t __stdcall Stop()
{
    try
    {
        player.Stop();
    }
    catch (std::runtime_error &)
    {
        return 1;
    }

    return 0;
}

STREAMPLAYER_API int32_t __stdcall Uninitialize()
{
    try
    {
        player.Uninitialize();
    }
    catch (std::runtime_error &)
    {
        return 1;
    }

    return 0;
}