#include "ovlreader.h"

using namespace std;
extern std::ofstream _stdoutput;

//searches the next match of "A r i a l" in the buffer. Search starts at curr_pos.
//curr_pos will be set to new position on match
bool OVLReader::GotoNextFontTag()
{
    char fontstr[] = FONTSTR;

    //find match of "A r i a l". Start search at curr_pos
    for (unsigned int i = curr_pos + (sizeof fontstr); i < length_buffer; i++)
    {
        if (i + sizeof fontstr >= length_buffer) return false; //we reached the buffer end already

        if (!memcmp(buffer + i, fontstr, sizeof fontstr - 1)) //found match for FONTSTR in buffer. Store this position in curr_pos
        {
            curr_pos = i;
            return true;
        }
    }

    return false; //whole buffer was scanned but no FONTSTR found.
}

//Open ovl file and read content into buffer
bool OVLReader::OpenOVL(string filename)
{
    //open OVL file
    ifstream is;
    is.open(filename.c_str(), ios::binary );
    if (is.fail())
    {
        LastError << "Error> Cannot open file " << filename << endl;
        return false;
    }

     //get length of file
     is.seekg (0, ios::end);
     length_buffer = is.tellg();
     is.seekg (0, ios::beg);

     //allocate memory
     buffer = new (nothrow) char [length_buffer];
     if (!buffer)
     {
        LastError << "Error> Cannot allocate memory for file " << filename << endl;
        return false;
     }
     else
     {
         // read data as a block
         is.read (buffer,length_buffer);
         is.close();
     }

     //data read now into buffer. set curr_pos to start
     curr_pos = 0;
     return true;
}

//true if c is alphanumeric
bool OVLReader::IsAlnum(char c)
{
    if (c == 61) return true;
    if (c == 46) return true;

    if (c < 48 || c > 122) return false;
    else
    if (c < 65 && c > 57) return false;
    else
    if (c < 97 && c > 90) return false;

    return true;
}

//reads a string from OVL file in buffer. This string can be a spine (e.g. "12m") or some other information (e.g. "day=7") in the OVL-File
string OVLReader::ReadOVLString()
{
    char counter = 0;
    string extracted;

    //curr pos is the position where the FONTSTR has been detected.
    //the difference between the FONTSTR pos and the position where the string is stored is always the same (CHARPOS)
    //read from curr_pos + CHARPOS as long as there does not occur more then one time "0x00". Two times "0x00" means that the string has ended.
    //counter counts the occurance of "0x00". If this happens more than one time (meaning a string ends) the reading stops.
    for (int i = curr_pos + CHARPOS; counter < 2; i++)
    {
        //if there is nothing just read and increase the counter...
        if (buffer[i] == 0x00) counter++;
        else
        {
            //is this a regular string object or something different like a vector graphic?
            if (IsAlnum(buffer[i]) == true)
            {
                //yeah, got a string. Read character, reset counter and continue
                extracted+=buffer[i];
                counter = 0;
            }
            else
            //something was found, but it was not alphanumeric. So the whole object might be something like a vector graphic...
            return "NoAlnum";

        }
    }

    //an empty element was found
    if (extracted == "") return "NoValue";
    else return extracted;
}

string OVLReader::ReadOVLCategory()
{
    //The color of a object is stored at curr_pos + COLPOS. This is always constant...
    unsigned char a = buffer[curr_pos + COLPOS];
    unsigned char b = buffer[curr_pos + COLPOS+1];
    unsigned char c = buffer[curr_pos + COLPOS+2];

    if ( a == 255 && b == 0 && c == 0) return "lost"; //red
    if ( a == 255 && b == 255 && c == 255) return "stable"; //white
    if ( a == 0 && b == 255 && c == 0) return "gained"; //green
    if ( a == 0 && b == 0 && c == 255) return "info"; //blue
    if ( a == 255 && b == 0 && c == 255) return "comment"; //pink; see it as a comment function
    return "NoCategory"; //an other color.
}

void OVLReader::ReadOVLPositions(double &x, double &y)
{
    union
    {
        double d;
        unsigned char b[7];
    } c;
    c.d = 0;

    for (int i = 0; i < 8; i++)
    {
        c.b[i] = buffer[curr_pos + i + XPOS];
    }
    x = c.d;
    for (int i = 0; i < 8; i++)
    {
        c.b[i] = buffer[curr_pos + i + YPOS];
    }
    y = c.d;
    return;
}

