#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <postgresql/libpq-fe.h>
#include "sinf_interfaces.h"
#include <math.h>


#define BLUE "[0,0,254]"
#define GREEN "[0,254,0]"
#define RED "[254,0,0]"
#define BLACK "[0,0,0]"
#define WHITE "[254,254,254]"
#define FLOOR "[0,32,64]"
#define Oven "[140,45,25]"
#define Yellow "[255,255,0]"
#define Amarelo_escurinho "[255,215,0]"
#define Orange "[255,69,0]"
#define Grey "[128,128,128]"
#define IceIceBaby "[65,105,225]"
#define Verdinho "[50,205,50]"
#define MAX_MSG_SIZE 75

const char *channelRGB = "/tmp/ttyV10";               //definir canal onde enviamos informaçao para a RGBMatrix
const char *channelMSGCreator = "/tmp/ttyV12";        //definir canal onde recebemos e enviamos informação para o MsgCreatorConf

char received_data[MAX_MSG_SIZE];
char *token;

int caracteres[4];
int id;
int act=0;

FILE *sensor_data_channel;
FILE *matrix_channel;
FILE *act_channel;

float ret;
struct sensor sensores[5];
struct RGB rgb_data[5];
struct actuator atuadores[5];

PGconn *conn; 
PGresult *res;
const char *dbconn;

int regra_temp_min=20;
int regra_temp_max=22;
int regra_light_min=500;
int regra_light_max=2000;
int regra_curr_min=10;
int regra_curr_max=50;
int regra_hum_min=20;
int regra_hum_max=80;
int regra_power=10000;

void check_sensors(){
	//printf("entrou");
  char *data[100], *char_data[10]; 
  int i = 0;
  
  int32_t sensor_data;                //Exemplo de mensagem: 7E 45 00 FF FF 00 05 0A 3F 0C 00 00 00 E1 00 00 19 8E 7F 0D 48 81 7E
  
  strcpy(char_data,"");

  sensor_data_channel = fopen(channelMSGCreator, "r");
  
  if (fgets(received_data,MAX_MSG_SIZE,sensor_data_channel) != NULL){
    token = strtok(received_data," ");    
  }
  
  while (token != NULL){	              //Função que guarda a mensagem
	data[i++] = token;
	token = strtok(NULL," ");	  
  }
  
  // Mote - token 6
  strcat(char_data, data[6]);
  id = strtol(char_data,NULL,16);

  // Temperature - tokens 16 and 17
  strcpy(char_data,"");
  strcat(char_data, data[16]);
  strcat(char_data, data[17]);
  sensor_data = strtol(char_data,NULL,16);
    
  sensores[id-1].temp = -39.6 + 0.01 * sensor_data;  

  // Humidade- Tokens 18 e 19
  strcpy(char_data,"");
  strcat(char_data, data[18]);
  strcat(char_data, data[19]);
  sensor_data = strtol(char_data,NULL,16);
    
  sensores[id-1].hum= -2.0468 + (0.0367 * sensor_data) +(-1.5955* pow(10,-6)* pow(sensor_data,2)); 


  // Corrente - Tokens 14 e 15
  strcpy(char_data,"");
  strcat(char_data, data[14]);
  strcat(char_data, data[15]);
  sensor_data = strtol(char_data,NULL,16);
  //printf("%d\n", sensor_data);
    
  sensores[id-1].curr= pow(10,5)*(((0.769*sensor_data)/4096)*1.5/100);
  
 // Luz- Tokens 12 e 13
  strcpy(char_data,"");
  strcat(char_data, data[12]);
  strcat(char_data, data[13]);
  sensor_data = strtol(char_data,NULL,16);
    
  sensores[id-1].light= pow(10,6)*(((0.625*sensor_data)/4096)*1.5/100);

  sensores[id-1].power=230*sensores[id-1].curr;

  
  printf("Mote:%d Luz:%d Corrente:%d Temperatura:%d Humidade:%d Potência:%d\n", id, sensores[id-1].light, sensores[id-1].curr, sensores[id-1].temp, sensores[id-1].hum, sensores[id-1].power);	
}

