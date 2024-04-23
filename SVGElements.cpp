#include "SVGElements.hpp"

namespace svg {

//* BASE ELEMENT
SVGElement::SVGElement(const std::string &id, const Transform &t) : id_(id), transform_(t) {}

SVGElement::~SVGElement() {}

//* ELLIPSE

Ellipse::Ellipse(const std::string &id, const Transform &t, const Color &fill, const Point &center, const Point &radius)
    : SVGElement(id, t), fill_(fill), center_(center), radius_(radius) {}

//* CIRCLE

Circle::Circle(const std::string &id, const Transform &t, const Color &fill, const Point &center, int radius)
    : Ellipse(id, t, fill, center, Point{ radius, radius }) {}

//* POLY

void Ellipse::draw(PNGImage &img) const { img.draw_ellipse(center_, radius_, fill_); }


} // namespace svg