//tests if a OVL file is consistent. A consistent file needs at least one valid spine and dendrite_id, day and length tag
bool OVLReader::IsConsistent(string filename)
{
    //returns true if a file has id, day, length, spine and a spine_type (lost/gained/stable)
    bool has_id = false;
    bool has_day = false;
    bool has_spine = false;
    bool has_category = false;
    bool has_length = false;
    string ovl_string;
    string ovl_category;

    if (!OpenOVL(filename))
    {
        return false; //was not able to open file
    }

    //important note: the first FONTSRT tag has to be skipped. Has something to do with the OVL format...
    if (!GotoNextFontTag())
    {
        //if there is no font tag at all, the file is not consistent.
        LastError  << "Error> Could not find first font tag in " << filename << endl;
        return false;
    }

    while (!has_id || !has_day || !has_spine || !has_category || !has_length) //scan the buffer until all the required data have been detected to make the file consistent
    {
        if (GotoNextFontTag() == false) break; //goto next tag or exit loop if no more tags exist

        //read the objects using curr_pos as basement. Objects have to be strings with certain colors
        ovl_string = ReadOVLString();
        ovl_category = ReadOVLCategory();

        //is string usable?
        if (ovl_string == "NoAlnum" || ovl_string == "NoValue" || ovl_category == "NoCategory")
        {
            //no it's not, then skip this position and move forward
            continue;
        }

        if (ovl_string.find("id=")!=string::npos)
        {
            //bingo, a dendrite id tag was found
            has_id = true;
            continue;
        }
        else
        if (ovl_string.find("l=")!=string::npos)
        {
            //bingo, a dendrite length tag was found
            has_length = true;
            continue;
        }
        else
        if (ovl_string.find("day=")!=string::npos)
        {
            //bingo, a day tag was found
            has_day = true;
            continue;
         }
         else
         {
            if (ovl_category == "lost" || ovl_category == "gained" || ovl_category == "stable")
            {
                //this must be a spine
                has_spine = true;
                has_category = true;
                continue;
            }
            else continue;
        }
    }

    //while loop ended. If spine, categoiry, length, id and day are okay then the file is consistent
    if (has_id && has_day && has_spine && has_category && has_length)
        return true;
    else
    {
        if (!has_id) LastError << "Error> No dendrite id found in " << filename << endl;
        if (!has_day) LastError << "Error> No day found in "<<filename<< endl;
        if (!has_spine) LastError << "Error> No spine found in "<<filename<< endl;
        if (!has_category) LastError << "Error> No category found in "<<filename<< endl;
        if (!has_length) LastError << "Error> No length found in " << filename<< endl;

        return false;
    }
}

//scans the current working directory for OVL files.
//if a OVL file was found then check it for consistency.
//te function returns a list of files which ARE consistent
vector<string> OVLReader::ScanFiles()
{
    vector<string> r;
    vector<string> filenames;
    //find ovl files
    filenames = _FindFilesInCWD(".ovl");

    //when none produce error
    if (filenames.size() == 0)
    {
        char cwd[256];
        getcwd(cwd, 256);
        LastError << "Error> No ovl files found in \""<<cwd<<"\"" << endl;
        return r;
    }

    //otherwise, for each file
    for (vInt i = 0; i < filenames.size(); i++)
    {
        //check if file is consistent
            if (IsConsistent(filenames[i]) == false)
            {
                _stdoutput << "Warning> File \"" << filenames[i] << "\" is inconsistent. File will be skipped." << endl;
                continue;
            }
            else
            {
                _stdoutput << "Info> File \"" << filenames[i] << "\" is consistent. File can be used for analysis." << endl;
                r.push_back(filenames[i]);
            }
    }

    return r;
}

