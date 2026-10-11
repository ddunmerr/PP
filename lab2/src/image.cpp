#include "include/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master/stb_image_write.h"

std::uint8_t* Image::Pixel(int x, int y)
{
	return data.data() + (static_cast<std::size_t>(y) * width + x) * 3;
}

const std::uint8_t* Image::Pixel(int x, int y) const
{
	return data.data() + (static_cast<std::size_t>(y) * width + x) * 3;
}

bool LoadImage(const std::string& path, Image& out, std::string& err)
{
	int w = 0, h = 0, channels = 0;
	// force 3 channels: RGB, 8 bit
	stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &channels, 3);
	if (!pixels)
	{
		err = "cannot read image '" + path + "': " + (stbi_failure_reason() ? stbi_failure_reason() : "unknown error");
		return false;
	}
	out.width = w;
	out.height = h;
	out.data.assign(pixels, pixels + static_cast<std::size_t>(w) * h * 3);
	stbi_image_free(pixels);
	return true;
}

bool SaveImage(const std::string& path, const Image& img, std::string& err)
{
	// stb_image_write умеет писать BMP (и PNG/JPG). По заданию — BMP.
	int ok = stbi_write_bmp(path.c_str(), img.width, img.height, 3, img.data.data());
	if (!ok)
	{
		err = "cannot write image '" + path + "'";
		return false;
	}
	return true;
}

Image MakeEmpty(int width, int height)
{
	Image img;
	img.width = width;
	img.height = height;
	img.data.assign(static_cast<std::size_t>(width) * height * 3, 0);
	return img;
}
