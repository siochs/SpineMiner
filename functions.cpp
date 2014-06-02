#include "functions.h"

using namespace std;
using namespace cimg_library;
extern std::ofstream _stdoutput;

//hope we don't have more then 48 clusters
#define MAX_COLORS  37
const unsigned char col[MAX_COLORS][3] =
        {
            {190,190,190},  //gray = other spines
            {0,0,0},        //black = noise
            {0,0,255},      //blue
            {0,255,0},      //green
            {255,255,0},    //yellow
            {255,0,0},      //red
            {160,32,240},   //purple
            {139,35,35},    //brown
            {0,139,139},    //cyan
            {0,0,255},      //blue
            {0,255,0},      //green
            {255,255,0},    //yellow
            {255,0,0},      //red
            {160,32,240},   //purple
            {139,35,35},    //brown
            {0,139,139},    //cyan
            {0,0,255},      //blue
            {0,255,0},      //green
            {255,255,0},    //yellow
            {255,0,0},      //red
            {160,32,240},   //purple
            {139,35,35},    //brown
            {0,139,139},    //cyan
            {0,0,255},      //blue
            {0,255,0},      //green
            {255,255,0},    //yellow
            {255,0,0},      //red
            {160,32,240},   //purple
            {139,35,35},    //brown
            {0,139,139},    //cyan
            {0,0,255},      //blue
            {0,255,0},      //green
            {255,255,0},    //yellow
            {255,0,0},      //red
            {160,32,240},   //purple
            {139,35,35},    //brown
            {0,139,139},    //cyan
        };


void ShowUsage()
{
    _stdoutput << "Info> You may proceed using the following options. All outputs will be also available in \"log.txt\":" << endl;
    _stdoutput << "SpineMiner.exe --> tests the ovl files in the current working directory for consistency." << endl;
    _stdoutput << "\t-w:<filename> --> builds database from OVL files in the specified <filename>." << endl;
    _stdoutput << "\t-w:<filename> -ignoremorphologies --> use this, if your spines have no morphology tags." << endl;
    _stdoutput << "\t-w:<filename> -nofilopodia --> filopodia like spines will be counted as thin likes." << endl;
    _stdoutput << endl;
    _stdoutput << "\t-r:<filename> --> test database integrity and shows command line options." << endl;
    _stdoutput << "\t-r:<filename> -calcsurvival:<imaging_timepoint> --> spine survival will be recalculated using <imaging_timepoint> as start (default = 1)." << endl;
    _stdoutput << "\t-r:<filename> -calcnewgained:<imaging_timepoint> --> recalculates the survival of spines which were gained at <imaging_timepoint> (default = 2)" << endl;
    _stdoutput << "\t-r:<filename> -calctransients:<max_lifetime> --> redetection of transient spines by a defined <max_lifetime> (default <= 8)." << endl;
    _stdoutput << "\t-r:<filename> -survival:<dendrite_id> --> shows a foretaste of a survival courve for a specified <dendrite_id>." << endl;
    _stdoutput << "\t-r:<filename> -trace:<dendrite_id>,<spine_id> --> shows the spine fate of specified by its <dendrite_id> and <spine_id>." << endl;
    _stdoutput << "\t-r:<filename> -check --> test database integrity." << endl;
    _stdoutput << "\t-r:<filename> -tor:<dendrite_id> --> shows the turnover rate of a dendrite specified by <dendrite_id>." << endl;
    _stdoutput << "\t-r:<filename> -dbscan:<dendrite_id>,<epsilon>,<minPts> --> runs a dbscan on a specified <dendrite_id> (use \"*\" for all dendrites) using <epsilon> and <minPts> as algorithm parameters." << endl;
    _stdoutput << "\t-r:<filename> -quickdbscan --> runs a dbscan on all dendrites unsurvived." << endl;
    _stdoutput << "\t-r:<filename> -pipe:<sqlfile> --> batch processing of SQL statements stored in the specified <sqlfile>. The results will be stored in <sqlfile>.result.txt." << endl;
    _stdoutput << endl;
    _stdoutput << "\t-q:<filename> --> Opens a SQL session to accept statements. Quit with \"!q\"" << endl;
    _stdoutput << endl;
    _stdoutput << "\t-replace:<findstring>,<replacestring> --> find & replace for all ovl files in current working directory. Warning: function may causes data corruption (beta status)." << endl;
    _stdoutput << endl;
    _stdoutput << "\t-c:<filename> --> converts the database into a CSV-file which also contains dendrite, spine and spacial characteristics." << endl;
    _stdoutput << endl;
    _stdoutput << "\t-outputtocerr --> Output is directed to cerr. File logging will be deactivated." << endl;

    return;
}


void FindReplace(string find, string replace)
{
    //go through all ovl files.
    OVLReader O;
    vector <string> filenames;
    fstream is, os;
    string tmp;
    bool replaced = false;

    union b2i                   //byte to integer "converter"
    {
        int integer;
        char byte[3];
    } c;

    filenames = _FindFilesInCWD(".ovl");
//    for (int i = 0; i < filenames.size(); i++) _stdoutput << filenames[i] << endl;


    //maximum length of 20 chars allowed
    if (find.length() > 20 || replace.length() > 20)
    {
        _stdoutput << "Error> Find- or replace string > 20 characters. Stopped." << endl;
        return;
    }

    //hope we got files...
    if (filenames.size() == 0)
    {
        _stdoutput << endl << O.LastError.str();
        _stdoutput << "Info> Spinecounter stopped." << endl;
        return;
    }

    //for each file in list...
    for (vInt n = 0; n < filenames.size(); n++)
    {
        is.open(filenames[n].c_str(), ios::binary | ios::in );
        if (is.fail() || os.fail())
        {
            _stdoutput << "Error> Cannot open file " << filenames[n] << endl;
            return;
        }

        //get file size
        is.seekg(0, ios::end);
        size_t fileSize = is.tellg();
        is.seekg(0, ios::beg);

        //create byte array having the same size as the file
        vector<char> data(fileSize, 0);
        vector<char> vfind;
        vector<char> vreplace;

        //load file content into byte array
        is.read(reinterpret_cast<char*>(&data[0]), fileSize);

        //expand the find string with '0x00' after each char
        for (size_t i = 0; i < find.length(); i++)
        {
            vfind.push_back(find[i]);
            vfind.push_back(0x00);
        }
        vfind.push_back(0x00);

        //expand the replace string with '0x00' after each char
        for (size_t i = 0; i < replace.length(); i++)
        {
            vreplace.push_back(replace[i]);
            vreplace.push_back(0x00);
        }
        vreplace.push_back(0x00);

        //create temporary output file
        tmp = filenames[n];
        tmp += ".out.ovl";
        os.open(tmp.c_str(), ios::binary | ios::out );

        //now search for the pattern in data array
        std::vector<char>::iterator it;
        it = std::search (data.begin(), data.end(), vfind.begin(), vfind.end()); //on hit, iterator points on a position != data.end()
        while (it!=data.end())
        {
            int pos = it-data.begin(); //calc position from iterator
            data.erase(it, it+vfind.size()); //delete from array
            //formula how data is stored at 0x46: Per TextBox: C8 + (2 + (2*N)) <- N = number of letters
            //data[0x46] = data[0x46] - (vfind.size() + 1); //store at 0x46 that vfind was erased
            //assuming this is an integer: load integer form byte array
            c.byte[0] = data[0x46];
            c.byte[1] = data[0x47];
            c.byte[2] = data[0x48];
            c.byte[3] = data[0x49];
            c.integer = c.integer - (vfind.size() + 1); //change integer by the value it was stripped
            data[0x46] = c.byte[0]; //store zhe integer in the byte array
            data[0x47] = c.byte[1];
            data[0x48] = c.byte[2];
            data[0x49] = c.byte[3];

            data.insert(it, vreplace.begin(), vreplace.end()); //make the insert operation of the replace string in array

            //data[0x46] = data[0x46] + (vreplace.size() + 1); //store at 0x46 that vfind was added
            c.byte[0] = data[0x46]; //load again integer from byte array
            c.byte[1] = data[0x47];
            c.byte[2] = data[0x48];
            c.byte[3] = data[0x49];
            c.integer = c.integer + (vreplace.size() + 1); //change integer by value it was expanded
            data[0x46] = c.byte[0]; //store back in byte array
            data[0x47] = c.byte[1];
            data[0x48] = c.byte[2];
            data[0x49] = c.byte[3];

            //also remark at position pos - 252 the new letter count:
            data[pos - 252] = vreplace.size() + 1;
            _stdoutput << "Info> \"" << find << "\" in \"" << filenames[n] <<"\" at position " << pos << " was replaced with \"" << replace << "\"" << endl;

            //search again
            it = std::search (data.begin(), data.end(), vfind.begin(), vfind.end());
            replaced = true;
        }

        if (replaced == false)
        {
            _stdoutput << "Info> No match for \"" << find << "\" found in \"" << filenames[n] <<"\". Skipped." << endl;
        }

        os.write(reinterpret_cast<char*>(&data[0]), data.size()); //store data array back to tmpfile
        is.close(); os.close();

        remove(filenames[n].c_str()); //rename tmpfile
        rename(tmp.c_str(), filenames[n].c_str());

        if (is.is_open()) is.close();
        if (os.is_open()) os.close();

    }
    return;
}

//You may pipe a set of SQL queries from filename to the database <- plugins for SpineMiner are piped through here
void SQLPipe(string filename, Database &db)
{
    db.protect_db = true;
    dbResults r;
    string tmp;

    //the output file will be <sqlfile>.result.txt
    fstream o;
    tmp = filename;
    tmp += ".result.txt";
    o.open(tmp.c_str(), ios::out);

    fstream i;
    tmp = filename;
    i.open(tmp.c_str(), ios::in);

    string line;
    while(getline(i,line))
    {
        if (line == "") continue;
        r=db.Query(line);
        for (vInt i = 0; i < r.size(); i++)
        {
            for (vInt j = 0; j < r[i].size(); j++)
                o << r[i][j] << "\t";

            o << endl;
        }
    }

    o.close();
    i.close();

    _stdoutput << "Info> SQL pipe complete. Results stored in \"" << filename << ".result.txt\"." << endl;
    db.protect_db = false;
    return;
}

//exits this program
void Quit()
{
    _stdoutput << endl << endl << "--- ABORTED ANALYSIS --- " << endl << endl;
    //file.close();
    //Sleep(1000);
    exit(-1);

    return;
}

//Recalculates all values which need to be calculated from other column values.
//Needed when changes have been adapted
void UpdateDB(Database &db)
{
    /*
    _stdoutput << "Info> Updating database...";
    db.Query(""
    "UPDATE dendrites SET frac_transient_spines = 100 * (CAST(transient_spines AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_gained_filopodia = 100 * (CAST(gained_filopodia AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_gained_mushrooms = 100 * (CAST(gained_mushrooms AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_gained_thins = 100 * (CAST(gained_thins AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_gained_stubbies = 100 * (CAST(gained_stubbies AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_lost_filopodia = 100 * (CAST(lost_filopodia AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_lost_mushrooms = 100 * (CAST(lost_mushrooms AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_lost_thins = 100 * (CAST(lost_thins AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_lost_stubbies = 100 * (CAST(lost_stubbies AS DOUBLE)) / (gained_spines + stable_spines), "

    "frac_gained_transient_filopodia = 100 * (CAST(gained_transient_filopodia AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_gained_transient_mushrooms = 100 * (CAST(gained_transient_mushrooms AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_gained_transient_thins = 100 * (CAST(gained_transient_thins AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_gained_transient_stubbies = 100 * (CAST(gained_transient_stubbies AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_lost_transient_filopodia = 100 * (CAST(lost_transient_filopodia AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_lost_transient_mushrooms = 100 * (CAST(lost_transient_mushrooms AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_lost_transient_thins = 100 * (CAST(lost_transient_thins AS DOUBLE)) / (gained_spines + stable_spines), "
    "frac_lost_transient_stubbies = 100 * (CAST(lost_transient_stubbies AS DOUBLE)) / (gained_spines + stable_spines)"

     ";");
    _stdoutput << "." << endl;
    */
    return;
}

//a spine is seen as regained when it appears at the same location again. Note: this necessarily is NOT the same spine. You won't be able to proof that
void CalcRegainedSpines(Database &db)
{
   _stdoutput << "Info> Calculating regained spines." << endl;

   string Query;
   string fate = "";
   //int pos = 0;
   //int day_regained = 0;
   dbResults r_allgained, r_days, r_ids, r_waslost;

   //get all id_s
   r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites;");
   for (vInt j = 0; j < r_ids.size(); j++)
   {
        //go through all ids
        //get timeline of this id
        Query = "SELECT day FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[j][0];
        Query += "\" ORDER BY DAY;";
        r_days = db.Query(Query);
        //for (int k = r_days.size(); k >= 0; k--)
        for (vInt k = 0; k < r_days.size(); k++)
        {
            //go through all days backwards
            //get all gained spines
            Query = "SELECT spine_id FROM spines WHERE dendrite_id = \"";
            Query += r_ids[j][0];
            Query += "\" AND day = ";
            Query += r_days[k][0];
            Query += " AND spine_type = \"gained\";";
            r_allgained = db.Query(Query);

            //having now all the gained spines for a certain day.
            for (vInt l = 0; l < r_allgained.size(); l++)
            {

                //go through each gained spine. Was it lost before?
                Query = "SELECT spine_type, day FROM spines WHERE dendrite_id = \"";
                Query += r_ids[j][0];
                Query += "\" AND day < ";
                Query += r_days[k][0];
                Query += " AND spine_id = \"";
                Query += r_allgained[l][0];
                Query += "\" ORDER BY day DESC LIMIT 1;";
                r_waslost = db.Query(Query);

                if (r_waslost.size() > 0 && r_waslost[0][0] == "lost")
                {
                    //mark the spine as regained from now on and in future.
                    //you need to set this spine as regained also at later timepoints because this is needed for survival calculation, which excludes regained spines
                    //why? because a spine which is gained at the same position is not necessarily the same spine which was lost before
                    //discuss this issue with the scientists in this field if you like. For me this is just playing...
                    Query = "UPDATE spines SET is_regained = 1 WHERE dendrite_id = \"";
                    Query += r_ids[j][0];
                    Query += "\" AND spine_id = \"";
                    Query += r_allgained[l][0];
                    Query += "\" AND day >= ";
                    Query += r_days[k][0];
                    Query += ";";
                    db.Query(Query);


                    //TODO: calc and add latency time of this spine...

                    //now update the regained_spine count in dendrites
                    Query = "UPDATE dendrites SET regained_spines = regained_spines + 1 WHERE dendrite_id = \"";
                    Query += r_ids[j][0];
                    Query += "\" AND day = ";
                    Query += r_days[k][0];
                    Query += ";";
                    db.Query(Query);
                    _stdoutput << "Info> Marked spine \"" << r_allgained[l][0] << "\" from dendrite \"" << r_ids[j][0] << "\" gained at day "<< r_days[k][0] <<" as regained spine." << endl;
                }
                else
                continue;
            }
        }
    }

    return;
}

