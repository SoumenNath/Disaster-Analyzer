#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CDD.h"
//struct that stores the 7 record values that is read from the bin file
struct record2 {
    char sG; //event subgroup
    char T; //event type
    int f; //FATALITIES
    int in; //INJURED / INFECTED
    int e; //EVACUATED
    int u; //UTILITY - PEOPLE AFFECTED
    float m; //magnitude 
};
//Arrays keeping track of the types of disasters
char *types[20] = {"Flood", "Drought", "Heat Event", "Cold Event", "Storm Surge", "Storms and Severe Thunderstorms", "Storm - Unspecified / Other", "Hurricane / Typhoon / Tropical Storm", "Geomagnetic Storm", "Winter Storm", "Tornado", "Tsunami", "Avalanche", "Landslide", "Earthquake", "Volcano", "Wildfire", "Epidemic", "Pandemic", "Infestation"};
char typesC[20] = {FLOOD, DROUGHT, HEAT_EVENT, COLD_EVENT, STORM_SURGE, STORMS_SEVERE_THUNDERSTORMS, STORM_UNSPECIFIED_OTHER, HURRICANE_TYPHOON_TROPICAL, GEOMAGNETIC_STORM, WINTER_STORM, TORNADO, TSUNAMI, AVALANCHE, LANDSLIDE, EARTHQUAKE, VOLCANO, WILDFIRE, EPIDEMIC, PANDEMIC, INFESTATION};
//array keeping track of the fatalities of each disaster
int disF[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//array keeping track of the evacuees of each disaster
int disE[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//2D array keeping track of the occurence of each disaster as well as the index of disaster in the types array
int disO[20][2] = {{0,0}, {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7}, {0,8}, {0,9}, {0,10}, {0,11}, {0,12}, {0,13}, {0,14}, {0,15}, {0,16}, {0,17}, {0,18}, {0,19}};


int main(){
    // Open the bin file for reading only
    FILE *fr;
    fr = fopen("CDD2.bin", "r");
    if (!fr) {
        printf("ERROR: Could not open file\n");
        exit(1);
    }
    //read in the number of disasters
    unsigned short int n;
    fread(&n, sizeof(unsigned short int), 1, fr);
    printf("Number of disasters: %u\n", n);
    //convert value to int 
    int nD = (int) n;
    //allocate memory for an array to hold all the records
    struct record2 *rArray = malloc(nD * sizeof(struct record2));
    for (int i=0; i<nD; i+=1) {
        //read in the 7 fields and store them in the struct array
        fread(&rArray[i].sG, sizeof(char), 1, fr);
        fread(&rArray[i].T, sizeof(char), 1, fr);
        fread(&rArray[i].f, sizeof(int), 1, fr);
        fread(&rArray[i].in, sizeof(int), 1, fr);
        fread(&rArray[i].e, sizeof(int), 1, fr);
        fread(&rArray[i].u, sizeof(int), 1, fr);
        //if the type was an earthquake then store the magnitude from the file in the struct array
        //otherwise set the magnitude to 0
        if (rArray[i].T == 15){
            fread(&rArray[i].m, sizeof(float), 1, fr);
        }
        else{
            rArray[i].m = 0.0; 
        }
    }   
    fclose(fr); //close the file
    //variables to keep track of the total FATALITIES, INJURED / INFECTED, EVACUATED, and UTILITY - PEOPLE AFFECTED respectively
    int tF=0, tI=0, tE=0, tU=0;
    //variable to keep track of the highest magnitude
    float hMag = 0.0;
    //traverse the struct array
    for (int i=0; i<nD; i++){
        //find the highest magnitude
        if (rArray[i].m>hMag){
            hMag = rArray[i].m;
        }
        //increment the total values
        tF += rArray[i].f;
        tI += rArray[i].in;
        tE += rArray[i].e;
        tU += rArray[i].u;
        
        for (int j=0; j<20; j++){
            //find the index of the type of the record in the typeC array
            if (rArray[i].T == typesC[j]){
                //use that index to increase the values in the same index in the other three corresponding arrays
                disF[j] += rArray[i].f;
                disE[j] += rArray[i].e;
                disO[j][0] += 1;
            }
        }
    }
    //print the following
    printf("\nTotal number of fatalities : %d\n", tF);
    printf("Total number of injured/infected: %d\n", tI);
    printf("Total number of evacuations: %d\n", tE);
    printf("Total number of utilities-affected: %d\n", tU);
    //find the index of the the disaster type with the most fatalities
    int max = disF[0];
    int maxI = 0;
    for (int i = 1; i < 20; i++) {
        if (disF[i] > max) {
            maxI = i;
            max = disF[i];
        }
    }
    //use that index to print the values in the same index in the types and disF array respectively
    printf("\nThe type of disaster that had the most fatalities: %s\n", types[maxI]);
    printf("The number of fatalities from that particular disaster: %d\n", disF[maxI]);
    //repeat the same process for the distart type which caused the most evacuess
    max = disE[0];
    maxI = 0;
    for (int i = 1; i < 20; i++) {
        if (disE[i] > max) {
            maxI = i;
            max = disE[i];
        }
    }
    printf("The type of disaster that resulted with the most evacuations: %s\n", types[maxI]);
    printf("The number of evacuated from that particular disaster: %d\n", disE[maxI]);
    //print the magnitude of the highest array
    printf("\nThe strongest earthquake magnitude: %f\n\n", hMag);
    //use selection sort to sort the list of the 20 disasters via their occurences
    int minI = 0; 
    for (int i = 0; i<19; i++) { 
        minI = i; 
        //find the index of the smallest value in the array
        for (int j = i+1; j < 20; j++){
          if (disO[j][0] < disO[minI][0]){
              minI = j; 
          }   
        }
        //swap the entire row (occurence and index) of the current index with the min index
        for (int j=0; j<2; j++){
            int temp;
            temp = disO[minI][j];
            disO[minI][j] =  disO[i][j];
            disO[i][j] = temp;
        }
    }
    //print out the list of disasters with their occurence from highest to lowest number of occurences
    for (int i=19; i>-1; i--){
        printf("Disaster type %s has occured: %d times\n", types[disO[i][1]], disO[i][0]);
    }
    //free the memory
    free(rArray);
}