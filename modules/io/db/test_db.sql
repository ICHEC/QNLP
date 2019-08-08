BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "corpus" (
	"id"	INTEGER,
	"type"	TEXT,
	"name"	TEXT,
	"bin_id"	INTEGER,
	"weight"	REAL,
	"mapping_dir"	INTEGER,
	PRIMARY KEY("id")
);
CREATE TABLE IF NOT EXISTS "basis" (
	"id"	INTEGER,
	"type"	TEXT,
	"name"	TEXT,
	"bin_id"	INTEGER,
	"weight"	REAL,
	"mapping_dir"	INTEGER,
	PRIMARY KEY("id")
);
INSERT INTO "corpus" VALUES (1,'noun','Jack',0,0.0,NULL);
INSERT INTO "corpus" VALUES (2,'noun','Jill',1,0.0,NULL);
INSERT INTO "corpus" VALUES (3,'noun','hill',2,0.0,NULL);
INSERT INTO "corpus" VALUES (4,'noun','pail',3,0.0,NULL);
INSERT INTO "corpus" VALUES (5,'noun','water',4,0.0,NULL);
INSERT INTO "corpus" VALUES (6,'noun','crown',5,0.0,NULL);
INSERT INTO "corpus" VALUES (7,'noun','Up',6,0.0,NULL);
INSERT INTO "corpus" VALUES (8,'noun','home',7,0.0,NULL);
INSERT INTO "corpus" VALUES (9,'noun','As',8,0.0,NULL);
INSERT INTO "corpus" VALUES (10,'noun','head',9,0.0,NULL);
INSERT INTO "corpus" VALUES (11,'noun','vinegar',10,0.0,NULL);
INSERT INTO "corpus" VALUES (12,'noun','paper',11,0.0,NULL);
INSERT INTO "corpus" VALUES (13,'verb','went',0,0.0,NULL);
INSERT INTO "corpus" VALUES (14,'verb','fetch',1,0.0,NULL);
INSERT INTO "corpus" VALUES (15,'verb','fell',2,0.0,NULL);
INSERT INTO "corpus" VALUES (16,'verb','broke',3,0.0,NULL);
INSERT INTO "corpus" VALUES (17,'verb','came',4,0.0,NULL);
INSERT INTO "corpus" VALUES (18,'verb','tumbling',5,0.0,NULL);
INSERT INTO "corpus" VALUES (19,'verb','got',6,0.0,NULL);
INSERT INTO "corpus" VALUES (20,'verb','did',7,0.0,NULL);
INSERT INTO "corpus" VALUES (21,'verb','caper',8,0.0,NULL);
INSERT INTO "corpus" VALUES (22,'verb','bed',9,0.0,NULL);
INSERT INTO "corpus" VALUES (23,'verb','mend',10,0.0,NULL);
INSERT INTO "basis" VALUES (1,'noun','Jack',0,0.0,NULL);
INSERT INTO "basis" VALUES (2,'noun','Jill',1,0.0,NULL);
INSERT INTO "basis" VALUES (3,'noun','hill',2,0.0,NULL);
INSERT INTO "basis" VALUES (4,'noun','pail',3,0.0,NULL);
INSERT INTO "basis" VALUES (5,'noun','water',4,0.0,NULL);
INSERT INTO "basis" VALUES (6,'noun','crown',5,0.0,NULL);
INSERT INTO "basis" VALUES (7,'noun','Up',6,0.0,NULL);
INSERT INTO "basis" VALUES (8,'noun','home',7,0.0,NULL);
INSERT INTO "basis" VALUES (9,'noun','As',8,0.0,NULL);
INSERT INTO "basis" VALUES (10,'noun','head',9,0.0,NULL);
INSERT INTO "basis" VALUES (11,'noun','vinegar',10,0.0,NULL);
INSERT INTO "basis" VALUES (12,'noun','paper',11,0.0,NULL);
INSERT INTO "basis" VALUES (13,'verb','went',0,0.0,NULL);
INSERT INTO "basis" VALUES (14,'verb','fetch',1,0.0,NULL);
INSERT INTO "basis" VALUES (15,'verb','fell',2,0.0,NULL);
INSERT INTO "basis" VALUES (16,'verb','broke',3,0.0,NULL);
INSERT INTO "basis" VALUES (17,'verb','came',4,0.0,NULL);
INSERT INTO "basis" VALUES (18,'verb','tumbling',5,0.0,NULL);
INSERT INTO "basis" VALUES (19,'verb','got',6,0.0,NULL);
INSERT INTO "basis" VALUES (20,'verb','did',7,0.0,NULL);
INSERT INTO "basis" VALUES (21,'verb','caper',8,0.0,NULL);
INSERT INTO "basis" VALUES (22,'verb','bed',9,0.0,NULL);
INSERT INTO "basis" VALUES (23,'verb','mend',10,0.0,NULL);
COMMIT;