//shapshifting frequency is the percentage of spines that change their morphology per day
//detects first the amout of spines that change their shape
//the value is then divided by the amount of total living spines * 100 (=%)
//the value ist then divided by the days between the timepoints
//=> % of spine-shape-changes per day
void CalcShapeshiftingFrequency(Database &db)
{
    string Query;
    dbResults r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites;");
    dbResults r_days, r_spine_ids, r_tmp;
    int counter = 0;
    double frequency = 0;
    //db.QueryCount = 0;

    //erasing the table
    db.Query("UPDATE dendrites SET shapeshifting_frequency = NULL;");

    //for each id
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        //get all the days
        Query = "SELECT DISTINCT day, gained_spines, stable_spines FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY day;";
        r_days = db.Query(Query);

        //start with timepoint 1, not 0, because timepoint -1 does not exist and would cause error!
        //and only if there are more than two entries
        if (r_days.size() > 1)
        {
            //for each day. Start with timepoint 1
            for (vInt j = 1; j < r_days.size(); j++)
            {
                counter = 0;
                frequency = 0;
                //get all she spine ids which are stable at this timepoint
                Query = "SELECT DISTINCT spine_id, morphology FROM spines WHERE dendrite_id = \"";
                Query += r_ids[i][0];
                Query += "\" AND day = ";
                Query += r_days[j][0];
                Query += " AND spine_type = \"stable\" ORDER BY spine_id;";
                r_spine_ids = db.Query(Query);

                //for each spine
                for (vInt k = 0; k < r_spine_ids.size(); k++)
                {
                    //has the spine had the same morphology before?
                    Query = "SELECT morphology FROM spines WHERE dendrite_id = \"";
                    Query += r_ids[i][0];
                    Query += "\" AND day = ";
                    Query += r_days[j - 1][0];
                    Query += " AND spine_id = \"";
                    Query += r_spine_ids[k][0];
                    Query += "\";";
                    r_tmp = db.Query(Query);
                    if (r_tmp.size() != 0 && r_tmp[0][0] != r_spine_ids[k][1])
                    {
                        counter ++;
                    }
                }

                //fraction of spines which change shapes
                frequency = 100 * (double) counter / (atoi(r_days[j][1].c_str()) + atoi(r_days[j][2].c_str()));
                //per day...
                frequency = frequency / (atoi(r_days[j][0].c_str()) - atoi(r_days[j - 1][0].c_str()));

                Query = "UPDATE dendrites SET shapeshifting_frequency = ";
                Query += _ftoa(frequency);
                Query += " WHERE dendrite_id = \"";
                Query += r_ids[i][0];
                Query += "\" AND day = ";
                Query += r_days[j][0];
                Query += ";";
                db.Query(Query);
                _stdoutput << "\rInfo> Calculating shapeshifting frequency of dendrite " << r_ids[i][0] << " at day " << r_days[j][0] << "   ";
                //_stdoutput << "\rInfo> Performed queries: " << db.QueryCount;
            }

        }
        else
        {
                _stdoutput << "Warning> Cannot calculate the shapeshifting frequency for dendrite_id \"" << r_ids[i][0] << "\". Skipped" << endl;
                continue;
        }

    }

    _stdoutput << endl;

    return;
}

//calc the morphologies of transient spines
/*
void CalcTransientGainedLostMorphs(Database &db)
{
    _stdoutput << "Info> Calculating now which morphologies the transient spines have." << endl;

    db.Query("UPDATE dendrites SET gained_transient_filopodia = 0, gained_transient_mushrooms = 0, gained_transient_thins = 0, gained_transient_stubbies = 0, transient_filopodia = 0, transient_mushrooms = 0, transient_thins = 0, transient_stubbies = 0;");

    //which morphologies have gained transient spines?
    dbResults r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites;");
    dbResults r_days, r_tmp, r_spine_ids;
    string Query;

    db.QueryCount = 0;
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        //get the days
        Query = "SELECT DISTINCT day FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY day;";
        r_days = db.Query(Query);

        //go through each day
        for (vInt j = 0; j < r_days.size(); j++)
        {
            //get gained transient filopodia
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"f\" AND is_transient = 1;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_transient_filopodia = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained transient mushrooms
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"m\" AND is_transient = 1;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_transient_mushrooms = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained transient thins
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"t\" AND is_transient = 1;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_transient_thins = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained transient stubbies
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"s\" AND is_transient = 1;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_transient_stubbies = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get transient filopodia
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"f\" AND is_transient = 1;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET transient_filopodia = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get transient mushrooms
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"m\" AND is_transient = 1;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET transient_mushrooms = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get transient thins
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"t\" AND is_transient = 1;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET transient_thins = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get transient stubbies
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"s\" AND is_transient = 1;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET transient_stubbies = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

        }

        //which morphology have the lost transient spines
        //get a list of transient spines
        Query = "SELECT DISTINCT spine_id FROM spines WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" AND spine_type != \"lost\" AND is_transient = 1";
        r_spine_ids = db.Query(Query);
        dbResults r_fate;

        //now go through each spine
        for (vInt m = 0; m < r_spine_ids.size(); m++)
        {
            //get the spine fate
            Query = "SELECT day, spine_type, morphology, is_transient FROM spines WHERE spine_id = \"";
            Query += r_spine_ids[m][0];
            Query += "\" AND dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" ORDER BY day;";
            r_fate = db.Query(Query);

            //go through fate
            Query = "UPDATE dendrites SET ";

            for (vInt n = 0; n < r_fate.size(); n++)
            {
                if(r_fate[n][3] == "1") //is this spine now transient?
                {
                    //does it exist next day?
                    if(n + 1 < r_fate.size() && r_fate[n+1][3] == "0")
                    {
                        if(r_fate[n+1][2] == "f") Query += "lost_transient_filopodia = lost_transient_filopodia + 1";
                        if(r_fate[n+1][2] == "m") Query += "lost_transient_mushrooms = lost_transient_mushrooms + 1";
                        if(r_fate[n+1][2] == "t") Query += "lost_transient_thins = lost_transient_thins + 1";
                        if(r_fate[n+1][2] == "s") Query += "lost_transient_stubbies = lost_transient_stubbies + 1";
                        Query += " WHERE dendrite_id = \"";
                        Query += r_ids[i][0];
                        Query += "\" AND day = ";
                        Query += r_fate[n+1][0];
                        Query += ";";
                        db.Query(Query);
                    }
                    else //still there next day or end of fate reached
                        continue;
                }
                else
                    continue;
            }

        }

*/
       /*
        //get the spine_ids for each dendrite
        Query = "SELECT DISTINCT spine_id FROM spines WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\";";
        r_spine_ids = db.Query(Query);

        //for each spine id, give the morphology when lastly seen alive
        for (vInt k = 0; k < r_spine_ids.size(); k ++)
        {
            Query = "SELECT morphology, day FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND spine_id = \"";
            Query += r_spine_ids[k][0];
            Query += "\" AND is_transient = 1 AND spine_type = \"lost\" ORDER BY day LIMIT 1;";
            r_tmp = db.Query(Query);
            if (r_tmp.size() > 0)
            {
                Query = "UPDATE dendrites SET ";
                if (r_tmp[0][0] == "f") Query += "lost_transient_filopodia = lost_transient_filopodia + 1";
                if (r_tmp[0][0] == "m") Query += "lost_transient_mushrooms = lost_transient_mushrooms + 1";
                if (r_tmp[0][0] == "t") Query += "lost_transient_thins = lost_transient_thins + 1";
                if (r_tmp[0][0] == "s") Query += "lost_transient_stubbies = lost_transient_stubbies + 1";

                Query += " WHERE dendrite_id = \"";
                Query += r_ids[i][0];
                Query += "\" AND day = ";
                Query += r_tmp[0][1];
                Query += ";";
                db.Query(Query);
                //_stdoutput << Query << endl;
            }
        }
        */

/*
        _stdoutput << "\rInfo> Performed queries: " << db.QueryCount;
    }

    _stdoutput << endl;

    return;
}*/

//calcs morphs of gained, l
/*
void CalcGainedLostMorphs(Database &db)
{
    _stdoutput << "Info> Calculating now which morphologies the spines have." << endl;

    //do we need to reset the columns here?

    //which morphologies have the spines?
    dbResults r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites;");
    dbResults r_days, r_tmp, r_spine_ids;
    string Query;

    db.QueryCount = 0;
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        //get the days
        Query = "SELECT DISTINCT day FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY day;";
        r_days = db.Query(Query);

        //go through each day
        for (vInt j = 0; j < r_days.size(); j++)
        {
            //get gained filopodia
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"f\";";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_filopodia = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained mushrooms
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"m\";";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_mushrooms = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained thins
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"t\";";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_thins = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained stubbies
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"s\";";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_stubbies = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //persistant:
            //get gained filopodia
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"f\" AND is_transient = 0;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET persistent_filopodia = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained mushrooms
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"m\" AND is_transient = 0;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET persistent_mushrooms = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained thins
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"t\" AND is_transient = 0;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET persistent_thins = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained stubbies
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"s\" AND is_transient = 0;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET persistent_stubbies = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);
        }

        //which morphology have the lost spines
        //get a list of all spine ids
        Query = "SELECT DISTINCT spine_id FROM spines WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\";";
        r_spine_ids = db.Query(Query);
        dbResults r_fate;

        //now go through each spine
        for (vInt m = 0; m < r_spine_ids.size(); m++)
        {
            //get the spine fate
            Query = "SELECT day, spine_type, morphology, is_transient FROM spines WHERE spine_id = \"";
            Query += r_spine_ids[m][0];
            Query += "\" AND dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" ORDER BY day;";
            r_fate = db.Query(Query);

            //go through fate
            Query = "UPDATE dendrites SET ";

            for (vInt n = 0; n < r_fate.size(); n++)
            {
                if(r_fate[n][1] != "lost") //if this spine exists
                {
                    //but is lost the next day
                    if(n + 1 < r_fate.size() && r_fate[n+1][1] == "lost")
                    {
                        if(r_fate[n+1][2] == "f") Query += "lost_filopodia = lost_filopodia + 1";
                        if(r_fate[n+1][2] == "m") Query += "lost_mushrooms = lost_mushrooms + 1";
                        if(r_fate[n+1][2] == "t") Query += "lost_thins = lost_thins + 1";
                        if(r_fate[n+1][2] == "s") Query += "lost_stubbies = lost_stubbies + 1";
                        Query += " WHERE dendrite_id = \"";
                        Query += r_ids[i][0];
                        Query += "\" AND day = ";
                        Query += r_fate[n+1][0];
                        Query += ";";
                        db.Query(Query);
                    }
                    else //still there next day or end of fate reached
                        continue;
                }
                else
                    continue;
            }

        }*/


        /*//select morphology from spines where dendrite_id = 20 and spine_id = 13 and spine_type = "lost" order by day limit 1;

        //get the spine_ids for each dendrite
        Query = "SELECT DISTINCT spine_id FROM spines WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\";";
        r_spine_ids = db.Query(Query);

        //for each spine id, give the morphology and the date lost
        for (vInt k = 0; k < r_spine_ids.size(); k ++)
        {
            Query = "SELECT morphology, day FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND spine_id = \"";
            Query += r_spine_ids[k][0];
            Query += "\" AND spine_type = \"lost\" ORDER BY day LIMIT 1;";
            r_tmp = db.Query(Query);
            if (r_tmp.size() > 0)
            {
                Query = "UPDATE dendrites SET ";

                if (r_tmp[0][0] == "f") Query += "lost_filopodia = lost_filopodia + 1";
                if (r_tmp[0][0] == "m") Query += "lost_mushrooms = lost_mushrooms + 1";
                if (r_tmp[0][0] == "t") Query += "lost_thins = lost_thins + 1";
                if (r_tmp[0][0] == "s") Query += "lost_stubbies = lost_stubbies + 1";

                Query += " WHERE dendrite_id = \"";
                Query += r_ids[i][0];
                Query += "\" AND day = ";
                Query += r_tmp[0][1];
                Query += ";";
                db.Query(Query);
                //_stdoutput << Query << endl;
            }
        }*/

       /* _stdoutput << "\rInfo> Performed queries: " << db.QueryCount;
    }
    _stdoutput << endl;

    return;
}*/

