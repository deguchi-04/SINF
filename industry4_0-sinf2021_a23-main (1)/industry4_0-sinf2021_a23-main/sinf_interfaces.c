#include "sinf_interfaces.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


void write_2_RGB_example(){
	
	// Print the colors of the cells to the RGB Matrix
	
	for (int j=0; j<4; j++){
		switch (j){
					
			case 1: 
			printf("%s,%s,%s,%s","["BLUE,GREEN,RED,BLACK"]\n");
		
			case 2:
			printf("%s,%s,%s,%s","["GREEN,BLACK,RED,RED"]\n");
			
			case 3: 
			printf("%s,%s,%s,%s","["BLUE,BLUE,BLACK,RED"]\n");
			
			case 0:
			printf("%s,%s,%s,%s","["BLACK,GREEN,BLUE,RED"]\n");
		}
	}
	
};

void read_from_terminal_example(){

	//Read a message published on a socat channel named /tmp/ttyV10

	FILE *fp = fopen("/tmp/ttyV10", "r");

	while(1){
		
		if (fgets(received_msg,MAX_MSG_SIZE,fp) != NULL){
			//printf("%s", received_msg);
			
			write_2_RGB_example();
		}
		
	}
	
};

