#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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

  
  printf("Mote:%d Luz:%.2f Corrente:%.2f Temperatura:%.2f Humidade:%.2f Potência:%.2f\n", id, sensores[id-1].light, sensores[id-1].curr, sensores[id-1].temp, sensores[id-1].hum, sensores[id-1].power);	
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
  
  if (sensores[id-1].temp > 22){
    strcpy(rgb_data[id-1].temp_sens,RED);
    //strcpy(rgb_data[id-1].temp_act,IceIceBaby);
  }
  
  else if (sensores[id-1].temp < 20){
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
  if (sensores[id-1].hum > 80){
    strcpy(rgb_data[id-1].hum_sens,BLUE);
  }
  
  else if (sensores[id-1].hum < 20){
    strcpy(rgb_data[id-1].hum_sens,Orange);
  }
  
  else{
    strcpy(rgb_data[id-1].hum_sens,GREEN);
  }


  //Corrente
  if (sensores[id-1].curr > 40){
    strcpy(rgb_data[id-1].curr_sens,RED);
  }
  
  else if (sensores[id-1].curr < 10){
    strcpy(rgb_data[id-1].curr_sens,Grey);
  }
  
  else{
    strcpy(rgb_data[id-1].curr_sens,GREEN);
  }


  //Luz
  if (sensores[id-1].light < 500 ){
    strcpy(rgb_data[id-1].light_sens,Grey);
  }
  
  else if (sensores[id-1].light > 2000){
    strcpy(rgb_data[id-1].light_sens,RED);
  }

  else if (sensores[id-1].light >= 500 && sensores[id-1].light < 1250) {
    strcpy(rgb_data[id-1].light_sens,Yellow);
  }
  
  else{
    strcpy(rgb_data[id-1].light_sens,Amarelo_escurinho);
  }

  //Potencia
  if (sensores[id-1].power > 10000) {
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
     FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[1].light_sens,FLOOR,\
     FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,Oven,rgb_data[1].temp_sens,rgb_data[1].temp_act,FLOOR,rgb_data[1].curr_sens,FLOOR,\
     FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[1].hum_sens,FLOOR,\
     FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,\
     FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[2].light_sens,FLOOR,FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[3].light_sens,FLOOR,\
     FLOOR,Oven,rgb_data[2].temp_sens,rgb_data[2].temp_act,FLOOR,rgb_data[2].curr_sens,FLOOR,FLOOR,Oven,rgb_data[3].temp_sens,rgb_data[3].temp_act,FLOOR,rgb_data[3].curr_sens,FLOOR,\
     FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[2].hum_sens,FLOOR,FLOOR,Oven,Oven,Oven,FLOOR,rgb_data[3].hum_sens,FLOOR,\
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
  
  act_channel = fopen("MsgCreatorConf.txt", "r+");
  if (act_channel==NULL){
    return 0;
  }

  if (sensores[id-1].temp > 22 || sensores[id-1].temp < 20 || sensores[id-1].light > 2000 || sensores[id-1].light < 500 || sensores[id-1].curr > 40 || sensores[id-1].curr < 10 || sensores[id-1].hum > 80 || sensores[id-1].hum < 20){
    
    while (1)
    {
      i = fgetc(act_channel);
     
      if (i ==','){
       
        virgulas++;

        if (virgulas==6) {

          i = fgetc(act_channel);
          pos = ftell(act_channel)-1;
          if (i=='+' && sensores[id-1].light > 2000) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("-",act_channel);
          }
          else if (i=='-' && sensores[id-1].light < 500) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("+",act_channel);
          }
        }

        if (virgulas==10) {

          i = fgetc(act_channel);
          pos = ftell(act_channel)-1;
          if (i=='+' && sensores[id-1].curr > 40) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("-",act_channel);
          }
          else if (i=='-' && sensores[id-1].curr < 10) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("+",act_channel);
          }
        }

        if (virgulas==14){
          
          i = fgetc(act_channel);
          pos = ftell(act_channel)-1;
          // aux = 0;
          if (i=='+' && sensores[id-1].temp > 22) {
            
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
          else if (i=='-' && sensores[id-1].temp < 20) {
           
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
          if (i=='+' && sensores[id-1].hum > 80) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("-",act_channel);
          }
          else if (i=='-' && sensores[id-1].hum < 20) {
            fseek(act_channel,pos,SEEK_SET);                    //Move cursor para pos
            fputs("+",act_channel);
          }
          break;
        }
      }
    }
  }

  fclose(act_channel);

}


int main()
{	
  
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

  while(1){  
  
  // Obtain data from the temperature sensor
  check_sensors();
  // Decide and Print the colors of the cells to the RGB Matrix
  writeRGBmatrix();
  
  update_act();
  }	
}
