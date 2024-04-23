/// @file shape.hpp
#ifndef __svg_SVGElements_hpp__
#define __svg_SVGElements_hpp__

#include "Color.hpp"
#include "PNGImage.hpp"
#include "Point.hpp"
#include "string"

namespace svg {

class Transform {
  private:
    int transX_, transY_;
    int rotate_;
    int scale_;
    int origX_, origY_;

  public:
    /// @brief Object that represents a transformation.
    /// @param tx   Translation in X
    /// @param ty   Translation in Y
    /// @param r    Rotation
    /// @param s    Scale
    /// @param ox   Origin X
    /// @param oy   Origin Y
    Transform(int tx, int ty, int r, int s, int ox, int oy)
        : transX_(tx), transY_(ty), rotate_(r), scale_(s), origX_(ox), origY_(oy) {}

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
};

class SVGElement {
  protected:
    std::string id_;
    Transform   transform_;

  public:
    /// @param id   Element's ID
    /// @param t    Transformation
    SVGElement(const std::string &id, const Transform &t);

    virtual ~SVGElement();
    virtual void draw(PNGImage &img) const = 0;
};

void readSVG(const std::string &svg_file, Point &dimensions, std::vector<SVGElement *> &svg_elements);
void convert(const std::string &svg_file, const std::string &png_file);

class Ellipse : public SVGElement {
  public:
    /// @brief          Elipse Element
    /// @param id       Element's ID
    /// @param t        Transformation
    /// @param fill     Fill Color
    /// @param center   Ellipse Center
    /// @param radius   Point representing the x and y radius
    Ellipse(const std::string &id, const Transform &t, const Color &fill, const Point &center, const Point &radius);

    // TODO Ellipse Draw
    void draw(PNGImage &img) const override;

  private:
    Color fill_;
    Point center_;
    Point radius_;
};

class Circle : public Ellipse {
  public:
    /// @brief          Circle Element
    /// @param id       Element's ID
    /// @param t        Transformation
    /// @param fill     Fill Color
    /// @param center   Circle Center
    /// @param radius   Circle Radius
    Circle(const std::string &id, const Transform &t, const Color &fill, const Point &center, int radius);

    // TODO Circle Draw
    void draw(PNGImage &img) const override;

  private:
    int radius_;
};

class Poly : public SVGElement {
  public:
    /// @brief          Abstract Class for a set of Points
    /// @param id       Element's ID
    /// @param t        Transformation
    /// @param points   Points
    Poly(const std::string &id, const Transform &t, const std::vector<Point> &points);

    // TODO Poly Draw
    void draw(PNGImage &img) const override;

  private:
    std::vector<Point> points_;
};

class PolyLine : public Poly {
  public:
    /// @brief          Polygon Element with stroke
    /// @param id       Element's ID
    /// @param t        Transformation
    /// @param points   Points
    /// @param color    Stroke Color
    PolyLine(const std::string &id, const Transform &t, const std::vector<Point> &points, const Color &color);

    // TODO PolyLine Draw
    void draw(PNGImage &img) const override;

  private:
    Color color_;
};

class Line : public PolyLine {
  public:
    /// @brief          Line Element
    /// @param id       Element's ID
    /// @param t        Transformation
    /// @param point1   Start Point
    /// @param point2   End Point
    /// @param color    Stroke Color
    Line(const std::string &id, const Transform &t, const Point &point1, const Point &point2, const Color &color);

    // TODO Line Draw
    void draw(PNGImage &img) const override;

  private:
    Color color_;
};

class PolyGon : public Poly {
  public:
    /// @brief          Polygon Element with fill
    /// @param id       Element's ID
    /// @param t        Transformation
    /// @param points   Points
    /// @param color    Fill Color
    PolyGon(const std::string &id, const Transform &t, const std::vector<Point> &points, const Color &color);

    // TODO PolyGon Draw
    void draw(PNGImage &img) const override;

  private:
    Color color_;
};

class Rectangle : public PolyGon {
  public:
    /// @brief          Rectangle Element
    /// @param id       Element's ID
    /// @param t        Transformation
    /// @param origin   Start Point
    /// @param width    Width
    /// @param height   Height
    /// @param color    Fill Color
    Rectangle(
        const std::string &id, const Transform &t, const Point &origin, int width, int height, const Color &color
    );

    // TODO Rectangle Draw
    void draw(PNGImage &img) const override;
};

class UseElement : public SVGElement {
  public:
    /// @brief          Object that represents a reference to another element
    /// @param id       Element's ID
    /// @param t        Transformation
    /// @param href     Reference to third Element
    UseElement(const std::string &id, const Transform &t, const std::string &href);

    /// @return Reference to third Element
    const std::string &get_href() const;

    // TODO Use Draw
    void draw(PNGImage &img) const override;

  private:
    std::string href_;
};


} // namespace svg
#endif
