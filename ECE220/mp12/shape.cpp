#include "shape.hpp"


//Partners: sap3, abhaysp2, ashwind2
//Base class
//Please implement Shape's member functions
//constructor, getName()

Shape::Shape(string name){
    //TODO
    name_ = name;
}
string Shape::getName(){
    //TODO
    return name_;
}


//Rectangle
//Please implement the member functions of Rectangle:
//getArea(), getVolume(), operator+, operator-
//copy(), clear()

void Rectangle::copy(const Rectangle& other){
    //TODO
    spec = new double[2];
    spec[0] = other.getWidth();
    spec[1] = other.getLength();

}
void Rectangle::clear(){
    //TODO
    delete [] spec;
}
Rectangle::Rectangle(double width, double length):Shape("Rectangle"){
    spec = new double[2];
    spec[0] = width;
    spec[1] = length;
}
Rectangle::Rectangle(const Rectangle& other):Shape("Rectangle"){
    copy(other);
}
Rectangle::~Rectangle(){
    clear();
}
const Rectangle& Rectangle::operator = (const Rectangle& other){
    clear();
    copy(other);
    return *this;
}
double Rectangle::getArea()const{
    //TODO
    return (double)getWidth()*getLength();
}
double Rectangle::getVolume()const{
    //TODO
    return 0;
}
Rectangle Rectangle::operator + (const Rectangle& rhs){
    //TODO
    double lengthR1 = getLength();
    double widthR1 = getWidth();
    double lengthR2 = rhs.getLength();
    double widthR2 = rhs.getWidth();
    double lengthR3 = lengthR2 + lengthR1;
    double widthR3 = widthR1 + widthR2;

    return Rectangle(widthR3, lengthR3);
}

Rectangle Rectangle::operator - (const Rectangle& rhs){
    //TODO
    double lengthR1 = getLength();
    double widthR1 = getWidth();
    double lengthR2 = rhs.getLength();
    double widthR2 = rhs.getWidth();
    double lengthR3 = max(0.0, lengthR1 - lengthR2);
    double widthR3 = max(0.0, widthR1 - widthR2);

    return Rectangle(widthR3,lengthR3);
}

// double * spec;
//spec[0] should be width
//spec[1] should be length
double Rectangle::getWidth()const{return spec? spec[0]:0;}
double Rectangle::getLength()const{return spec? spec[1]:0;}
void Rectangle::setWidth(double width){
    if (spec == NULL)
        return;

    spec[0] = width;
}
void Rectangle::setLength(double length){
    if (spec == NULL)
        return;

    spec[1] = length;
}


//Rectprism
//Please implement the member functions of RectPrism:
//constructor, getArea(), getVolume(), operator+, operator-
//@@Insert your code here
void RectPrism::copy(const RectPrism& other){
    //TODO
    spec = new double[3];
    spec[0] = other.getLength();
    spec[1] = other.getWidth();
    spec[2] = other.getHeight();
}
void RectPrism::clear(){
    //TODO
    delete [] spec;
}
RectPrism::RectPrism(double width, double length, double height):Shape("RectPrism"){
    spec = new double[3];
    spec[0] = length;
    spec[1] = width;
    spec[2] = height;
}
RectPrism::RectPrism(const RectPrism& other):Shape("RectPrism"){
    copy(other);
}
RectPrism::~RectPrism(){
    clear();
}
const RectPrism& RectPrism::operator = (const RectPrism& other){
    clear();
    copy(other);
    return *this;
}
double RectPrism::getVolume()const{
    //TODO
    return (double)getWidth()*getLength()*getHeight();
}
double RectPrism::getArea()const{
    //TODO
    return (double)2*(getLength()*getWidth() + getLength()*getHeight() + getWidth()*getHeight());
}
RectPrism RectPrism::operator + (const RectPrism& rhs){
    //TODO
    double lengthR1 = getLength();
    double widthR1 = getWidth();
    double heightR1 = getHeight();
    double lengthR2 = rhs.getLength();
    double widthR2 = rhs.getWidth();
    double heightR2 = rhs.getHeight();
    double lengthR3 = lengthR1 + lengthR2;
    double widthR3 = widthR1 + widthR2;
    double heightR3 = heightR1 + heightR2;

    return RectPrism(widthR3,lengthR3,heightR3);
}

RectPrism RectPrism::operator - (const RectPrism& rhs){
    //TODO
    double lengthR1 = getLength();
    double widthR1 = getWidth();
    double heightR1 = getHeight();
    double lengthR2 = rhs.getLength();
    double widthR2 = rhs.getWidth();
    double heightR2 = rhs.getHeight();
    double lengthR3 = max(0.0, lengthR1 - lengthR2);
    double widthR3 = max(0.0, widthR1 - widthR2);
    double heightR3 = max(0.0, heightR1 - heightR2);

    return RectPrism(widthR3,lengthR3,heightR3);
}

// double * spec;
//spec[0] should be length
//spec[1] should be width
//spec[2] should be height
double RectPrism::getWidth()const{return spec? spec[1]:0;}
double RectPrism::getHeight()const{return spec? spec[2]:0;}
double RectPrism::getLength()const{return spec? spec[0]:0;}
void RectPrism::setWidth(double width){
    if (spec == NULL)
        return;

    spec[1] = width;
}
void RectPrism::setHeight(double height){
    if (spec == NULL)
        return;
    spec[2] = height;
}
void RectPrism::setLength(double length){
    if (spec == NULL)
        return;

    spec[0] = length;
}


// Read shapes from test.txt and initialize the objects
// Return a vector of pointers that points to the objects
vector<Shape*> CreateShapes(char* file_name){
    //@@Insert your code here

    ifstream ifs (file_name, std::ifstream::in);
    double num_shapes = 0;
    ifs >> num_shapes;
    vector<Shape*> shape_ptrs(num_shapes, NULL);
    //TODO
    int i;
    double w = 0;
    double l = 0;
    double h = 0;
    string name;
    for(int i = 0; i<num_shapes; i++){
      ifs >> name;
      if(name == "Rectangle"){
        ifs >> w >> l;
        Shape* shape_ptr = new Rectangle(w, l);
        shape_ptrs[i] = shape_ptr;
      }
      else{
        ifs >> w >> l >> h;
        Shape* shape_ptr = new RectPrism(w, l, h);
        shape_ptrs[i] = shape_ptr;
      }
    }

    ifs.close();
    return shape_ptrs;
}

// call getArea() of each object
// return the max area
double MaxArea(vector<Shape*> shapes){
    double max_area = 0;
    //@@Insert your code here
    int i;
    for(i = 0; i<shapes.size(); i++){
      if(shapes[i]->getArea() > max_area){
        max_area = shapes[i]->getArea();
      }
    }

    return max_area;
}


// call getVolume() of each object
// return the max volume
double MaxVolume(vector<Shape*> shapes){
    double max_volume = 0;
    //@@Insert your code here
    int i;
    for(i = 0; i<shapes.size(); i++){
      if(shapes[i]->getVolume() > max_volume){
        max_volume = shapes[i]->getVolume();
      }
    }

    return max_volume;
}