void writeRGBmatrix(){

  
  strcpy(rgb_data[id-1].temp_sens,"");
  strcpy(rgb_data[id-1].temp_act,"");
  strcpy(rgb_data[id-1].hum_sens,"");
  strcpy(rgb_data[id-1].hum_act,"");
  strcpy(rgb_data[id-1].curr_sens,"");
  strcpy(rgb_data[id-1].curr_act,"");
  strcpy(rgb_data[id-1].light_sens,"");
  strcpy(rgb_data[id-1].light_act,"");
  
  matrix_channel = fopen(channelRGB, "w");
    
  // Temperatura
  
  if (sensores[id-1].temp > regra_temp_max){
    strcpy(rgb_data[id-1].temp_sens,RED);
    //strcpy(rgb_data[id-1].temp_act,IceIceBaby);
  }
  
  else if (sensores[id-1].temp < regra_temp_min){
    strcpy(rgb_data[id-1].temp_sens,IceIceBaby);
    //strcpy(rgb_data[id-1].temp_act,RED);
  }
  
  else{
    strcpy(rgb_data[id-1].temp_sens,Verdinho);
    //strcpy(rgb_data[id-1].temp_act,WHITE);
  }
  
  if(act==-1){
    strcpy(rgb_data[id-1].temp_act,IceIceBaby);
  }
  else if(act==1){
    strcpy(rgb_data[id-1].temp_act,RED);
  }
  else{
    strcpy(rgb_data[id-1].temp_act,Grey);
  }

  
  //Humidade
  if (sensores[id-1].hum > regra_hum_max){
    strcpy(rgb_data[id-1].hum_sens,BLUE);
  }
  
  else if (sensores[id-1].hum < regra_hum_min){
    strcpy(rgb_data[id-1].hum_sens,Orange);
  }
  
  else{
    strcpy(rgb_data[id-1].hum_sens,GREEN);
  }


  //Corrente
  if (sensores[id-1].curr > regra_curr_max){
    strcpy(rgb_data[id-1].curr_sens,RED);
  }
  
  else if (sensores[id-1].curr < regra_curr_min){
    strcpy(rgb_data[id-1].curr_sens,Grey);
  }
  
  else{
    strcpy(rgb_data[id-1].curr_sens,GREEN);
  }


  //Luz
  if (sensores[id-1].light < regra_light_min ){
    strcpy(rgb_data[id-1].light_sens,Grey);
  }
  
  else if (sensores[id-1].light > regra_light_max){
    strcpy(rgb_data[id-1].light_sens,RED);
  }

  else if (sensores[id-1].light >= regra_light_min && sensores[id-1].light < 0,8*regra_light_max) {
    strcpy(rgb_data[id-1].light_sens,Yellow);
  }
  
  else{
    strcpy(rgb_data[id-1].light_sens,Amarelo_escurinho);
  }

  //Potencia
  if (sensores[id-1].power > regra_power) {
    strcpy(rgb_data[0].temp_act,Grey);
    strcpy(rgb_data[1].temp_act,Grey);
    strcpy(rgb_data[2].temp_act,Grey);
    strcpy(rgb_data[3].temp_act,Grey);
    strcpy(rgb_data[4].temp_act,Grey);
  }
  

  fprintf(matrix_channel,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
  "["FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,\
     FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[0].light_sens,FLOOR,FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[1].light_sens,FLOOR,\
	   FLOOR,Oven,rgb_data[0].temp_sens,rgb_data[0].temp_act,FLOOR,rgb_data[0].curr_sens,FLOOR,FLOOR,Oven,rgb_data[1].temp_sens,rgb_data[1].temp_act,FLOOR,rgb_data[1].curr_sens,FLOOR,\
		 FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[0].hum_sens,FLOOR,FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[1].hum_sens,FLOOR,\
		 FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,\
     FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[2].light_sens,FLOOR,\
     FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,Oven,rgb_data[2].temp_sens,rgb_data[2].temp_act,FLOOR,rgb_data[2].curr_sens,FLOOR,\
     FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[2].hum_sens,FLOOR,\
     FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,\
     FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[3].light_sens,FLOOR,FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[4].light_sens,FLOOR,\
     FLOOR,Oven,rgb_data[3].temp_sens,rgb_data[3].temp_act,FLOOR,rgb_data[3].curr_sens,FLOOR,FLOOR,Oven,rgb_data[4].temp_sens,rgb_data[4].temp_act,FLOOR,rgb_data[4].curr_sens,FLOOR,\
     FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[3].hum_sens,FLOOR,FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[4].hum_sens,FLOOR,\
     FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,\
     FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR"]\n");	
}


