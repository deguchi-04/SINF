#ifndef _sinf_interfaces_h_
#define _sinf_interfaces_h_
#endif

//Variable Definition

struct sensor{
    int temp;
    int hum;
    int curr;
    int light;
    int power;
};

struct RGB{
    char temp_sens[20];
    char temp_act[20];
    char hum_sens[20];
    char hum_act[20];
    char curr_sens[20];
    char curr_act[20];
    char light_sens[20];
    char light_act[20];
};

struct actuator{
    int temp;
    int hum;
    int curr;
    int light;
};


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
char received_msg[MAX_MSG_SIZE];


//Function Definition

extern void write_2_RGB_example();
extern void read_from_terminal_example();

