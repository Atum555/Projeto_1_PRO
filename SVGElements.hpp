/// @file shape.hpp
#ifndef __svg_SVGElements_hpp__
#define __svg_SVGElements_hpp__

#include "Color.hpp"
#include "PNGImage.hpp"
#include "Point.hpp"
#include "string"

namespace svg {

void readSVG(
    const std::string &svg_file, Point &dimensions,
    std::vector<SVGElement *> &svg_elements
);
void convert(const std::string &svg_file, const std::string &png_file);

class Transform {
  public:
    /// @brief Object that represents a transformation.
    /// @param tx   Translation in X
    /// @param ty   Translation in Y
    /// @param r    Rotation
    /// @param s    Scale
    /// @param ox   Origin X
    /// @param oy   Origin Y
    Transform(int tx, int ty, int r, int s, int ox, int oy)
        : transX_(tx), transY_(ty), rotate_(r), scale_(s), origX_(ox),
          origY_(oy) {}

    /// @return Translation in X
    int getTransX() const { return transX_; }

    /// @return Translation in Y
    int getTransY() const { return transY_; }

    /// @return Rotation
    int getRotate() const { return rotate_; }

    /// @return Scale
    int getScale() const { return scale_; }

    /// @return Origin X
    int getOriginX() const { return origX_; }

    /// @return Origin Y
    int getOriginY() const { return origY_; }

  private:
    int transX_, transY_;
    int rotate_;
    int scale_;
    int origX_, origY_;
};

class SVGElement {
  public:
    SVGElement();
    virtual ~SVGElement();
    virtual void draw(PNGImage &img) const = 0;

  protected:
    std::string id;
};

class Ellipse : public SVGElement {
  public:
    Ellipse(const Color &fill, const Point &center, const Point &radius);
    void draw(PNGImage &img) const override;

  private:
    Color fill;
    Point center;
    Point radius;
};
} // namespace svg
#endif