string SpineType(string dendrite_id, string spine_id, string timepoint, Database &db)
{
    string Query = "SELECT spine_type FROM spines WHERE dendrite_id =\"";
    Query += dendrite_id;
    Query += "\" AND spine_id = \"";
    Query += spine_id;
    Query += "\" AND day = ";
    Query += timepoint;
    Query += ";";
    dbResults r = db.Query(Query);
    if (r.size() > 0) return r[0][0];
    else return "";
}

void WriteSum(Database &db, fstream &f, string identifier)
{
    //create temprorary table which stores all information ready for output.
   string Query, iQuery;
   dbResults r_days = db.Query("SELECT DISTINCT day FROM dendrites ORDER BY day;");
   dbResults r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites;");
   dbResults r_mean, r_values;

   if (r_ids.size() == 0 || r_days.size() == 0)
   {
        _stdoutput << "Error> Export failure." << endl;
        Quit();
   }

   //create new export table
   //build a variable query.
   //we don't know how many dendrites and values exist. Therefore glue the query together...
   Query = "CREATE TABLE tmp_sum (day INTEGER, mean DOUBLE";
   for (vInt i = 0; i < r_ids.size(); i++)
   {
        Query += ", \"id_";
        Query += r_ids[i][0];
        Query += "\" DOUBLE";
   }
   Query += ");";
   db.Query(Query);

   //for each day)
   for (vInt i = 0; i < r_days.size(); i++)
   {
        Query = "SELECT AVG(";
        Query += identifier;
        Query += ") FROM dendrites WHERE day = ";
        Query += r_days[i][0];
        Query += ";";
        r_mean = db.Query(Query);

        iQuery = "INSERT INTO tmp_sum VALUES (";
        iQuery += r_days[i][0];
        iQuery += ",";
        if(r_mean[0][0] == "") iQuery += "NULL"; else iQuery += r_mean[0][0];
        for (vInt j = 0; j < r_ids.size(); j++)
        {
            Query = "SELECT ";
            Query += identifier;
            Query += " FROM dendrites WHERE day = ";
            Query += r_days[i][0];
            Query += " AND dendrite_id = \"";
            Query += r_ids[j][0];
            Query += "\";";
            r_values = db.Query(Query);
            if (r_values.size() == 0) iQuery += ",NULL";
            else
            {
                iQuery += ",";
                if (r_values[0][0] == "") iQuery += "NULL"; else iQuery += r_values[0][0];
            }
        }
        iQuery += ");";
        db.Query(iQuery);
        //_stdoutput << "\rInfo> Converted dendrite " << db.QueryCount;
   }
   string header = db.GetTableHeader("tmp_sum",';');
   f << "Summary " << identifier << ";" << endl;
   f << header << endl;

   r_ids = db.Query("SELECT * FROM tmp_sum;");
   for (vInt i = 0; i < r_ids.size(); i++)
   {
        for (vInt j = 0; j < r_ids[i].size(); j++)
        {
            f << r_ids[i][j] << ";";
        }
        f << endl;
    }

   db.Query("DROP TABLE tmp_sum;");
   f << endl << endl;
   _stdoutput << "\rInfo> " << f.tellp() << " bytes written...";

    return;
}

void WriteRegainedSpineData(Database &db, fstream &f)
{
        //export the latency times for each dendrite
    _stdoutput << "Info> Building regained spines latencies..." << endl;
    vector<vector<string> > l;
    vector<string> t;
    dbResults r_regained, r_fate, r_ids;
    string daylost = "", Query = "";
    int delta;
    bool end_of_list;

    r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites ORDER BY dendrite_id;");
    //for each id
    for (vInt i = 0; i < r_ids.size(); i++)
    {

        //using daylost to save some memory ;)
        daylost = "\"id_";
        daylost += r_ids[i][0];
        daylost += "\"";
        t.push_back(daylost);
        daylost = "";

        //get all regained spines on gained
        Query = "SELECT spine_id, day FROM spines WHERE spine_type = \"gained\" AND is_regained = 1 AND dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY spine_id;";
        r_regained = db.Query(Query);
        //for each regained spine
        for (vInt j = 0; j < r_regained.size(); j++)
        {
            //get the fate of this spine BEFORE it was gaines
            Query = "SELECT day, spine_type FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND spine_id = \"";
            Query += r_regained[j][0];
            Query += "\" AND day < ";
            Query += r_regained[j][1];
            Query += " ORDER BY day DESC;";
            r_fate = db.Query(Query);
            end_of_list = false;
            //go trough the fate. Store the day lost. if at this tp the spine was stable/gained stop the loop.
            for (vInt k = 0; k < r_fate.size(); k++)
            {
                if (r_fate[k][1] == "lost") daylost = r_fate[k][0];
                else
                    break;

                //if end reached but still lost: we cannot use this fpor calc
                if (k+1 == r_fate.size()) end_of_list = true;

            }
            //now we know the day it was lost. make the delta
            if (!end_of_list)
            {
                delta = _atoi(r_regained[j][1]) - _atoi(daylost);
                //_stdoutput << "Info> spine " << r_regained[j][0] << " regained at day " << r_regained[j][1] << " and last seen lost at day " << daylost << " has latency of " << delta << endl;
                t.push_back(_itoa(delta));
            }
            else
            {
                //_stdoutput << "Info> latency for spine " << r_regained[j][0] << " regained at day " << r_regained[j][1] << " cannot be determined. " << endl;
            }
        }
        l.push_back(t);
        t.clear();
        //now we have the latencies l for this dendrite.
    }

    //for file conversion: tranpose matrix
    l = _TransposeMatrix(l);
    f << endl << "Latencies of regained spines:;" << endl;
    for (vInt i = 0; i < l.size(); i++)
    {
        for (vInt j = 0; j  < l[i].size(); j++)
        {
            f << l[i][j] << ";";
        }
        f << endl;
    }
    l.clear();

    //get the lifetime of regained spines
    _stdoutput << "Info> Building regained spines lifetimes..." << endl;
    for (vInt i = 0; i < r_ids.size(); i++)
    {

        //using daylost to save some memory ;)
        daylost = "\"id_";
        daylost += r_ids[i][0];
        daylost += "\"";
        t.push_back(daylost);
        daylost = "";

        //get all regained spines on gained
        Query = "SELECT spine_id, day FROM spines WHERE spine_type = \"gained\" AND is_regained = 1 AND dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY spine_id;";
        r_regained = db.Query(Query);
        //for each regained spine
        for (vInt j = 0; j < r_regained.size(); j++)
        {
            //get the fate of this spine AFTER it was gained
            Query = "SELECT day, spine_type FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND spine_id = \"";
            Query += r_regained[j][0];
            Query += "\" AND day >= ";
            Query += r_regained[j][1];
            Query += " ORDER BY day ASC;";
            r_fate = db.Query(Query);
            end_of_list = false;
            //go trough the fate. Store the day lost. if at this tp the spine was stable/gained stop the loop.
            for (vInt k = 0; k < r_fate.size(); k++)
            {
                daylost = r_fate[k][0];
                if (r_fate[k][1] == "lost")
                {
                        break;
                }

                //if end reached but not lost so far...
                if (k+1 == r_fate.size()) end_of_list = true;

            }
            //now we know the day it was lost. make the delta
            if (!end_of_list)
            {
                    delta = _atoi(daylost) - _atoi(r_regained[j][1]);
                    //_stdoutput << "Info> spine " << r_regained[j][0] << " regained at day " << r_regained[j][1] << " and was lost at day " << daylost << " has lifetime of " << delta << endl;
                    t.push_back(_itoa(delta));
            }
            else
            {
                    //_stdoutput << "Info> spine lifetime for spine " << r_regained[j][0] << " regained at day " << r_regained[j][1] << " cannot be determined. " << endl;
            }
        }
        l.push_back(t);
        t.clear();
        //now we have the lifetimes l for this dendrite.
    }

    //for file conversion: tranpose matrix
    l = _TransposeMatrix(l);
    f << endl << "Maximum lifetimes of regained spines:;" << endl;
    for (vInt i = 0; i < l.size(); i++)
    {
        for (vInt j = 0; j  < l[i].size(); j++)
        {
            f << l[i][j] << ";";
        }
        f << endl;
    }
    l.clear();

    //what are the morphologies of regained spines?
    _stdoutput << "Info> Building regained spines morphologies..." << endl;
    for (vInt i = 0; i < r_ids.size(); i++)
    {

        //using daylost to save some memory ;)
        daylost = "\"id_";
        daylost += r_ids[i][0];
        daylost += "\"";
        t.push_back(daylost);
        daylost = "";

        //get all regained spines morphologies on gained
        Query = "SELECT COUNT(morphology) FROM spines WHERE spine_type = \"gained\" AND is_regained = 1 AND dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" AND morphology = \"f\";";
        r_regained = db.Query(Query);
        t.push_back(r_regained[0][0]);

        Query = "SELECT COUNT(morphology) FROM spines WHERE spine_type = \"gained\" AND is_regained = 1 AND dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" AND morphology = \"m\";";
        r_regained = db.Query(Query);
        t.push_back(r_regained[0][0]);

        Query = "SELECT COUNT(morphology) FROM spines WHERE spine_type = \"gained\" AND is_regained = 1 AND dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" AND morphology = \"t\";";
        r_regained = db.Query(Query);
        t.push_back(r_regained[0][0]);

        Query = "SELECT COUNT(morphology) FROM spines WHERE spine_type = \"gained\" AND is_regained = 1 AND dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" AND morphology = \"s\";";
        r_regained = db.Query(Query);
        t.push_back(r_regained[0][0]);

        l.push_back(t);
        t.clear();
        //now we have the morphologies l for this dendrite.
    }

    //for file conversion: tranpose matrix
    l = _TransposeMatrix(l);
    f << endl << "Morphology contingency of regained spines (filopodia, mushroom, thin, stubby):;" << endl;
    for (vInt i = 0; i < l.size(); i++)
    {
        for (vInt j = 0; j  < l[i].size(); j++)
        {
            f << l[i][j] << ";";
        }
        f << endl;
    }
    l.clear();

    return;
}

void WriteClusterData(Database &db, fstream &f)
{
    string Query = "";
    string tmp = "";
    vector<string> t;
    vector<vector<string> > l;

    _stdoutput << "Info> Writing cluster/distance data..." << endl;
    dbResults r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites ORDER BY dendrite_id;");
    dbResults r_dist;

    //write distance matrix
    //for each id


    for (vInt i = 0; i < r_ids.size(); i++)
    {
        //get the distances
        Query = "SELECT distance FROM distances WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY distance;";
        r_dist = db.Query(Query);

        if (r_dist.size() == 0)
        {
            // nothing to export for this dendrite...
            continue;
        }

        tmp = "\"id_";
        tmp += r_ids[i][0];
        tmp += "\"";
        r_dist[0].insert(r_dist[0].begin(), tmp);
        for (vInt j = 0; j < r_dist.size(); j++)
        {
            t.push_back(r_dist[j][0]);
        }
        tmp = "";
        l.push_back(t);
        t.clear();
    }



    //for file conversion: tranpose matrix
    l = _TransposeMatrix(l);
    f << endl << "Minimum distance of gained spines:;" << endl;
    for (vInt i = 0; i < l.size(); i++)
    {
        for (vInt j = 0; j  < l[i].size(); j++)
        {
            f << l[i][j] << ";";
        }
        f << endl;
    }
    l.clear();

    //write cluster info
    f << endl << "Cluster dataset:;" << endl;
    f << "dendrite_id;epsilon;minPts;clusters;" << endl;
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        Query = "SELECT epsilon, minPts, clusters FROM clusters WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\";";
        //abuse r_dist here
        r_dist = db.Query(Query);

        for (vInt j = 0; j < r_dist.size(); j++)
        {
            f << r_ids[i][0] << ";" << r_dist[j][0] << ";" << r_dist[j][1] << ";" << r_dist[j][2] << ";" << endl;
        }
    }

    return;
}

