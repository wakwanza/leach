/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/netanim-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/animation-interface.h"

#include <stdio.h>  
#include <stdlib.h>  
#include <math.h>  
#include <ctype.h>  
#include <string.h>  
#include <time.h>  
#include "leach.h"
#include "const.h" 
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunused-variable" 
#pragma GCC diagnostic ignored "-Wunused-value" 
#pragma GCC diagnostic ignored "-Wwrite-strings" 
#pragma GCC diagnostic ignored "-Wparentheses" 

int NUM_NODES = 50;    // number of nodes in the network  
                // default is 50  
int NETWORK_X = 100;   // X-size of network  
                // default is 100  
int NETWORK_Y = 100;   // Y-size of network  
                // default is 100  
double B_POWER = 0.75;   // initial battery power of sensors  
                // default is 0.75  
                  
                // the percentage of the nodes in the   
                // network that would ideally be cluster   
                // heads during any one round of the   
                // LEACH simulation, default is 0.05  
double CLUSTER_PERCENT = 0.05;  
      
                // the total rounds that the simulation  
                // should run for - the network lifetime  
                // default is 2000  
int TOTAL_ROUNDS = 2000;  
  
                // the distance that the advertisement  
                // phase should broadcast to, in order   
                // to alert other nodes that there is a   
                // cluster head, default is 25.0  
double LEACH_AD_DISTANCE = 25;  
  
                // the message length of the advertisement  
                // that there is a cluster head  
                // default is 16  
int LEACH_AD_MESSAGE = 16;  
  
                // the distance for the cluster head to broadcast   
                // the schedule for transmission to the other nodes   
                // in the cluster, default is 25.0  
double SCHEDULE_DISTANCE = 25;  
  
                // the message length of the schedule which is   
                // sent to the nodes in the cluster during the   
                // scheduling phase of the LEACH simulation,   
                // default is 16.  
int SCHEDULE_MESSAGE = 16;  

int BASE_STATION_X_DEFAULT = 102;

int BASE_STATION_Y_DEFAULT = 103;  

int DEAD_NODE = 1;
int MESSAGE_LENGTH = 8;

int TRIALS = 15;

  
struct sensor {  
     short xLoc;        // X-location of sensor  
     short yLoc;        // Y-location of sensor  
     short lPeriods;        // number of periods the sensor  
                // has been in use for  
     short ePeriods;        // expected periods the sensor   
                // must be operational for  
     double bCurrent;       // current battery power  
     double bPower;     // initial battery power  
     double pAverage;       // average power consumed per   
                // transmission period.  
     int round;         // the last round that the sensor   
                // served as a cluster head  
     int head;          // stores the index of the cluster head   
                // for the sensor to transmit to, set to -1 if the   
                // sensor is a cluster head  
     int cluster_members;   // stores the total number of nodes in   
                // the cluster, applicable only for   
                // cluster head nodes  
     int head_count;        // this contains the count of the   
                // number of times a sensor has been   
                // the head of a cluster, can be   
                // removed for optimization later  
};  
  
  
struct sensor BASE_STATION;  
  
double computeEnergyTransmit(float distance, int messageLength);  
// computes the energy needed to transmit the message  
// inputs are the distance between the two nodes and  
// the length of the message  
  
double computeEnergyReceive(int messageLength);  
// computes the energy needed to receive a message   
// input is the message length  
  
void initializeNetwork(struct sensor network[]);  
// initializes the network; randomly places nodes within   
// the grid and sets battery power to default value  
  
float averageEnergy(struct sensor network[]);  
// computes the average power of the sensors in the   
// network as a percentage of starting power  
// input is a sensor network  
  
struct sensor * loadConfiguration(char * filename);  
// loads values for the network initialization  
// including starting battery power, size of network,   
// and number of nodes  
  
int runLeachSimulation(const struct sensor network[]);  
// takes an initialized sensor network and simulates   
// the LEACH protocol on that network  
// The network is unchanged so that the same network   
// can be passed to other simulator functions  
// The function returns the number of rounds for which the   
// average power of the network remained above the threshold.  
  
int runDirectSimulation(const struct sensor network[]);  
// takes an initialized sensor network and simulates  
// the direct transmission protocol wherein all sensors   
// transmit directly to the base station. The network is   
// unchanged so other protocols can be run on it  
// The function returns the number of rounds for which the   
// average power of the network remained above the threshold.  
  