//Opens a consistent OVL file. This function must be called when you can be sure that the file is consistent.
//all data will be stored into the database.
//typically a ovl file contains a dendrite with spines at a certain timepoint.
int OVLReader::ReadAllData(string ovl_filename, Database &db)
{
    //at this point we can be sure that the file has spines, types, length, id, days etc stored
    //push data extracted from file here
    OpenOVL(ovl_filename);
    GotoNextFontTag(); //skip the first. This has something to do with the OVL format...

    string ovl_string;
    string ovl_category;
    double x,y;
    string Query;

    string dendrite_id;
    double dendrite_length;
    int day = 0;
    vector<string> spine_ids;
    vector<string> spine_types;
    vector<char> spine_morphs;
    vector<string> spine_xcoords;
    vector<string> spine_ycoords;

    int stable_spines = 0;
    int lost_spines = 0;
    int gained_spines = 0;
    int living_spines = 0;
    int filopodia = 0;
    int mushrooms = 0;
    int stubbies = 0;
    int thins = 0;

    int sum_spines = 0;

    while (GotoNextFontTag()) // do the while loop as long as the end of tag list is not reached
    {
        //read the objects using curr_pos as basement. Objects must have strings with certain colors
        ovl_string = ReadOVLString();
        ovl_category = ReadOVLCategory();
        ReadOVLPositions(x,y);


        //are strings usable?
        if(ovl_string.length() < 1 && IgnoreSpineMorphologies == true) _stdoutput << endl << "Warning> symbol \"" << ovl_string << "\" in " << ovl_filename << " has less than one character. Empty text box? ...skipped." << endl;
        else
        if(ovl_string.length() < 2 && IgnoreSpineMorphologies == false) _stdoutput << endl << "Warning> symbol \"" << ovl_string << "\" in " << ovl_filename << " has less than two characters. No morphology tag? ...skipped." << endl;

        if (ovl_string == "NoAlnum" || ovl_string == "NoValue" || ovl_category == "NoCategory")
        {
            //no it's not, then skip this position and move forward
            //comments (meaning color = pink) will be skipped without Warning.
            if (ovl_category != "comment") _stdoutput << endl << "Warning> "<< ovl_filename <<" detected ("<<ovl_string<<","<<ovl_category<<") ...skipped." << endl;
            //continue;
        }
        else
        if (ovl_string.find("id=")!=string::npos)
        {

            //bingo, a dendrite id tag was found
            ovl_string.replace(0,3,"");
            dendrite_id = ovl_string;
            //continue;

        }
        else
        if (ovl_string.find("l=")!=string::npos)
        {

            //bingo, a dendrite length tag was found
            ovl_string.replace(0,2,"");
            dendrite_length = atof(ovl_string.c_str());
            //continue;

        }
        else
        if (ovl_string.find("day=")!=string::npos)
        {

            //bingo, a day tag was found
            ovl_string.replace(0,4,"");
            day = atoi(ovl_string.c_str());
            //continue;

         }
         else
         {


            if (ovl_category == "lost" || ovl_category == "gained" || ovl_category == "stable")
            {

                //a string, with other than "info" caregory -> spine

                //spine id contains morphology info: last char at string end (e.g. "14m". m stands for mushroom morphology)
                char m;
                if (IgnoreSpineMorphologies == true) //if the user has no morphologies added or if he wants to ignore them, all morphologies will set to '?'
                {
                    m = '?';
                }
                else
                {
                    m = ovl_string[ovl_string.length() - 1]; //if there are morphologies used, then copy the last character away to obtain the spine morphology(14m => m).

                    if (m == 'f' && NoFilopodia == true) m = 't'; //if the user wants filopodia counted as thins, then f will be changed to t
                }

                if (m == 'm' || m == 's' || m == 't' || m == 'f' || m == '?' )
                {
                    if(m == 'm' && ovl_category != "lost") mushrooms ++; else //increase counter values for the spine morphologies
                    if(m == 's' && ovl_category != "lost") stubbies ++; else
                    if(m == 't' && ovl_category != "lost") thins ++; else
                    if(m == 'f' && ovl_category != "lost")
                    {
                        if (NoFilopodia == true) //if filopodia shall be counted as thins, increase thins
                            thins ++;
                        else
                            filopodia ++;
                    }

                    if (ovl_category == "stable") stable_spines ++; //increase counter values for the spine types
                    else
                    if (ovl_category == "gained") gained_spines ++;
                    else
                    if (ovl_category == "lost") lost_spines ++;

                    //problem: if you add morphology tags to your spines but you use the -ignoremorphology option then the survival calculation will result shit.
                    //reason: with ignoremorphologies the text-box "16m" in the ovl will be translated into a spine with the id = "16m". But 16m is not the same spine id as 16f. This spine will then not recognized as survived.
                    //if you dont use ignoremorphologies the text "16m" will reult in a spine with the id = 16.
                    //solution: if "m,f,s,t" is the last character of an id, strip it out?
                    //kill last character, but only when spine morphologies are not ignored to get the real spine id (14m => 14)
                    if (IgnoreSpineMorphologies == false) ovl_string.erase(ovl_string.end() - 1);

                    //add to list
                    spine_ids.push_back(ovl_string);
                    spine_types.push_back(ovl_category);
                    spine_morphs.push_back(m);
                    spine_xcoords.push_back(_ftoa(x));
                    spine_ycoords.push_back(_ftoa(y));
                }
                else
                {
                    //error handling
                    _stdoutput << endl << "Error> spine \"" << ovl_string << "\" on dendrite \"" << dendrite_id << "\" ("<< ovl_filename <<") has no valid morphology." << endl;
                    //the setabortflag is used to avoid program termination. Instead all errors will be collected and after that the program stops. This helps the user debugging his OVL files.
                    db.SetAbortFlag();
                }
            }
        }
    }

    //now insert everything that was collected from that file into the database
    for (unsigned int k = 0; k < spine_ids.size(); k++)
    {
        //first: check if the spine which is going to be inserted already exists....
        Query = "SELECT spine_id FROM spines WHERE spine_id = \"";
        Query += spine_ids[k];
        Query += "\" AND dendrite_id = \"";
        Query += dendrite_id;
        Query += "\" AND day = ";
        Query += _itoa(day);
        Query += ";";
        dbResults tmp = db.Query(Query);
        if (tmp.size() > 0) //if yes
        {
            _stdoutput << endl << "Error> multiple insert of spine \"" << spine_ids[k] << "\" from dendrite \"" << dendrite_id << "\" at day " << _itoa(day) << " causes database redundancy. ";
            _stdoutput << "Recheck " << ovl_filename << " before you continue." << endl;
            db.SetAbortFlag();
        }
        else //if not
        {
            Query = "INSERT INTO spines VALUES(\"";
            Query += spine_ids[k];
            Query += "\", \"";
            Query += spine_types[k];
            Query += "\", \"";
            Query += spine_morphs[k];
            Query += "\", \"";
            Query += dendrite_id;
            Query += "\", ";
            Query += _itoa(day);
            Query += ", 0, 0, ";
            Query += spine_xcoords[k];
            Query += ", ";
            Query += spine_ycoords[k];
            Query += ");";
            db.Query(Query);
            sum_spines++;
        }
    }

    living_spines = stable_spines + gained_spines;

    //here it's just assumed that the dendrite with this ID just does not exist... a little bit insecure...
    //many records remain "0". Those values will be calculated afterwards when all spine data have been collected.
    Query = "INSERT INTO dendrites VALUES(\"";
    Query += dendrite_id;                                           //dendrite id
    Query += "\", ";
    Query += _itoa(day);                                            //day
    Query += ", ";
    Query += _ftoa(dendrite_length);                                //dendrite length
    Query += ", ";
    Query += _itoa(stable_spines);                                  //stable spines
    Query += ", ";
    Query += _itoa(gained_spines);                                  //gained spines
    Query += ", 0, ";                                               //lost spines
    Query += _itoa(living_spines);                                  //living spines
    Query += ", ";
    Query += _ftoa((double)(living_spines)/dendrite_length);        //spine density
    Query += ", 0";                                                 //frac survival
    Query += ", 0";                                                 //gained+lost spines
    Query += ", 0";                                                 //transient spines
    Query += ", 0, ";                                               //TOR
    Query += _itoa(filopodia);                                      //filopodia
    Query += ", ";
    Query += _itoa(mushrooms);                                      //mushrooms
    Query += ", ";
    Query += _itoa(thins);                                          //thins
    Query += ", ";
    Query += _itoa(stubbies);                                       //stubbies
    Query += ", 0";                                                 //newgained survivqal
    Query += ", 0";                                                 //gained filopodia
    Query += ", 0";                                                 //gained mushrooms
    Query += ", 0";                                                 //gained thins
    Query += ", 0";                                                 //gained stubbies
    Query += ", 0";                                                 //stable filopodia
    Query += ", 0";                                                 //stable mushrooms
    Query += ", 0";                                                 //stable thins
    Query += ", 0";                                                 //stable stubbies
    Query += ", 0";                                                 //lost filopodia
    Query += ", 0";                                                 //lost mushrooms
    Query += ", 0";                                                 //lost thins
    Query += ", 0";                                                 //lost stubbies
    Query += ", 0";                                                 //gained transient filopodia
    Query += ", 0";                                                 //gained transient mushrooms
    Query += ", 0";                                                 //gained transient thins
    Query += ", 0";                                                 //gained transient stubbies
    Query += ", 0";                                                 //stable transient filopodia
    Query += ", 0";                                                 //stable transient mushrooms
    Query += ", 0";                                                 //stable transient thins
    Query += ", 0";                                                 //stable transient stubbies
    Query += ", 0";                                                 //lost transient filopodia
    Query += ", 0";                                                 //lost transient mushrooms
    Query += ", 0";                                                 //lost transient thins
    Query += ", 0";                                                 //lost transient stubbies
    Query += ", 0";                                                 //shapeshifting frequency
    Query += ", 0, \"";                                               //regained spines
    Query += ovl_filename;                                          //file
    Query += "\");";
/*
    Query += "0, 0, 0, ";
    Query += _itoa(filopodia);
    Query += ", ";
    Query += _ftoa(100 * (double)filopodia / living_spines);
    Query += ", ";
    Query += _itoa(mushrooms);
    Query += ", ";
    Query += _ftoa(100 * (double)mushrooms / living_spines);
    Query += ", ";
    Query += _itoa(thins);
    Query += ", ";
    Query += _ftoa(100 * (double)thins / living_spines);
    Query += ", ";
    Query += _itoa(stubbies);
    Query += ", ";
    Query += _ftoa(100 * (double)stubbies / living_spines);
    Query += ", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \"";
    Query += ovl_filename;
    Query += "\");";*/
    db.Query(Query);
    //_stdoutput << "\rInfo> Added " << sum_dendrites << " dendrites and " << sum_spines << " spines ...";

    return sum_spines;
}
