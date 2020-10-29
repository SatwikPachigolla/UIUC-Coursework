#ifndef SHAPE_H_
#define SHAPE_H_

#include<iostream>
#include<cmath>
#include<string>
#include<algorithm>
#include<vector>
#include<fstream>
#include <iomanip>    

using namespace std;


class Shape{
public:
  	Shape(string name);
  	string getName();
  	virtual double getArea() const = 0;
  	virtual double getVolume() const = 0;
	
private:
  string name_;


};



class Rectangle : public Shape{
public:
  	Rectangle(double width, double length);
    Rectangle(const Rectangle& other);//copy constructor overload
    ~Rectangle();//destructor overload
    const Rectangle& operator = (const Rectangle& other);//assignment overload
  	virtual double getArea() const;
  	virtual double getVolume() const;
	Rectangle operator + (const Rectangle& rec);
	Rectangle operator - (const Rectangle& rec);
	double getWidth() const;
	double getLength() const;
  void setWidth(double width);
  void setLength(double length);
private:
  void copy(const Rectangle& other);//copy helper
  void clear();//destroy helper
	double * spec;
  //spec[0] should be width
  //spec[1] should be length
};



class RectPrism : public Shape{
public:
  	RectPrism(double width, double length, double height);
    RectPrism(const RectPrism& other);//copy constructor overload
    ~RectPrism();//destructor overload
    const RectPrism& operator = (const RectPrism& other);//assignment overload
  	virtual double getVolume() const;
  	virtual double getArea() const;
	RectPrism operator + (const RectPrism& rectp);
	RectPrism operator - (const RectPrism& rectp);
	double getWidth() const;
	double getLength() const;
	double getHeight() const;
  void setWidth(double width);
  void setLength(double length);
  void setHeight(double height);
private:
  void copy(const RectPrism& other);//copy helper
  void clear();//destroy helper
  double * spec;
  //spec[0] should be length
  //spec[1] should be width 
  //spec[2] should be height
};

vector<Shape*> CreateShapes(char* file_name);
double MaxArea(vector<Shape*> shapes);
double MaxVolume(vector<Shape*> shapes);

#endif

