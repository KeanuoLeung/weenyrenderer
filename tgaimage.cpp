#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cmath>
#include "tgaimage.h"

TGAImage::TGAImage() : data(NULL), width(0), height(0), bytespp(0) {}

TGAImage::TGAImage(int w, int h, int bpp) : data(NULL), width(w), height(h), bytespp(bpp)
{
  unsigned long nbytes = width * height * bytespp;
  data = new unsigned char[nbytes];
  memset(data, 0, nbytes);
}

TGAImage::TGAImage(const TGAImage &img)
{
  width = img.width;
  height = img.height;
  bytespp = img.bytespp;
  unsigned long nbytes = width * height * bytespp;
  data = new unsigned char[nbytes];
  memcpy(data, img.data, nbytes);
}

TGAImage::~TGAImage()
{
  if (data)
    delete[] data;
}

TGAImage &TGAImage::operator=(const TGAImage &img)
{
  if (this != &img)
  {
    if (data)
      delete[] data;
    width = img.width;
    height = img.height;
    bytespp = img.bytespp;
    unsigned long nbytes = width * height * bytespp;
    data = new unsigned char[nbytes];
    memcpy(data, img.data, nbytes);
  }
  return *this;
}

bool TGAImage::read_tga_file(const char *filename)
{
  if (data)
    delete[] data;
  data = NULL;
  std::ifstream in;
  in.open(filename, std::ios::binary);
  if (!in.is_open())
  {
    std::cerr << "cant't open file " << filename << "\n";
    in.close();
    return false;
  }
  TGA_Header header;
  in.read((char *)&header, sizeof(header));
  if (!in.good())
  {
    in.close();
    std::cerr << "an error occured while reading the header\n";
    return false;
  }
  width = header.width;
  height = header.height;
  bytespp = header.bitsperpixel >> 3;
  if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA))
  {
    in.close();
    std::cerr << "bad bpp (or width/height) value\n";
    return false;
  }
  unsigned long nbytes = bytespp * width * height;
  data = new unsigned char[nbytes];
  if (3 == header.datatypecode || 2 == header.datatypecode)
  {
    in.read((char *)data, nbytes);
    if (!in.good())
    {
      in.close();
      std::cerr << "an error occured while reading the data\n";
      return false;
    }
  }
  else
  {
    in.close();
    std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
    return false;
  }
  if (!(header.imagedescriptor & 0x20))
  {
    flip_vertically();
  }
  if (header.imagedescriptor & 0x10)
  {
    flip_horizontally();
  }
  std::cerr << width << "x" << height << "y" << bytespp * 8 << "\n";
  in.close();
  return true;
}

bool TGAImage::write_tga_file(const char *filename)
{
  unsigned char developer_area_ref[4] = {0, 0, 0, 0};
  unsigned char extension_area_ref[4] = {0, 0, 0, 0};
  unsigned char footer[18] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'};
  std::ofstream out;
  out.open(filename, std::ios::binary);
  if (!out.is_open())
  {
    std::cerr << "cant't open file " << filename << "\n";
    out.close();
    return false;
  }
  TGA_Header header;
  memset((void *)&header, 0, sizeof(header));
  header.bitsperpixel = bytespp << 3;
  header.width = width;
  header.height = height;
  header.datatypecode = (bytespp == GRAYSCALE ? 3 : 2);
  header.imagedescriptor = 0x20; // top-left origin
  out.write((char *)&header, sizeof(header));
  if (!out.good())
  {
    out.close();
    std::cerr << "cant't dump the tga file\n";
    return false;
  }

  out.write((char *)data, width * height * bytespp);
  if (!out.good())
  {
    std::cerr << "can't unload raw data\n";
    out.close();
    return false;
  }

  out.write((char *)developer_area_ref, sizeof(developer_area_ref));
  if (!out.good())
  {
    std::cerr << "cant't dump the tga file\n";
    out.close();
    return false;
  }
  out.write((char *)extension_area_ref, sizeof(extension_area_ref));
  if (!out.good())
  {
    std::cerr << "can't dump the tga file\n";
    out.close();
    return false;
  }
  out.write((char *)footer, sizeof(footer));
  if (!out.good())
  {
    std::cerr << "cant't dump the tga file\n";
    out.close();
    return false;
  }
  out.close();
  return true;
}

TGAColor TGAImage::get(int x, int y)
{
  if (!data || x < 0 || y < 0 || x >= width || y >= height)
  {
    return TGAColor();
  }
  return TGAColor(data + (x + y * width) * bytespp, bytespp);
}

bool TGAImage::set(int x, int y, TGAColor c)
{
  if (!data || x < 0 || y < 0 || x >= width || y >= height)
  {
    return false;
  }
  memcpy(data + (x + y * width) * bytespp, c.raw, bytespp);
  return true;
}

int TGAImage::get_bytespp()
{
  return bytespp;
}

int TGAImage::get_width()
{
  return width;
}

int TGAImage::get_height()
{
  return height;
}

bool TGAImage::flip_horizontally()
{
  if (!data)
    return false;
  int half = width >> 1;
  for (int i = 0; i < half; i++)
  {
    for (int j = 0; j < height; j++)
    {
      TGAColor c1 = get(i, j);
      TGAColor c2 = get(width - 1 - i, j);
      set(i, j, c2);
      set(width - 1 - i, j, c1);
    }
  }
  return true;
}

bool TGAImage::flip_vertically()
{
  if (!data)
    return false;
  unsigned long bytes_per_line = width * bytespp;
  unsigned char *line = new unsigned char[bytes_per_line];
  int half = height >> 1;
  for (int j = 0; j < half; j++)
  {
    unsigned long l1 = j * bytes_per_line;
    unsigned long l2 = (height - 1 - j) * bytes_per_line;
    memmove((void *)line, (void *)(data + l1), bytes_per_line);
    memmove((void *)(data + l1), (void *)(data + l2), bytes_per_line);
    memmove((void *)(data + l2), (void *)line, bytes_per_line);
  }
  delete[] line;
  return true;
}

unsigned char *TGAImage::buffer()
{
  return data;
}

void TGAImage::clear()
{
  memset((void *)data, 0, width * height * bytespp);
}

bool TGAImage::scale(int w, int h)
{
  if (w <= 0 || h <= 0 || !data)
    return false;
  unsigned char *tdata = new unsigned char[w * h * bytespp];
  int nscanline = 0;
  int oscanline = 0;
  int erry = 0;
  unsigned long nlinebytes = w * bytespp;
  unsigned long olinebytes = width * bytespp;
  for (int j = 0; j < height; j++)
  {
    int errx = width - w;
    int nx = -bytespp;
    int ox = -bytespp;
    for (int i = 0; i < width; i++)
    {
      ox += bytespp;
      errx += w;
      while (errx >= (int)width)
      {
        errx -= width;
        nx += bytespp;
        memcpy(tdata + nscanline + nx, data + oscanline + ox, bytespp);
      }
    }
    erry += h;
    oscanline += olinebytes;
    while (erry >= (int)height)
    {
      if (erry >= (int)height << 1) // it means we jump over a scanline
        memcpy(tdata + nscanline + nlinebytes, tdata + nscanline, nlinebytes);
      erry -= height;
      nscanline += nlinebytes;
    }
  }
  delete[] data;
  data = tdata;
  width = w;
  height = h;
  return true;
}
