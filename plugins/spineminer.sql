--This is a SQL plugin example 
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
DELETE FROM out;

SELECT "BEGIN TABLE PERSISTENT SPINE DENSITY: DAY, DENSITY. STARTING FROM DAY 3";

UPDATE var SET v = 3; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT((gained_spines + stable_spines - transient_spines) / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 7; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT((gained_spines + stable_spines - transient_spines) / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 10; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT((gained_spines + stable_spines - transient_spines) / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 14; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT((gained_spines + stable_spines - transient_spines) / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 17; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT((gained_spines + stable_spines - transient_spines) / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 21; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT((gained_spines + stable_spines - transient_spines) / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 23; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT((gained_spines + stable_spines - transient_spines) / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 28; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT((gained_spines + stable_spines - transient_spines) / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 31; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT((gained_spines + stable_spines - transient_spines) / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 35; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT((gained_spines + stable_spines - transient_spines) / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;


SELECT "BEGIN TABLE TRANSIENT SPINE DENSITY: DAY, DENSITY. STARTING FROM DAY 3";

UPDATE var SET v = 3; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(transient_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 7; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(transient_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 10; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(transient_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 14; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(transient_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 17; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(transient_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 21; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(transient_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 23; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(transient_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 28; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(transient_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 31; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(transient_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 35; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(transient_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;


DROP TABLE out;
DROP TABLE var;

SELECT "END";