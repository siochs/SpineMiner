CREATE TABLE var (v INTEGER);

SELECT "REGAINED SPINE DENSITY";
INSERT INTO var VALUES (3); -- enter here the day you want to analyze

UPDATE var SET v = 3; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(regained_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 7; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(regained_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 10; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(regained_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 14; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(regained_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 17; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(regained_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 21; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(regained_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 23; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(regained_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 28; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(regained_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 31; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(regained_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 35; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(regained_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 37; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(regained_spines / length, " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;


SELECT "FRAC REGAINED SPINES"

UPDATE var SET v = 3; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(100*regained_spines / (gained_spines+stable_spines), " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 7; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(100*regained_spines / (gained_spines+stable_spines), " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 10; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(100*regained_spines / (gained_spines+stable_spines), " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 14; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(100*regained_spines / (gained_spines+stable_spines), " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 17; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(100*regained_spines / (gained_spines+stable_spines), " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 21; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(100*regained_spines / (gained_spines+stable_spines), " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 23; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(100*regained_spines / (gained_spines+stable_spines), " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 28; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(100*regained_spines / (gained_spines+stable_spines), " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 31; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(100*regained_spines / (gained_spines+stable_spines), " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 35; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(100*regained_spines / (gained_spines+stable_spines), " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;

UPDATE var SET v = 37; -- enter here the day you want to analyze
SELECT (SELECT * FROM var), GROUP_CONCAT(100*regained_spines / (gained_spines+stable_spines), " ") FROM dendrites WHERE day = (SELECT * FROM var) GROUP BY day;


DROP TABLE var;

SELECT "END";