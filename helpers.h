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

using namespace std;

//vars
typedef vector<vector<double> > Matrix;
typedef vector<int>::size_type vInt;

//forwards
long _atoi(string s);
string _itoa(int f);
string _ftoa(double f);
double _atof(string s);
int _AinB(vector<vector<string> > A, vector<vector<string> > B);
bool _FileExists(string StrFilename);
bool _IsInt(string check);
//template<class T> T _fromString(string s);
double _distance2d(double x1, double y1, double x2, double y2);
bool _IsEqual(double a, double b);
void _PrintMatrix(Matrix &m);
bool _FindMinimumInMatrix(Matrix m, int &x, int &y, double &v);
void _RemoveEntityFromSymmetricMatrix(Matrix &m, unsigned int x);
vector<double> _ExtractMinimaFromMatrix(Matrix &m);
bool _IsDouble(string s);
vector<string> _FindFilesInCWD(string identifyer);
vector<vector<string> > _TransposeMatrix(vector<vector<string> > m);

#endif // __HELPERS_H__
