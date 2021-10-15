// #define _KEANUO_DEBUGING__
#include "tgaimage.h"
#include "tinymath.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void line(TGAImage &img, int x0, int y0, int x1, int y1, TGAColor color)
{
  // slope = rise / run = (y2 - y1) / (x2 - x1)
  float slope = calc_slope(x0, y0, x1, y1);
  bool steep = false;
  if (slope > 1)
  {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
    slope = calc_slope(x0, y0, x1, y1);
  }
  // make it right order
  if (x0 > x1)
  {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  // draw the line in loop of points
  for (int x = x0; x < x1; ++x)
  {
    int y = slope * (x - x0) + y0;
    if (steep)
      img.set(y, x, color);
    else
      img.set(x, y, color);
  }
}

void triangle(TGAImage &img, int x0, int y0, int x1, int y1, int x2, int y2, TGAColor color)
{
  // sort the points base on their's y bottom to top
  if (y0 > y1)
  {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  if (y0 > y2)
  {
    std::swap(x0, x2);
    std::swap(y0, y2);
  }
  if (y1 > y2)
  {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }
  printf("y0: %d, y1: %d, y2: %d\n", y0, y1, y2);
  line(img, x0, y0, x1, y1, color);
  line(img, x1, y1, x2, y2, color);
  line(img, x2, y2, x0, y0, color);
  int total_level = y2 - y0;
  printf("total level: %d\n", total_level);

  // draw lower triangle
  {
    int y3 = y1;
    float slope = calc_slope(x2, y2, x0, y0);
    // y = kx + b
    // b = y - kx
    // x = (y - b) / k
    float b = (float)y0 - slope * x0;
    int x3 = (y3 - b) / slope;
    printf("this is b: %.2f\nthis is slope: %.2f\n", b, slope);
    printf("lines %d %d %d %d", x0, y0, x2, y2);
    printf("x3 x: %d, y: %d\n", x3, y3);
    line(img, x3, y3, x1, y1, white);
  }
}

int main(int argc, char *argv[])
{
  TGAImage image(100, 100, TGAImage::RGB);
  image.set(52, 41, red);
  // line(image, 10, 10, 90, 30, white);
  // line(image, 10, 10, 30, 90, red);
  triangle(image, 70, 70, 10, 10, 70, 30, red);
  image.flip_vertically();
  image.write_tga_file("output.tga");
  return 0;
}