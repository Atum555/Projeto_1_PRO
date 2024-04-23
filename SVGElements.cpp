#include "SVGElements.hpp"

namespace svg {
//* BASE ELEMENT
SVGElement::SVGElement(const std::string &id, const Transform &t) : id_(id), transform_(t) {}

SVGElement::~SVGElement() {}

//* ELLIPSE

Ellipse::Ellipse(const std::string &id, const Transform &t, const Color &fill, const Point &center, const Point &radius)
    : SVGElement(id, t), fill(fill), center(center), radius(radius) {}

void Ellipse::draw(PNGImage &img) const { img.draw_ellipse(center, radius, fill); }

//* POLY

void Ellipse::draw(PNGImage &img) const {
    img.draw_ellipse(center, radius, fill);
}

// @todo provide the implementation of SVGElement derived classes
// HERE -->

} // namespace svg
