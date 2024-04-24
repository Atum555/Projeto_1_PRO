#include "SVGElements.hpp"
#include "external/tinyxml2/tinyxml2.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace tinyxml2;

namespace svg {
void readSVG(const string &svg_file, Point &dimensions, vector<SVGElement *> &svg_elements) {
    // Load SVG FIle
    XMLDocument doc;
    XMLError    r = doc.LoadFile(svg_file.c_str());
    if (r != XML_SUCCESS) throw runtime_error("Unable to load " + svg_file); // Abort if Errors

    // Get Dimensions
    XMLElement *xml_elem = doc.RootElement();
    dimensions.x         = xml_elem->IntAttribute("width");
    dimensions.y         = xml_elem->IntAttribute("height");

    XMLNode *node = doc.FirstChild();                // <svg> Node
    if (node->NoChildren()) return;                  // Check if there are any elements

    XMLElement *element = node->FirstChildElement(); // First actual Element

    while (true) {
        const char *p;                               // Temp Variable Declaration

        // Get Element Name and ID
        const string elemName = element->Name();
        p                     = element->Attribute("id");
        const string id       = p ? p : "";

        // Get Element Transformation
        p                      = element->Attribute("transform");
        const string traStr    = p ? p : "a";
        int          traTransX = 0;
        int          traTransY = 0;
        int          traRotate = 0;
        int          traScale  = 0;

        // Create sStream from substring inside Parenthesis
        size_t        start = traStr.find_first_of('(') + 1;
        size_t        end   = traStr.find_first_of(')');
        istringstream traStrStream(traStr.substr(start, end - start));

        // Parse Values
        switch (traStr[0]) {
        case 't': // Translate
            traStrStream >> traTransX;
            traStrStream >> traTransY;
            break;
        case 'r': // Rotate
            traStrStream >> traRotate;
            break;
        case 's': // Scale
            traStrStream >> traScale;
            break;
        default: break;
        }

        // Get Element Transformation Origin
        p                         = element->Attribute("transform-origin");
        const string traOriginStr = p ? p : "";
        int          traOriX      = 0;
        int          traOriY      = 0;

        // Parse Origin
        if (traOriginStr.size()) {
            istringstream traOriginStrStream(traOriginStr);
            traOriginStrStream >> traOriX;
            traOriginStrStream >> traOriY;
        }

        // Generate Transform Object
        std::vector<Transform> elemTransform;
        elemTransform.push_back({ traTransX, traTransY, traRotate, traScale, traOriX, traOriY });

        // Select Element Type
        if (elemName == "g") {}   // TODO Group Element Parser

        if (elemName == "use") {} // TODO Group Element Parser

        if (elemName == "ellipse") {
            p = element->Attribute("fill");

            // Add Element
            svg_elements.push_back(new Ellipse(
                id, elemTransform, parse_color(string(p)), { element->IntAttribute("cx"), element->IntAttribute("cy") },
                { element->IntAttribute("rx"), element->IntAttribute("ry") }
            ));
        }

        if (elemName == "circle") {
            p = element->Attribute("fill");

            // Add Element
            svg_elements.push_back(new Circle(
                id, elemTransform, parse_color(string(p)), { element->IntAttribute("cx"), element->IntAttribute("cy") },
                element->IntAttribute("r")
            ));
        }

        if (elemName == "polyline") {
            // Parse Color
            p           = element->Attribute("stroke");
            Color color = parse_color(p);

            // Parse Points
            vector<Point> points;
            p = element->Attribute("points");
            istringstream issPoints(p);
            int           x;
            char          c;
            int           y;
            while (issPoints >> x) {
                issPoints >> c;
                issPoints >> y;
                points.push_back({ x, y });
            }

            // Add Element
            svg_elements.push_back(new PolyLine(id, elemTransform, points, color));
        }

        if (elemName == "line") {
            p = element->Attribute("stroke");

            // Add Element
            svg_elements.push_back(new Line(
                id, elemTransform, { element->IntAttribute("x1"), element->IntAttribute("y1") },
                { element->IntAttribute("x2"), element->IntAttribute("y2") }, parse_color(string(p))
            ));
        }

        if (elemName == "polygon") {
            // Parse Color
            p           = element->Attribute("fill");
            Color color = parse_color(p);

            // Parse Points
            vector<Point> points;
            p = element->Attribute("points");
            istringstream issPoints(p);
            int           x;
            char          c;
            int           y;
            while (issPoints >> x) {
                issPoints >> c;
                issPoints >> y;
                points.push_back({ x, y });
            }

            // Add Element
            svg_elements.push_back(new PolyGon(id, elemTransform, points, color));
        }

        if (elemName == "rect") {
            p = element->Attribute("fill");

            // Add Element
            svg_elements.push_back(new Rectangle(
                id, elemTransform, { element->IntAttribute("x"), element->IntAttribute("y") },
                element->IntAttribute("width"), element->IntAttribute("height"), parse_color(string(p))
            ));
        }

        // Get Next Node and Repeat
        element = element->NextSiblingElement();
        if (element == nullptr) break; // Break if no next Node
    }
} // namespace svg