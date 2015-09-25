#include "frame.h"
#include <cassert>
#include <boost/thread/locks.hpp>
#include <Objbase.h>
#define DRAWDIB_INCLUDE_STRETCHDIB
#include <Vfw.h>

using namespace std;
using namespace boost;
using namespace FFmpeg;
using namespace FFmpeg::Facade;

Frame::Frame(uint32_t width, uint32_t height, AVPicture &avPicture)
    : width_(width), height_(height)
{
    int32_t lineSize = avPicture.linesize[0];
    uint32_t padding = GetPadding(lineSize);

    pixelsPtr_ = new uint8_t[height_ * (lineSize + padding)];

	::SecureZeroMemory(&bmpInfo_, sizeof(bmpInfo_));
	bmpInfo_.bmiHeader.biBitCount = 24;
	bmpInfo_.bmiHeader.biHeight = height_;
	bmpInfo_.bmiHeader.biWidth = width_;
	bmpInfo_.bmiHeader.biPlanes = 1;
	bmpInfo_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo_.bmiHeader.biCompression = BI_RGB;

    Update(avPicture);
}

void Frame::Update(AVPicture &avPicture)
{
    unique_lock<mutex> lock(mutex_);

    int32_t lineSize = avPicture.linesize[0];
    uint32_t padding = GetPadding(lineSize);

    for (int32_t y = 0; y < height_; ++y)
    {
        ::CopyMemory(pixelsPtr_ + (lineSize + padding) * y,
            avPicture.data[0] + (height_ - y - 1) * lineSize, lineSize);

        ::SecureZeroMemory(pixelsPtr_ + (lineSize + padding) * y + lineSize, padding);
    }
}

void Frame::Draw(HWND window, int zoom, int cross, Frame *pip, int pip_width, int pip_top, int pip_left)
{
	unique_lock<mutex> lock(mutex_);

	PAINTSTRUCT ps;

	RECT rc = { 0, 0, 0, 0 };
	::GetClientRect(window, &rc);

	if (rc.right == rc.left) return;

	HDC hdc = ::BeginPaint(window, &ps);
	assert(hdc != nullptr);

	HDRAWDIB hdd = ::DrawDibOpen();

	if (pip != nullptr)
	{
		unique_lock<mutex> lock(pip->mutex_);

		LONG pip_height = pip->height_;
		if (pip_width <= 0)
			pip_width = pip->width_;
		else
			pip_height = pip_width * pip->height_ / pip->width_;
		if (pip_left < 0) pip_left = (width_ - pip_width) / 2;
		if (pip_top < 0) pip_top = (height_ - pip_height) / 2;

		::StretchDIB(
			&bmpInfo_.bmiHeader, pixelsPtr_, pip_left, pip->height_ - pip_top - pip_height, pip_width, pip_height,
			&(pip->bmpInfo_.bmiHeader), pip->pixelsPtr_, 0, 0, pip->width_, pip->height_);
	}

	int xSrc = 0;
	int ySrc = 0;
	int dxSrc = width_;
	int dySrc = height_;

	if (zoom > 1) {
		dxSrc = width_ / zoom;
		dySrc = height_ / zoom;
		xSrc = (width_ - dxSrc) / 2;
		ySrc = (height_ - dySrc) / 2;
	}

	if (cross > 0) {
		cross = cross / zoom;
		int cw = cross / 8;
		if (cw < 1) cw = 1;
		int xCntr = width_ / 2;
		int yCntr = height_ / 2;
		for (int x = -cross; x <= cross; x++) {
			for (int y = -cw; y <= cw; y++) {
				brightenUp(pixelsPtr_ + (width_ * (yCntr + y) + xCntr + x) * 3, 0x6F);
			}
		}
		for (int y = -cross; y < -cw; y++) {
			for (int x = -cw; x <= cw; x++) {
				brightenUp(pixelsPtr_ + (width_ * (yCntr + y) + xCntr + x) * 3, 0x6F);
			}
		}
		for (int y = cw + 1; y <= cross; y++) {
			for (int x = -cw; x <= cw; x++) {
				brightenUp(pixelsPtr_ + (width_ * (yCntr + y) + xCntr + x) * 3, 0x6F);
			}
		}
	}

	::DrawDibDraw(hdd, hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
		&bmpInfo_.bmiHeader, pixelsPtr_, xSrc, ySrc, dxSrc, dySrc, DDF_HALFTONE);

	::DrawDibClose(hdd);

	::EndPaint(window, &ps);
}

void Frame::brightenUp(uint8_t *ptr, uint8_t value) {
	for (int x = 0; x < 3; x++) {
		if (*ptr < (0xFF - value)) *ptr += value; else *ptr = 0xFF;
		ptr++;
	}
}

void Frame::ToBmp(uint8_t **bmpPtr)
{
    assert(bmpPtr != nullptr);

    unique_lock<mutex> lock(mutex_);

    *bmpPtr =
        static_cast<uint8_t *>(::CoTaskMemAlloc(sizeof(BITMAPINFOHEADER) + height_ * width_ * 3));

    if (*bmpPtr == nullptr)
        throw runtime_error("CoTaskMemAlloc failed");

    BITMAPINFOHEADER *headerPtr = reinterpret_cast<BITMAPINFOHEADER *>(*bmpPtr);
    ::SecureZeroMemory(headerPtr, sizeof(BITMAPINFOHEADER));
    headerPtr->biBitCount = 24;
    headerPtr->biHeight = height_;
    headerPtr->biWidth = width_;
    headerPtr->biPlanes = 1;
    headerPtr->biSize = sizeof(BITMAPINFOHEADER);
    headerPtr->biCompression = BI_RGB;

    uint8_t* pixelsPtr = *bmpPtr + sizeof(BITMAPINFOHEADER);
    ::CopyMemory(pixelsPtr, pixelsPtr_, height_ * width_ * 3);
}