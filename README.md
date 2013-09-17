# SpineMiner 
----------

Simon M. Ochs, 2013

## Abstract
SpineMiner is an image overlay processor. Zeiss' [ZEN](http://www.zeiss.de/ZEN "ZEN") 2009 (or higher) software provides text annotations of microscopy images which are stored as so called OVERLAY-Files (.ovl). This feature is commonly used in neuroscience when analyzing two-photon in vivo imaging data of dendritic spines, cell bodies or other objects. Each text-annotation is stored within a binary stream in the ovl-file. Since this file format is not open, the information for extracting the relevant data (text, text-color, x- and y-position of the text-box) had to be obtained by reverse engineering. During extraction of the ovl-files, the data is stored in a small mySQL database which follows a very simple 1:N entity relationship model. The database is embedded as [SQLite3](http://www.sqlite.org/ "SQLite3") library within the application source. When the database creation is complete, SpineMiner processes the tables to calculate a whole bunch of scientific readouts. If you wish, you can also command SpineMiner to process your own SQL-Plugin to generate a readout which is not natively implemented in the sourcecode. All results can be converted as comma-separated values. 
Currently, data acquisition and calculation of readouts is done using [Microsoft's Excel](http://office.microsoft.com/excel‎ "Microsoft Excel") or similar table calculators which is very time consuming. Using SpineMiner, this step can be shortened to 1/1000 of the time you spent in front of MS Excel. SpineMiner currently runs on command line. A userfriendly GUI for Windows already exists and will be also published on GitHub as "SpineMinerXT".

## Scientific Background  
In terms of neurosciences, memory is based on the plasticity of neuronal networks[1]. Neurons are interconnected via synapses. A synapse is composed of three parts[1]:

1. The presynapse which receives action potencials from the Neuron. It releases neurotransmitters (NT) into the... 
2. ...Synaptic cleft.
3. NTs are taken up from the cleft at the postsynapse (=dendritic spine) by NT-receptors. This leads to the formation of a new electric current integrated along the dendrite of the inputted neuron. 

In a way, spines are associated with learning and cognitive function[2]. Using state-of-the-art two-photon microscopy techniques, spines and neurons can be observed even in the living brain in frequent intervals over months[3,4]. To quantify in vivo kinetics, the image stacks have to be evaluated for each timepoint by hand. There are approaches to do quantification automatically which usually fails due to heavy fluctuations in image quality. Therefore the best way to get those information still is to tag synapses or cell bodies with colored circles or letters and to count those objects manually. The last step, transferring circles/letters and/or their colors into tables is pretty time cosuming and can be shortened using a defined annotation protocoll for the overlay-files and SpineMiner.

### The SpineMiner annotation protocoll using [ZEN](http://www.zeiss.de/ZEN "ZEN") 2009 Software
ZEN is capable to read a high diversity of file formats, including LSM, TIFF, FCS, RAW, EPS and others. Note that you don't necessarily have to count spines with this workflow. The whole system also works with cell bodies or other objects of whose kinetics you are interested in. However, for clarity we will proceed as if we are working with spines from now on.
A demonstrating counting video can be wached on here: [http://vimeo.com/72959890](http://vimeo.com/72959890 "http://vimeo.com/72959890")

 
- Timepoint 1 
	1. Open your image file.
	2. Select the text-tool from the overlay options. Default font = arial. Color = blue (#4). Write "id=X" into the image (without quotes), whereas X can be any alphanumeric character (a-z A-Z 0-9). X represents a unique dendrite ID and stays constant for all imaging timepoints.
	3. Add "l=LL.LL" (without quotes, color = blue (#4)). l represents the dendrite length, LL.LL a floating point value, usually in micrometers.
	4. Add "day=dd" (without quotes, color = blue (#4)). day represents the imaging timepoint (which does not necessarily have to be days). NOTE: THE FIRST TIMEPOINT STARTS WITH day=1.
	5. Now add spines/cells whatever you like. Add a tag to each object. The tag has to have the format "Ic" (without quotes, e.g. 1s, 2f, 12m, 19t...) using color = white (#1; "stable"). I stands for the spine ID which has to be a decimal number. c stands for a single character reflecting the morphology of the spine: s=stubby, f=filopodia, m=mushroom, t=thin. NOTE: YOU HAVE TO DECIDE FIRST IF YOU WANT TO QUANTIFY SPINE MORPHOLOGIES OR NOT. DON'T STOP USING MORPHOLOGIES IN THE MIDDLE OF YOUR EVALUATION BECAUSE SPINEMINER CAN'T WORK WITH MIXTURES. If you are indeed counting spines, i recommend ALLWAYS TO USE MORPHOLOGY TAGS.
	6. Save your overlay file.
- Timepoint 2
	1. Open the next imaging timepoint.
	2. Reopen the overlay you just saved.
	3. Update the "day=" tag.
	4. Adapt the textbox positions to their according spines.
	5. Update the morphology tags (perhaps a spine shifted from mushroom to stubby?).
	6. Update spines: when a spine is gone, give it the color red (#3; "lost"). When a new spine appeared, give it a NEW UNIQUE ID which was NOT USED BEFORE and the color green (#2; "gained").
	7. Save your overlay as NEW file.
- Timepoint i
	1. Open the next imaging timepoint.
	2. Reopen the overlay you just saved, update the "day=" tag.  
	3. Adapt the textbox positions to their according spines.
	4. Update the morphology tags. 
	5. Update spines: when a spine was lost, give it the color red. When a new spine was gained, give it a new ID and the color green. A spine, which was "gained" (=green) the timepoint before and still exists is now to be marked as "stable". So give it the color white. When you are sure, that a spines pertubes at the same location where it was lost before, then just change it's color from red to green again. This spine will later be detected as "regained spine".
	6. Save your overlay as NEW file.
	7. Continue with Timepoint i.


## Generated Readouts
Some of the scientific readouts generated by SpineMiner are totally new and not published so far. Some of them will be published by Ochs et al. in near future[6]. The following Readouts can be generated by SpineMiner. NOTE: readouts tagged with "*" can be adapted by user input:

- Spine density (spines / micrometer)
- *Survival (fraction of spines detected at timepoint N still present at timepoint N+1, N+1+1, N+1+1+1, N+i?)
- *New gained survival (fraction of gained spines detected at timepoint N still present at timepoint N+1, N+1+1, N+1+1+1, N+i?)
- dTOR (daily Turnover Rate. dTOR(t, t-1) = (gained spines+lost spines/2*all spines) / ((t) - (t-1)) )
- *Transient spines (spines which survive less than N timepoints)
- *Persistent spines (spines which survive longer than N timepoints)
- Gained spines
- Stable spines
- Lost spines
- Shapeshifting frequency (how many spines change their shape per day)
- Density of mushrooms, stubbies, thins and filopodia
- *Density of transient mushrooms, stubbies, thins and filopodia
- *Density of persistent, stubbies, thins and filopodia
- *Density of persistent/transient & gained/lost/stable mushrooms, stubbies, thins and filopodia
- Regained spines
- Regained/Gained ratio
- Regained spines latency times
- Regained spines lifetimes
- Regained spines morphologies
- Minimum distances between gained spines
- *Density based clustering of gained spines[5]

## Generating New Readouts Using mySQL-Plugins
In the case you miss something you may feel free to edit the sourcecode of SpineMiner. However, in any case you can write your own SQL-Plugins to generate more readouts. The SQL-Plugin is just a batch of SQL-Commands you can pipe directly to SpineMiner and the database using the -pipe option.
Example:

    --Here we would like to obtain the number of transient spines which are mushroom, transient or thin
    CREATE TABLE out (d INTEGER, m INTEGER, s INTEGER, t INTEGER, f INTEGER);
    CREATE TABLE var (v INTEGER);
    
    SELECT "BEGIN TABLE TRANSIENT FILOPODIA: DAY, MUSHROOM, STUBBY, THIN, FILOPODIA";
    
    INSERT INTO var VALUES (1); -- enter here the day you want to analyze
    INSERT INTO out VALUES ((SELECT * FROM var) , (SELECT COUNT(spinesM.spine_id) FROM spines AS spinesM WHERE spinesM.is_transient = 1 AND spinesM.morphology = "m" AND spinesM.day = (SELECT * FROM var)) , (SELECT COUNT(spinesS.spine_id) FROM spines AS spinesS WHERE spinesS.is_transient = 1 AND spinesS.morphology = "s" AND spinesS.day = (SELECT * FROM var)) , (SELECT COUNT(spinesT.spine_id) FROM spines AS spinesT WHERE spinesT.is_transient = 1 AND spinesT.morphology = "t" AND spinesT.day = (SELECT * FROM var)) , (SELECT COUNT(spinesF.spine_id) FROM spines AS spinesF WHERE spinesF.is_transient = 1 AND spinesF.morphology = "f" AND spinesF.day = (SELECT * FROM var)));

    UPDATE var SET v = 3; -- enter here the day you want to analyze
    INSERT INTO out VALUES ((SELECT * FROM var) , (SELECT COUNT(spinesM.spine_id) FROM spines AS spinesM WHERE spinesM.is_transient = 1 AND spinesM.morphology = "m" AND spinesM.day = (SELECT * FROM var)) , (SELECT COUNT(spinesS.spine_id) FROM spines AS spinesS WHERE spinesS.is_transient = 1 AND spinesS.morphology = "s" AND spinesS.day = (SELECT * FROM var)) , (SELECT COUNT(spinesT.spine_id) FROM spines AS spinesT WHERE spinesT.is_transient = 1 AND spinesT.morphology = "t" AND spinesT.day = (SELECT * FROM var)) , (SELECT COUNT(spinesF.spine_id) FROM spines AS spinesF WHERE spinesF.is_transient = 1 AND spinesF.morphology = "f" AND spinesF.day = (SELECT * FROM var)));

    UPDATE var SET v = 7; -- enter here the day you want to analyze
    INSERT INTO out VALUES ((SELECT * FROM var) , (SELECT COUNT(spinesM.spine_id) FROM spines AS spinesM WHERE spinesM.is_transient = 1 AND spinesM.morphology = "m" AND spinesM.day = (SELECT * FROM var)) , (SELECT COUNT(spinesS.spine_id) FROM spines AS spinesS WHERE spinesS.is_transient = 1 AND spinesS.morphology = "s" AND spinesS.day = (SELECT * FROM var)) , (SELECT COUNT(spinesT.spine_id) FROM spines AS spinesT WHERE spinesT.is_transient = 1 AND spinesT.morphology = "t" AND spinesT.day = (SELECT * FROM var)) , (SELECT COUNT(spinesF.spine_id) FROM spines AS spinesF WHERE spinesF.is_transient = 1 AND spinesF.morphology = "f" AND spinesF.day = (SELECT * FROM var)));

    UPDATE var SET v = 10; -- enter here the day you want to analyze
    INSERT INTO out VALUES ((SELECT * FROM var) , (SELECT COUNT(spinesM.spine_id) FROM spines AS spinesM WHERE spinesM.is_transient = 1 AND spinesM.morphology = "m" AND spinesM.day = (SELECT * FROM var)) , (SELECT COUNT(spinesS.spine_id) FROM spines AS spinesS WHERE spinesS.is_transient = 1 AND spinesS.morphology = "s" AND spinesS.day = (SELECT * FROM var)) , (SELECT COUNT(spinesT.spine_id) FROM spines AS spinesT WHERE spinesT.is_transient = 1 AND spinesT.morphology = "t" AND spinesT.day = (SELECT * FROM var)) , (SELECT COUNT(spinesF.spine_id) FROM spines AS spinesF WHERE spinesF.is_transient = 1 AND spinesF.morphology = "f" AND spinesF.day = (SELECT * FROM var)));
    
    UPDATE var SET v = 14; -- enter here the day you want to analyze
    INSERT INTO out VALUES ((SELECT * FROM var) , (SELECT COUNT(spinesM.spine_id) FROM spines AS spinesM WHERE spinesM.is_transient = 1 AND spinesM.morphology = "m" AND spinesM.day = (SELECT * FROM var)) , (SELECT COUNT(spinesS.spine_id) FROM spines AS spinesS WHERE spinesS.is_transient = 1 AND spinesS.morphology = "s" AND spinesS.day = (SELECT * FROM var)) , (SELECT COUNT(spinesT.spine_id) FROM spines AS spinesT WHERE spinesT.is_transient = 1 AND spinesT.morphology = "t" AND spinesT.day = (SELECT * FROM var)) , (SELECT COUNT(spinesF.spine_id) FROM spines AS spinesF WHERE spinesF.is_transient = 1 AND spinesF.morphology = "f" AND spinesF.day = (SELECT * FROM var)));
    
    UPDATE var SET v = 17; -- enter here the day you want to analyze
    INSERT INTO out VALUES ((SELECT * FROM var) , (SELECT COUNT(spinesM.spine_id) FROM spines AS spinesM WHERE spinesM.is_transient = 1 AND spinesM.morphology = "m" AND spinesM.day = (SELECT * FROM var)) , (SELECT COUNT(spinesS.spine_id) FROM spines AS spinesS WHERE spinesS.is_transient = 1 AND spinesS.morphology = "s" AND spinesS.day = (SELECT * FROM var)) , (SELECT COUNT(spinesT.spine_id) FROM spines AS spinesT WHERE spinesT.is_transient = 1 AND spinesT.morphology = "t" AND spinesT.day = (SELECT * FROM var)) , (SELECT COUNT(spinesF.spine_id) FROM spines AS spinesF WHERE spinesF.is_transient = 1 AND spinesF.morphology = "f" AND spinesF.day = (SELECT * FROM var)));

    UPDATE var SET v = 21; -- enter here the day you want to analyze
    INSERT INTO out VALUES ((SELECT * FROM var) , (SELECT COUNT(spinesM.spine_id) FROM spines AS spinesM WHERE spinesM.is_transient = 1 AND spinesM.morphology = "m" AND spinesM.day = (SELECT * FROM var)) , (SELECT COUNT(spinesS.spine_id) FROM spines AS spinesS WHERE spinesS.is_transient = 1 AND spinesS.morphology = "s" AND spinesS.day = (SELECT * FROM var)) , (SELECT COUNT(spinesT.spine_id) FROM spines AS spinesT WHERE spinesT.is_transient = 1 AND spinesT.morphology = "t" AND spinesT.day = (SELECT * FROM var)) , (SELECT COUNT(spinesF.spine_id) FROM spines AS spinesF WHERE spinesF.is_transient = 1 AND spinesF.morphology = "f" AND spinesF.day = (SELECT * FROM var)));

    UPDATE var SET v = 23; -- enter here the day you want to analyze
    INSERT INTO out VALUES ((SELECT * FROM var) , (SELECT COUNT(spinesM.spine_id) FROM spines AS spinesM WHERE spinesM.is_transient = 1 AND spinesM.morphology = "m" AND spinesM.day = (SELECT * FROM var)) , (SELECT COUNT(spinesS.spine_id) FROM spines AS spinesS WHERE spinesS.is_transient = 1 AND spinesS.morphology = "s" AND spinesS.day = (SELECT * FROM var)) , (SELECT COUNT(spinesT.spine_id) FROM spines AS spinesT WHERE spinesT.is_transient = 1 AND spinesT.morphology = "t" AND spinesT.day = (SELECT * FROM var)) , (SELECT COUNT(spinesF.spine_id) FROM spines AS spinesF WHERE spinesF.is_transient = 1 AND spinesF.morphology = "f" AND spinesF.day = (SELECT * FROM var)));

    UPDATE var SET v = 28; -- enter here the day you want to analyze
    INSERT INTO out VALUES ((SELECT * FROM var) , (SELECT COUNT(spinesM.spine_id) FROM spines AS spinesM WHERE spinesM.is_transient = 1 AND spinesM.morphology = "m" AND spinesM.day = (SELECT * FROM var)) , (SELECT COUNT(spinesS.spine_id) FROM spines AS spinesS WHERE spinesS.is_transient = 1 AND spinesS.morphology = "s" AND spinesS.day = (SELECT * FROM var)) , (SELECT COUNT(spinesT.spine_id) FROM spines AS spinesT WHERE spinesT.is_transient = 1 AND spinesT.morphology = "t" AND spinesT.day = (SELECT * FROM var)) , (SELECT COUNT(spinesF.spine_id) FROM spines AS spinesF WHERE spinesF.is_transient = 1 AND spinesF.morphology = "f" AND spinesF.day = (SELECT * FROM var)));
    
    UPDATE var SET v = 31; -- enter here the day you want to analyze
    INSERT INTO out VALUES ((SELECT * FROM var) , (SELECT COUNT(spinesM.spine_id) FROM spines AS spinesM WHERE spinesM.is_transient = 1 AND spinesM.morphology = "m" AND spinesM.day = (SELECT * FROM var)) , (SELECT COUNT(spinesS.spine_id) FROM spines AS spinesS WHERE spinesS.is_transient = 1 AND spinesS.morphology = "s" AND spinesS.day = (SELECT * FROM var)) , (SELECT COUNT(spinesT.spine_id) FROM spines AS spinesT WHERE spinesT.is_transient = 1 AND spinesT.morphology = "t" AND spinesT.day = (SELECT * FROM var)) , (SELECT COUNT(spinesF.spine_id) FROM spines AS spinesF WHERE spinesF.is_transient = 1 AND spinesF.morphology = "f" AND spinesF.day = (SELECT * FROM var)));
    
    UPDATE var SET v = 35; -- enter here the day you want to analyze
    INSERT INTO out VALUES ((SELECT * FROM var) , (SELECT COUNT(spinesM.spine_id) FROM spines AS spinesM WHERE spinesM.is_transient = 1 AND spinesM.morphology = "m" AND spinesM.day = (SELECT * FROM var)) , (SELECT COUNT(spinesS.spine_id) FROM spines AS spinesS WHERE spinesS.is_transient = 1 AND spinesS.morphology = "s" AND spinesS.day = (SELECT * FROM var)) , (SELECT COUNT(spinesT.spine_id) FROM spines AS spinesT WHERE spinesT.is_transient = 1 AND spinesT.morphology = "t" AND spinesT.day = (SELECT * FROM var)) , (SELECT COUNT(spinesF.spine_id) FROM spines AS spinesF WHERE spinesF.is_transient = 1 AND spinesF.morphology = "f" AND spinesF.day = (SELECT * FROM var)));

    -- Now get the results
    SELECT * FROM out;

    -- Make clean
    DROP TABLE out;
	DROP TABLE var;

Saving this batch in `my.sql` and run `SpineMiner.exe -r:<filename> -pipe:my.sql` will run the plugin. The results are stored in `my.sql.results.txt`.

## Building SpineMiner
SpineMiner is written in ISO C++ and should be compilable on multiple platforms. Tested operating systems: Windows XP SP3 x32, Windows 7 x64. 

### Using [Code::Blocks](http://www.codeblocks.org/ "Code::Blocks")
This repository includes also a Code::Blocks Project file (.cbp). You may build SpineMiner with the open source IDE Code::Blocks and the included MinGW C++ & GCC Compiler.
Last working configuration: Code::Blocks 12.11 + MingW32 4.6.2. 

### Using [MinGW32](http://www.mingw.org/ "MinGW") for Windows 
You may download and install MinGW32 C++ (GCC) and MinGW32 GCC (GCC) for Windows. You can cross-compile SpineMiner by prompting the following commands in your shell:

    mingw32-g++ -Wall -fexceptions -g -c database.cpp -o obj\Debug\database.o
    mingw32-g++ -Wall -fexceptions -g -c dbscan.cpp -o obj\Debug\dbscan.o
    mingw32-g++ -Wall -fexceptions -g -c functions.cpp -o obj\Debug\functions.o
    mingw32-g++ -Wall -fexceptions -g -c helpers.cpp -o obj\Debug\helpers.o
    mingw32-gcc -Wall -fexceptions -g -c kdtree\kdtree.c -o obj\Debug\kdtree.o
    mingw32-g++ -Wall -fexceptions -g -c main.cpp -o obj\Debug\main.o
    mingw32-g++ -Wall -fexceptions -g -c ovlreader.cpp -o obj\Debug\ovlreader.o
    mingw32-gcc -Wall -fexceptions -g -c sqlite\sqlite3.c -o obj\Debug\sqlite3.o
    mingw32-g++ -o bin\Debug\SpineMiner.exe obj\Debug\database.o obj\Debug\dbscan.o obj\Debug\functions.o obj\Debug\helpers.o obj\Debug\kdtree.o obj\Debug\main.o obj\Debug\ovlreader.o obj\Debug\sqlite3.o -lgdi32 
   
Note: you could also use `g++/gcc` instead of the `mingw32-g++/mingw32-gcc` variant. However, `mingw32-` is recommended because this assures cross-compilation for the "mingw target".
You may save time by executing build.bat when you are on a Windows machine.
Last working configuration: MinGW32 C++ (GCC) 4.6.2 and MinGW32 GCC (GCC) 4.6.2. Installation: 0.5 beta 20120426-1

### External libraries
For density based clustering of gained spines and database support, [SQLite3](http://www.sqlite.org/ "SQLite3"), John Tsiombikas' [kd-tree](https://code.google.com/p/kdtree/ "kd-tree") and [CImg](http://cimg.sourceforge.net/ "CImg") libraries were added to the project. To assure build, those libraries are included in this repository. To set up CImg in SpineMiner the g++-linker needs the `-lgdi32` option.

##Running SpineMiner
As highlighted in `main.c` and in function `void ShowUsage()`, SpineMiner can be called using the following arguments:

    * SpineMiner.exe --> tests the ovl files in the current working directory for consistency
    * SpineMiner.exe -w:<filename> --> tests the ovl files in the current working directory for consistency and reads content into database <filename>
    * SpineMiner.exe -w:<filename> -ignoremorphologies --> use this, if your spines have no morphology tags
    * SpineMiner.exe -w:<filename> -nofilopodia --> all as filopodia tagged spines will be treated as thin spines
    *
    * SpineMiner.exe -r:<filename> --> reads the database and showing dendrites and spines table
    * SpineMiner.exe -r:<filename> -calcsurvival:<imaging_timepoint> --> recalculates spine survival using <imaging_timepoint> as start (default = 1)
    * SpineMiner.exe -r:<filename> -calctransients:<period> --> recalculates transient spines by a given lifespan period (days) (default = 8)
    * SpineMiner.exe -r:<filename> -calcnewgained:<imaging_timepoint> --> recalculates the survival of spines which were gained at <imaging_timepoint> (default = 2)
    * SpineMiner.exe -r:<filename> -survival:<id> --> shows survival courve for a given dendrite id
    * SpineMiner.exe -r:<filename> -trace:<dendrite_id>,<spine_id> --> shows the fate of a spine, identified by <dendrite_id> and <spine_id>
    * SpineMiner.exe -r:<filename> -check --> tests the database for spine counting errors.
    * SpineMiner.exe -r:<filename> -tor:<dendrite_id> --> shows the turnover rate of a dendrite identified by <dendrite_id>
    * SpineMiner.exe -r:<filename> -pipe:<filename2> --> Executes SQL commands in <filename2>. Results are stored in <filename2>.result.txt
    * SpineMiner.exe -r:<filename> -dbscan:<dendrite_id>,<epsilon>,<minPts> --> when * is used as dendrite_id all dendrites will calced with given eps and minPts
    * SpineMiner.exe -r:<filename> -quickdbscan --> runs a dbscan on all dendrites unsurvived.
    *
    * SpineMiner.exe -replace:<findstring>,<replacestring> --> find & replace for all ovl files in current working directory. BETA STATUS
    *
    * SpineMiner.exe -c:<filename> --> converts the database into csv format so it's readable for excel
    *
    * SpineMiner.exe -q:<filename> --> Open SQL session. Quit with "!q"
    * 
    * SpineMiner.exe -outputtocerr --> Output is directed to cerr. File logging will be deactivated.


       
SpineMiner works in the CWD. So it has to be executed where your ovl-files are located. 

## Known Issues
### Code Improvements

- Add extra space bewteen `//` and the comment.
- Remove `using` declaration in header files to avoid namespace clash.
- Change post- to pre-increment (++i) when working with SQL query results.
- `OVLReader::ReadAllData()` function is too long. Split into parser and database-insertion (`OVLReader::Notify( struct data )`?) subroutines.
- Inside classes, change output from std::cout to `public field stringstream _output` and work with that.
- Add a note to `sqlite3_errmsg()` test because this function allways returns an english statement "not an error", even when different locale settings are used.
- When using `INSERT` statements, explicitely name the inserted columns (e.g.  `INSERT INTO table (col1, col2, col3) values (i, j, "k");`). Then this statement will also work when the table design has been changed.
- More protection to class fields and use `set/get()` functions instead.
- Make `OVLReader` class independent of `Database` class and bring the whole system into an observer/subject pattern. When OVL-File read is complete, call `Notify()` event which is implemented by the observer to update the database. (maybe this is overkill)
- Implement `exceptions`.
- Make `_distance2d()`, `_ExtractMinimaFromMatrix()` and other helper functions type independent by using templates.
- Rebuild the command line argument parser using switch/cases to make it more clear.
- Rename database columns from "spine" to "object", e.g. spine\_density to object\_density, spines to objects, dendrites to carriers etc.

### Bugs

- Sometimes, the survival fractions contain in the last rows values which should not be there.
- Find&Replace feature is highly experimental and crashes several times.

## Citations
1. Yuste, R. Dendritic Spines. The MIT Press, 2010.
2. Fu, M. et al. Experience-dependent structural plasticity in the cortex. Trends Neurosci., 34(4):177–187, 2011.
3. Holtmaat, A. et al. Long-term, high-resolution imaging in the mouse neocortex through a chronic cranial window. Nat Protoc, 4(8):1128–1144, 2009.
4. Fuhrmann, M. et al. Dendritic pathology in prion disease starts at the synaptic spine. J. Neurosci., 27(23):6224–6233, 2007.
5. Ester, M. et al. A density-based algorithm for discovering clusters in large spatial databases with noise. AAAI Press, KDD-98:226-231, 1996.
6. Ochs. SM et al. Database assisted high efficacy readout processing of two-photon in vivo imaging data. Manuscript in preparation, 2013.
