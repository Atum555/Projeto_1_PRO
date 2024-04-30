#include "SVGElements.hpp"
#include "external/tinyxml2/tinyxml2.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace tinyxml2;

namespace svg {


/// @brief          Get Transformation from element atributes
/// @param element  Pointer to Element
/// @return         Transformation
Transform getTransform(const XMLElement *element) {
    // Init values
    const char *p;
    int         traTransX = 0;
    int         traTransY = 0;
    int         traRotate = 0;
    int         traScale  = 1;
    int         traOriX   = 0;
    int         traOriY   = 0;

    // Get Transformation Attribute String
    p             = element->Attribute("transform");
    string traStr = p ? p : "a"; // "a" because later First caracter of string will be read

    // Substitute possible ',' for ' ' (spaces)
    for (size_t i = 0; i < traStr.size(); i++) {
        if (traStr[i] == ',') traStr[i] = ' ';
    }

    // Create sStream from substring inside Parenthesis
    size_t        start = traStr.find_first_of('(') + 1;
    size_t        end   = traStr.find_first_of(')');
    istringstream traStrStream(start != string::npos ? traStr.substr(start, end - start) : "");

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

    // Parse Origin
    if (traOriginStr.size()) {
        istringstream traOriginStrStream(traOriginStr);
        traOriginStrStream >> traOriX;
        traOriginStrStream >> traOriY;
    }

    return Transform(traTransX, traTransY, traRotate, traScale, traOriX, traOriY);
}

/// @brief                  Parse an Element and add them to the vector
/// @param element          Element Pointer
/// @param svg_elements     Vector to add to
/// @param transforms       Previous transformations
void parseElement(
    const XMLElement *element, vector<SVGElement *> &svg_elements, const vector<Transform> &inheritedTransforms = {}
) {
    const char *p;                                   // Temp Variable Declaration

    const string elemName = element->Name();         // Get Element Name

    p               = element->Attribute("id");      // Get Element ID
    const string id = p ? p : "";

    const Transform   trans = getTransform(element); // Get Element Transformation
    vector<Transform> transList;                     // Generate List of Transformations

    transList.push_back(trans); // Add Element Transformation
    
    transList.insert(
        transList.end(), inheritedTransforms.begin(), inheritedTransforms.end()
    );                          // Append Inherited Transformations



    // Select Element Type
    if (elemName == "g") {
        vector<SVGElement *> children;
        const XMLElement    *child = element->FirstChildElement(); // First group Element

        // Loop Through Children
        while (true) {
            // Parse Element
            parseElement(child, children, transList);

            // Get Next Element
            child = child->NextSiblingElement();
            if (child == nullptr) break; // Break if no more elements
        }
        
        // Add Element
        svg_elements.push_back(new GroupElement(id, transList, children));
    }

    if (elemName == "use") {} // TODO Group Element Parser

    if (elemName == "ellipse") {
        p = element->Attribute("fill");

        // Add Element
        svg_elements.push_back(new Ellipse(
            id, transList, parse_color(string(p)), { element->IntAttribute("cx"), element->IntAttribute("cy") },
            { element->IntAttribute("rx"), element->IntAttribute("ry") }
        ));
    }

    if (elemName == "circle") {
        p = element->Attribute("fill");

        // Add Element
        svg_elements.push_back(new Circle(
            id, transList, parse_color(string(p)), { element->IntAttribute("cx"), element->IntAttribute("cy") },
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
        string pStr(p);

        for (size_t i = 0; i < pStr.size(); i++) {
            if (pStr[i] == ',') pStr[i] = ' ';
        }

        istringstream issPoints(pStr);

        int x;
        int y;
        while (issPoints >> x) {
            issPoints >> y;
            points.push_back({ x, y });
        }

        // Add Element
        svg_elements.push_back(new PolyLine(id, transList, points, color));
    }

    if (elemName == "line") {
        p = element->Attribute("stroke");

        // Add Element
        svg_elements.push_back(new Line(
            id, transList, { element->IntAttribute("x1"), element->IntAttribute("y1") },
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
        string pStr(p);

        for (size_t i = 0; i < pStr.size(); i++) {
            if (pStr[i] == ',') pStr[i] = ' ';
        }

        istringstream issPoints(pStr);

        int x;
        int y;
        while (issPoints >> x) {
            issPoints >> y;
            points.push_back({ x, y });
        }

        // Add Element
        svg_elements.push_back(new PolyGon(id, transList, points, color));
    }

    if (elemName == "rect") {
        p = element->Attribute("fill");

        // Add Element
        svg_elements.push_back(new Rectangle(
            id, transList, parse_color(string(p)), { element->IntAttribute("x"), element->IntAttribute("y") },
            element->IntAttribute("width"), element->IntAttribute("height")
        ));
    }
}

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

    // Loop Through Elements
    while (true) {
        // Parse Element
        parseElement(element, svg_elements);

        // Get Next Element
        element = element->NextSiblingElement();
        if (element == nullptr) break; // Break if no more elements
    }
}
} // namespace svg