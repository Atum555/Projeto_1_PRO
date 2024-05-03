#include "SVGElements.hpp"
#include "external/tinyxml2/tinyxml2.h"
#include <bits/stdc++.h>

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

    XMLNode *node = doc.FirstChild(); // <svg> Node
    if (node->NoChildren()) return;   // Check if there are any elements

    // Create auxilary vector to store all elements (including subelements) with ID's
    vector<SVGElement *> svg_elems_id;

    XMLElement *element = node->FirstChildElement(); // First actual Element

    // Loop Through Elements
    while (true) {
        // Parse Element
        parseElement(element, svg_elements, svg_elems_id);

        // Get Next Element
        element = element->NextSiblingElement();
        if (element == nullptr) break; // Break if no more elements
    }
}

void parseElement(
    const XMLElement *element, vector<SVGElement *> &elementList, vector<SVGElement *> &elementListID,
    const vector<Transform> &transforms
) {
    const char *p;                                                           // Temporary Pointer Variable Declaration

    const string elemName = element->Name();                                 // Get Element Name

    p               = element->Attribute("id");                              // Get Element ID
    const string id = p ? p : "";                                            // Element might not have an ID

    vector<Transform> transList;                                             // Generate List of Transformations
    const Transform   trans = getTransform(element);                         // Get Element Transformation
    transList.push_back(trans);                                              // Add Element Transformation

    transList.insert(transList.end(), transforms.begin(), transforms.end()); // Append Inherited Transformations
                                                                             // This is done after because inherited
                                                                             // transformations need to be applied last


    // Create Element Pointer
    SVGElement *eP = nullptr;

    // Parse Each Element Differently

    // Ellipse
    if (elemName == "ellipse") {
        // Parse Color
        p           = element->Attribute("fill");
        Color color = parse_color(p);

        // Parse Center and Radius
        Point center({ element->IntAttribute("cx"), element->IntAttribute("cy") });
        Point radius({ element->IntAttribute("rx"), element->IntAttribute("ry") });

        // Create Element
        eP = new Ellipse(id, transList, color, center, radius);
    }

    // Circle
    if (elemName == "circle") {
        // Parse Color
        p           = element->Attribute("fill");
        Color color = parse_color(p);

        // Parse Center and Radius
        Point center({ element->IntAttribute("cx"), element->IntAttribute("cy") });
        int   radius = element->IntAttribute("r");

        // Create Element
        eP = new Circle(id, transList, color, center, radius);
    }

    // PolyLine
    if (elemName == "polyline") {
        // Parse Color
        p           = element->Attribute("stroke");
        Color color = parse_color(p);

        // Read Points
        vector<Point> points;
        p           = element->Attribute("points");
        string pStr = p ? p : "";

        // Substitute possible ',' for ' ' (spaces)
        for (size_t i = 0; i < pStr.size(); i++)
            if (pStr[i] == ',') pStr[i] = ' ';

        // Parse Points
        istringstream issPoints(pStr);
        int           x;
        int           y;
        while (issPoints >> x) {
            issPoints >> y;
            points.push_back({ x, y });
        }

        // Create Element
        eP = new PolyLine(id, transList, points, color);
    }

    // Line
    if (elemName == "line") {
        // Parse Color
        p           = element->Attribute("stroke");
        Color color = parse_color(p);

        // Parse Points
        Point point1 = { element->IntAttribute("x1"), element->IntAttribute("y1") };
        Point point2 = { element->IntAttribute("x2"), element->IntAttribute("y2") };

        // Create Element
        eP = new Line(id, transList, point1, point2, color);
    }

    // PolyGon
    if (elemName == "polygon") {
        // Parse Color
        p           = element->Attribute("fill");
        Color color = parse_color(p);

        // Read Points
        vector<Point> points;
        p           = element->Attribute("points");
        string pStr = p ? p : "";

        // Substitute possible ',' for ' ' (spaces)
        for (size_t i = 0; i < pStr.size(); i++)
            if (pStr[i] == ',') pStr[i] = ' ';

        // Parse Points
        istringstream issPoints(pStr);
        int           x;
        int           y;
        while (issPoints >> x) {
            issPoints >> y;
            points.push_back({ x, y });
        }

        // Create Element
        eP = new PolyGon(id, transList, points, color);
    }

    // Rectangle
    if (elemName == "rect") {
        // Parse Color
        p           = element->Attribute("fill");
        Color color = parse_color(p);

        // Parse Origin, Width and Height
        Point origin = { element->IntAttribute("x"), element->IntAttribute("y") };
        int   width  = element->IntAttribute("width");
        int   height = element->IntAttribute("height");

        // Create Element
        eP = new Rectangle(id, transList, color, origin, width, height);
    }

    // Group
    if (elemName == "g") {
        // Create List of Children
        vector<SVGElement *> children;

        // Get FirstChild
        const XMLElement *child = element->FirstChildElement();

        // Loop Through Children
        while (true) {
            // Parse Child
            parseElement(child, children, elementListID, transList);

            // Get Next Child
            child = child->NextSiblingElement();
            if (child == nullptr) break; // Break if no more children
        }


        // Create Element
        eP = new GroupElement(id, transList, children);
    }

    // Use
    if (elemName == "use") {
        // Get href ignoring "#" caracter
        p = element->Attribute("href");
        string href(p + 1);

        // Create Copy Element Pointer
        SVGElement *copyEP;

        // Find Matching Element
        for (SVGElement *idElem : elementListID) {
            if (idElem->getID() == href)
                copyEP = idElem->copy(transList); // Make a Copy of the element with extra Transformation
        }

        // Create Use Element
        if (copyEP) eP = new UseElement(id, transList, copyEP);
    }

    // If everything went well add pointer to lists
    if (eP) {
        // If Element has an ID add it to the List with elements who have an ID
        if (id.size()) elementListID.push_back(eP);

        // Add Element to List
        elementList.push_back(eP);
    }
}

Transform getTransform(const XMLElement *element) {
    // Init values
    const char *p;
    int         translateX = 0;
    int         translateY = 0;
    int         scale      = 1;
    int         rotate     = 0;
    int         originX    = 0;
    int         originY    = 0;

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
    istringstream traStream(start != string::npos ? traStr.substr(start, end - start) : "");

    // Parse Values According to Type of Transformation
    switch (traStr[0]) {
    case 't': // Translate
        traStream >> translateX;
        traStream >> translateY;
        break;
    case 'r': // Rotate
        traStream >> rotate;
        break;
    case 's': // Scale
        traStream >> scale;
        break;
    default: break;
    }

    // Get Element Transformation Origin
    p                      = element->Attribute("transform-origin");
    const string originStr = p ? p : "";

    // Parse Origin
    if (originStr.size()) {
        istringstream originStream(originStr);
        originStream >> originX;
        originStream >> originY;
    }

    return Transform(translateX, translateY, rotate, scale, originX, originY);
}


} // namespace svg