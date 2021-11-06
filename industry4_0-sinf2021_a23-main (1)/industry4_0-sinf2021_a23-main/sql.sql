CREATE SCHEMA sprint2;


CREATE TABLE sprint2.mote(
    mote smallint PRIMARY KEY CHECK (mote > 0)
);

CREATE TABLE sprint2.atuador(
    id smallint PRIMARY KEY CHECK (id > 0),
    nome varchar(128) UNIQUE,
    tipo varchar(128) NOT NULL,
    mote smallint REFERENCES sprint2.mote(mote)
);

CREATE TABLE sprint2.sensor(
    id smallint PRIMARY KEY CHECK (id > 0),
    nome varchar(128) UNIQUE,
    tipo varchar(128) NOT NULL,
    mote smallint REFERENCES sprint2.mote(mote)
);

CREATE TABLE sprint2.config(
	id  smallint PRIMARY KEY CHECK (id > 0),
	mote smallint REFERENCES sprint2.mote(mote),
	Atuador1 varchar(128) REFERENCES sprint2.atuador(nome),
    Atuador2 varchar(128) REFERENCES sprint2.atuador(nome),
    Atuador3 varchar(128) REFERENCES sprint2.atuador(nome),
    Atuador4 varchar(128) REFERENCES sprint2.atuador(nome),
 	Sensor1 varchar(128) REFERENCES sprint2.sensor(nome),
    Sensor2 varchar(128) REFERENCES sprint2.sensor(nome),
    Sensor3 varchar(128) REFERENCES sprint2.sensor(nome),
    Sensor4 varchar(128) REFERENCES sprint2.sensor(nome),
    Sensor5 varchar(128) REFERENCES sprint2.sensor(nome)
);

CREATE TABLE sprint2.regras(
    id_regra smallint PRIMARY KEY CHECK (id_regra > 0),
    sensor varchar(128),
    valor integer CHECK (valor >= 0),
    condiçao varchar(128) NOT NULL,
    atuador varchar(128),
    resultado smallint
);

CREATE TABLE sprint2.historico_atuador(
    mediçao SERIAL PRIMARY KEY,
    atuador varchar(128) REFERENCES sprint2.atuador(nome) ON DELETE SET NULL ON UPDATE CASCADE,
    time_stamp TIMESTAMP,
    estado smallint
);

CREATE TABLE sprint2.historico_sensor(
    mediçao SERIAL PRIMARY KEY,
    sensor varchar(128) REFERENCES sprint2.sensor(nome) ON DELETE SET NULL ON UPDATE CASCADE,
    time_stamp TIMESTAMP,
    valor smallint
);


INSERT INTO sprint2.mote (mote) VALUES (1);
INSERT INTO sprint2.mote (mote) VALUES (2);
INSERT INTO sprint2.mote (mote) VALUES (3);
INSERT INTO sprint2.mote (mote) VALUES (4);
INSERT INTO sprint2.mote (mote) VALUES (5);


INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (1,'Atuador_Luz_1','Luz',1);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (2,'Atuador_Curr_1','Corrente',1);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (3,'Atuador_Temp_1','Temperatura',1);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (4,'Atuador_Hum_1','Humidade',1);

INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (5,'Atuador_Luz_2','Luz',2);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (6,'Atuador_Curr_2','Corrente',2);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (7,'Atuador_Temp_2','Temperatura',2);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (8,'Atuador_Hum_2','Humidade',2);

INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (9,'Atuador_Luz_3','Luz',3);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (10,'Atuador_Curr_3','Corrente',3);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (11,'Atuador_Temp_3','Temperatura',3);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (12,'Atuador_Hum_3','Humidade',3);

INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (13,'Atuador_Luz_4','Luz',4);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (14,'Atuador_Curr_4','Corrente',4);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (15,'Atuador_Temp_4','Temperatura',4);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (16,'Atuador_Hum_4','Humidade',4);

INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (17,'Atuador_Luz_5','Luz',5);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (18,'Atuador_Curr_5','Corrente',5);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (19,'Atuador_Temp_5','Temperatura',5);
INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (20,'Atuador_Hum_5','Humidade',5);


INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (1,'Luz_1','Luz',1);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (2,'Curr_1','Corrente',1);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (3,'Temp_1','Temperatura',1);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (4,'Hum_1','Humidade',1);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (5,'Power_1','Potencia',1);

INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (6,'Luz_2','Luz',2);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (7,'Curr_2','Corrente',2);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (8,'Temp_2','Temperatura',2);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (9,'Hum_2','Humidade',2);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (10,'Power_2','Potencia',2);

INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (11,'Luz_3','Luz',3);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (12,'Curr_3','Corrente',3);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (13,'Temp_3','Temperatura',3);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (14,'Hum_3','Humidade',3);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (15,'Power_3','Potencia',3);

INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (16,'Luz_4','Luz',4);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (17,'Curr_4','Corrente',4);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (18,'Temp_4','Temperatura',4);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (19,'Hum_4','Humidade',4);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (20,'Power_4','Potencia',4);

INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (21,'Luz_5','Luz',5);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (22,'Curr_5','Corrente',5);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (23,'Temp_5','Temperatura',5);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (24,'Hum_5','Humidade',5);
INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (25,'Power_5','Potencia',5);


INSERT INTO sprint2.config VALUES (1,1,'Atuador_Luz_1','Atuador_Curr_1','Atuador_Temp_1','Atuador_Hum_1','Luz_1','Curr_1','Temp_1','Hum_1','Power_1');
INSERT INTO sprint2.config VALUES (2,2,'Atuador_Luz_2','Atuador_Curr_2','Atuador_Temp_2','Atuador_Hum_2','Luz_2','Curr_2','Temp_2','Hum_2','Power_2');
INSERT INTO sprint2.config VALUES (3,3,'Atuador_Luz_3','Atuador_Curr_3','Atuador_Temp_3','Atuador_Hum_3','Luz_3','Curr_3','Temp_3','Hum_3','Power_3');
INSERT INTO sprint2.config VALUES (4,4,'Atuador_Luz_4','Atuador_Curr_4','Atuador_Temp_4','Atuador_Hum_4','Luz_4','Curr_4','Temp_4','Hum_4','Power_4');
INSERT INTO sprint2.config VALUES (5,5,'Atuador_Luz_5','Atuador_Curr_5','Atuador_Temp_5','Atuador_Hum_5','Luz_5','Curr_5','Temp_5','Hum_5','Power_5');


INSERT INTO sprint2.regras VALUES (1,'Luz',500,'<','Luz',1);
INSERT INTO sprint2.regras VALUES (2,'Luz',2000,'>','Luz',0);
INSERT INTO sprint2.regras VALUES (3,'Corrente',10,'<','Corrente',1);
INSERT INTO sprint2.regras VALUES (4,'Corrente',50,'>','Corrente',0);
INSERT INTO sprint2.regras VALUES (5,'Temperatura',20,'<','Temperatura',1);
INSERT INTO sprint2.regras VALUES (6,'Temperatura',22,'>','Temperatura',0);
INSERT INTO sprint2.regras VALUES (7,'Humidade',20,'<','Humidade',1);
INSERT INTO sprint2.regras VALUES (8,'Humidade',80,'>','Humidade',0);
INSERT INTO sprint2.regras VALUES (9,'Potencia',10000,'>','Luz',0);
INSERT INTO sprint2.regras VALUES (10,'Potencia',10000,'>','Corrente',0);
INSERT INTO sprint2.regras VALUES (11,'Potencia',10000,'>','Temperatura',0);
INSERT INTO sprint2.regras VALUES (12,'Potencia',10000,'>','Humidade',0);
