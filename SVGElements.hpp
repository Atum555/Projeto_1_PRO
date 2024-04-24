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
    std::string            id_;
    std::vector<Transform> transforms_;

    /// @param id   Element's ID
    /// @param t    Transformations
    SVGElement(const std::string &id, const std::vector<Transform> &t);

    virtual ~SVGElement();
    virtual void draw(PNGImage &img) const = 0;
};

void readSVG(const std::string &svg_file, Point &dimensions, std::vector<SVGElement *> &svg_elements);
void convert(const std::string &svg_file, const std::string &png_file);

class Ellipse : public SVGElement {
  public:
    /// @brief          Ellipse Element
    /// @param id       Element's ID
    /// @param t        Transformations
    /// @param fill     Fill Color
    /// @param center   Ellipse Center
    /// @param radius   Point representing the x and y radius
    Ellipse(
        const std::string &id, const std::vector<Transform> &t, const Color &fill, const Point &center,
        const Point &radius
    );

    void draw(PNGImage &img) const override;

  protected:
    Color color_;
    Point center_;
    Point radius_;
};

class Circle : public Ellipse {
  public:
    /// @brief          Circle Element
    /// @param id       Element's ID
    /// @param t        Transformations
    /// @param fill     Fill Color
    /// @param center   Circle Center
    /// @param radius   Circle Radius
    Circle(const std::string &id, const std::vector<Transform> &t, const Color &fill, const Point &center, int radius);

    void draw(PNGImage &img) const override final;
};

class Poly : public SVGElement {
  public:
    /// @brief          Abstract Class for a set of Points
    /// @param id       Element's ID
    /// @param t        Transformations
    /// @param points   Points
    Poly(const std::string &id, const std::vector<Transform> &t, const std::vector<Point> &points);

  protected:
    std::vector<Point> points_;
};

class PolyLine : public Poly {
  public:
    /// @brief          Polygon Element with stroke
    /// @param id       Element's ID
    /// @param t        Transformations
    /// @param color    Stroke Color
    /// @param points   Points
    PolyLine(
        const std::string &id, const std::vector<Transform> &t, const std::vector<Point> &points, const Color &stroke
    );

    void draw(PNGImage &img) const override final;

  protected:
    Color color_;
};

class Line : public PolyLine {
  public:
    /// @brief          Line Element
    /// @param id       Element's ID
    /// @param t        Transformations
    /// @param point1   Start Point
    /// @param point2   End Point
    /// @param color    Stroke Color
    Line(
        const std::string &id, const std::vector<Transform> &t, const Point &point1, const Point &point2,
        const Color &stroke
    );
};

class PolyGon : public Poly {
  public:
    /// @brief          Polygon Element with fill
    /// @param id       Element's ID
    /// @param t        Transformations
    /// @param points   Points
    /// @param color    Fill Color
    PolyGon(
        const std::string &id, const std::vector<Transform> &t, const std::vector<Point> &points, const Color &fill
    );

    void draw(PNGImage &img) const override final;

  protected:
    Color color_;
};

class Rectangle : public PolyGon {
  public:
    /// @brief          Rectangle Element
    /// @param id       Element's ID
    /// @param t        Transformations
    /// @param origin   Start Point
    /// @param width    Width
    /// @param height   Height
    /// @param color    Fill Color
    Rectangle(
        const std::string &id, const std::vector<Transform> &t, const Color &fill, const Point &origin, int width,
        int height
    );
};

class UseElement : public SVGElement {
  public:
    /// @brief          Object that represents a reference to another element
    /// @param id       Element's ID
    /// @param t        Transformations
    /// @param ref     Reference to third Element
    UseElement(const std::string &id, const std::vector<Transform> &t, SVGElement *ref);
    ~UseElement();

    void draw(PNGImage &img) const override final;

  protected:
    SVGElement *ref_;
};

class GroupElement : public SVGElement {
  public:
    /// @brief          Object that represents a group of elements
    /// @param id       Element's ID
    /// @param t        Transformations
    /// @param elems    Vector of Child Elements
    GroupElement(const std::string &id, const std::vector<Transform> &t, const std::vector<SVGElement *> elems);
    ~GroupElement();

    void draw(PNGImage &img) const override final;

  protected:
    std::vector<SVGElement *> elems_;
};
} // namespace svg
#endif