void Convert(Database &db, string filename)
{
    UpdateDB(db);
    _stdoutput << "Info> Running conversion to csv format." << endl;
    string header = db.GetTableHeader("dendrites",';');

    dbResults r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites ORDER BY dendrite_id;");
    dbResults r_dendrite;
    //dbResults r_spine_ids;
    //dbResults r_spine_days;
    dbResults r_sids;
    dbResults r_tps;
    //dbResults r_days, r_tmp;
    string Query;
    string Ofilename;

    if (r_ids.size() == 0)
    {
        _stdoutput << "Error> Empty table in" << filename << endl;
        return;
    }

    //db.QueryCount = 0;

    fstream f;
    Ofilename = filename;
    Ofilename += ".dendrites.csv";
    f.open(Ofilename.c_str(), ios::out);
    if (f.fail())
    {
        _stdoutput << "Error> cannot access \"" << Ofilename <<"\". File already open?" << endl;
        Quit();
    }

    //write summaries as you like them being exported
    _stdoutput << "Info> Writing dendrite data summaries..." << endl;
    f << "Spine density:;" << endl;
    WriteSum(db, f, "spine_density");

    f << "Survival fraction:;" << endl;
    WriteSum(db, f, "frac_survival");

    f << "Survival fraction of new gained spines:;" << endl;
    WriteSum(db, f, "newgained_survival");

    f << "daily turnover rate:;" << endl;
    WriteSum(db, f, "tor");

    f << "Density of transient spines:;" << endl;
    WriteSum(db, f, "transient_spines/length");

    f << "Fraction of transient spines:;" << endl;
    WriteSum(db, f, "CAST(100*transient_spines AS DOUBLE)/living_spines");

    f << "Density of persistent spines:;" << endl;
    WriteSum(db, f, "(CAST(living_spines - transient_spines AS DOUBLE))/length");

    f << "Fraction of persistent spines:;" << endl;
    WriteSum(db, f, "(100*CAST(living_spines - transient_spines AS DOUBLE))/living_spines");

    f << "Fraction of gained, stable, lost and gained+lost spines:;" << endl;
    WriteSum(db, f, "CAST(100*gained_spines AS DOUBLE) / living_spines");
    WriteSum(db, f, "CAST(100*stable_spines AS DOUBLE) / living_spines");
    WriteSum(db, f, "CAST(100*lost_spines AS DOUBLE) / living_spines");
    WriteSum(db, f, "CAST(100*gainedlost AS DOUBLE) / living_spines");

    f << "Density of gained, stable, lost and gained+lost spines:;" << endl;
    WriteSum(db, f, "CAST(gained_spines AS DOUBLE) / length");
    WriteSum(db, f, "CAST(stable_spines AS DOUBLE) / length");
    WriteSum(db, f, "CAST(lost_spines AS DOUBLE) / length");
    WriteSum(db, f, "CAST(gainedlost AS DOUBLE) / length");

    f << "Shapeshifting frequency:;" << endl;
    WriteSum(db, f, "shapeshifting_frequency");


    f << "Density of filopoia, mushrooms, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST(filopodia AS DOUBLE)/length");
    WriteSum(db, f, "CAST(mushrooms AS DOUBLE)/length");
    WriteSum(db, f, "CAST(thins AS DOUBLE)/length");
    WriteSum(db, f, "CAST(stubbies AS DOUBLE)/length");


    f << "Density of persistent filopodia, mushroom, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST((filopodia - gained_transient_filopodia + stable_transient_filopodia) AS DOUBLE)/length");
    WriteSum(db, f, "CAST((mushrooms - gained_transient_mushrooms + stable_transient_mushrooms) AS DOUBLE)/length");
    WriteSum(db, f, "CAST((thins - gained_transient_thins + stable_transient_thins) AS DOUBLE)/length");
    WriteSum(db, f, "CAST((stubbies - gained_transient_stubbies + stable_transient_stubbies) AS DOUBLE)/length");


    f << "Density of transient filopodia, mushroom, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST(gained_transient_filopodia + stable_transient_filopodia AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_transient_mushrooms + stable_transient_mushrooms AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_transient_thins + stable_transient_thins AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_transient_stubbies + stable_transient_stubbies AS DOUBLE)/length");


    f << "Density of gained filopodia, mushroom, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST(gained_filopodia AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_mushrooms AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_thins AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_stubbies AS DOUBLE)/length");


    f << "Density of stable filopodia, mushroom, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST(stable_filopodia AS DOUBLE)/length");
    WriteSum(db, f, "CAST(stable_mushrooms AS DOUBLE)/length");
    WriteSum(db, f, "CAST(stable_thins AS DOUBLE)/length");
    WriteSum(db, f, "CAST(stable_stubbies AS DOUBLE)/length");


    f << "Density of lost filopodia, mushroom, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST(lost_filopodia AS DOUBLE)/length");
    WriteSum(db, f, "CAST(lost_mushrooms AS DOUBLE)/length");
    WriteSum(db, f, "CAST(lost_thins AS DOUBLE)/length");
    WriteSum(db, f, "CAST(lost_stubbies AS DOUBLE)/length");


    f << "Density of gained persistent filopodia, mushroom, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST(gained_filopodia - gained_transient_filopodia AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_mushrooms - gained_transient_mushrooms AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_thins - gained_transient_thins AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_stubbies - gained_transient_stubbies AS DOUBLE)/length");


    f << "Density of stable persistent filopodia, mushroom, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST(stable_filopodia - stable_transient_filopodia AS DOUBLE)/length");
    WriteSum(db, f, "CAST(stable_mushrooms - stable_transient_mushrooms AS DOUBLE)/length");
    WriteSum(db, f, "CAST(stable_thins - stable_transient_thins AS DOUBLE)/length");
    WriteSum(db, f, "CAST(stable_stubbies - stable_transient_stubbies AS DOUBLE)/length");


    f << "Density of lost persistent filopodia, mushroom, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST(lost_filopodia - lost_transient_filopodia AS DOUBLE)/length");
    WriteSum(db, f, "CAST(lost_mushrooms - lost_transient_mushrooms AS DOUBLE)/length");
    WriteSum(db, f, "CAST(lost_thins - lost_transient_thins AS DOUBLE)/length");
    WriteSum(db, f, "CAST(lost_stubbies - lost_transient_stubbies AS DOUBLE)/length");


    f << "Density of gained transient filopodia, mushroom, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST(gained_transient_filopodia AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_transient_mushrooms AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_transient_thins AS DOUBLE)/length");
    WriteSum(db, f, "CAST(gained_transient_stubbies AS DOUBLE)/length");


    f << "Density of stable transient filopodia, mushroom, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST(stable_transient_filopodia AS DOUBLE)/length");
    WriteSum(db, f, "CAST(stable_transient_mushrooms AS DOUBLE)/length");
    WriteSum(db, f, "CAST(stable_transient_thins AS DOUBLE)/length");
    WriteSum(db, f, "CAST(stable_transient_stubbies AS DOUBLE)/length");


    f << "Density of lost transient filopodia, mushroom, thins, stubbies:;" << endl;
    WriteSum(db, f, "CAST(lost_transient_filopodia AS DOUBLE)/length");
    WriteSum(db, f, "CAST(lost_transient_mushrooms AS DOUBLE)/length");
    WriteSum(db, f, "CAST(lost_transient_thins AS DOUBLE)/length");
    WriteSum(db, f, "CAST(lost_transient_stubbies AS DOUBLE)/length");

    f << "Regained spines:;" << endl;
    WriteSum(db, f, "regained_spines");
    f << "Regained spines density:;" << endl;
    WriteSum(db, f, "regained_spines/length");
    f << "Frac regained spines:;" << endl;
    WriteSum(db, f, "100*regained_spines/CAST((gained_spines+stable_spines) AS DOUBLE)");
    f << "Ratio regained/gained:;" << endl;
    WriteSum(db, f, "regained_spines/CAST(gained_spines AS DOUBLE)");

    _stdoutput << endl;

    WriteRegainedSpineData(db, f);
    WriteClusterData(db, f);


    //we're done with the summaries. now export the details
    _stdoutput << "Info> Writing details..." << endl;
    f << endl;
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        //write all dendrite values
        Query = "SELECT * FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY day;";
        r_dendrite = db.Query(Query);
        f << "Dendrite:" << endl;
        f << header << endl;

        for (vInt j = 0; j < r_dendrite.size(); j++)
        {
            for (vInt k = 0; k < r_dendrite[j].size(); k++)
                f << r_dendrite[j][k] << ";";
            f << endl;
        }

        //explain the spines which belong to this dendrite
        f << "Spines of dendrite \"" << r_ids[i][0] << "\":" << endl;

        //get the spine ids
        Query = "SELECT DISTINCT spine_id FROM spines WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\";";
        r_sids = db.Query(Query);

        //get the days
        Query = "SELECT DISTINCT day FROM spines WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY day;";
        r_tps = db.Query(Query);

        //write the days
        string type;
        f << "spine/day->;";
        for (vInt n = 0; n < r_tps.size(); n++)
        {
            f << r_tps[n][0] << ";";
        }

        f << endl;

        //for each spine id
        for (vInt l = 0; l < r_sids.size(); l++)
        {
            //write the id first. and then for each timepoint their fates...
            f << r_sids[l][0] << ";";
            for (vInt m = 0; m < r_tps.size(); m++)
            {
                type = SpineType(r_ids[i][0], r_sids[l][0], r_tps[m][0], db);
                if (type != "")
                {
                    f << type[0] << ";";
                }
                else
                f << " ;";

            }
            f << endl;
        }

        f << endl << endl << endl << endl;
        _stdoutput << "\rInfo> " << f.tellp() << " bytes written... ";
    }

    f.close();
    _stdoutput << endl << "Info> \"" << Ofilename << "\" Created." << endl;

    return;
}

//trace the fate of a single spine
string TraceFate(string spine_id, string dendrite_id, Database &db, bool showresults)
{
    dbResults r;
    string fatestring = "";
    string Query = "SELECT spine_type, day FROM spines WHERE spine_id = \"";
    Query += spine_id;
    Query += "\" AND dendrite_id = \"";
    Query += dendrite_id;
    Query += "\" ORDER BY day;";
    r = db.Query(Query);

    if (r.size() == 0)
    {
        _stdoutput << "Error> Spine \""<<spine_id<<"\" in dendrite \""<<dendrite_id<<"\" not found in the database." << endl;
        return "Error> An error occured in procedure TraceFate(...);\n";
    }


    if (showresults)
    {
        //print a plot of the spine fate
        _stdoutput << "Info> Fate trace of spine \""<<spine_id<<"\" in dendrite \""<<dendrite_id<<"\":" << endl;

        _stdoutput << endl << "     ";
        for (vInt i = 0; i < r.size(); i++)
        {
            //_stdoutput << (char) 218;
            _stdoutput << ".";
            _stdoutput << r[i][1];
            if (atoi(r[i][1].c_str()) < 10)  _stdoutput << "    ";
            if (atoi(r[i][1].c_str()) > 9 && atoi(r[i][1].c_str()) < 100) _stdoutput << "   ";
            if (atoi(r[i][1].c_str()) > 99 && atoi(r[i][1].c_str()) < 1000) _stdoutput << "  ";
            if (atoi(r[i][1].c_str()) > 999) _stdoutput << " ";
        }
        _stdoutput << endl << "     ";
        for (vInt i = 0; i < r.size(); i++)
        {
            _stdoutput << r[i][0][0];
            /*_stdoutput << (char) 196;
            _stdoutput << (char) 196;
            _stdoutput << (char) 196;
            _stdoutput << (char) 196;
            _stdoutput << (char) 196;*/
            _stdoutput << "-";
            _stdoutput << "-";
            _stdoutput << "-";
            _stdoutput << "-";
            _stdoutput << "-";

        }
        _stdoutput << ">" << endl << endl << endl;
    }
    else
    {
        for (vInt i = 0; i < r.size(); i++)
        {
            fatestring += r[i][0][0];
        }
    }
    return fatestring;
}

//which ovl files belonged to a dendrite?
void RetrieveFilesFromDendrite(Database &db, string dendrite_id)
{
    dbResults r;
    string Query = "SELECT day, file FROM dendrites WHERE dendrite_id =\"";
    Query += dendrite_id;
    Query += "\" ORDER BY day;";
    r = db.Query(Query);

    if (r.size() == 0)
    {
        _stdoutput << "Error> No filename association found for dendrite \"" << dendrite_id << "\"." << endl;
        return;
    }

    for (vInt i = 0; i < r.size(); i++)
    {
        _stdoutput << "Info> Day " << r[i][0] << ":\t" << r[i][1] << endl;
    }

    return;
}

//test if the spine fates are ok.
//NONO's are: gained -> gained || lost -> stable || stable -> gained. lost -> gained is okay, because it could be a regained one.
bool CheckFates(Database &db)
{
    _stdoutput << "Info> Checking spine fates..." << endl;
    bool ret = true;
    string fate = "";
    string Query;
    dbResults r = db.Query("SELECT DISTINCT spine_id, dendrite_id FROM spines");
    dbResults f;
    for (vInt i = 0; i < r.size(); i++)
    {
        //get the fate as string like "ssssslllll";
        fate = TraceFate(r[i][0], r[i][1], db, false);
        if (fate.find("ls")!=string::npos)
        {
            _stdoutput << "Error> Spine fate assignment faulty: a spine cannot be classified as stable when it was lost before (dendrite id=\""<<r[i][1]<<"\", spine id=\""<<r[i][0]<<"\", spine fate=\""<<fate<<"\")" << endl;
            TraceFate(r[i][0], r[i][1], db, true);
            _stdoutput << "Info> Please check the following files:" << endl;
            RetrieveFilesFromDendrite(db, r[i][1]);
            ret = false;
        }

        if (fate.find("sg")!=string::npos)
        {
            _stdoutput << "Error> Spine fate assignment faulty: a spine cannot be classified as gained when it was stable before (dendrite id=\""<<r[i][1]<<"\", spine id=\""<<r[i][0]<<"\", spine fate=\""<<fate<<"\")" << endl;
            TraceFate(r[i][0], r[i][1], db, true);
            _stdoutput << "Info> Please check the following files:" << endl;
            RetrieveFilesFromDendrite(db, r[i][1]);
            ret = false;
        }

        if (fate.find("gg")!=string::npos)
        {
            _stdoutput << "Error> Spine fate assignment faulty: a spine cannot be classified as gained when it was gained before (dendrite id=\""<<r[i][1]<<"\", spine id=\""<<r[i][0]<<"\", spine fate=\""<<fate<<"\")" << endl;
            TraceFate(r[i][0], r[i][1], db, true);
            _stdoutput << "Info> Please check the following files:" << endl;
            RetrieveFilesFromDendrite(db, r[i][1]);
            ret = false;
        }

        //if you would like to disable the possibility, that spines can be regained you may should adapt the following section
        /*if (fate.find("lg")!=string::npos)
        {
            _stdoutput << "Error> Spine fate assignment faulty: a spine cannot be classified as gained whn it was lost before (dendrite id=\""<<r[i][1]<<"\", spine id=\""<<r[i][0]<<"\", spine fate=\""<<fate<<"\")" << endl;
            TraceFate(r[i][0], r[i][1], db, true);
            _stdoutput << "Info> Please check the following files:" << endl;
            RetrieveFilesFromDendrite(db, r[i][1]);
            ret = true;
        }*/

    }

    //if morphologies are not ignored, check if lost spines change their morphologies. But this is really not important...
    /*if (O.IgnoreSpineMorphologies == false)
    {
        code can be implemented here, but so far not needed
    }*/

    return ret;
}

