#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

// general functions

vector<double> linspace(double low, double up, int size) {
	vector<double> array;
	array.resize(size);
	for (int i = 0; i < size; i++) {
		array[i] = low + (i*(up-low)/size);
	}
	return array;
}

int write_data(vector<vector<double>> data, int time, int size)
{
    ofstream output_file;
    output_file.open("data.txt");
    for (int ypos=0; ypos < size; ypos++)
    {
        for (int xpos=0; xpos < size; xpos++)
        {
            output_file << data[ypos][xpos] << '/';
        }
    }
    output_file.close();
    return 0;
}

vector<vector<double>> wind(double low, double up, int size, double speed, double low_bound) {
	vector<vector<double>> wind;
	wind.resize(size);
	for (int u = 0; u < size; u++){
		wind[u].resize(size);
		for (int v = 0; v < size; v++) {
			if ( v < ((up-low)/(low_bound-low))*size) {
				wind[u][v] = speed;
			}
			else {
				wind[u][v] = 0;
			}
		}
	}
	return wind;
}

// intial condition

vector<vector<double>> T_initial(double low, double up, int size, vector<double> x, vector<double> y) {
	vector<vector<double>> array;
	array.resize(size);
	for (int i = 0; i < size; i++) {
		array[i].resize(size);
		for (int v = 0; v < size; v++) {
			array[i][v] = exp(- (20/(up-low)) * (x[v]*x[v] + y[i]*y[i]));
		}
	}
	return array;
}

// next

double convection(vector<vector<vector<double>>> T, double dx, double dy, double dt, double D, vector<vector<double>> c, int tval, int yval, int xval){
	double conv = T[tval - 1][yval][xval] + dt*( D*( ((T[tval - 1][yval][xval + 1] - 2*T[tval - 1][yval][xval] + T[tval - 1][yval][xval - 1])/(dx*dx)) + ((T[tval - 1][yval + 1][xval] - 2*T[tval - 1][yval][xval] + T[tval - 1][yval - 1][xval])/(dy*dy)) ) - (c[xval][yval])*( (T[tval - 1][yval + 1][xval] - T[tval - 1][yval - 1][xval])/(2*dy) ) );
	return conv;
}

double conduction(vector<vector<vector<double>>> T, double dx, double dy, double dt, double D, int tval, int yval, int xval){
	double cond = T[tval - 1][yval][xval] + dt*( D*( ((T[tval - 1][yval][xval + 1] - 2*T[tval - 1][yval][xval] + T[tval - 1][yval][xval - 1])/(dx*dx)) + ((T[tval - 1][yval + 1][xval] - 2*T[tval - 1][yval][xval] + T[tval - 1][yval - 1][xval])/(dy*dy)) ) );
	return cond;
}

// main

int main() {

	// variables
	
	int size, lengh, time, duration, frames;
	double dx, dy, dt;
	double D, speed, low_bound;
	vector<vector<vector<double>>> T;
	vector<vector<double>> c;
	vector<double> x;
	vector<double> y;
	vector<double> t;

	// get parametters
	
	//default
	
	size = 100, lengh = 2, time = 10000, duration = 1, frames = 100, D = 1, speed = 20, low_bound = 0;
	x = linspace(-lengh, lengh, size), y = linspace(-lengh, lengh, size), t = linspace(0, duration, time);
	c = wind(-lengh, lengh, size, speed, low_bound);
	dx = x[1]-x[0], dy = dx, dt = t[1]-t[0];

	cout << "r = " << dt/((dx*dx)+(dy*dy) + dy) << endl;

	T.resize(time);
	T[0] = T_initial(-lengh, lengh, size, x, y);
	write_data(T[0], time, size);
	system("python3 plot-a-frame.py");

	
	// calcualte
	
	for (int t_i = 1; t_i < time; t_i++){
		cout << t_i << endl;
		T[t_i].resize(size);
		T[t_i][0] = T[t_i - 1][0];
		for (int y_i = 1; y_i < (size - 1); y_i++){
			T[t_i][y_i].resize(size);
			T[t_i][y_i][0] = T[t_i - 1][y_i][0];
			for (int x_i = 1; x_i < (size - 1); x_i++){
				if (x_i < ((2*lengh/(low_bound+lengh))*size) ) {
					T[t_i][y_i][x_i] = conduction(T, dx, dy, dt, D, t_i, y_i, x_i);
				}
				else {
					T[t_i][y_i][x_i] = convection(T, dx, dy, dt, D/10, c, t_i, y_i, x_i);
				}
			}
			T[t_i][y_i][size - 1] = T[t_i - 1][y_i][size - 1];
		}
		T[t_i][size - 1] = T[t_i - 1][size - 1];
		if (100*t_i%time == 0) {
			cout << 100*t_i/time << " %";
		}
		if (t_i%((int)round(time/frames)) == 0) {

			write_data(T[t_i], time, size);
			system("python3 plot-a-frame.py");
			cout << " image " << endl;
		}
		if (t_i > 5) {
			// free memory
			vector<double> T[t_i - 5];
		}
	}
	return 0;
}