void update_act(){
  
  int virgulas=0;
  int i;
  int pos;
  int vet[MAX_MSG_SIZE] = {};
  int aux;
  int aux2;
  int w;

  char act_number[100];
  char act_name[100];
  char query[100];
  int count=0;
  
  act_channel = fopen("MsgCreatorConf.txt", "r+");
  if (act_channel==NULL){
    return 0;
  }

  if (sensores[id-1].temp > regra_temp_max || sensores[id-1].temp < regra_temp_min || sensores[id-1].light > regra_light_max || sensores[id-1].light < regra_light_min || sensores[id-1].curr > regra_curr_max || sensores[id-1].curr < regra_curr_min || sensores[id-1].hum > regra_hum_max || sensores[id-1].hum < regra_hum_min){
    
    while (1)
    {
      i = fgetc(act_channel);
     
      if (i ==','){
       
        virgulas++;

        if (virgulas==6) {

          i = fgetc(act_channel);
          pos = ftell(act_channel)-1;
          if (i=='+' && sensores[id-1].light > regra_light_max) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("-",act_channel);
            for (w=0; w<5; w++) {
              atuadores[w].light=0;
              count=w+1;
              sprintf(act_number,"%d", count); 
              strcpy(act_name,"Atuador_Luz_");
		          strcat(act_name,act_number);
			        sprintf(query, "INSERT INTO sprint2.historico_atuador (atuador, time_stamp, estado) VALUES ('%s',now(),%d)",act_name,atuadores[w].light);
			        PQexec(conn, query);
            }
          }
          else if (i=='-' && sensores[id-1].light < regra_light_min) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("+",act_channel);
            for (w=0; w<5; w++) {
              atuadores[w].light=1;
              count=w+1;
              sprintf(act_number,"%d", count); 
              strcpy(act_name,"Atuador_Luz_");
		          strcat(act_name,act_number);
			        sprintf(query, "INSERT INTO sprint2.historico_atuador (atuador, time_stamp, estado) VALUES ('%s',now(),%d)",act_name,atuadores[w].light);
			        PQexec(conn, query);
            }
          }
        }

        if (virgulas==10) {

          i = fgetc(act_channel);
          pos = ftell(act_channel)-1;
          if (i=='+' && sensores[id-1].curr > regra_curr_max) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("-",act_channel);
            for (w=0; w<5; w++) {
              atuadores[w].curr=0;
              count=w+1;
              sprintf(act_number,"%d", count); 
              strcpy(act_name,"Atuador_Curr_");
		          strcat(act_name,act_number);
			        sprintf(query, "INSERT INTO sprint2.historico_atuador (atuador, time_stamp, estado) VALUES ('%s',now(),%d)",act_name,atuadores[w].curr);
			        PQexec(conn, query);
            }
          }
          else if (i=='-' && sensores[id-1].curr < regra_curr_min) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("+",act_channel);
            for (w=0; w<5; w++) {
              atuadores[w].curr=1;
              count=w+1;
              sprintf(act_number,"%d", count); 
              strcpy(act_name,"Atuador_Curr_");
		          strcat(act_name,act_number);
			        sprintf(query, "INSERT INTO sprint2.historico_atuador (atuador, time_stamp, estado) VALUES ('%s',now(),%d)",act_name,atuadores[w].curr);
			        PQexec(conn, query);
            }
          }
        }

        if (virgulas==14){
          
          i = fgetc(act_channel);
          pos = ftell(act_channel)-1;
          // aux = 0;
          if (i=='+' && sensores[id-1].temp > regra_temp_max) {
            
            // while (1) {
            //   vet[aux]=fgetc(act_channel);
            //   if (vet[aux]==NULL){
            //     break;
            //   }
            //   aux++;
            // }
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("-",act_channel);
            act=-1;
            for (w=0; w<5; w++) {
              atuadores[w].temp=0;
              count=w+1;
              sprintf(act_number,"%d", count);
              strcpy(act_name,"Atuador_Temp_");
		          strcat(act_name,act_number);
			        sprintf(query, "INSERT INTO sprint2.historico_atuador (atuador, time_stamp, estado) VALUES ('%s',now(),%d)",act_name,atuadores[w].temp);
			        PQexec(conn, query);
            }
            // aux2=0;
            // while(aux!=0)
            // {
            //   printf("%d\n", vet[aux]);
            //   aux--;
            // }
            // while(aux!=0) {
            //   fputc(vet[aux2],act_channel);
            //   aux2++;
            //   aux--;
            // }
            // break;
          }
          else if (i=='-' && sensores[id-1].temp < regra_temp_min) {
           
            // fseek(act_channel,1,SEEK_CUR);
            // while (1) {
            //   vet[aux]=fgetc(act_channel);
            //   if (vet[aux]==NULL){
            //     break;
            //   }
            //   aux++;
            // }
            fseek(act_channel,pos,SEEK_SET);
            fputs("+",act_channel);
            act=1;
            for (w=0; w<5; w++) {
              atuadores[w].temp=1;
              count=w+1;
              sprintf(act_number,"%d", count);
              strcpy(act_name,"Atuador_Temp_");
		          strcat(act_name,act_number);
			        sprintf(query, "INSERT INTO sprint2.historico_atuador (atuador, time_stamp, estado) VALUES ('%s',now(),%d)",act_name,atuadores[w].temp);
			        PQexec(conn, query);
            }
            // aux2=0;
            // while(aux!=0)
            // {
            //   printf("%d\n", vet[aux]);
            //   aux--;
            // }
            // while(aux!=0) {
            //   fputc(vet[aux2],act_channel);
            //   aux2++;
            //   aux--;
            // }
            // break;
          }
        }

        if (virgulas==18) {

          i = fgetc(act_channel);
          pos = ftell(act_channel)-1;
          if (i=='+' && sensores[id-1].hum > regra_hum_max) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("-",act_channel);
            for (w=0; w<5; w++) {
              atuadores[w].hum=0;
              count=w+1;
              sprintf(act_number,"%d", count);
              strcpy(act_name,"Atuador_Hum_");
		          strcat(act_name,act_number);
			        sprintf(query, "INSERT INTO sprint2.historico_atuador (atuador, time_stamp, estado) VALUES ('%s',now(),%d)",act_name,atuadores[w].hum);
			        PQexec(conn, query);
            }
          }
          else if (i=='-' && sensores[id-1].hum < regra_hum_min) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("+",act_channel);
            for (w=0; w<5; w++) {
              atuadores[w].hum=1;
              count=w+1;
              sprintf(act_number,"%d", count);
              strcpy(act_name,"Atuador_Hum_");
		          strcat(act_name,act_number);
			        sprintf(query, "INSERT INTO sprint2.historico_atuador (atuador, time_stamp, estado) VALUES ('%s',now(),%d)",act_name,atuadores[w].hum);
			        PQexec(conn, query);
            }
          }
          break;
        }
      }
    }
  }

  fclose(act_channel);

}