//show TOR on screen
void PrintTor(string dendrite_id, Database &db)
{
    dbResults r;
    string Query;
    Query = "SELECT day, tor FROM dendrites WHERE dendrite_id = \"";
    Query += dendrite_id;
    Query += "\" ORDER BY day;";
    r = db.Query(Query);

    if (r.size() == 0)
    {
        _stdoutput << "Error> No data retrievable for dendrite id \""<<dendrite_id<<"\"" << endl;
        return;
    }

    _stdoutput << "Info> Turnover rates for dendrite \""<<dendrite_id<<"\":"<< endl << endl;
    _stdoutput << "Day\tTOR" << endl;
    db.ShowLastResults();

    return;
}

//show ugly survival courve on screen
void PrintSurvival(string dendrite_id, Database &db)
{
    dbResults r;
    string Query;
    Query = "SELECT frac_survival, day FROM dendrites WHERE dendrite_id = \"";
    Query += dendrite_id;
    Query += "\" ORDER BY day DESC;";
    r = db.Query(Query);
    //db.ShowLastResults();

    if (r.size() == 0)
    {
        _stdoutput << "Error> Cannot print survival for dendrite \""<<dendrite_id<<"\": dendrite not found in the database." << endl;
        return;
    }


    _stdoutput << "Info> Survival courve of dendrite \""<<dendrite_id<<"\":" << endl;
    //_stdoutput <<endl<< "     " << (char) 179 << endl;
    _stdoutput <<endl<< "     " << "|" << endl;
    for (vInt i = 0; i < r.size(); i++)
    {

        if (atoi(r[i][1].c_str()) < 10)  _stdoutput << "    ";
        if (atoi(r[i][1].c_str()) > 9 && atoi(r[i][1].c_str()) < 100) _stdoutput << "   ";
        if (atoi(r[i][1].c_str()) > 99 && atoi(r[i][1].c_str()) < 1000) _stdoutput << "  ";
        if (atoi(r[i][1].c_str()) > 999) _stdoutput << " ";

        //_stdoutput << r[i][1] << (char) 180;
        _stdoutput << r[i][1] << "|";
        for (int j = 0; j < atoi(r[i][0].c_str())/10; j++)
            //_stdoutput << (char)178;
            _stdoutput << "#";
        _stdoutput << endl;
    }
    //_stdoutput << "     " << (char) 195;
    _stdoutput << "     " << "+";
    //for (int i = 0; i < 4; i++) _stdoutput <<  (char) 196;
    for (int i = 0; i < 4; i++) _stdoutput <<  "-";
    //_stdoutput << (char) 194;
    _stdoutput << "+";
    //for (int i = 0; i < 4; i++) _stdoutput <<  (char) 196;
    for (int i = 0; i < 4; i++) _stdoutput <<  "-";
    //_stdoutput << (char) 194 << (char) 196 << (char) 196<<(char) 196 <<  endl;
    _stdoutput << "+" << "-" << "-" << "-" <<  endl;
    _stdoutput << "     0%   50%  100%" << endl << endl << endl;

    return;
}

//calc survival:
//get the number of stable spines and watch how many of them still exist at later time points
void CalcSurvival(Database &db, int imaging_tp)
{
    _stdoutput << "Info> Calculating the spine survival using imaging timepoint " << _itoa(imaging_tp + 1) << " as start." << endl;

    //reset column
    db.Query("UPDATE dendrites SET frac_survival = NULL;");

    //get all dendrite ids
    string Query;
    dbResults r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites ORDER BY dendrite_id;");
    dbResults r_days, r_stables0_ids, r_stablesN_ids;
    string entrypoint;
    int stables_d0 = 0;
    int stables_dn = 0;
    double survival = 0;

    //db.QueryCount = 0;

    //for each id
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        //is imaging_tp realistic?
        Query = "SELECT count(day) FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" LIMIT 1;";
        r_days = db.Query(Query);
        if (imaging_tp + 1 > atoi(r_days[0][0].c_str()))
        {
            _stdoutput << "Warning> Cannot calculate survival with imaging entrypoint " << _itoa(imaging_tp + 1) << " when only " << r_days[0][0] << " timepoints available for dendrite \"" << r_ids[i][0] << "\". Skipped." << endl;
            continue;
        }


        //get the days and stable spines from imaging-timepoint until the end of select
        Query = "SELECT DISTINCT day, stable_spines FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY day LIMIT ";
        Query += _itoa(imaging_tp);
        Query += ",(SELECT COUNT(dendrite_id) FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\");";
        r_days = db.Query(Query);

        //hopefully, there will be some results and stable spines
        if (r_days.size() == 0 || r_days[0][1] == "0")
        {
            _stdoutput << endl << "Warning> Cannot calculate the spine survival at imaging timepoint " << _itoa(imaging_tp + 1) << " for dendrite_id \"" << r_ids[i][0] << "\" having " << r_days[0][1] << " stable spines. Skipped." << endl;
            continue;
        }

       //convert imaging timepoint to day
       entrypoint = r_days[0][0];

       //copy amount of stable spines into d0 and set there 100%
       stables_d0 = atoi(r_days[0][1].c_str());
       Query = "UPDATE dendrites SET frac_survival = 100 WHERE dendrite_id = \"";
       Query += r_ids[i][0];
       Query += "\" AND day = ";
       Query += entrypoint;
       Query += ";";
       db.Query(Query);

       //now get the spine ids of the stable spines at the entry-point
       Query = "SELECT spine_id FROM spines WHERE dendrite_id = \"";
       Query += r_ids[i][0];
       Query += "\" AND day = ";
       Query += entrypoint;
       Query += " AND spine_type = \"stable\" AND is_transient = 0;";
       r_stables0_ids = db.Query(Query);
       //This cannot be empty since this was checked before...

       //now go through each day until end
       for (vInt j = 1; j < r_days.size(); j++)
       {
            //get the ids of the stable spines at this day
            Query = "SELECT spine_id FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"stable\" AND is_regained = 0;";
            r_stablesN_ids = db.Query(Query);

            //now check how many of the stables here also existed at t0
            stables_dn = _AinB(r_stables0_ids, r_stablesN_ids);

            //calculate the survival
            survival = ((double) stables_dn / (double) stables_d0) * 100;

            //update the table
            Query = "UPDATE dendrites SET frac_survival = ";
            Query += _ftoa(survival);
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //_stdoutput << "\rInfo> Performed queries: " << db.QueryCount;
       }

    //PrintSurvival(r_ids[i][0], db);

    }

    UpdateDB(db);

    //_stdoutput << endl;

    return;
}


void CalcMorphs(Database &db)
{
    //_stdoutput << "Info> Building morphology database for " << endl;
    string Query;
    string types[4] = {"gained", "stable", "gained", "stable"}; //g
    string morphs[4] = {"filopodia", "mushrooms", "thins", "stubbies"}; //h
    dbResults r_ids, r_days, r_losts, r_count, r_fate;
    int counter = 0, transient_counter = 0;

    Query = "UPDATE dendrites SET gained_filopodia = 0, gained_mushrooms = 0, gained_thins = 0, gained_stubbies = 0, "
                                 "stable_filopodia = 0, stable_mushrooms = 0, stable_thins = 0, stable_stubbies = 0, "
                                 "lost_filopodia = 0, lost_mushrooms = 0, lost_thins = 0, lost_stubbies = 0, "
                                 "gained_transient_filopodia = 0, gained_transient_mushrooms = 0, gained_transient_thins = 0, gained_transient_stubbies = 0, "
                                 "stable_transient_filopodia = 0, stable_transient_mushrooms = 0, stable_transient_thins = 0, stable_transient_stubbies = 0, "
                                 "lost_transient_filopodia = 0, lost_transient_mushrooms = 0, lost_transient_thins = 0, lost_transient_stubbies = 0;";
    db.Query(Query);

    //first the gained and stable spines

    for (int g = 0; g < 4; g++)
    {
        for (int h = 0; h < 4; h++)
        {
            //get all ids
            r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites;");
            //for each id
            for (vInt i = 0; i < r_ids.size(); i++)
            {
                //for each day
                Query = "SELECT DISTINCT day FROM dendrites WHERE dendrite_id = \"";
                Query += r_ids[i][0];
                Query += "\" ORDER BY day;";
                r_days = db.Query(Query);

                for (vInt j = 0; j < r_days.size(); j++)
                {
                    Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
                    Query += r_ids[i][0];
                    Query += "\" AND day = ";
                    Query += r_days[j][0];
                    Query += " AND spine_type = \"";
                    Query += types[g];
                    Query += "\" AND morphology = \"";
                    Query += morphs[h][0];
                    Query += "\"";
                    if (g > 1) Query += " AND is_transient = 1";
                    Query += ";";

                    r_count = db.Query(Query);

                    Query = "UPDATE dendrites SET ";
                    Query += types[g];
                    Query += "_";
                    if (g > 1) Query += "transient_";
                    Query += morphs[h];
                    Query += " = ";
                    Query += r_count[0][0];
                    Query += " WHERE dendrite_id = \"";
                    Query += r_ids[i][0];
                    Query += "\" AND day = ";
                    Query += r_days[j][0];
                    Query += ";";
                    db.Query(Query);
                    //_stdoutput << Query << endl;
                }
            }
            _stdoutput << "\rInfo> Building morphology database for " << types[g] << " ";
            if (g > 1) _stdoutput << "transient ";
            _stdoutput << morphs[h] << "    ";
        }
    }

    //now the lost spines. The date when they were first seen lost counts.
    //for (int g = 0; g < 2; g++) //just transient or non transient
    {

        for (int h = 0; h < 4; h++)
        {
                //get all ids
                r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites;");
                //for each id
                for (vInt i = 0; i < r_ids.size(); i++)
                {
                    //for each day
                    Query = "SELECT DISTINCT day FROM dendrites WHERE dendrite_id = \"";
                    Query += r_ids[i][0];
                    Query += "\" ORDER BY day;";
                    r_days = db.Query(Query);

                    for (vInt j = 0; j < r_days.size(); j++)
                    {
                        //get a list of all dead spines
                        Query = "SELECT spine_id, day FROM spines WHERE dendrite_id = \"";
                        Query += r_ids[i][0];
                        Query += "\" AND day = ";
                        Query += r_days[j][0];
                        Query += " AND spine_type = \"lost\" AND morphology = \"";
                        Query += morphs[h][0];
                        Query += "\"";
                        //if (g > 0) Query += " AND is_transient = 1";
                        Query += ";";
                        r_losts = db.Query(Query);
                        //_stdoutput << Query << endl;

                        //for each lost spine
                        for (vInt k = 0; k < r_losts.size(); k++)
                        {
                            //was it gained or stable before?
                            Query = "SELECT spine_type, is_transient FROM spines WHERE dendrite_id = \"";
                            Query += r_ids[i][0];
                            Query += "\" AND day < ";
                            Query += r_days[j][0];
                            Query += " AND spine_id = \"";
                            Query += r_losts[k][0];
                            Query += "\" ORDER BY day DESC LIMIT 1;";
                            r_fate = db.Query(Query);

                            //yes, this one was gained or stable before but no transient.
                            if (r_fate[0][0] != "lost" /*&& r_fate[0][1] == "0"*/) counter++;
                            //yes, this one was gained or stable before but transient.
                            if (r_fate[0][0] != "lost" && r_fate[0][1] == "1") transient_counter++;

                        }

                        //now update the counted ones which were stable or gained before into the db
                        Query = "UPDATE dendrites SET lost_";
                        //if (g > 0) Query += "transient_";
                        Query += morphs[h];
                        Query += " = ";
                        Query += _itoa(counter);
                        //if (transient_counter > 0)
                        {
                            Query += ", lost_transient_";
                            Query += morphs[h];
                            Query += " = ";
                            Query += _itoa(transient_counter);
                        }
                        Query += " WHERE dendrite_id = \"";
                        Query += r_ids[i][0];
                        Query += "\" AND day = ";
                        Query += r_days[j][0];
                        Query += ";";
                        db.Query(Query);
                        counter = 0;
                        transient_counter = 0;
                        //_stdoutput << Query << endl;
                    }
                }
                _stdoutput << "\rInfo> Building morphology database for lost (transient) " << morphs[h] << "    ";
        }

    }
    _stdoutput << endl;



/*
    //which morphologies have the spines?
    //dbResults r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites;");
    dbResults r_tmp, r_spine_ids;


    db.QueryCount = 0;
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        //get the days
        Query = "SELECT DISTINCT day FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY day;";
        r_days = db.Query(Query);

        //go through each day
        for (vInt j = 0; j < r_days.size(); j++)
        {
            //get gained filopodia
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"f\";";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_filopodia = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained mushrooms
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"m\";";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_mushrooms = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained thins
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"t\";";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_thins = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained stubbies
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"gained\" AND morphology = \"s\";";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET gained_stubbies = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //persistant:
            //get gained filopodia
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"f\" AND is_transient = 0;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET persistent_filopodia = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained mushrooms
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"m\" AND is_transient = 0;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET persistent_mushrooms = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained thins
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"t\" AND is_transient = 0;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET persistent_thins = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            //get gained stubbies
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type != \"lost\" AND morphology = \"s\" AND is_transient = 0;";
            r_tmp = db.Query(Query);
            //update the table...
            Query = "UPDATE dendrites SET persistent_stubbies = ";
            Query += r_tmp[0][0];
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);
        }

        //which morphology have the lost spines
        //get a list of all spine ids
        Query = "SELECT DISTINCT spine_id FROM spines WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\";";
        r_spine_ids = db.Query(Query);
        dbResults r_fate;

        //now go through each spine
        for (vInt m = 0; m < r_spine_ids.size(); m++)
        {
            //get the spine fate
            Query = "SELECT day, spine_type, morphology, is_transient FROM spines WHERE spine_id = \"";
            Query += r_spine_ids[m][0];
            Query += "\" AND dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" ORDER BY day;";
            r_fate = db.Query(Query);

            //go through fate
            Query = "UPDATE dendrites SET ";

            for (vInt n = 0; n < r_fate.size(); n++)
            {
                if(r_fate[n][1] != "lost") //if this spine exists
                {
                    //but is lost the next day
                    if(n + 1 < r_fate.size() && r_fate[n+1][1] == "lost")
                    {
                        if(r_fate[n+1][2] == "f") Query += "lost_filopodia = lost_filopodia + 1";
                        if(r_fate[n+1][2] == "m") Query += "lost_mushrooms = lost_mushrooms + 1";
                        if(r_fate[n+1][2] == "t") Query += "lost_thins = lost_thins + 1";
                        if(r_fate[n+1][2] == "s") Query += "lost_stubbies = lost_stubbies + 1";
                        Query += " WHERE dendrite_id = \"";
                        Query += r_ids[i][0];
                        Query += "\" AND day = ";
                        Query += r_fate[n+1][0];
                        Query += ";";
                        db.Query(Query);
                    }
                    else //still there next day or end of fate reached
                        continue;
                }
                else
                    continue;
            }

        }

*/
        /*//select morphology from spines where dendrite_id = 20 and spine_id = 13 and spine_type = "lost" order by day limit 1;

        //get the spine_ids for each dendrite
        Query = "SELECT DISTINCT spine_id FROM spines WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\";";
        r_spine_ids = db.Query(Query);

        //for each spine id, give the morphology and the date lost
        for (vInt k = 0; k < r_spine_ids.size(); k ++)
        {
            Query = "SELECT morphology, day FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND spine_id = \"";
            Query += r_spine_ids[k][0];
            Query += "\" AND spine_type = \"lost\" ORDER BY day LIMIT 1;";
            r_tmp = db.Query(Query);
            if (r_tmp.size() > 0)
            {
                Query = "UPDATE dendrites SET ";

                if (r_tmp[0][0] == "f") Query += "lost_filopodia = lost_filopodia + 1";
                if (r_tmp[0][0] == "m") Query += "lost_mushrooms = lost_mushrooms + 1";
                if (r_tmp[0][0] == "t") Query += "lost_thins = lost_thins + 1";
                if (r_tmp[0][0] == "s") Query += "lost_stubbies = lost_stubbies + 1";

                Query += " WHERE dendrite_id = \"";
                Query += r_ids[i][0];
                Query += "\" AND day = ";
                Query += r_tmp[0][1];
                Query += ";";
                db.Query(Query);
                //_stdoutput << Query << endl;
            }
        }*/
/*
        _stdoutput << "\rInfo> Performed queries: " << db.QueryCount;
    }
    _stdoutput << endl;

    return;*/


    return;
}

