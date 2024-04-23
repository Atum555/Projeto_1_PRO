#include "SVGElements.hpp"

namespace svg {

//* BASE ELEMENT
SVGElement::SVGElement(const std::string &id, const Transform &t) : id_(id), transform_(t) {}

SVGElement::~SVGElement() {}

//


//* ELLIPSE

Ellipse::Ellipse(const std::string &id, const Transform &t, const Color &fill, const Point &center, const Point &radius)
    : SVGElement(id, t), fill_(fill), center_(center), radius_(radius) {}

Circle::Circle(const std::string &id, const Transform &t, const Color &fill, const Point &center, int radius)
    : Ellipse(id, t, fill, center, Point{ radius, radius }) {}

//


//* POLY

Poly::Poly(const std::string &id, const Transform &t, const std::vector<Point> &points)
    : SVGElement(id, t), points_(points) {}

PolyLine::PolyLine(const std::string &id, const Transform &t, const std::vector<Point> &points, const Color &color)
    : Poly(id, t, points), color_(color) {}

Line::Line(const std::string &id, const Transform &t, const Point &point1, const Point &point2, const Color &color)
    : PolyLine(id, t, { point1, point2 }, color) {}

PolyGon::PolyGon(const std::string &id, const Transform &t, const std::vector<Point> &points, const Color &color)
    : Poly(id, t, points), color_(color) {}

Rectangle::Rectangle(
    const std::string &id, const Transform &t, const Point &origin, int width, int height, const Color &color
)
    : PolyGon(
        id, t,
        {
            origin,
            {origin.x + width,          origin.y},
            {origin.x + width, origin.y + height},
            {        origin.x, origin.y + height},
        },
        color
    ) {}

//


//* Use

UseElement::UseElement(const std::string &id, const Transform &t, const std::string &href)
    : SVGElement(id, t), href_(href) {}

const std::string &UseElement::get_href() const { return href_; }

//


//* Draw

void Ellipse::draw(PNGImage &img) const {}

void Circle::draw(PNGImage &img) const {}

void Poly::draw(PNGImage &img) const {}

void PolyLine::draw(PNGImage &img) const {}

void Line::draw(PNGImage &img) const {}

void PolyGon::draw(PNGImage &img) const {}

void Rectangle::draw(PNGImage &img) const {}

void UseElement::draw(PNGImage &img) const {}

} // namespace svg
