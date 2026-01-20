#include "CaptureBoard.h"

CaptureBoard::CaptureBoard() {
	left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	top = GetSystemMetrics(SM_YVIRTUALSCREEN);
	img.width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	img.height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	screenWidth = img.width;
}

const BoardImage& CaptureBoard::returnImg() const { return img; }

void CaptureBoard::captureScreen() {
	HDC hScreenDC = GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(hScreenDC);

	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, img.width, img.height);
	SelectObject(hMemDC, hBitmap);

	BitBlt(hMemDC, 0, 0, img.width, img.height, hScreenDC, left, top, SRCCOPY);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = img.width;
	bmi.bmiHeader.biHeight = -img.height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	img.pixels.resize(img.width * img.height * 4);
	GetDIBits(hMemDC, hBitmap, 0, img.height, img.pixels.data(), &bmi, DIB_RGB_COLORS);

	screenWidth = img.width;

	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
	ReleaseDC(NULL, hScreenDC);
}

void CaptureBoard::findBoard() {
	Pixel target = { 166, 212, 77 };
	int tolerance = 10;
	int pixelCount = img.pixels.size() / 4;

	size_t start;
	for (start = 0; start < pixelCount; ++start) {
		size_t id = start * 4;
		Pixel pixel = img.getPixel(id);

		if (img.matchColor(target, pixel, tolerance))
			break;
	}
	left = start % screenWidth;
	top = start / screenWidth;

	size_t x;
	for (x = left; x < pixelCount; ++x) {
		size_t id = (top * screenWidth + x) * 4;
		Pixel pixel = img.getPixel(id);

		if (!img.matchColor(target, pixel, tolerance))
			break;
	}
	img.width = x - left;

	size_t y;
	for (y = top; y < pixelCount; ++y) {
		size_t id = (y * screenWidth + left) * 4;
		Pixel pixel = img.getPixel(id);

		if (!img.matchColor(target, pixel, tolerance))
			break;
	}
	img.height = y - top;

	size_t sid = (top * screenWidth + left) * 4;
	target = { (img.pixels[sid + 2]), (img.pixels[sid + 1]), (img.pixels[sid]) };
	for (x = 0; x < pixelCount; ++x) {
		size_t id = (top * screenWidth + left + x) * 4;
		Pixel pixel = img.getPixel(id);

		if (!img.matchColor(target, pixel, 0))
			break;
	}
	img.cellWidth = x;

	left += GetSystemMetrics(SM_XVIRTUALSCREEN);
}

void CaptureBoard::moveMouse(int x, int y) const {
	SetCursorPos(x, y);
}

void CaptureBoard::leftClick() const {
	INPUT input[2] = {};

	input[0].type = INPUT_MOUSE;
	input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	input[1].type = INPUT_MOUSE;
	input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	SendInput(2, input, sizeof(INPUT));
}

void CaptureBoard::rightClick() const {
	INPUT input[2] = {};

	input[0].type = INPUT_MOUSE;
	input[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

	input[1].type = INPUT_MOUSE;
	input[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

	SendInput(2, input, sizeof(INPUT));
}

void CaptureBoard::clickCell(int screenX, int screenY, ActionType action) const {
	moveMouse(screenX, screenY);

	Sleep(10);

	if (action == LCLICK)
		leftClick();
	else
		rightClick();

	Sleep(10);
}

void CaptureBoard::startGame() const {
	int screenX = left + img.width / 2;
	int screenY = top + img.height / 2;

	clickCell(screenX, screenY, LCLICK);
}

void CaptureBoard::applyActions(const std::vector<GridAction>& actions) const {
	for (const auto& action : actions) {
		int screenX = left + action.x * img.cellWidth + img.cellWidth / 2;
		int screenY = top + action.y * img.cellWidth + img.cellWidth / 2;

		clickCell(screenX, screenY, action.type);
	}
}