//calculate transients:
//measure for each lost spine the lifespan
//when lifespan < defined value: spine = transient
void CalcTransients(Database &db, int transients_period)
{
    string Query;
    dbResults r_gained;
    dbResults r_lost;
    dbResults r_ids;
    dbResults r_transients;
    dbResults r_days;
    dbResults r_tmp;
    int lifetime;

    _stdoutput << "Info> Calculating now amount of transient spines using a maximum lifespan of " << transients_period << " days." << endl;

    //reset db
    db.Query("UPDATE dendrites SET transient_spines = -1;"); // why was this set to -1?
    db.Query("UPDATE spines SET is_transient = 0;");

    //get all gained spines and ther days
    Query = "SELECT DISTINCT spine_id, dendrite_id, day FROM spines WHERE spine_type = \"gained\" ORDER BY dendrite_id AND day;";
    r_gained = db.Query(Query);
    if (r_gained.size() == 0)
    {
        _stdoutput << "Warning> Cannot find any gained spines for the calculation of transients. Skipped." << endl;
        return;
    }

    //for each gained spine
    for (vInt i = 0; i < r_gained.size(); i++)
    {
        //get the day when they have been lost
        Query = "SELECT DISTINCT day FROM spines WHERE spine_id = \"";
        Query += r_gained[i][0];
        Query += "\" AND dendrite_id = \"";
        Query += r_gained[i][1];
        Query += "\" AND day > ";
        Query += r_gained[i][2];
        Query += " AND spine_type = \"lost\" ORDER BY day ASC LIMIT 1";
        r_lost = db.Query(Query);
        if (r_lost.size() > 0)
        {
            //check the lifetime
            lifetime = atoi(r_lost[0][0].c_str()) - atoi(r_gained[i][2].c_str());
            if (lifetime <= transients_period && lifetime > 0)
            {
                //this one is transient. Mark this in the spine table as long it is alive
                _stdoutput << "Info> Spine \"" << r_gained[i][0] << "\" on dendrite \"" << r_gained[i][1] << "\" gained on day "<< r_gained[i][2] <<" and lost on day " << r_lost[0][0] << " has been detected as transient spine." << endl;
                Query = "UPDATE spines SET is_transient = 1 WHERE spine_id = \"";
                Query += r_gained[i][0];
                Query += "\" AND dendrite_id = \"";
                Query += r_gained[i][1];
                Query += "\" AND day < ";
                Query += r_lost[0][0];
                Query += " AND day >= ";
                Query += r_gained[i][2];
                Query += ";";
                db.Query(Query);
            }
        }
    }

    //update the number of transient spines in the dendrites table
    r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites ORDER BY dendrite_id;");
    //for each id
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        //for each day
        Query = "SELECT day FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY day;";
        r_days = db.Query(Query);
        for (vInt j = 0; j < r_days.size(); j++)
        {
            //count the # of transients
            Query = "SELECT COUNT(spine_id) FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND is_transient = 1;";
            r_transients = db.Query(Query);
            {
                Query = "UPDATE dendrites SET transient_spines = ";
                Query += r_transients[0][0];
                Query += " WHERE dendrite_id = \"";
                Query += r_ids[i][0];
                Query += "\" AND day = ";
                Query += r_days[j][0];
                Query += ";";
                db.Query(Query);
            }

        }

    }

/*
    //when were they lost?
    for (vInt i = 0; i < daysgained.size(); i++)
    {
        Query = "SELECT DISTINCT day, spine_type, spine_id, dendrite_id FROM spines WHERE spine_id = \"";
        Query += daysgained[i][0];
        Query += "\" AND dendrite_id = \"";
        Query += daysgained[i][1];
        Query += "\" AND day > ";
        Query += daysgained[i][2];
        Query += " AND spine_type = \"lost\" ORDER BY day ASC";
        dayslost = db.Query(Query);
        if (dayslost.size() > 0)
        {
            //for lifetime use the first record from query
            lifetime = atoi(dayslost[0][0].c_str()) - atoi(daysgained[i][2].c_str());
            if (lifetime <= transients_period && lifetime > 0)
            {
                //yeah, found a transient spine. now update the dendrite dataset at the days where this spine was alive...
                Query = "UPDATE dendrites SET transient_spines = transient_spines + 1 WHERE dendrite_id = \"";
                //Query += dayslost[0][2];
                //Query += "\" AND dendrite_id = \"";
                Query += dayslost[0][3];
                Query += "\" AND day < ";
                Query += dayslost[0][0];
                Query += " AND day >= ";
                Query += daysgained[i][2];
                Query += ";";
                db.Query(Query);
                _stdoutput << "Info> Spine \"" << dayslost[0][2] << "\" on dendrite \"" << dayslost[0][3] << "\" gained on day "<< daysgained[i][2] <<" and lost on day " << dayslost[0][0] << " has been detected as transient spine." << endl;

                //update the spines table
                Query = "UPDATE spines SET is_transient = 1 WHERE spine_id = \"";
                Query += dayslost[0][2];
                Query += "\" AND dendrite_id = \"";
                Query += dayslost[0][3];
                Query += "\" AND day < ";
                Query += dayslost[0][0];
                Query += " AND day >= ";
                Query += daysgained[i][2];
                Query += ";";
                db.Query(Query);
            }
        }
    }
*/
    //in case this function was directly turned on from the command line, also the morphologies
    //have to be recalculated.
    //autodetect if morphologies are in the database
    r_tmp = db.Query("SELECT DISTINCT morphology FROM spines;");
    if (r_tmp.size() >= 3) CalcMorphs(db);

        //finally, because at the last imaging day and at the first imaging day we cannot define if there is a tranient spine
    r_tmp = db.Query("SELECT MIN(day), MAX(day) FROM dendrites;");
    Query = "UPDATE dendrites SET "
        "transient_spines = NULL, "

        "gained_transient_filopodia = NULL, "
        "gained_transient_mushrooms = NULL, "
        "gained_transient_thins = NULL, "
        "gained_transient_stubbies = NULL, "

        "stable_transient_filopodia = NULL, "
        "stable_transient_mushrooms = NULL, "
        "stable_transient_thins = NULL, "
        "stable_transient_stubbies = NULL, "

        "lost_transient_filopodia = NULL, "
        "lost_transient_mushrooms = NULL, "
        "lost_transient_thins = NULL, "
        "lost_transient_stubbies = NULL ";


    Query += "WHERE day = ";
    Query += r_tmp[0][0];
    Query += " OR day = ";
    Query += r_tmp[0][1];
    Query += ";";
    db.Query(Query);


    UpdateDB(db);

    return;
}


//calc dTOR according to holtmaat et al. 2005
void CalcTor(Database &db)
{
    string Query;
    dbResults r_ids;
    dbResults r_gainedlost;
    double tor;

    //_stdoutput << "Info> Calculating TOR(t, t-1) = (gained spines+lost spines/2*all spines) / ((t) - (t-1))" << endl;

    Query = "SELECT DISTINCT dendrite_id FROM dendrites;";
    r_ids = db.Query(Query);
    //db.QueryCount = 0;
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        Query = "SELECT gainedlost, day, gained_spines, stable_spines FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY day;";
        r_gainedlost = db.Query(Query);
        tor = 0;
        //if (r_
        //for (int j = 1;
        //_stdoutput << r_gainedlost.size() << endl;
        if (r_gainedlost.size() > 1)
        {
            for (vInt j = 1; j < r_gainedlost.size(); j++)
            {
             tor = (atof(r_gainedlost[j][0].c_str())/(2*( atof(r_gainedlost[j][2].c_str()) + atof(r_gainedlost[j][3].c_str())))) / (atof(r_gainedlost[j][1].c_str()) - atof(r_gainedlost[j-1][1].c_str()));
             Query = "UPDATE dendrites SET tor = ";
             Query += _ftoa(tor);
             Query += " WHERE dendrite_id = \"";
             Query += r_ids[i][0];
             Query += "\" AND day = ";
             Query += r_gainedlost[j][1];
             Query += ";";
             //_stdoutput << Query << endl;
             db.Query(Query);
             //_stdoutput << "\rInfo> Performed queries: " << db.QueryCount;
             _stdoutput << "\rInfo> Calculating dTOR(" << r_gainedlost[j][1] << "," << r_gainedlost[j-1][1] << ") = ("<< r_gainedlost[j][0] <<") / (2*" << (atof(r_gainedlost[j][2].c_str()) + atof(r_gainedlost[j][3].c_str())) << ") / ("<< r_gainedlost[j][1] << "-" << r_gainedlost[j-1][1] << ")" ;
            }
        }
        else _stdoutput << "Warning> Not enough data to calculate TOR for dendrite with id \""<<r_ids[i][0]<<"\". Skipped." << endl;
    }
    _stdoutput << endl;

    dbResults r_tmp = db.Query("SELECT MIN(day), MAX(day) FROM dendrites;");
    Query = "UPDATE dendrites SET tor = NULL WHERE day = ";
    Query += r_tmp[0][0];
    Query += ";";
    db.Query(Query);

    return;
}

