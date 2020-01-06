#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CDD.h"
//struct that will store all the record information
//the line represents a 2d array that will keep track of the 21 strings in each line of the file
struct record {
    char line[21][100];
};

//Arrays keeping track of the types of disasters
char *types[20] = {"Flood", "Drought", "Heat Event", "Cold Event", "Storm Surge", "Storms and Severe Thunderstorms", "Storm - Unspecified / Other", "Hurricane / Typhoon / Tropical Storm", "Geomagnetic Storm", "Winter Storm", "Tornado", "Tsunami", "Avalanche", "Landslide", "Earthquake", "Volcano", "Wildfire", "Epidemic", "Pandemic", "Infestation"};
char typesC[20] = {FLOOD, DROUGHT, HEAT_EVENT, COLD_EVENT, STORM_SURGE, STORMS_SEVERE_THUNDERSTORMS, STORM_UNSPECIFIED_OTHER, HURRICANE_TYPHOON_TROPICAL, GEOMAGNETIC_STORM, WINTER_STORM, TORNADO, TSUNAMI, AVALANCHE, LANDSLIDE, EARTHQUAKE, VOLCANO, WILDFIRE, EPIDEMIC, PANDEMIC, INFESTATION};


int main(){
    FILE *fr;
    FILE *fw;
    // Open the csv file for reading only
    fr = fopen("CDD2.csv", "r");
    if (!fr) {
        printf("ERROR: Could not open file\nWill exit the program, please try agian!\n");
        exit(1);
    }
    //buffer to hold the contents of each line
    char string[500];
    //variable to keep track of the number of disasters in the file
    unsigned short int numDisasters = 0;
    //variable to keep track of the total number of lines in the file
    int totalLines = 0;
    //use fgets to loop through the file
    while(fgets(string, sizeof(string), fr)) {
        //create a pointer to store each string without the comma
        //use strok to separate each line to the different strings
        char *pt;
        pt = strtok (string,",");
        totalLines++;
        //if the pointer holds the string Disaster, then increment the numDisasters variable
        if (strcmp("Disaster", pt) == 0){
            numDisasters++;
        }
    }
    fclose(fr); //close the file pointer
    //printf("Number of disasters: %u\n", numDisasters);

    // Open the binary file for writing only
    fw = fopen("CDD2.bin", "w");
    if (!fw) {
        printf("ERROR: Could not open file\n");
        exit(1);
    }
    //write the number of disaster to the bin file then close the file pointer
    fwrite(&numDisasters, sizeof(unsigned short int), 1, fw);
    fclose(fw);

    //allocate memory for an array to hold all the records
    struct record *rArray = malloc(totalLines * sizeof(struct record));
    //open the csv file for reading again
    FILE *fr2;
    fr2 = fopen("CDD2.csv", "r");
    if (!fr2) {
        printf("ERROR: Could not open file\nWill exit the program, please try agian!\n");
        exit(1);
    }
    //use for loops to populate the struct
    for (int i=0; i<totalLines; i++) {
        //variable that holds the return value of fscanf
        int ret;
        for (int j=0; j<20; j++){
            //variable that holds the string inside each column
            char words[99];
            //use fscanf to read get each string and check the return value
            ret = fscanf(fr2, "%[^,]s", words);
            //if the return value is 0, then populate the value in the struct with the string "blank"
            //otherise populate the value in the struct with the actual string from the file
            if (ret == 0 && i!=0){
                strcpy(rArray[i].line[j], "blank"); 
            }
            else{
                strcpy(rArray[i].line[j], words);
            }
            //use seek to move on to read the next value in the line
            fseek(fr2, 1, SEEK_CUR);
        }
        char words2[99];
        ret = fscanf(fr2, "%[^\n]s", words2);
        if (ret == 0){
            strcpy(rArray[i].line[20], "blank"); 
        }
        else{
            strcpy(rArray[i].line[20], words2);
        }
        //use seek to move on to read the next line of the file
        fseek(fr2, 1, SEEK_CUR);
    }   
    fclose(fr2); //close the file
    //open the bin file for writing, but in append mode
    FILE *fw2;
    fw2 = fopen("CDD2.bin", "a");
    if (!fw) {
        printf("ERROR: Could not open file\n");
        exit(1);
    }
    //use loops to traverse the struct write the necessary elements
    for (int i=0; i<totalLines; i++){
        int flag = 0;
        //only write natural disasters
        if (strcmp("Natural",  rArray[i].line[1]) == 0){
            char constant = '1';
            //check the the 3rd value in the line array and write the even subgroup char to the file
            if (strcmp("Meteorological - Hydrological",  rArray[i].line[2]) == 0){
                constant = METEOROLOGICAL_HYDROLOGICAL;
            }
            else if (strcmp("Geological",  rArray[i].line[2]) == 0){
                constant = GEOLOGICAL;
            }
            else if(strcmp("Biological",  rArray[i].line[2]) == 0){
                constant = BIOLOGICAL;
            }
            fwrite(&constant, sizeof(char), 1, fw2);
            char t = 1;
            //check the 4th value in the line array and write the approrpriate type char to the file
            //if the type char is 15 (earthquake) set the flag to 1 to make sure the magnitude is written to the file
            for (int j=0; j<20; j++){
                if(strcmp(types[j],  rArray[i].line[3]) == 0){
                    t = typesC[j];
                    if (t==15){
                        flag = 1;
                    }
                   
                }
            }
            fwrite(&t, sizeof(char), 1, fw2);
            int f=0, in=0, e=0, u=0;
            //check if the following indices in the struct array are blank, if not get the inter value of the strings at those indices
            if (rArray[i].line[6] != "blank"){
                f = atoi(rArray[i].line[6]);
            }
            if (rArray[i].line[7] != "blank"){
                in = atoi(rArray[i].line[7]);
            }
            if (rArray[i].line[8] != "blank"){
                e = atoi(rArray[i].line[8]);
            }
            if (rArray[i].line[19] != "blank"){
                u = atoi(rArray[i].line[19]);
            }
            //write the four ints to the file
            fwrite(&f, sizeof(int), 1, fw2);
            fwrite(&in, sizeof(int), 1, fw2);
            fwrite(&e, sizeof(int), 1, fw2);
            fwrite(&u, sizeof(int), 1, fw2);
            //if the type of this record was an earthquake then rget the float value from the struct and write it to the bin file
            if (flag==1){
                float m = atof(rArray[i].line[20]);
                fwrite(&m, sizeof(float), 1, fw2);   
            }
        }
    }
    fclose(fw2); //close the file
}