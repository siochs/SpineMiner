#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <limits>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
//#include <sys/stat.h>


#define EPSILON 0.001
#define MAX 1.7976931348623158e+308


//vars
typedef std::vector<std::vector<double> > Matrix;
typedef std::vector<int>::size_type vInt;

//forwards
long _atoi(std::string s);
std::string _itoa(int f);
std::string _ftoa(double f);
double _atof(std::string s);
int _AinB(std::vector<std::vector<std::string> > A, std::vector<std::vector<std::string> > B);
bool _FileExists(std::string StrFilename);
bool _IsInt(std::string check);
//template<class T> T _fromstd::string(std::string s);
double _distance2d(double x1, double y1, double x2, double y2);
bool _IsEqual(double a, double b);
void _PrintMatrix(Matrix &m);
bool _FindMinimumInMatrix(Matrix m, int &x, int &y, double &v);
void _RemoveEntityFromSymmetricMatrix(Matrix &m, unsigned int x);
std::vector<double> _ExtractMinimaFromMatrix(Matrix &m);
bool _IsDouble(std::string s);
std::vector<std::string> _FindFilesInCWD(std::string identifyer);
std::vector<std::vector<std::string> > _TransposeMatrix(std::vector<std::vector<std::string> > m);

#endif // __HELPERS_H__