//calc NGS. This is the survival of a population of spines, which were gained at a defined time point
void CalcNewGainedSurvival(Database &db, int imaging_tp)
{
    string Query;

    _stdoutput << "Info> Calculating the survival of spines which were gained at imaging timepoint " << _itoa(imaging_tp + 1) << "." << endl;

    //format table
    db.Query("UPDATE dendrites SET newgained_survival = NULL;");

    //get all dendrite ids
    dbResults r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites ORDER BY dendrite_id;");
    dbResults r_days, r_gained_ids, r_stable_ids;
    //dbResults survived;

    string entrypoint;
    int spines_d0 = 0;
    int spines_dn = 0;
    double survival = 0;
//    db.QueryCount = 0;

    //for all IDs
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        //is imaging_tp realistic?
        Query = "SELECT count(day) FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" LIMIT 1;";
        r_days = db.Query(Query);
        if (imaging_tp + 1 > atoi(r_days[0][0].c_str()))
        {
            _stdoutput << "Warning> Cannot calculate new gained survival with imaging entrypoint " << _itoa(imaging_tp + 1) << " when only " << r_days[0][0] << " timepoints available for dendrite \"" << r_ids[i][0] << "\". Skipped." << endl;
            continue;
        }

        //get the days and gained spines from imaging-timepoint until the end of select
        Query = "SELECT DISTINCT day, gained_spines FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\" ORDER BY day LIMIT ";
        Query += _itoa(imaging_tp);
        Query += ",(SELECT COUNT(dendrite_id) FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\");";
        r_days = db.Query(Query);
        //_stdoutput << Query;
        //db.ShowLastResults();

        //hopefully, there will be some results and gained spines
        if (r_days.size() == 0 || r_days[0][1] == "0")
        {
            _stdoutput << endl << "Warning> Cannot calculate the survival of new gained spines at imaging timepoint " << _itoa(imaging_tp + 1) << " for dendrite_id \"" << r_ids[i][0] << "\" having " << r_days[0][1] << " gained spines. Skipped." << endl;
            continue;
        }

/*        if ((imaging_tp + 1) > atoi(r_days[0][2].c_str()))
        {
            _stdoutput << endl << "Error> Imaging timepoint " << itoa(imaging_tp + 1) << " does not exist. Try less. Stopped." << endl;
            return;
        }*/

       //convert imaging timepoint to day
       entrypoint = r_days[0][0];

       //copy amount of gained spines into d0 and set there 100%
       spines_d0 = atoi(r_days[0][1].c_str());
       Query = "UPDATE dendrites SET newgained_survival = 100 WHERE dendrite_id = \"";
       Query += r_ids[i][0];
       Query += "\" AND day = ";
       Query += entrypoint;
       Query += ";";
       db.Query(Query);

       //now get the spine ids of the gaines spines at the entry-point
       Query = "SELECT spine_id FROM spines WHERE dendrite_id = \"";
       Query += r_ids[i][0];
       Query += "\" AND day = ";
       Query += entrypoint;
       Query += " AND spine_type = \"gained\";";
       r_gained_ids = db.Query(Query);
       //This cannot be empty since this was checked before...

       //now go through each day until end
       for (vInt j = 1; j < r_days.size(); j++)
       {


            //get the ids of the stable spines at this day
            Query = "SELECT spine_id FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += " AND spine_type = \"stable\";";
            r_stable_ids = db.Query(Query);

            //now check how many of the gained ids are in the stable ids at this timepoint
            spines_dn = _AinB(r_gained_ids, r_stable_ids);

            //now we know, how many of the gained ids were found in the stable ids. calculate the survival
            survival = ((double) spines_dn / (double) spines_d0) * 100;

            //update the table
            Query = "UPDATE dendrites SET newgained_survival = ";
            Query += _ftoa(survival);
            Query += " WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];
            Query += ";";
            db.Query(Query);

            _stdoutput << "\rInfo> NGS for dendrite \"" << r_ids[i][0] << "\" at day " << r_days[j][0] << " is " << survival << ".    ";
       }
    }

    _stdoutput << endl;

    UpdateDB(db);

    return;
}

void ShowTableDendrites(Database &db)
{
    //_stdoutput << endl << "Table \"dendrites\":" << endl;
    //_stdoutput << "ID\tDay\tLength\tStable\tGained\tLost\tDensity\t%Stable\t%Gained\t%Lost\t%Surv.\tGnd&Lst\tTranst.\tFilop.\t%Filop.\tdTOR" << endl;
    string header = db.GetTableHeader("dendrites", '\t');
    _stdoutput << header << endl;
    //db.Query("SELECT dendrite_id, day, length, stable_spines, gained_spines, lost_spines, spine_density, frac_stable, frac_gained, frac_lost, frac_survival, gainedlost, transient_spines, filopodia, frac_filopodia, tor from dendrites ORDER BY dendrite_id AND day;");
    db.Query("SELECT * FROM dendrites ORDER BY dendrite_id AND day;");
    db.ShowLastResults();
}

void ShowTableSpines(Database &db)
{
    //_stdoutput << endl << "Table \"spines\":" << endl;
    //_stdoutput << "ID\tType\tDndr.ID\tDay" << endl;
    string header = db.GetTableHeader("spines", '\t');
    _stdoutput << header << endl;
    db.Query("SELECT * from spines ORDER BY dendrite_id AND spine_id AND day;");
    db.ShowLastResults();
}

//scans the directory for ovl files and if they are consistent for SpineMiner
void ScanOnly()
{
    vector <string> filenames;

    OVLReader O;
    filenames = O.ScanFiles();
    if (filenames.size() == 0)
    {
        _stdoutput << endl << O.LastError.str();
        _stdoutput << "Info> SpineMiner stopped." << endl;
        return;
    }
    return;
}

//calculates the distances between all living spines
//NOTE: in this scope we are interested about the distance distribution of gained spines
//since there are not that much gained spines per day it does not make sense do calc the distances daily
//therefore the daily sectioning is commented out here.
//it is implemented also to calc the distances for other spine_types but we don't use this at te moment
void CalcDistances(Database &db, bool stable, bool gained, bool lost)
{
    _stdoutput << "Info> Calculating minimum spine distances for [";
    if (stable == true) _stdoutput << " stable ";
    if (gained == true) _stdoutput << " gained ";
    if (lost == true) _stdoutput << " lost ";
    _stdoutput << "] spines..." << endl;

    if (stable == false && gained == false && lost == false)
    {
        _stdoutput << "Error> Cannot calculate distance Matrix for nothing!?";
        return;
    }

    string Query;
    dbResults r_ids;
    //dbResults r_days; not needed here? read comment above
    dbResults r_spines;
    vector <double> d_tmp;
    vector <double> v_mins;
    //std::vector<double >::iterator it;
    Matrix m;
    double dist;
    //db.QueryCount = 0;

    db.Query("CREATE TABLE distances (dendrite_id TEXT, day INTEGER, distance DOUBLE);");
    //for each dendrite id
    r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites;");
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        //for each day? please read note above.
        /*Query = "SELECT DISTINCT day FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\";";
        r_days = db.Query(Query);
        for (vInt j = 0; j < r_days.size(); j++)
        {
            //get a list of desired spines
            Query = "SELECT spine_id, x_coord, y_coord, spine_type FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\" AND day = ";
            Query += r_days[j][0];*/

            Query = "SELECT spine_id, x_coord, y_coord, spine_type FROM spines WHERE dendrite_id = \"";
            Query += r_ids[i][0];
            Query += "\"";
            if (gained == false) Query += " AND spine_type != \"gained\"";
            if (stable == false) Query += " AND spine_type != \"stable\"";
            if (lost == false) Query += " AND spine_type != \"lost\"";
            Query += ";";
            r_spines = db.Query(Query);
            //db.ShowLastResults();
            m.clear();
            for (vInt k = 0; k < r_spines.size(); k++)
            {
                d_tmp.clear();
                //calc distance to each other
                for (vInt l = 0; l < r_spines.size(); l++)
                {
                    dist = _distance2d(_atof(r_spines[k][1]), _atof(r_spines[k][2]), _atof(r_spines[l][1]), _atof(r_spines[l][2]));
                    d_tmp.push_back(dist);
                }
                m.push_back(d_tmp);
            }

            //get the minimum distances out of the matrices
            v_mins = _ExtractMinimaFromMatrix(m);

            //insert into database
            for (vInt n = 0; n < v_mins.size(); n++)
            {
                Query = "INSERT INTO distances VALUES(\"";
                Query += r_ids[i][0];
                Query += "\", ";
                Query += "NULL"; //ignore days option
                //Query += r_days[j][0];
                Query += ", ";
                Query += _ftoa(v_mins[n]);
                Query += ");";
                db.Query(Query);
                _stdoutput << "\rInfo> Added minimum distance of " << v_mins[n] << " to dendrite \"" << r_ids[i][0] << "\".";
            }
        //}
    }
    _stdoutput << endl;
}


//calculates clusters for all dendrites
//PLEASE READ NOTE OVER CalcDistances. This explains why for day NULL values are inserted.
void CalcCluster(Database &db, DBScan &dbs, string dendrite_id, double eps, unsigned int minPts, bool make_jpeg, bool wait_click)
{
    string Query;
    dbResults r_gained, r_exists, r_boundaries, r_others;
    long diff_x = 0;
    long diff_y = 0;

    //obviously cimg does not support jpg as native format.
    //the jpeg support can be implemented later...
    //so let's stay at bmp first.
    string jpegfile = "dbscan_";
    jpegfile += dendrite_id;
    jpegfile += ".bmp";

    dbs.Clear();

    //do we already have values for this dendrite?
    Query = "SELECT * FROM clusters WHERE dendrite_id = \"";
    Query += dendrite_id;
    Query += "\" LIMIT 1;";
    r_exists = db.Query(Query);
    if (r_exists.size() > 0)
    {
        _stdoutput << "Info> Cluster values for dendrite \"" << dendrite_id << "\" will be recalculated." << endl;
        Query = "DELETE FROM clusters WHERE dendrite_id = \"";
        Query += dendrite_id;
        Query += "\";";
        db.Query(Query);

        if (_FileExists(jpegfile) == true) remove(jpegfile.c_str());
    }


    //now get the gained spines and their coords of the specified dendrite
    Query = "SELECT dendrite_id, spine_id, x_coord, y_coord FROM spines WHERE spine_type = \"gained\" AND dendrite_id = \"";
    Query += dendrite_id;
    Query += "\";";
    r_gained = db.Query(Query);

    if (r_gained.size() == 0)
    {
        _stdoutput << "Warning> Cannot run dbscan on dendrite (\"" << dendrite_id << "\"). Maybe it doesn't have gained spines? Skipped." << endl;
        return;
    }
    else
    {
        _stdoutput << "\rInfo> A dbscan on dendrite \"" << dendrite_id <<"\" with epsilon=" << eps << " and minPts=" << minPts << " revealed ";
    }

    //now we have all spines.
    for (vInt i = 0; i < r_gained.size(); i++)
    {
        //insert each coordinate of the gained spines into the dbscan class
        dbs.InsertPoint(_atof(r_gained[i][2]), _atof(r_gained[i][3]));
    }

    //run the dbscan
    dbs.RunDBScan(eps, minPts);
    _stdoutput << dbs.nCluster << " clusters.";
    if (dbs.nCluster == 0) _stdoutput << " Maybe you should increase eps and/or decrease minPts?";
    _stdoutput << endl;

    //insert this info into the database
    Query = "INSERT INTO clusters VALUES (\"";
    Query += dendrite_id;
    Query += "\", NULL, ",
    Query += _ftoa(eps);
    Query += ", ";
    Query += _itoa(minPts);
    Query += ", ";
    Query += _itoa(dbs.nCluster);
    Query += ");";
    db.Query(Query);


    if (make_jpeg == true)
    {
        //get the image boundaries
        //Query = "SELECT max(x_coord), max(y_coord), min(x_coord), min(y_coord) FROM spines WHERE spine_type = \"gained\" AND dendrite_id = \"";
        Query = "SELECT max(x_coord), max(y_coord), min(x_coord), min(y_coord) FROM spines WHERE spine_type != \"lost\" AND dendrite_id = \"";
        Query += dendrite_id;
        Query += "\";";
        r_boundaries = db.Query(Query);
        //now we have the max x,y, and min x,y.
        //use min x,y to substract from all coordinates.
        diff_x = 0 - _atoi(r_boundaries[0][2]);
        diff_y = 0 - _atoi(r_boundaries[0][3]);
        //give it some extra space so that no drawn object is cutted
        diff_x += PADDING_X;
        diff_y += PADDING_Y;

        //shift to right to have some space for text
        diff_x += EXTRA_PADDING_X;

        //TODO: bild ausgehend von XY-Koordinaten scalen...
        CImg<unsigned char> img( _atoi(r_boundaries[0][0]) + diff_x + PADDING_X , _atoi(r_boundaries[0][1]) + diff_y + PADDING_Y , 1 , 3 );
        img.fill(255,255,255);
        //CImgDisplay img_d(_atoi(r_boundaries[0][0]) + diff_x + PADDING_X, _atoi(r_boundaries[0][1]) + diff_y + PADDING_Y, "SPINEMINER DBSCAN DATA", 3, true, false);
        CImgDisplay img_d(img,"SPINEMINER DBSCAN DATA");
        img_d.move(100,100);
        string caption = "Dendrite \"";
        caption += dendrite_id;
        caption += "\"\nclusters=";
        caption += _itoa(dbs.nCluster);
        caption += "\nepsilon=";
        caption += _ftoa(eps);
        caption += "\nMinPts=";
        caption += _itoa(minPts);

        //get all the other stable spines and draw them gray and shady to approximately show dendrite shape...
        Query = "SELECT x_coord, y_coord FROM spines WHERE spine_type = \"stable\" AND dendrite_id = \"";
        Query += dendrite_id;
        Query += "\";";
        r_others = db.Query(Query);
        for (vInt i = 0; i < r_others.size(); i++)
        {
            img.draw_circle(_atoi(r_others[i][0]) + diff_x , _atoi(r_others[i][1]) + diff_y , BACKGROUND_STABLE_SPINES_RADIUS, col[0], BACKGROUND_STABLE_SPINES_OPACITY);
        }


        //go through each point in cluster...
        for (vInt i = 0; i < dbs.vecNodes.size(); i++)
        {
            if (dbs.vecNodes[i]->m_nCluster >= MAX_COLORS)
            {
                _stdoutput << "Warning> Cannot assign more than " << MAX_COLORS << " to the clusters. Skipped" << endl;
                return;
            }

            img.draw_circle(dbs.vecNodes[i]->GetX() + diff_x , dbs.vecNodes[i]->GetY() + diff_y , IMAGE_SPINERADIUS, col[dbs.vecNodes[i]->m_nCluster + 2], IMAGE_SPINEOPACITY);


            img_d.display(img).wait(WAIT_DRAW_CIRCLE);
        }

        img.blur(IMAGE_BLUR);
        img.draw_text(10,10,caption.c_str(),col[1]);

        if (wait_click == true) img_d.display(img).wait();
        else
        img_d.display(img).wait(WAIT_SHOW_CLUSTERING);
        img.save_bmp(jpegfile.c_str());
    }

    return;
}

