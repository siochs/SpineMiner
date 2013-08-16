#ifndef __OVLREADER_H__
#define __OVLREADER_H__

#include <vector>
#include <fstream>
#include <dir.h>
#include <string.h>
#include "database.h"
#include "helpers.h"


#define FONTSTR {0x41, 0x00, 0x72, 0x00, 0x69, 0x00, 0x61, 0x00}
#define CHARPOS 156
#define COLPOS  -68
#define XPOS 108
#define YPOS  116

using namespace std;

class OVLReader
{
    public:
        OVLReader(){};
        ~OVLReader(){delete[] buffer;};
        vector <string> ScanFiles();
        int ReadAllData(string ovl_filename, Database &db);

        stringstream LastError;
        bool IgnoreSpineMorphologies;
        bool NoFilopodia;

    private:
        bool OpenOVL(string filename);
        bool GotoNextFontTag();
        string ReadOVLString();
        bool IsAlnum(char c);
        string ReadOVLCategory();
        bool IsConsistent(string filename);
        void ReadOVLPositions( double &x, double &y );

        char* buffer;                   //binary content of OVL file which is currently in use
        unsigned int length_buffer;              //length of OVL file which is currently in use
        int curr_pos;                   //current reading position in buffer
};

#endif // __OVLREADER_H__
