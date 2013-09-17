#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__


#include <dir.h>
#include <algorithm>
#include <fstream>
//#include <stdio.h>

//#include <unistd.h>

#include "database.h"
#include "ovlreader.h"
#include "helpers.h"
#include "dbscan.h"
#include "CImg/cimg.h"

#define SURVIVAL_STARTTIMEPOINT 0
#define NEWGAINEDSURVIVAL_STARTTIMEPOINT 1
#define TRANSIENTS_TIMEDIFF 8
#define EPSILON_ASSUMPTION_CORR_FACTOR  "1.1"
#define DEFAULT_MINPTS  4
#define IMAGE_SPINERADIUS   5
#define IMAGE_SPINEOPACITY   1
#define IMAGE_BLUR   3
#define WAIT_DRAW_CIRCLE    1
#define WAIT_SHOW_CLUSTERING    1
#define PADDING_X   20
#define PADDING_Y   20
#define EXTRA_PADDING_X 100
#define BACKGROUND_STABLE_SPINES_OPACITY    0.03
#define BACKGROUND_STABLE_SPINES_RADIUS     20

//using namespace cimg_library;

//vars



//Forwards
void ShowUsage(void);
void FindReplace(std::string find, std::string replace);
void Quit(void);
void SQLPipe(std::string filename, Database &db);
void UpdateDB(Database &db);
void CalcShapeshiftingFrequency(Database &db);
//void CalcTransientGainedLostMorphs(Database &db);
//void CalcGainedLostMorphs(Database &db);
std::string SpineType(std::string dendrite_id, std::string spine_id, std::string timepoint, Database &db);
void WriteSum(Database &db, std::fstream &f, std::string identifier);
void WriteRegainedSpineData(Database &db, std::fstream &f);
void WriteClusterData(Database &db, std::fstream &f);
void Convert(Database &db, std::string filename);
std::string TraceFate(std::string spine_id, std::string dendrite_id, Database &db, bool showresults);
void RetrieveFilesFromDendrite(Database &db, std::string dendrite_id);
bool CheckFates(Database &db);
void PrintTor(std::string dendrite_id, Database &db);
void PrintSurvival(std::string dendrite_id, Database &db);
void CalcSurvival(Database &db, int imaging_tp);
void CalcTransients(Database &db, int transients_period);
void CalcTor(Database &db);
void CalcNewGainedSurvival(Database &db, int imaging_tp);
void ShowTableDendrites(Database &db);
void ShowTableSpines(Database &db);
void ScanOnly();
void NewAnalysis(std::string dbfile, bool ignoremorphologies, bool nofilopodia);
void CalcCluster(Database &db, DBScan &dbs, std::string dendrite_id, double eps, unsigned int minPts, bool make_jpeg, bool wait_click);
void PreCalcClusters(Database &db, DBScan &dbs, bool table_exists);
void CalcClusterFromPrompt(Database &db, DBScan &dbs, std::string dendrite_id, double eps, unsigned int minPts, bool make_jpeg);
void CalcMorphs(Database &db);

#endif // __FUNCTIONS_H__