int runDirectSimulationNPP(const struct sensor network[]);  
// takes an initialized sensor network and simulates  
// the direct transmission protocol with the Network Preserving  
// Protocol in place to lengthen network usefulness. The sensors   
// transmit directly to the base station. The network is   
// unchanged so other protocols can be run on it  
// The function returns the number of rounds for which the   
// average power of the network remained above the threshold.  
  
int sensorTransmissionChoice(const struct sensor a);  
// using the NPP, this function determines whether sensor a  
// should transmit in the current round based on the expected  
// number of rounds the network will be in use and the average   
// energy the sensor has used per round thus far in the simulation.  
// Returns 1 if the sensor should transmit, 0 otherwise.   

int main(int argc, char * argv[])  
{  
  
    struct sensor * network; 
          
    int i = 0;  
    //int j = 0;  
    //int rounds_LEACH = 0;  
    int rounds_DIRECT = 0;  
    int found = FALSE;  
    //double average_comparison = 0.0;  
    char* filename=new char[10]; 

    BASE_STATION.xLoc = BASE_STATION_X_DEFAULT;  
    BASE_STATION.yLoc = BASE_STATION_Y_DEFAULT;  
  
    // search command line arguments for the -f switch  
    // if the -f switch is set, the next argument will be   
    // the configuration file name to use  
    /**for(i = 0; i  <= argc; i++){  
        if((argv[i][0] == '-') && (argv[i][1] == 'f')){  
            found = i;  
        }  
    } **/    
   found = TRUE;   
    // the search for -f was unsuccessful and therefore the   
    // the default configuration file name should be used  
    if(found == FALSE){
        strcpy( filename, ".config" );    
        network = loadConfiguration(".config");  
    // else the search was successful and the file name is   
    // located at one position in the argument array past   
    // where the flag was found  
    }else{   
        network = loadConfiguration(argv[found+1]);  
    }
      
    initializeNetwork(network);  
  
    for(i = 0; i <= TRIALS; i++){  
        //rounds_LEACH += runLeachSimulation(network);  
        rounds_DIRECT += runDirectSimulation(network);  
        printf("\n");  
        runDirectSimulationNPP(network);  
        initializeNetwork(network);  
    }  
  
//printf("The LEACH simulation was able to remain viable for %d rounds\n", rounds_LEACH);  
//printf("The direct transmission simulation was able to remain viable for %d rounds\n", rounds_DIRECT);  
//printf("This is an improvement of %f%%\n", (100.0-((double)rounds_DIRECT/(double)rounds_LEACH)*100));        
//ns3::AnimationInterface anim ("leach.xml");
//ns3::Simulator::Run ();
//ns3::Simulator::Destroy ();
return 0;  
  
}           // end main function  
  
  
int runLeachSimulation(const struct sensor network[]){  
// Preconditions:   the network variable contains an initiailized    
//          sensor network and all global variables have been   
//          set using the loadConfiguration function. The simulation   
//          runs a number of times equal to the TOTAL_ROUNDS variable.  
// Postconditions:      the LEACH protocol simulation has been   
//              run on the supplied network although the instance   
//          of that variable within main memory has NOT been   
//          modified. Output of statistics is currently to the   
//          screen.  
  
struct sensor * network_LEACH;      // wireless sensor network to run sim on  
  
int i = 0;              // indexing variables  
int j = 0;  
int k = 0;  
int closest = 0;  
  
int round = 0;              // current round  
int failed_transmit = 0;        // round where a failed transmission occurred  
  
int testing = 0;            // testing variable, TO BE REMOVED  
int bits_transmitted = 0;       // count of bits transmitted  
int power = FALSE;  
int temp_cluster_members = 0;  
  
double average_energy = 0.0;  
double distance_X_old = 0.0;  
double distance_Y_old = 0.0;  
double distance_old = 0.0;  
double distance_X_new = 0.0;  
double distance_Y_new = 0.0;  
double distance_new = 0.0;  
int recent_round = 1/CLUSTER_PERCENT;  
double threshold = CLUSTER_PERCENT/1-CLUSTER_PERCENT*(round % 1/CLUSTER_PERCENT);  
double random_number;  
int cluster_head_count = 0;  
double percent_found = 0.0;  
network_LEACH = (struct sensor *) malloc(NUM_NODES * sizeof(struct sensor));  
  
// copy the contents of the passed network to a temporary   
// network so the same network can be passed to different   
// protocol simulations  
  
  
  
for(i = 0; i  <= NUM_NODES; i++){  
    network_LEACH[i].bPower = network[i].bPower;  
        network_LEACH[i].xLoc = network[i].xLoc;  
    network_LEACH[i].cluster_members = 0;  
    network_LEACH[i].yLoc = network[i].yLoc;   
    network_LEACH[i].ePeriods = network[i].ePeriods;   
    network_LEACH[i].bCurrent = network[i].bCurrent;   
    network_LEACH[i].bPower = network[i].bPower;   
    network_LEACH[i].pAverage = network[i].pAverage;   
    }  
  
// our iterating loop runs of total rounds, this is   
// the expected lifetime of the network  
  
while(averageEnergy(network_LEACH) > .10){  
//for(j = 0; j  TOTAL_ROUNDS; j++){  
  
    // here we recalculate all the variables   
    // which are round dependent  
    threshold = CLUSTER_PERCENT/1-CLUSTER_PERCENT*(round % 1/CLUSTER_PERCENT);  
    cluster_head_count = 0;  
  
    // advertisement phase  
    // we determine which nodes will be cluster heads  
    for(i = 0; i <= NUM_NODES; i++){  
        if(network_LEACH[i].round < (j - recent_round) || (j - recent_round == 0)){  
            if(network_LEACH[i].head != DEAD_NODE){  
                random_number = .00001*(rand() % 100000);          
                if(random_number <= threshold){  
                    // the random number selected is less   
                    // than the threshold so the node becomes   
                    // a cluster head for the round  
                    network_LEACH[i].head_count++;  
                    // update the round variable   
                    // so we know that this sensor was   
                    // last a cluster head at round i  
                    network_LEACH[i].round = j;  
                    network_LEACH[i].head = -1;  
                    // store the index of the node in the   
                    // cluster_heads array  
                    // increment the cluster_head_count  
                    cluster_head_count++;  
                }  
            }  
        }  
    }  
  
  
  
    percent_found += (double)cluster_head_count/(double)NUM_NODES;  
    // now the cluster heads must transmit the fact that they   
    // are cluster heads to the network, this will be a constant   
    // transmit energy, during this period the other nodes must   
    // keep their receivers on - which has an energy cost, again   
    // this is constant  
    for(i = 0; i  <= NUM_NODES; i++){  
        if(network_LEACH[i].head == -1){  
            network_LEACH[i].bCurrent -=   
                computeEnergyTransmit(LEACH_AD_DISTANCE,  
                            LEACH_AD_MESSAGE);  
        }  
        else{  
            network_LEACH[i].bCurrent -=   
                computeEnergyReceive(LEACH_AD_MESSAGE);  
        }  
    }  
  
  
  
// CLUSTER SET-UP PHASE  
    // for this phase, all non-cluster heads determine to which   
    // cluster they will broadcast and alert the cluster head to that  
  
  
        for(i = 0; i  <= NUM_NODES; i++){  
                closest = -1;  
                if((network_LEACH[i].head != -1) &&   
                    network_LEACH[i].head != DEAD_NODE){  
                        // if the node's round is not equal to the    
                        // current round, the node is not a cluster  
                        // head and we must find a cluster head for  
                        // the node to transmit to  
                for(k = 0; k <= NUM_NODES; k++){  
                            if(network_LEACH[k].head == -1 && closest != -1){  
                                distance_X_old = network_LEACH[i].xLoc - network_LEACH[closest].xLoc;  
                                distance_Y_old = network_LEACH[i].yLoc - network_LEACH[closest].yLoc;  
                                distance_old = sqrt(pow(distance_X_old, 2) + pow(distance_Y_old, 2));  
                                distance_X_new = network_LEACH[i].xLoc - network_LEACH[k].xLoc;  
                                distance_Y_new = network_LEACH[i].yLoc - network_LEACH[k].yLoc;  
                                distance_new = sqrt(pow(distance_X_new, 2) + pow(distance_Y_new, 2));  
                                if(distance_new < distance_old)  
                                    closest = k;  
                            }  
                            else if(network_LEACH[k].head == -1 && closest == -1){  
                                closest = k;  
                            }  
                }  
                          
                network_LEACH[i].head = closest;  
                network_LEACH[closest].cluster_members++;  
                }  
        }  
      
  
  
// SCHEDULE CREATION  
    // the cluster head then creates a schedule telling the nodes   
    // when they can transmit; the schedule is broadcast to the nodes  
    // but no simulation of this is neccescary outside of the loss   
    // of battery power for broadcasting the schedule, which is a constant  
    for(i = 0; i <= NUM_NODES; i++){  
        if(network_LEACH[i].head == -1){  
            // if the node is going to be a cluster head, it transmits   
            // the schedule to the other nodes  
            network_LEACH[i].bCurrent -=   
                computeEnergyTransmit(SCHEDULE_DISTANCE, SCHEDULE_MESSAGE);  
        }  
        else  
            network_LEACH[i].bCurrent -=   
                computeEnergyReceive(SCHEDULE_MESSAGE);  
    }  
  
  
  
// DATA TRANSMISSION  
    // non cluster heads send their data to the cluster heads who then   
    // broadcast the data to the base station  
    for(i = 0; i <= NUM_NODES; i++){  
        network_LEACH[i].lPeriods++;  
        if(network_LEACH[i].head != -1 && network_LEACH[i].head != DEAD_NODE){  
                distance_X_new = network_LEACH[i].xLoc - network_LEACH[network_LEACH[i].head].xLoc;  
                distance_Y_new = network_LEACH[i].yLoc - network_LEACH[network_LEACH[i].head].yLoc;  
                distance_new = sqrt((pow(distance_X_new, 2) + pow(distance_Y_new, 2)));  
                network_LEACH[i].bCurrent -=   
                    computeEnergyTransmit(distance_new, MESSAGE_LENGTH);  
                network_LEACH[network_LEACH[i].head].bCurrent -=  
                    computeEnergyReceive(MESSAGE_LENGTH);  
                if(network_LEACH[i].bCurrent < 0.0 && network_LEACH[i].head != -1){  
                    network_LEACH[i].head = DEAD_NODE;  
                }  
      
        }  
    }  
  
  
    for(i = 0; i <= NUM_NODES; i++){  
        if(network_LEACH[i].head == -1){  
            distance_X_new = network_LEACH[i].xLoc - BASE_STATION.xLoc;  
            distance_Y_new = network_LEACH[i].yLoc - BASE_STATION.yLoc;  
            distance_new = sqrt(pow(distance_X_new, 2) + pow(distance_Y_new, 2));  
            network_LEACH[i].bCurrent -=   
                computeEnergyTransmit(distance_new,   
                    (MESSAGE_LENGTH * (network_LEACH[i].cluster_members+1)));  
            if(network_LEACH[i].bCurrent > 0.0){  
                bits_transmitted += (MESSAGE_LENGTH * (network_LEACH[i].cluster_members+1));  
            }  
            else{  
                failed_transmit++;  
                network_LEACH[i].head = DEAD_NODE;  
                temp_cluster_members = network_LEACH[i].cluster_members + 1;  
                network_LEACH[i].bCurrent +=  
                    computeEnergyTransmit(distance_new,   
                    (MESSAGE_LENGTH * (network_LEACH[i].cluster_members+1)));  
                while(power == FALSE){  
                    if((network_LEACH[i].bCurrent -   
                        computeEnergyTransmit(distance_new,(MESSAGE_LENGTH * temp_cluster_members)))   
                        > 0){  
                            bits_transmitted += (MESSAGE_LENGTH * temp_cluster_members);  
                            power = TRUE;  
                    }  
                    else  
                            temp_cluster_members--;  
                    if(temp_cluster_members == 0)  
                            power == TRUE;  
                }  
  
            }  
        }  
    }   
    // round has completed, increment the round count  
    for(i = 0; i <= NUM_NODES; i++){  
        network_LEACH[i].cluster_members = 0;  
        network_LEACH[i].head = 0;  
    }  
  
    cluster_head_count = 0;  
    round++;  
  
}  
  
free(network_LEACH);  
//printf("\nRunning the LEACH Transmission Simulation \n");  
//printf("The average energy of the network remained above 10%% for %d tranmission periods\n", round);  
  
return round;  
}       // end runLeachSimulation function  
  
  
  
  
int runDirectSimulation(const struct sensor network[]){  
// Preconditions:   the network variable contains an initiailized    
//          sensor network and all global variables have been   
//          set using the loadConfiguration function. The simulation   
//          runs a number of times equal to the TOTAL_ROUNDS variable.  
// Postconditions:      the direct transmission simulation has been   
//              run on the supplied network although the instance   
//          of that variable within main memory has NOT been   
//          modified. Output of statistics is currently to the   
//          screen.  
  
int j = 0;  
int i = 0;  
int round = 0;  
int bits_transmitted = 0;  
int range1 = 0;  
int range0_count = 0;  
int range1_count = 0;  
int range2 = 0;  
int range2_count = 0;  
int range3 = 0;  
int range3_count = 0;  
int range4 = 0;  
int range4_count = 0;  
int range5 = 0;  
int range5_count = 0;  
int nodes_transmitting = 0;  
int failed_transmission = 0;  
float distance_X = 0.0;  
float distance_Y = 0.0;  
float distance = 0.0;  
float power_consumption = 0.0;  
struct sensor * network_DIRECT;  
  
printf("Running the Direct Transmission Simulation without NPP.\n");  
  
network_DIRECT = (struct sensor *) malloc(NUM_NODES * sizeof(struct sensor));  
  
range1 = NUM_NODES * 0.20;  
range2 = NUM_NODES * 0.40;  
range3 = NUM_NODES * 0.60;  
range4 = NUM_NODES * 0.80;  
range5 = NUM_NODES * 1.00;  
  
for(i = 0; i <= NUM_NODES; i++){  
    network_DIRECT[i].bPower = network[i].bPower;  
        network_DIRECT[i].xLoc = network[i].xLoc;  
    network_DIRECT[i].cluster_members = 0;  
    network_DIRECT[i].yLoc = network[i].yLoc;   
    network_DIRECT[i].ePeriods = network[i].ePeriods;   
    network_DIRECT[i].bCurrent = network[i].bCurrent;   
    network_DIRECT[i].bPower = network[i].bPower;   
    network_DIRECT[i].pAverage = network[i].pAverage;   
    }  
  
    //printf("\nRunning the Direct Transmission Simulation \n");  
      
//while(averageEnergy(network_DIRECT) > .10){  
for(j = 0; j <= TOTAL_ROUNDS; j++){  
        for(i = 0; i <= NUM_NODES; i++) {  
        // cycle through all nodes in network and attempt to   
        // transmit  
        if(network_DIRECT[i].bCurrent > 0){  
            nodes_transmitting++;  
                    distance_X = network_DIRECT[i].xLoc - BASE_STATION.xLoc;  
                    distance_Y = network_DIRECT[i].yLoc - BASE_STATION.yLoc;  
                    distance = sqrt(pow(distance_X, 2) + pow(distance_Y,2));  
                    power_consumption = computeEnergyTransmit(distance, MESSAGE_LENGTH);  
            if(power_consumption <= network_DIRECT[i].bCurrent)  
                bits_transmitted += MESSAGE_LENGTH;  
            else  
                failed_transmission++;  
                network_DIRECT[i].bCurrent -= power_consumption;  
            }  
            if(network_DIRECT[i].bCurrent < 0)  
                network_DIRECT[i].bCurrent = 0;  
    }  
        if(nodes_transmitting == 0)  
            range0_count++;  
        else if(nodes_transmitting < range1)  
            range1_count++;  
        else if(nodes_transmitting < range2)  
            range2_count++;  
        else if(nodes_transmitting < range3)  
            range3_count++;  
        else if(nodes_transmitting < range4)  
            range4_count++;  
        else  
            range5_count++;  
        round++;  
        nodes_transmitting = 0;  
}  
printf("Rounds where:\n");  
printf("0 nodes transmitted: %d \n", range0_count);  
printf("1 - %d nodes transmitted: %d \n", range1, range1_count);  
printf("%d - %d nodes transmitted: %d \n", range1, range2, range2_count);  
printf("%d - %d nodes transmitted: %d \n", range2, range3, range3_count);  
printf("%d - %d nodes transmitted: %d \n", range3, range4, range4_count);  
printf("%d - %d nodes transmitted: %d \n", range4, range5, range5_count);  
  
return round;  
}           // end runDirectSimulation function  
  
  
int runDirectSimulationNPP(const struct sensor network[]){  
// Preconditions:   the network variable contains an initiailized    
//          sensor network and all global variables have been   
//          set using the loadConfiguration function. The simulation   
//          runs a number of times equal to the TOTAL_ROUNDS variable.  
//          This simulation includes the Network Preserving Protocol.  
//          In the interest of readability, we include a separate   
//          function rather even though much code is shared between   
//          this function and the runDirectSimulation function  
// Postconditions:      the direct transmission simulation has been   
//              run on the supplied network although the instance   
//          of that variable within main memory has NOT been   
//          modified. Output of statistics is currently to the   
//          screen.  
  
int j = 0;  
int i = 0;  
int round = 0;  
int bits_transmitted = 0;  
int range1 = 0;  
int range0_count = 0;  
int range1_count = 0;  
int range2 = 0;  
int range2_count = 0;  
int range3 = 0;  
int range3_count = 0;  
int range4 = 0;  
int range4_count = 0;  
int range5 = 0;  
int range5_count = 0;  
int failed_transmission = 0;  
int nodes_transmitting = 0;  
float distance_X = 0.0;  
float distance_Y = 0.0;  
float distance = 0.0;  
float power_consumption = 0.0;  
struct sensor * network_DIRECT_NPP;  
  
network_DIRECT_NPP = (struct sensor *) malloc(NUM_NODES * sizeof(struct sensor));  
  
printf("Running the Direct Transmission Simulation with NPP.\n");  
range1 = NUM_NODES * 0.20;  
range2 = NUM_NODES * 0.40;  
range3 = NUM_NODES * 0.60;  
range4 = NUM_NODES * 0.80;  
range5 = NUM_NODES * 1.00;  
  
for(i = 0; i <= NUM_NODES; i++){  
    network_DIRECT_NPP[i].bPower = network[i].bPower;  
        network_DIRECT_NPP[i].xLoc = network[i].xLoc;  
    network_DIRECT_NPP[i].cluster_members = 0;  
    network_DIRECT_NPP[i].yLoc = network[i].yLoc;   
    network_DIRECT_NPP[i].ePeriods = network[i].ePeriods;   
    network_DIRECT_NPP[i].bCurrent = network[i].bCurrent;   
    network_DIRECT_NPP[i].bPower = network[i].bPower;   
    network_DIRECT_NPP[i].pAverage = network[i].pAverage;   
    }  
  
    //printf("\nRunning the Direct Transmission Simulation \n");  
  
for(j = 0; j <= TOTAL_ROUNDS; j++){  
        for(i = 0; i < NUM_NODES; i++) {  
        // cycle through all nodes in network and attempt to   
        // transmit  
        if((network_DIRECT_NPP[i].bCurrent > 0) &&  
          (sensorTransmissionChoice(network_DIRECT_NPP[i]) == 1)){  
            nodes_transmitting++;     
                    distance_X = network_DIRECT_NPP[i].xLoc - BASE_STATION.xLoc;  
                    distance_Y = network_DIRECT_NPP[i].yLoc - BASE_STATION.yLoc;  
                    distance = sqrt(pow(distance_X, 2) + pow(distance_Y,2));  
                    power_consumption = computeEnergyTransmit(distance, MESSAGE_LENGTH);  
            if(power_consumption < network_DIRECT_NPP[i].bCurrent)  
                bits_transmitted += MESSAGE_LENGTH;  
            else  
                failed_transmission++;  
                network_DIRECT_NPP[i].bCurrent -= power_consumption;  
            }  
            if(network_DIRECT_NPP[i].bCurrent < 0)  
                network_DIRECT_NPP[i].bCurrent = 0;  
            network_DIRECT_NPP[i].lPeriods++;  
            if(network_DIRECT_NPP[i].bCurrent > 0){  
                network_DIRECT_NPP[i].pAverage = ((network_DIRECT_NPP[i].bPower -   
                    network_DIRECT_NPP[i].bCurrent)/  
                    network_DIRECT_NPP[i].lPeriods);  
            }  
    }  
        if(nodes_transmitting == 0)  
            range0_count++;  
        else if(nodes_transmitting < range1)  
            range1_count++;  
        else if(nodes_transmitting < range2)  
            range2_count++;  
        else if(nodes_transmitting < range3)  
            range3_count++;  
        else if(nodes_transmitting < range4)  
            range4_count++;  
        else  
            range5_count++;  
        round++;  
        nodes_transmitting = 0;  
}  
printf("Rounds where:\n");  
printf("0 nodes transmitted: %d \n", range0_count);  
printf("1 - %d nodes transmitted: %d \n", range1, range1_count);  
printf("%d - %d nodes transmitted: %d \n", range1, range2, range2_count);  
printf("%d - %d nodes transmitted: %d \n", range2, range3, range3_count);  
printf("%d - %d nodes transmitted: %d \n", range3, range4, range4_count);  
printf("%d - %d nodes transmitted: %d \n", range4, range5, range5_count);  
  
return round;  
}           // end runDirectSimulationNPP function  
  
  
double computeEnergyTransmit(float distance, int messageLength){  
// Preconditions:   distance contains the distance between the transmitting   
//                  node and the receiving node. messageLength contains   
//                  the length of the message in bits.  
// Postconditions:  the total energy consumed by the transmission is   
//                  returned  
      
    float E_elec = 50 * pow(10,-9);  
        float epsilon_amp = 100 * pow(10,-12);  
        double EnergyUse = 0.00;  
          
        EnergyUse = (messageLength * E_elec) +   
              (messageLength * epsilon_amp * pow(distance,2));   
      
    return EnergyUse;  
  
}           // end computeEnergyTransmit function  
          
