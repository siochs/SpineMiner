/*
* SpineMiner 1.0
* Ochs SM 2013
* In line with the Gnu Public Licence I agree with redistribution and modification of my code as long as you do no harm.
*
* Requires: MinGW32 compiler (inst-20120426)
*   g++ (GCC): 4.6.2
*   gcc (GCC): 4.6.2
*   linker options: -lgdi32
*
* This project was created using Code::Blocks 12.11 for Windows.
* Use the SpineMiner.cbp to build.
* Otherwise you may create SpineMiner from scratch or use build.bat:
*   g++.exe -Wall -fexceptions -g -c database.cpp -o obj\Debug\database.o
*   g++.exe -Wall -fexceptions -g -c dbscan.cpp -o obj\Debug\dbscan.o
*   g++.exe -Wall -fexceptions -g -c functions.cpp -o obj\Debug\functions.o
*   g++.exe -Wall -fexceptions -g -c helpers.cpp -o obj\Debug\helpers.o
*   gcc.exe -Wall -fexceptions -g -c kdtree\kdtree.c -o obj\Debug\kdtree.o
*   g++.exe -Wall -fexceptions -g -c main.cpp -o obj\Debug\main.o
*   g++.exe -Wall -fexceptions -g -c ovlreader.cpp -o obj\Debug\ovlreader.o
*   gcc.exe -Wall -fexceptions -g -c sqlite\sqlite3.c -o obj\Debug\sqlite3.o
*   g++.exe  -o bin\Debug\SpineMiner.exe obj\Debug\database.o obj\Debug\dbscan.o obj\Debug\functions.o obj\Debug\helpers.o obj\Debug\kdtree.o obj\Debug\main.o obj\Debug\ovlreader.o obj\Debug\sqlite3.o -lgdi32
*
*
* Usage:
* SpineMiner.exe                                                  --> tests the ovl files in the current working directory for consistency
* SpineMiner.exe -w:<filename>                                    --> tests the ovl files in the current working directory for consistency and reads content into database <filename>
* SpineMiner.exe -w:<filename> -ignoremorphologies                --> use this, if your spines have no morphology tags
* SpineMiner.exe -w:<filename> -nofilopodia                       --> all as filopodia tagged spines will be treated as thin spines
*
* SpineMiner.exe -r:<filename>                                    --> reads the database and showing dendrites and spines table
* SpineMiner.exe -r:<filename> -calcsurvival:<imaging_timepoint>  --> recalculates spine survival using <imaging_timepoint> as start (default = 1)
* SpineMiner.exe -r:<filename> -calctransients:<period>           --> recalculates transient spines by a given lifespan period (days) (default = 8)
* SpineMiner.exe -r:<filename> -calcnewgained:<imaging_timepoint> --> recalculates the survival of spines which were gained at <imaging_timepoint> (default = 2)
* SpineMiner.exe -r:<filename> -survival:<id>                     --> shows survival courve for a given dendrite id
* SpineMiner.exe -r:<filename> -trace:<dendrite_id>,<spine_id>    --> shows the fate of a spine, identified by <dendrite_id> and <spine_id>
* SpineMiner.exe -r:<filename> -check                             --> tests the database for spine counting errors.
* SpineMiner.exe -r:<filename> -tor:<dendrite_id>                 --> shows the turnover rate of a dendrite identified by <dendrite_id>
* SpineMiner.exe -r:<filename> -pipe:<filename2>                  --> Executes SQL commands in <filename2>. Results are stored in <filename2>.result.txt
* SpineMiner.exe -r:<filename> -dbscan:<dendrite_id>,<epsilon>,<minPts>
*                                                           --> when * is used as dendrite_id all dendrites will calced with given eps and minPts
* SpineMiner.exe -r:<filename> -quickdbscan                       --> runs a dbscan on all dendrites unsurvived.
*
* SpineMiner.exe -replace:<findstring>,<replacestring>            --> find & replace for all ovl files in current working directory
*
* SpineMiner.exe -c:<filename>                                    --> converts the database into csv format so it's readable for excel
*
* SpineMiner.exe -q:<filename>                                    --> Open SQL session. Quit with "!q"
*
* SpineMiner.exe -outputtocerr                                    --> Changes standard output to cerr. File logging will be disabled
*/