void db_historico_sensores()  //preenche o historico dos sensores
{
  char sensor_number[100];
  char sensor_name[100];
  char query[100];
  int aux=0;

  for(int j=0; j<5; j++){ 
    aux = j+1;
		sprintf(sensor_number,"%d", aux); 
			
		strcpy(sensor_name,"Luz_");
		strcat(sensor_name,sensor_number);
		sprintf(query, "INSERT INTO sprint2.historico_sensor (sensor,time_stamp,valor) VALUES ('%s',now(), %d)",sensor_name, sensores[j].light);
		res = PQexec(conn,query);
			
		strcpy(sensor_name,"Curr_");
		strcat(sensor_name,sensor_number);
		sprintf(query, "INSERT INTO sprint2.historico_sensor (sensor,time_stamp,valor) VALUES ('%s',now(), %d)",sensor_name, sensores[j].curr);
		res = PQexec(conn,query);
			
		strcpy(sensor_name,"Temp_");
		strcat(sensor_name,sensor_number);
		sprintf(query, "INSERT INTO sprint2.historico_sensor (sensor,time_stamp,valor) VALUES ('%s',now(), %d)",sensor_name, sensores[j].temp);
		res = PQexec(conn,query);

    strcpy(sensor_name,"Hum_");
		strcat(sensor_name,sensor_number);
		sprintf(query, "INSERT INTO sprint2.historico_sensor (sensor,time_stamp,valor) VALUES ('%s',now(), %d)",sensor_name, sensores[j].hum);
		res = PQexec(conn,query);

    strcpy(sensor_name,"Power_");
		strcat(sensor_name,sensor_number);
		sprintf(query, "INSERT INTO sprint2.historico_sensor (sensor,time_stamp,valor) VALUES ('%s',now(), %d)",sensor_name, sensores[j].power);
		res = PQexec(conn,query);
	}
}


