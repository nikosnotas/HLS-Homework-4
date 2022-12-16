#include <iostream>
#include <cstdlib>
#include "ac_int.h"
#include "ac_math.h"
#include "struct.h" //include this file to use Point struct

#include "mc_scverify.h"

#pragma hls_design top
template<int N, int M>
bool CCS_BLOCK(kmeans)(Point points[N], int ID[N], Point center[M]) {
	
	//create count array to store the quantity of points in each team
	ac_int<32, false> count[M];
	ac::init_array<AC_VAL_0>(count, M); //initialize to zero via init_array method
	
	//create sum array to accumulate the sum of x values in each team
	ac_int<32, false> sum_x[M];
	ac::init_array<AC_VAL_0>(sum_x, M);
    
    //create sum array to accumulate the sum of y values in each team
    ac_int<32, false> sum_y[M];
	ac::init_array<AC_VAL_0>(sum_y, M);
	
	
	POINTS: for (int i = 0; i < N; i++) {
		ac_int<17, false> distance; //variable to store the calculated distance
		ac_int<16, false> index = 0; //variable to store the index of each point 
		ac_int<17, false> closest; //variable to store the closest distance to a point of all centers
		ac_int<16, false> x_p = points[i].x; //load x value of current point
		ac_int<16, false> y_p = points[i].y; //load y value of current point 
		
		closest.set_val<AC_VAL_MAX>(); //initialize closest distance to maximum value via set_val method
		//in this case (for 17 bits size) the max value is 131071 (2^17-1) 
		
		
		DIST: for (int j = 0; j < M; j++) {
			ac_int<16, false> x, y;
			ac_int<16, false> x_c = center[j].x; //load x value of current center
			ac_int<16, false> y_c = center[j].y; //load y value of current center
			
			ac_math::ac_abs(x_p - x_c, x); //use ac_abs function to calculate absolute value
			ac_math::ac_abs(y_p - y_c, y);
			
			distance = x + y; //calculate distance between current point and current center 
			
            if (distance < closest) { //check if current distance is smaller
            	closest = distance;
				index = j;	
			}
		}
		
		ID[i] = index; //update ID array with the index which represent the team of current point
		count[index] += 1; //increase the counter for this team (number of points)
		
		sum_x[index] += x_p; //calculate the sum of x values of points for this team
		sum_y[index] += y_p; //calculate the sum of y values of points for this team
	}


	Point new_center[M]; //initialize new array with Point elements to store the new centers
	bool change = false; //initialize bool variable to check if centers have changed
	
	CENTERS: for (int j = 0; j < M; j++) {
		ac_math::ac_div(sum_x[j], count[j], new_center[j].x); //calculate x value of new center
		ac_math::ac_div(sum_y[j], count[j], new_center[j].y); //calculate y value of new center
		
		if (new_center[j].x != center[j].x || new_center[j].y != center[j].y) change = true; //check if there is change 
		
		center[j].x = new_center[j].x; //update center array with the calculated values
		center[j].y = new_center[j].y;  
	}
	
	return change; 
}

CCS_MAIN(int argc, char *argv[]) {
	
	srand(30);
	
	const int N = 30; //define number of points
	const int M = 3; //define number of centers
	
	const int lower = 0; //define lower value of random generated points
	const int upper = 1000; //define upper value of random generated points
	
	Point points[N]; //initialize points array 
	Point center[M]; //initialize center array
	int ID[N]; //initialize ID array
	
	
	for (int i = 0; i < N; i++) {
		points[i].x = (rand() % (upper - lower + 1) + lower); //generate random points between lower and upper values 
		points[i].y = (rand() % (upper - lower + 1) + lower);
	}
	
	std::cout << "The initial centers are:" << std::endl;
	
	for (int j = 0; j < M; j++) {
		center[j].x = (rand() % (upper - lower + 1) + lower); //generate random centers between lower and upper values 
		center[j].y = (rand() % (upper - lower + 1) + lower);
		std::cout << center[j].x << ", " << center[j].y << std::endl;
	}
	
	
	bool go_on = true;
	while (go_on) {
	   go_on = kmeans <N, M> (points, ID, center);   
	}
	
	std::cout << "The 3 centers are:" << std::endl;
	std::cout << center[0].x << ", " << center[0].y << std::endl;
	std::cout << center[1].x << ", " << center[1].y << std::endl;
	std::cout << center[2].x << ", " << center[2].y << std::endl;

	CCS_RETURN(0);
}