void CalcClusterFromPrompt(Database &db, DBScan &dbs, string dendrite_id, double eps, unsigned int minPts, bool make_jpeg)
{
    dbResults r_ids;
    //if the user entered *
    if (dendrite_id == "*")
    {
        //get all the dendrites
        r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites ORDER BY dendrite_id");
        if (r_ids.size() == 0)
        {
            _stdoutput << "Error> No dendrites found for a DBScan." << endl;
            return;
        }

        //else run dbscan for all dendrites
        for (vInt i = 0; i < r_ids.size(); i++)
        {
            CalcCluster(db, dbs, r_ids[i][0], eps, minPts, make_jpeg, false);
        }
    }
    else CalcCluster(db, dbs, dendrite_id, eps, minPts, make_jpeg, true);
    //otherwise he wants a specific dendrite to be calced

    return;
}

void PreCalcClusters(Database &db, DBScan &dbs, bool table_exists)
{
    string Query;
    dbResults r_ids;
    dbResults r_assumedepsilon;
    //does table clusters does not exist create clusters table
    if (!table_exists) db.Query("CREATE TABLE clusters (dendrite_id TEXT, day INTEGER, epsilon DOUBLE, minPts INTEGER, clusters INTEGER);");

    _stdoutput << "Info> This is a quick DBScan of all dendrites. NOTE: the used values for epsilon and minPts should be adjusted individually!" << endl;
    _stdoutput << "Info> To do this run with -dbscan option." << endl;

    //get all the ids
    r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites ORDER BY dendrite_id");
    if (r_ids.size() == 0)
    {
        _stdoutput << "Error> No dendrites found for a quick DBScan." << endl;
        return;
    }

    //for each dendrite
    for (vInt i = 0; i < r_ids.size(); i++)
    {
        //get the AVG*1.12 value of distances as a preassumption for the dbscans
        Query = "SELECT AVG(distance)*";
        Query += EPSILON_ASSUMPTION_CORR_FACTOR;
        Query += " FROM distances WHERE dendrite_id = \"";
        Query += r_ids[i][0];
        Query += "\";";
        r_assumedepsilon = db.Query (Query);

        //now calc dbscan with this assumption
        CalcCluster(db, dbs, r_ids[i][0], _atof(r_assumedepsilon[0][0]), DEFAULT_MINPTS, true, false);
    }

    return;
}



//creates a new database
void NewAnalysis(string dbfile, bool ignoremorphologies, bool nofilopodia)
{
    OVLReader O;
    Database db;
    DBScan dbs;
    vector <string> filenames;
    string Query;
    int m = 0;

    //get a list of ovl files which can be used
    filenames = O.ScanFiles();

    if (filenames.size() == 0)
    {
        _stdoutput << endl << O.LastError.str();
        _stdoutput << "Info> Spinecounter stopped." << endl;
        return;
    }

    _stdoutput << "Info> Creating now virtual neurons in " << dbfile << endl;
    if (ignoremorphologies == true) _stdoutput << "Info> Spine morphologies will be ignored." << endl;
    if (nofilopodia == true) _stdoutput << "Info> Filopodia will be counted as thin spines." << endl;
    //pass the commandline options for morphologies / filopodia to the class instance
    O.IgnoreSpineMorphologies = ignoremorphologies;
    O.NoFilopodia = nofilopodia;

    for (vInt n = 0; n < filenames.size(); n++)
    {
        //read all data from the files into the database
        if (!db.IsOpen())
        {
            if(!db.Open(dbfile))
            {
                _stdoutput << "Error> Cannot open database file. Aborted." << endl;
                return;
            }

            //db.Query("CREATE TABLE dendrites (dendrite_id TEXT, day INTEGER, length DOUBLE, stable_spines INTEGER, frac_stable_spines DOUBLE, gained_spines INTEGER, frac_gained_spines DOUBLE, lost_spines INTEGER, frac_lost_spines DOUBLE, spine_density DOUBLE, frac_survival DOUBLE, gainedlost INTEGER, frac_gainedlost DOUBLE, transient_spines INTEGER, frac_transient_spines DOUBLE, tor DOUBLE, filopodia INTEGER, frac_filopodia DOUBLE, mushrooms INTEGER, frac_mushrooms DOUBLE, thins INTEGER, frac_thins DOUBLE, stubbies INTEGER, frac_stubbies DOUBLE, newgained_survival DOUBLE, gained_filopodia INTEGER, frac_gained_filopodia DOUBLE, gained_mushrooms INTEGER, frac_gained_mushrooms DOUBLE, gained_thins INTEGER, frac_gained_thins DOUBLE, gained_stubbies INTEGER, frac_gained_stubbies DOUBLE, lost_filopodia INTEGER, frac_lost_filopodia DOUBLE, lost_mushrooms INTEGER, frac_lost_mushrooms DOUBLE, lost_thins INTEGER, frac_lost_thins DOUBLE, lost_stubbies INTEGER, frac_lost_stubbies DOUBLE, gained_transient_filopodia INTEGER, frac_gained_transient_filopodia DOUBLE, gained_transient_mushrooms INTEGER, frac_gained_transient_mushrooms DOUBLE, gained_transient_thins INTEGER, frac_gained_transient_thins DOUBLE, gained_transient_stubbies INTEGER, frac_gained_transient_stubbies  DOUBLE, lost_transient_filopodia INTEGER, frac_lost_transient_filopodia DOUBLE, lost_transient_mushrooms INTEGER, frac_lost_transient_mushrooms DOUBLE, lost_transient_thins INTEGER, frac_lost_transient_thins DOUBLE, lost_transient_stubbies INTEGER, frac_lost_transient_stubbies DOUBLE, persistent_filopodia INTEGER, persistent_mushrooms INTEGER, persistent_thins INTEGER, persistent_stubbies INTEGER, transient_filopodia INTEGER, transient_mushrooms INTEGER, transient_thins INTEGER, transient_stubbies INTEGER, shapeshifting_frequency DOUBLE, regained_spines INTEGER, file TEXT);");
            db.Query("CREATE TABLE dendrites (dendrite_id TEXT, day INTEGER, length DOUBLE, stable_spines INTEGER, gained_spines INTEGER, lost_spines INTEGER, living_spines INTEGER, spine_density DOUBLE, frac_survival DOUBLE, gainedlost INTEGER, transient_spines INTEGER, tor DOUBLE, filopodia INTEGER, mushrooms INTEGER, thins INTEGER, stubbies INTEGER, newgained_survival DOUBLE, gained_filopodia INTEGER, gained_mushrooms INTEGER, gained_thins INTEGER, gained_stubbies INTEGER, stable_filopodia INTEGER, stable_mushrooms INTEGER, stable_thins INTEGER, stable_stubbies INTEGER, lost_filopodia INTEGER, lost_mushrooms INTEGER, lost_thins INTEGER, lost_stubbies INTEGER, gained_transient_filopodia INTEGER, gained_transient_mushrooms INTEGER, gained_transient_thins INTEGER, gained_transient_stubbies INTEGER, stable_transient_filopodia INTEGER, stable_transient_mushrooms INTEGER, stable_transient_thins INTEGER, stable_transient_stubbies INTEGER, lost_transient_filopodia INTEGER, lost_transient_mushrooms INTEGER, lost_transient_thins INTEGER, lost_transient_stubbies INTEGER, shapeshifting_frequency DOUBLE, regained_spines INTEGER, file TEXT);");
            db.Query("CREATE TABLE spines (spine_id TEXT, spine_type TEXT, morphology TEXT, dendrite_id TEXT, day INTEGER, is_transient BOOLEAN, is_regained BOOLEAN, x_coord DOUBLE, y_coord DOUBLE);");
        }

        m += O.ReadAllData(filenames[n], db);

        _stdoutput << "\rInfo> Added " << n + 1 << " timepoints and " << m << " spines...";

    }

    _stdoutput << endl;

    //now start wih calculations
    db.Query("SELECT COUNT(DISTINCT \"dendrite_id\") FROM dendrites;");
    if (db.Results.size() > 0)
    {
        _stdoutput << "Info> Database created." << endl;
    }
    else
    {
        _stdoutput << "Error> Database empty." << endl;
        Quit();
    }
    /*_stdoutput << "Info> Added "<<db.Results[0][0]<<" dendrites and ";
    db.Query("SELECT COUNT(\"day\") FROM dendrites;");
    _stdoutput << db.Results[0][0] << " imaging timepoints with ";
    db.Query("SELECT COUNT(\"spine_id\") FROM spines;");
    _stdoutput << db.Results[0][0] << " spines in total." << endl;*/

    // Updating the LOST table.
    // Problem: between day 1 and two a stable spine becomes lost. In parallel, a lost spine becomes regained.
    // By calculating the differenence between spines numbers marked in red at day 0 and 1 it would result in zero!
    // Solution: calculate lost spines individually: count lost only when it was gained/or stable before
    _stdoutput << "Info> Updating table for lost spines." << endl;
    dbResults r_ids, r_days, r_losts, r_statusbefore;
    int counter = 0;
    db.Query("UPDATE dendrites SET lost_spines = 0"); //format the column
    //get the ids
    r_ids = db.Query("SELECT DISTINCT dendrite_id FROM dendrites;");
    //db.QueryCount = 0;
    //for each id
    for (vInt p = 0; p < r_ids.size(); p++)
    {
        //get the days
        Query = "SELECT DISTINCT day FROM dendrites WHERE dendrite_id = \"";
        Query += r_ids[p][0];
        Query += "\" ORDER BY day;";
        r_days = db.Query(Query);
        //we start with day1
        if (r_days.size() > 0)
        {
            //for each day
            for (vInt q = 1; q < r_days.size(); q++)
            {
                //get the lost spines
                Query = "SELECT spine_id FROM spines WHERE dendrite_id = \"";
                Query += r_ids[p][0];
                Query += "\" AND day = ";
                Query += r_days[q][0];
                Query += " AND spine_type = \"lost\";";
                r_losts = db.Query(Query);

                //for each lost spine
                //_stdoutput << r_days[q][0] << "/" << r_losts.size() << endl;
                for (vInt r = 0; r < r_losts.size(); r++)
                {
                    //was the lost spine stable or gained before?
                    Query = "SELECT spine_type, day FROM spines WHERE dendrite_id = \"";
                    Query += r_ids[p][0];
                    Query += "\" AND day < ";
                    Query += r_days[q][0];
                    Query += " AND spine_id = \"";
                    Query += r_losts[r][0];
                    Query += "\" ORDER BY day DESC LIMIT 1;";
                    r_statusbefore = db.Query(Query);
                    //_stdoutput << Query << endl;
                    //db.ShowLastResults();
                    //_stdoutput << "spine_id = " << r_losts[r][0] << "curr_day = " << r_days[q][0] << ", day before = " << r_statusbefore[0][1] << endl;
                    if (r_statusbefore[0][0] != "lost") counter++;
                    //db.QueryCount ++;
                }

                Query = "UPDATE dendrites SET lost_spines = ";
                Query += _itoa(counter);
                Query += " WHERE dendrite_id = \"";
                Query += r_ids[p][0];
                Query += "\" AND day = ";
                Query += r_days[q][0];
                Query += ";";
                db.Query(Query);

                _stdoutput << "\rInfo> Added " << counter << " lost spines to dendrite \"" << r_ids[p][0] << "\" at day " << r_days[q][0] << ".";

                counter = 0;
            }
        }
        else
        {
            _stdoutput << "Warning> Found only one imaging timepoint for dendrite \"" << r_ids[p][0] << "\". Cannot calculate lost fraction. Skipped." << endl;
        }
    }
    //update the gainedlost
    db.Query("UPDATE dendrites SET gainedlost = gained_spines + lost_spines;");
    /*db.Query("UPDATE dendrites SET gainedlost = gained_spines + lost_spines;");
    db.Query("UPDATE dendrites SET frac_lost_spines = 100 * (CAST(lost_spines AS DOUBLE)) / (gained_spines + stable_spines);");
    db.Query("UPDATE dendrites SET frac_gainedlost = 100 * (CAST(gainedlost AS DOUBLE)) / (gained_spines + stable_spines);");*/
    _stdoutput << endl;

    //redundancy or any other Errors occured?
    if (db.GetAbortFlag())
    {
        db.Close();
        db.Remove();
        _stdoutput << endl << "Error> There have been errors detected in your OVL-files. Please fix errors and retry. Analysis aborted.";
        Quit();
    }

    CalcTransients(db, TRANSIENTS_TIMEDIFF);

    CalcRegainedSpines(db);

    CalcTor(db);

    CalcDistances(db, false, true, false);

    PreCalcClusters(db, dbs, false);

    CalcSurvival(db, SURVIVAL_STARTTIMEPOINT);

    CalcNewGainedSurvival(db, NEWGAINEDSURVIVAL_STARTTIMEPOINT);

    if (!ignoremorphologies) CalcShapeshiftingFrequency(db);

    //if (!ignoremorphologies) CalcGainedLostMorphs(db);

    if (!CheckFates(db))
    {
        _stdoutput << "Error> Wrong spine fate assignments. Please recheck your overlays to avoid statistical errors." << endl;
        db.SetAbortFlag();
    }
    else _stdoutput << "Info> Database clean. No errors in spine fates found." << endl;


    if (O.LastError.str() != "")
    {
        _stdoutput << endl << "Error> Errors occured:" << endl << O.LastError.str();
    }

    UpdateDB(db);

    db.Close();

    //redundancy or any other Errors occured?
    if (db.GetAbortFlag())
    {
        db.Close();
        db.Remove();
        _stdoutput << endl << "Error> There have been errors detected in your OVL-files. Please fix errors and retry. Analysis aborted.";
        Quit();
    }

    _stdoutput << endl;

    return;
}