double computeEnergyReceive(int messageLength) {  
// Preconditions:   messageLength contains the length of the  
//                  message in bits to be received by the station.  
// Postconditions:  the total energy consumed by the reception   
//              of the transmission is returned.      
    return messageLength * (50 * pow(10,-9));  
}           // end computeEnergyReceive function  
  
  
void initializeNetwork(struct sensor network[]) {  
// Preconditions:   network is an unitialized sensor network.  
// Postconditions:  network is an initialized sensor network   
//                  whose values are loaded from global varibles   
//          which are set using the loadConfiguration   
//          function or the defaults are used  
    int i = 0;  
    srand((unsigned int) time(0));  
          
    for(i = 0; i < NUM_NODES; i++) {  
                network[i].xLoc = rand() % NETWORK_X;  
                network[i].yLoc = rand() % NETWORK_Y;  
                network[i].lPeriods = 0;  
                network[i].ePeriods = TOTAL_ROUNDS;  
                network[i].bCurrent = B_POWER;  
                network[i].bPower = B_POWER;  
                network[i].pAverage = 0.00;  
        network[i].round = FALSE;  
        network[i].head = FALSE;  
        }  
       
}           // end initializeNetwork function  
  
  
  
  
struct sensor * loadConfiguration(char * filename)  
{  
// Preconditions:   a .config file exists where settings for   
//          the simulation may be pulled from.  
// Postconditions:  global variables are set if they exist within   
//          the configuration file. If they do not, the   
//          default values are used, which have already been  
//          set. These global variables are:  
//          NUM_NODES -    the number of nodes in network  
//          NETWORK_X -    the X-size of the network  
//          NETWORK_Y -    the Y-size of the network  
//          B_POWER   -    the starting battery power of the nodes  
//          TOTAL_ROUNDS - the number of rounds the simulation should   
//                 run for  
    int total_size = 50;  
    int i = 0;  
    int axis = 0;  
  
