--This is a SQL plugin example 
--Here we would like to obtain the number of transient spines which are mushroom, transient or thin
CREATE TABLE var (v INTEGER);


SELECT "BEGIN TABLE DENSITY OF GAINED REGAINED SPINES: DAY, DENSITY";

INSERT INTO var VALUES (1); -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

UPDATE var SET v = 3; -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

UPDATE var SET v = 7; -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

UPDATE var SET v = 10; -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

UPDATE var SET v = 14; -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

UPDATE var SET v = 17; -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

UPDATE var SET v = 21; -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

UPDATE var SET v = 23; -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

UPDATE var SET v = 28; -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

UPDATE var SET v = 31; -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

UPDATE var SET v = 35; -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

UPDATE var SET v = 37; -- enter here the day you want to analyze
SELECT "DAY",(SELECT * FROM var); 
SELECT COUNT(spines.spine_id) / dendrites.length FROM spines JOIN dendrites ON spines.dendrite_id = dendrites.dendrite_id AND spines.day = dendrites.day WHERE spines.spine_type = "gained" AND spines.is_regained = 1 AND spines.day = (SELECT * FROM var) GROUP BY dendrites.length;

DROP TABLE var;

