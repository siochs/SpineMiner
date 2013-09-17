#include "helpers.h"

using namespace std;
extern std::ofstream _stdoutput;

long _atoi(string s)
{
    stringstream ss;
    long l;

  /*if( (ss >> l).fail() )
  {
     return -
  }*/
    ss << s;
    ss >> l;
    return l;
}

//converts int to string
string _itoa(int f)
{
    stringstream ss (stringstream::in | stringstream::out);
    ss << f;
    return ss.str();
}

//converts double to string
string _ftoa(double f)
{
    stringstream ss (stringstream::in | stringstream::out);
    ss << f;
    return ss.str();
}

//converts string to double
double _atof(string s)
{
    stringstream ss;
    double d;
    ss << s;
    ss >> d;

    return d;
}


//detects how often the elements from array a appear in array b
//e.g. A = {a,b,c,d,e}; B = {0,1,c,q,l,e}; returns 2;
int _AinB(vector<vector<string> > A, vector<vector<string> > B)
{
    int ret = 0;
    if (A.size() == 0 || B.size() == 0) return 0;

    for (vInt i = 0; i < A.size(); i++)
    {
        for (vInt j = 0; j < B.size(); j++)
        {
            if (A[i][0] == B[j][0]) ret ++;
        }
    }

    return ret;
}

//checks if file exists
bool _FileExists(string StrFilename)
{
    ifstream Test(StrFilename.c_str());
    if(!Test)
        return false;
    else
        return true;
/*
    //check file attributes
    return( (GetFileAttributes(StrFilename.c_str()) == INVALID_FILE_ATTRIBUTES)
            ? FALSE  //can't read -> file does not exist
            : TRUE); //file exists*/
}

bool _IsInt(string check)
{
     unsigned long int length = check.length();
     unsigned long int loop = 0;
     while( loop < length) if( !isdigit(check[loop++]) ) return false;

     return true;
}

bool _IsDouble(string s)
{
  istringstream ss(s);
  double d;
  return (ss >> d) && (ss >> std::ws).eof();
}

double _distance2d(double x1, double y1, double x2, double y2)
{
    return sqrt(pow((x1-x2),2)+pow((y1-y2),2));
}


bool _IsEqual(double a, double b)
{
    return fabs(a - b) < std::numeric_limits<double>::epsilon();
}


void _PrintMatrix(Matrix &m)
{
     for (vInt i = 0; i < m.size(); i++)
     {
         for (vInt j = 0; j < m[i].size(); j++)
         {
             _stdoutput << m[i][j] << "\t";
         }
         _stdoutput << endl;
     }

    return;
}

bool _FindMinimumInMatrix(Matrix m, int &x, int &y, double &v)
{
    bool foundsomething = false;
    double v1 = MAX;

    for (vInt i = 0; i < m.size(); i++)
    {
        for (vInt j = 0; j < m[i].size(); j++)
        {
            if (m[i][j] < v1 && !_IsEqual(m[i][j],0))
            {
                //store the coordinates
                x = j; y = i;
                v1 = m[i][j];
                foundsomething = true;
            }
        }
    }
    v = v1;

    return foundsomething;
}


void _RemoveEntityFromSymmetricMatrix(Matrix &m, unsigned int x)
{
    for (vInt i = 0; i < m.size(); i++)
    {
        if (i == x)
        {
            //set the whole row to zero
              for (vInt j = 0; j < m[i].size(); j++)
                m[i][j] = 0.00;
        }
        //set the whole col to zero
        m[i][x] = 0.00;
    }

    return;
}

vector<double> _ExtractMinimaFromMatrix(Matrix &m)
{
    int x = 0, y = 0; double v1 = MAX;
    vector<double> v_mins;
    while (_FindMinimumInMatrix(m,x,y,v1))
    {
        v_mins.push_back(v1);
        _RemoveEntityFromSymmetricMatrix(m,x);
        /*_stdoutput << endl << v1 << endl;
        PrintMatrix(m);*/
    }

    return v_mins;
}

vector<string> _FindFilesInCWD(string identifyer)
{
    DIR *dir;
    struct dirent *files;
    dir = opendir(".");
    vector <string> filenames;
    string tmp;
    files = readdir(dir);
    while (files != NULL)
    {
        tmp = files->d_name;
        if(tmp.find(".ovl") != string::npos) filenames.push_back(tmp);

        files = readdir(dir);
    }

    return filenames;
}

/*template <class T>
void _TransposeMatrix( std::vector< std::vector<T> > a, std::vector< std::vector<T> > b, int width, int height)
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            b[j][i] = a[i][j];
        }
    }
} */

vector<vector<string> > _TransposeMatrix(vector<vector<string> > m)
{
    vInt rows = 0;
    vInt cols = m.size();

    for (vInt i = 0; i < cols; i++)
    {
        if (m[i].size() > rows) rows = m[i].size();
    }

    vector<vector<string> > trans(rows, vector<string>(cols));

    for(vInt i=0; i<m.size(); i++)
    {
        for (vInt j=0; j<m[i].size(); j++)
        {
            trans[j][i] = m[i][j];
        }
    }

    return trans;
}

/*static bool sort_using_greater_than(double u, double v)
{
   return u > v;
}*/




