#include "BoardImage.h"

Pixel BoardImage::getPixel(int id) const {
	return { pixels[id + 2], pixels[id + 1], pixels[id] };
}

bool BoardImage::matchColor(const Pixel& target, const Pixel& pixel, int tolerance) const {
	return (abs(target.r - pixel.r) <= tolerance &&
			abs(target.g - pixel.g) <= tolerance &&
			abs(target.b - pixel.b) <= tolerance);
}