int main()
{	
  int cont=24;

  PGresult *reg;
  int temp_min;
  int temp_max;
  int curr_min;
  int curr_max;
  int hum_min;
  int hum_max;
  int light_min;
  int light_max;
  int power;

  char query[100];

  for(int i = 0; i<5; i++) {
    strcpy(rgb_data[i].temp_sens,BLACK);
    strcpy(rgb_data[i].temp_act,BLACK);
    strcpy(rgb_data[i].hum_sens,BLACK);
    strcpy(rgb_data[i].hum_act,BLACK);
    strcpy(rgb_data[i].curr_sens,BLACK);
    strcpy(rgb_data[i].curr_act,BLACK);
    strcpy(rgb_data[i].light_sens,BLACK);
    strcpy(rgb_data[i].light_act,BLACK);
  }

  for(int j=0; j<5; j++) {
    atuadores[j].temp=1;
    atuadores[j].hum=1;
    atuadores[j].curr=1;
    atuadores[j].light=1;
  }

	
	dbconn = "host = 'db.fe.up.pt' dbname = 'sinf2021a23' user = 'sinf2021a23' password = 'QkYgVxMv'";

	conn = PQconnectdb(dbconn);
  PQexec(conn,"CREATE SCHEMA sprint2");
	//PQexec(conn,"SET search_path TO sprint2_teste");
	
	if (!conn){
		printf(stderr, "libpq error: PQconnectdb returned NULL. \n\n");
		PQfinish(conn);
		exit(1);
	}
	
	else if (PQstatus(conn) != CONNECTION_OK){
		printf(stderr, "Connection to DB failed: %s", PQerrorMessage(conn));
		PQfinish(conn);
		exit(1);
	}

	else {
		printf("Connection OK \n");
		//res = PQexec(conn, "INSERT INTO test_1920 (id, name, age) VALUES (1, 'Jane Doe', 32)");
		//PQfinish(conn);
	}
  PQexec(conn, "CREATE TABLE sprint2.mote(mote smallint PRIMARY KEY CHECK (mote > 0))");
	PQexec(conn, "CREATE TABLE sprint2.atuador(id smallint PRIMARY KEY CHECK (id > 0),nome varchar(128) UNIQUE,tipo varchar(128) NOT NULL,mote smallint REFERENCES sprint2.mote(mote))");
  PQexec(conn, "CREATE TABLE sprint2.sensor(id smallint PRIMARY KEY CHECK (id > 0),nome varchar(128) UNIQUE,tipo varchar(128) NOT NULL,mote smallint REFERENCES sprint2.mote(mote))");
  PQexec(conn, "CREATE TABLE sprint2.config(id  smallint PRIMARY KEY CHECK (id > 0),mote smallint REFERENCES sprint2.mote(mote),Atuador1 varchar(128) REFERENCES sprint2.atuador(nome),Atuador2 varchar(128) REFERENCES sprint2.atuador(nome),Atuador3 varchar(128) REFERENCES sprint2.atuador(nome),Atuador4 varchar(128) REFERENCES sprint2.atuador(nome),Sensor1 varchar(128) REFERENCES sprint2.sensor(nome),Sensor2 varchar(128) REFERENCES sprint2.sensor(nome),Sensor3 varchar(128) REFERENCES sprint2.sensor(nome),Sensor4 varchar(128) REFERENCES sprint2.sensor(nome),Sensor5 varchar(128) REFERENCES sprint2.sensor(nome))");
  PQexec(conn, "CREATE TABLE sprint2.regras(id_regra smallint PRIMARY KEY CHECK (id_regra > 0),sensor varchar(128),valor integer CHECK (valor >= 0),condiçao varchar(128) NOT NULL,atuador varchar(128),resultado smallint)");
  PQexec(conn, "CREATE TABLE sprint2.historico_atuador(mediçao SERIAL PRIMARY KEY,atuador varchar(128) REFERENCES sprint2.atuador(nome) ON DELETE SET NULL ON UPDATE CASCADE,time_stamp TIMESTAMP,estado smallint)");
  PQexec(conn, "CREATE TABLE sprint2.historico_sensor(mediçao SERIAL PRIMARY KEY,sensor varchar(128) REFERENCES sprint2.sensor(nome) ON DELETE SET NULL ON UPDATE CASCADE,time_stamp TIMESTAMP,valor smallint)");
  PQexec(conn, "INSERT INTO sprint2.mote (mote) VALUES (1)");
  PQexec(conn, "INSERT INTO sprint2.mote (mote) VALUES (2)");
  PQexec(conn, "INSERT INTO sprint2.mote (mote) VALUES (3)");
  PQexec(conn, "INSERT INTO sprint2.mote (mote) VALUES (4)");
  PQexec(conn, "INSERT INTO sprint2.mote (mote) VALUES (5)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (1,'Atuador_Luz_1','Luz',1)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (2,'Atuador_Curr_1','Corrente',1)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (3,'Atuador_Temp_1','Temperatura',1)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (4,'Atuador_Hum_1','Humidade',1)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (5,'Atuador_Luz_2','Luz',2)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (6,'Atuador_Curr_2','Corrente',2)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (7,'Atuador_Temp_2','Temperatura',2)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (8,'Atuador_Hum_2','Humidade',2)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (9,'Atuador_Luz_3','Luz',3)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (10,'Atuador_Curr_3','Corrente',3)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (11,'Atuador_Temp_3','Temperatura',3)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (12,'Atuador_Hum_3','Humidade',3)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (13,'Atuador_Luz_4','Luz',4)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (14,'Atuador_Curr_4','Corrente',4)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (15,'Atuador_Temp_4','Temperatura',4)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (16,'Atuador_Hum_4','Humidade',4)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (17,'Atuador_Luz_5','Luz',5)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (18,'Atuador_Curr_5','Corrente',5)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (19,'Atuador_Temp_5','Temperatura',5)");
  PQexec(conn, "INSERT INTO sprint2.atuador (id, nome, tipo, mote) VALUES (20,'Atuador_Hum_5','Humidade',5)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (1,'Luz_1','Luz',1)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (2,'Curr_1','Corrente',1)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (3,'Temp_1','Temperatura',1)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (4,'Hum_1','Humidade',1)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (5,'Power_1','Potencia',1)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (6,'Luz_2','Luz',2)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (7,'Curr_2','Corrente',2)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (8,'Temp_2','Temperatura',2)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (9,'Hum_2','Humidade',2)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (10,'Power_2','Potencia',2)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (11,'Luz_3','Luz',3)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (12,'Curr_3','Corrente',3)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (13,'Temp_3','Temperatura',3)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (14,'Hum_3','Humidade',3)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (15,'Power_3','Potencia',3)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (16,'Luz_4','Luz',4)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (17,'Curr_4','Corrente',4)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (18,'Temp_4','Temperatura',4)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (19,'Hum_4','Humidade',4)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (20,'Power_4','Potencia',4)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (21,'Luz_5','Luz',5)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (22,'Curr_5','Corrente',5)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (23,'Temp_5','Temperatura',5)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (24,'Hum_5','Humidade',5)");
  PQexec(conn, "INSERT INTO sprint2.sensor (id, nome, tipo, mote) VALUES (25,'Power_5','Potencia',5)");
  PQexec(conn, "INSERT INTO sprint2.config (id, mote, Atuador1, Atuador2, Atuador3, Atuador4, Sensor1, Sensor2, Sensor3, Sensor4, Sensor5) VALUES (1,1,'Atuador_Luz_1','Atuador_Curr_1','Atuador_Temp_1','Atuador_Hum_1','Luz_1','Curr_1','Temp_1','Hum_1','Power_1')");
  PQexec(conn, "INSERT INTO sprint2.config (id, mote, Atuador1, Atuador2, Atuador3, Atuador4, Sensor1, Sensor2, Sensor3, Sensor4, Sensor5) VALUES (2,2,'Atuador_Luz_2','Atuador_Curr_2','Atuador_Temp_2','Atuador_Hum_2','Luz_2','Curr_2','Temp_2','Hum_2','Power_2')");
  PQexec(conn, "INSERT INTO sprint2.config (id, mote, Atuador1, Atuador2, Atuador3, Atuador4, Sensor1, Sensor2, Sensor3, Sensor4, Sensor5) VALUES (3,3,'Atuador_Luz_3','Atuador_Curr_3','Atuador_Temp_3','Atuador_Hum_3','Luz_3','Curr_3','Temp_3','Hum_3','Power_3')");
  PQexec(conn, "INSERT INTO sprint2.config (id, mote, Atuador1, Atuador2, Atuador3, Atuador4, Sensor1, Sensor2, Sensor3, Sensor4, Sensor5) VALUES (4,4,'Atuador_Luz_4','Atuador_Curr_4','Atuador_Temp_4','Atuador_Hum_4','Luz_4','Curr_4','Temp_4','Hum_4','Power_4')");
  PQexec(conn, "INSERT INTO sprint2.config (id, mote, Atuador1, Atuador2, Atuador3, Atuador4, Sensor1, Sensor2, Sensor3, Sensor4, Sensor5) VALUES (5,5,'Atuador_Luz_5','Atuador_Curr_5','Atuador_Temp_5','Atuador_Hum_5','Luz_5','Curr_5','Temp_5','Hum_5','Power_5')");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (1,'Luz',500,'<','Luz',1)");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (2,'Luz',2000,'>','Luz',0)");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (3,'Corrente',10,'<','Corrente',1)");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (4,'Corrente',50,'>','Corrente',0)");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (5,'Temperatura',20,'<','Temperatura',1)");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (6,'Temperatura',22,'>','Temperatura',0)");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (7,'Humidade',20,'<','Humidade',1)");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (8,'Humidade',80,'>','Humidade',0)");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (9,'Potencia',10000,'>','Luz',0)");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (10,'Potencia',10000,'>','Corrente',0)");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (11,'Potencia',10000,'>','Temperatura',0)");
  PQexec(conn, "INSERT INTO sprint2.regras (id_regra, sensor, valor, condiçao, atuador, resultado) VALUES (12,'Potencia',10000,'>','Humidade',0)");

  //PQexec(conn, "CREATE TABLE sprint2.teste(id smallint)");
  while(1){  
  
  // Obtain data from the temperature sensor
  check_sensors();
  // Decide and Print the colors of the cells to the RGB Matrix
  writeRGBmatrix();
  
  update_act();


  cont++;
  if (cont==25){  //apenas para não estar constantemente a atualizar os valor na DB
    db_historico_sensores();
    //printf("Entrou aqui1 \n");
    reg=PQexec(conn, "SELECT id_regra, valor FROM sprint2.regras");
    //printf("Entrou aqui2 \n");
    for( int cnt=0; cnt<12; cnt++) {
      //printf("Entrou aqui3 \n");
      if(strcmp(PQgetvalue(reg,cnt,0),"1")==0) {
        light_min = atoi(PQgetvalue(reg,cnt,1));
      }
      if(strcmp(PQgetvalue(reg,cnt,0),"2")==0) {
        light_max = atoi(PQgetvalue(reg,cnt,1));
      }
      if(strcmp(PQgetvalue(reg,cnt,0),"3")==0) {
        curr_min = atoi(PQgetvalue(reg,cnt,1));
      }
      if(strcmp(PQgetvalue(reg,cnt,0),"4")==0) {
        curr_max = atoi(PQgetvalue(reg,cnt,1));
      }
      if(strcmp(PQgetvalue(reg,cnt,0),"5")==0) {
        temp_min = atoi(PQgetvalue(reg,cnt,1));
      }
      if(strcmp(PQgetvalue(reg,cnt,0),"6")==0) {
        temp_max = atoi(PQgetvalue(reg,cnt,1));
      }
      if(strcmp(PQgetvalue(reg,cnt,0),"7")==0) {
        hum_min = atoi(PQgetvalue(reg,cnt,1));
      }
      if(strcmp(PQgetvalue(reg,cnt,0),"8")==0) {
        hum_max = atoi(PQgetvalue(reg,cnt,1));
      }
      if(strcmp(PQgetvalue(reg,cnt,0),"9")==0) {
        power = atoi(PQgetvalue(reg,cnt,1));
      }
      if(strcmp(PQgetvalue(reg,cnt,0),"10")==0) {
        power = atoi(PQgetvalue(reg,cnt,1));
      }
      if(strcmp(PQgetvalue(reg,cnt,0),"11")==0) {
        power = atoi(PQgetvalue(reg,cnt,1));
      }
      if(strcmp(PQgetvalue(reg,cnt,0),"12")==0) {
        power = atoi(PQgetvalue(reg,cnt,1));
      }
    }

    if (regra_light_min!=light_min) {
      regra_light_min=light_min;
      printf("Mínimo do atuador luz alterado para: %d\n",regra_light_min);
    }
    if (regra_light_max!=light_max) {
      regra_light_max=light_max;
      printf("Máximo do atuador luz alterado para: %d\n",regra_light_max);
    }
    if (regra_curr_min!=curr_min) {
      regra_curr_min=curr_min;
      printf("Mínimo do atuador corrente alterado para: %d\n",regra_curr_min);
    }
    if (regra_curr_max!=curr_max) {
      regra_curr_max=curr_max;
      printf("Máximo do atuador corrente alterado para: %d\n",regra_curr_max);
    }
    if (regra_temp_min!=temp_min) {
      regra_temp_min=temp_min;
      printf("Mínimo do atuador temperatura alterado para: %d\n",regra_temp_min);
    }
    if (regra_temp_max!=temp_max) {
      regra_temp_max=temp_max;
      printf("Máximo do atuador temperatura alterado para: %d\n",regra_temp_max);
    }
    if (regra_hum_min!=hum_min) {
      regra_hum_min=hum_min;
      printf("Mínimo do atuador humidade alterado para: %d\n",regra_hum_min);
    }
    if (regra_hum_max!=hum_max) {
      regra_hum_max=hum_max;
      printf("Máximo do atuador humidade alterado para: %d\n",regra_hum_max);
    }
    if (regra_power!=power) {
      regra_power=power;
      sprintf(query, "UPDATE sprint2.regras SET valor = %d WHERE id_regra=9",regra_power);
			PQexec(conn, query);
      sprintf(query, "UPDATE sprint2.regras SET valor = %d WHERE id_regra=10",regra_power);
			PQexec(conn, query);
      sprintf(query, "UPDATE sprint2.regras SET valor = %d WHERE id_regra=11",regra_power);
			PQexec(conn, query);
      sprintf(query, "UPDATE sprint2.regras SET valor = %d WHERE id_regra=12",regra_power);
			PQexec(conn, query);

      printf("Regra da potência alterada para: %d\n",regra_power);
    }

    cont=0;
  }
  }	
}