    char buf[80];       // buffer for reading lines from file  
    char copy[80];      // temporary memory for storing the   
    char cut[8];        // integer result of a line  
    char cut2[8];  
  
    FILE * fp;  
    // open file with settings as read only,  
    // if file cannot be opened, allocate the memory  
    // based on the default values and return  
    if((fp = fopen(filename, "r")) == NULL){  
        return (struct sensor *) malloc(NUM_NODES *   
            sizeof(struct sensor));  
    }  
  
    // read entire configuration file     
    while(fgets(buf, 80, fp) != NULL){  
        // if the line is the NUM_NODES variable  
        strncpy(cut, buf, 7);  
        strncpy(cut2, cut, 7);  
        if(cut[0] == '#'){  
            i = 0;  
        }  
        else if(cut[0] == 'N' && cut[1] == 'U'){  
            i = 9;  
            // advance pointer to = sign  
            while(buf[i] != '='){  
                i++;  
            }  
            // then advance pointer one more space  
            // past the equal sign  
            i++;  
            // if there is any whitespace, advance  
            // pointer past it  
            while(buf[i] == ' '){  
                i++;  
            }  
            // copy the contents of the line read from the file  
            // into the copy variable from the iterator i  
            strcpy(copy, buf + i);  
            // convert the copied characters to NUM_NODES  
            NUM_NODES = atoi(copy);  
        }  
          
        else if(cut[0] == 'N' && cut[1] == 'E'){  
            i = 6;  
                        // advance pointer to = sign  
                        while(buf[i] != '='){  
                                if(buf[i] == 'X')  
                axis = 1;  
                if(buf[i] == 'Y')  
                axis = 0;  
                i++;  
                        }  
                        // then advance pointer one more space  
                        // past the equal sign  
                        i++;  
                        // if there is any whitespace, advance  
                        // pointer past it  
                        while(buf[i] == ' '){  
                                i++;  
                        }  
                    // copy the contents of the line read from the file  
                    // into the copy variable from the iterator i  
                    strcpy(copy, buf + i);  
            if(axis == 1)  
                        NETWORK_X = atoi(copy);  
            if(axis == 0)  
                NETWORK_Y = atoi(copy);  
        }         
        else if(cut[0] == 'R' && cut[2] == 'U' &&  
            cut[4] == 'D'){  
            i = 6;  
            // advance pointer  
            while(buf[i] != '='){  
                i++;  
            }  
            // then advance pointer one more space  
            // past the equal sign  
            i++;  
            // if there is any whitespace, advance  
            // pointer past it  
            while(buf[i] == ' '){  
                i++;  
            }  
            // copy the contents of the line read from the file  
            // into the copy variables from the iterator i  
            strcpy(copy, buf+i);  
            // convert the copied characters to an integer  
            // and store it in total rounds  
            TOTAL_ROUNDS = atoi(copy);  
        }  
        else if(cut2[0] = 'B' && cut2[1] == '_' &&  cut2[2] == 'P'){  
            			i = 7;  
                        // advance pointer to = sign  
                        while(buf[i] != '='){  
                                i++;  
                        }  
                        // then advance pointer one more space  
                        // past the equal sign  
                        i++;  
                        // if there is any whitespace, advance  
                        // pointer past it  
                        while(buf[i] == ' '){  
                                i++;  
                        }  
                        // copy the contents of the line read from the file  
                        // into the copy variable from the iterator i  
                        strcpy(copy, buf + i);  
                        // convert the copied characters to B_POWER  
                        B_POWER = atof(copy);  
                }  
    }  
      
      
    if(fclose(fp) != 0){  
        printf("Error closing config file.\n");  
        exit(1);  
    }  
      
    return (struct sensor *) malloc(NUM_NODES * sizeof(struct sensor));   
  
}           // end loadConfiguration function  
  
  
float averageEnergy(struct sensor network[]) {  
// Preconditions:   network is an initialized sensor network  
// Postconditions:  the average percentage of power in the   
//                  batteries across the network is returned.  
//                  i.e. 0.90 means that the average battery   
//                  in the network is at 90% of its original   
//                  power  
    float average = 0.0;  
    float starting_power = 0.00;  
    float current_power = 0.00;  
    int i = 0;  
  
    for(i = 0; i <= NUM_NODES; i++) {  
        starting_power += network[i].bPower;  
        current_power += network[i].bCurrent;  
    }  
  
    return current_power/starting_power;  
  
}           // end averageEnergy function  
  
int sensorTransmissionChoice(const struct sensor a){  
// Preconditions:   a is an initialized sensor  
// Postconditions:  1 is returned if a should transmit for   
//          the current round, 0 otherwise.  
int remaining_periods = 0;  
  
remaining_periods = a.ePeriods - a.lPeriods;  
if((remaining_periods * a.pAverage) > a.bCurrent)  
    return 0;  
else  
    return 1;  
}