#include <stdio.h>
#include <string>
#include <iostream>
#include <dir.h>

#include "ioswitch.h"
#include "helpers.h"
#include "database.h"
#include "functions.h"

#define VERSION_STR "Info> SpineMiner 1.1 (Simon Ochs 2013)."

using namespace std;

std::ofstream _stdoutput;

//#include "functions.cpp"

int main(int argc, char *argv[])
{
    // output normally cout
    _stdoutput.copyfmt(std::cout);
    _stdoutput.clear(std::cout.rdstate());
    _stdoutput.basic_ios<char>::rdbuf(std::cout.rdbuf());

    //Everything on _stdoutput will be logged in log.txt. See ioswitch.hpp
    //DeleteFile("log.txt");
    remove("log.txt");

    BEGIN_IOSWITCH;

    //string params[argc];    //params from command line
    vector<string> params;
    char cwd[256];          //current working directory storage

    _stdoutput << endl;
    _stdoutput << VERSION_STR << endl;

    time_t tmStartTime;
    time(&tmStartTime);

    if (argc > 1)
    {
        //trascribe parameters to string list
        for (int p = 0; p < argc; p++)
        {
            //everything will be piped through cerr, when -outputtocerr option is given
            //this is needed when buffering of standard output is unwished, e.g. when interfacing with windows api
            if ( strcmp("-outputtocerr" , argv[p]) == 0 )
            {
                _stdoutput.copyfmt(std::cerr);
                _stdoutput.clear(std::cerr.rdstate());
                _stdoutput.basic_ios<char>::rdbuf(std::cerr.rdbuf());
                _stdoutput << "Info> Output will be bypassed to std::cerr. File logging disabled." << endl;
            }
            else
            {
                params.push_back(argv[p]);
            }

            //params[p] = argv[p];
            //is param -outputtocerr?
        }

        argc = params.size();

        //parse parameters
        if (params[1].find("-w:")!=string::npos)
        {
            params[1].replace(0,3,"");

            if (params[1] != "" && _FileExists(params[1]) == false)
            {
                if (argc == 3 && params[2].find("-ignoremorphologies")!=string::npos) NewAnalysis(params[1], true, false);
                else
                if (argc == 3 && params[2].find("-nofilopodia")!=string::npos) NewAnalysis(params[1], false, true);
                else
                NewAnalysis(params[1], false, false);
            }
            else
            {
                _stdoutput << "Error> The database file \"" << params[1] << "\" already exists or the specified name is not valid. " << endl;
                return 0;
            }
        }
        else
        if (params[1].find("-r:")!=string::npos && argc == 2)
        {
            params[1].replace(0,3,"");
            //only -r option
            if (params[1] != "" && _FileExists(params[1]) == true)
            {
                ShowUsage();
                Database db;
                db.Open(params[1]);
                //ShowTableSpines(db);
                //ShowTableDendrites(db);
                if (!CheckFates(db))
                {
                    _stdoutput << "Error> Wrong spine fate assignments. Please check your overlays." << endl;
                }
                else _stdoutput << "Info> Database test OK. No errors in spine fates found." << endl;
                db.Close();
            }
            else
            {
                _stdoutput << "Error> The database file \"" << params[1] << "\" does not exist or the specified name is not valid. " << endl;
                return 0;
            }
        }
        else
        if (params[1].find("-r:")!=string::npos && argc == 3)
        {
            params[1].replace(0,3,"");
            if (params[1] != "" && _FileExists(params[1]) == true)
            {
                if(params[2].find("-calcsurvival:")!=string::npos)
                {
                    params[2].replace(0,14,"");
                    if (params[2] != "" && _IsInt(params[2]))
                    {
                        Database db;
                        db.Open(params[1]);
                        CalcSurvival(db, atoi(params[2].c_str()) - 1);
                        db.Close();

                    }
                    else
                    {
                        _stdoutput << "Error> -calcsurvival parameter is not an integer or empty."<<endl;
                        return 0;
                    }
                }
                else
                if (params[2].find("-calctransients:")!=string::npos)
                {
                    params[2].replace(0,16,"");
                    if (params[2] != "" && _IsInt(params[2]))
                    {
                        _stdoutput << "Info> Recalculation of transient spines where transient spines survive " << params[2] << " days max." << endl;
                        Database db;
                        db.Open(params[1]);
                        CalcTransients(db, atoi(params[2].c_str()));
                        db.Close();
                    }
                    else
                    {
                        _stdoutput << "Error> -calctransients parameter is not an integer or empty."<<endl;
                        return 0;
                    }
                }
                else
                if(params[2].find("-calcnewgained:")!=string::npos)
                {
                    params[2].replace(0,15,"");
                    if (params[2] != "" && _IsInt(params[2]))
                    {
                        Database db;
                        db.Open(params[1]);
                        CalcNewGainedSurvival(db, atoi(params[2].c_str()) - 1);
                        db.Close();

                    }
                    else
                    {
                        _stdoutput << "Error> -calcnewgained parameter is not an integer or empty."<<endl;
                        return 0;
                    }
                }
                else
                if (params[2].find("-survival:")!=string::npos)
                {
                    params[2].replace(0,10,"");
                    if (params[2] != "")
                    {
                        Database db;
                        db.Open(params[1]);
                        PrintSurvival(params[2], db);
                        db.Close();
                    }
                    else
                    {
                        _stdoutput << "Error> -survival parameter is invalid."<<endl;
                        return 0;
                    }
                }
                else
                if (params[2].find("-trace:")!=string::npos)
                {
                    params[2].replace(0,7,"");
                    string dendrite = params[2].substr(0, params[2].find(","));
                    string spine = params[2].substr(params[2].find(",")+1, params[2].length());
                    if (dendrite != "" && spine != "")
                    {
                        Database db;
                        db.Open(params[1]);
                        TraceFate(spine, dendrite, db, true);
                        db.Close();
                    }
                    else
                    {
                        _stdoutput << "Error> at leat one parameter -trace option is invalid."<<endl;
                        return 0;
                    }
                }
                else
                if (params[2].find("-dbscan:")!=string::npos)
                {
                    params[2].replace(0,8,"");
                    string dendrite = params[2].substr(0, params[2].find(","));
                    params[2].replace(0,dendrite.length()+1,"");
                    string eps = params[2].substr(0, params[2].find(","));
                    params[2].replace(0,eps.length()+1,"");
                    string minPts = params[2].substr(0, params[2].length());

                    //got the parameters. check them
                    //dendrite is not allowed to be empty
                    //eps must be double and > 0
                    //minPts must be int and > 1

                    if (dendrite == "")
                    {
                        _stdoutput << "Error> dendrite parameter for -dbscan option is invalid."<<endl;
                        return 0;
                    }

                    if (!_IsDouble(eps) || _atof(eps) < 0.01)
                    {
                        _stdoutput << "Error> eps parameter for -dbscan option is invalid."<<endl;
                        return 0;
                    }

                    if(!_IsInt(minPts) || atoi(minPts.c_str()) <= 1)
                    {
                        _stdoutput << "Error> minPts parameter for -dbscan option is invalid."<<endl;
                        return 0;
                    }

                    //all ok
                    Database db;
                    DBScan Scan;
                    db.Open(params[1]);
                    CalcClusterFromPrompt(db, Scan, dendrite, _atof(eps), atoi(minPts.c_str()), true);
                    db.Close();
                }
                else
                if (params[2].find("-check")!=string::npos)
                {
                        Database db;
                        db.Open(params[1]);
                        if (!CheckFates(db)) _stdoutput << "Error> Wrong spine fate assignments. Please recheck your overlays to avoid statistical errors." << endl;
                        else _stdoutput << "Info> Database clean. No errors in spine fates found." << endl;
                        db.Close();
                }
                else
                if (params[2].find("-quickdbscan")!=string::npos)
                {
                        Database db;
                        DBScan Scan;
                        db.Open(params[1]);
                        PreCalcClusters(db, Scan, true);
                        db.Close();
                }
                else
                if (params[2].find("-tor:")!=string::npos)
                {
                    params[2].replace(0,5,"");
                    if (params[2] != "")
                    {
                        Database db;
                        db.Open(params[1]);
                        PrintTor(params[2], db);
                        db.Close();
                    }
                    else
                    {
                        _stdoutput << "Error> -tor parameter is invalid."<<endl;
                        return 0;
                    }
                }
                else
                if (params[2].find("-pipe:")!=string::npos)
                {
                    params[2].replace(0,6,"");
                    if (params[2] != "")
                    {
                        string oname = params[2];
                        oname += ".result.txt";
                        if (_FileExists(params[2]) == false || _FileExists(oname) == true)
                        {
                            _stdoutput << "Error> File \"" << params[2] << "\" does not exist or \"" << oname << "\" already exists." << endl;
                            return 0;
                        }

                        Database db;
                        db.Open(params[1]);
                        SQLPipe(params[2], db);
                        db.Close();
                    }
                    else
                    {
                        _stdoutput << "Error> -pipe parameter is invalid."<<endl;
                        return 0;
                    }
                }
                else
                {
                    _stdoutput << "Error> Invalid option for parameter \"-r:\" " << params[2] << endl;
                    return 0;
                }

            }
            else
            {
                _stdoutput << "Error> The database file \"" << params[1] << "\" does not exist or the specified name is not valid. " << endl;
                return 0;
            }
        }
        else
        if(params[1].find("-c:")!=string::npos && argc == 2)
        {
            params[1].replace(0,3,"");
            if (params[1] != "" && _FileExists(params[1]) == true)
            {
                Database db;
                db.Open(params[1]);
                Convert(db, params[1]);
                db.Close();
            }
            else
            {
                _stdoutput << "Error> The database file \"" << params[1] << "\" does not exist or the specified name is not valid. " << endl;
                return 0;
            }
        }
        else
        if(params[1].find("-q:")!=string::npos && argc == 2)
        {
            params[1].replace(0,3,"");
            if (params[1] != "" && _FileExists(params[1]) == true)
            {
                Database db;
                db.Open(params[1]);
                db.SQL();
                db.Close();
            }
            else
            {
                _stdoutput << "Error> The database file \"" << params[1] << "\" does not exist or the specified name is not valid. " << endl;
                return 0;
            }

        }
        else
        if (params[1].find("-replace:")!=string::npos)
        {
            params[1].replace(0,9,"");
            string find = params[1].substr(0, params[1].find(","));
            string replace = params[1].substr(params[1].find(",")+1, params[1].length());
            if (find != "" && replace != "" && find != replace)
            {
               FindReplace(find, replace);
            }
            else
            {
                _stdoutput << "Error> at leat one parameter -replace option is invalid or same."<<endl;
                return 0;
            }
        }
        else
        {
            _stdoutput << "Error> Parameter not recognized." << endl;
            return 0;
        }
    }
    else
    {
        //no arguments received
        ScanOnly();
        ShowUsage();
    }

    getcwd(cwd, 256);
    time_t lTimeNow;
    time(&lTimeNow);
    time_t tsDuration = lTimeNow - tmStartTime;
    _stdoutput << endl <<  "Info> Done in " << cwd << " after " << tsDuration << " seconds ";
    if(tsDuration < 30) _stdoutput << ":-)";
    if(tsDuration < 120 && tsDuration > 30) _stdoutput << ":-|";
    if(tsDuration > 120) _stdoutput << ":-(";
    _stdoutput << endl;

    END_IOSWITCH;

    return 1;
}
