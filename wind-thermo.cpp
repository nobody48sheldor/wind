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

int write_data(vector<vector<double>> data, int size, double speed)
{
    ofstream output_file;
    string file_data = "data";
    file_data += to_string((int)round(speed)) + ".txt";
    // cout << "making " << file_data << endl;
    output_file.open(file_data);
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

int write(double time, double speed)
{
    ofstream output_file;
    string file_name = "time/"+to_string((int)round(speed));
    output_file.open(file_name);
    cout << "making " << file_name << endl;
    output_file << time << '/' << speed;
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
			array[i][v] = exp(- (24/(up-low)) * (x[v]*x[v] + y[i]*y[i]));
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

int main(int argc, char *argv[]) {

	// variables
	
	int size, lengh, time, duration, frames, treshold;
	double dx, dy, dt;
	double D, speed, low_bound;
	vector<vector<vector<double>>> T;
	vector<vector<double>> c;
	vector<double> x;
	vector<double> y;
	vector<double> t;

	// get parametters
	
	//default
	
	size = 40, lengh = 5, time = 2000, duration = 4, frames = 100, D = 1, treshold=5, speed = atof(argv[1]), low_bound = 0;
	x = linspace(-lengh, lengh, size), y = linspace(-lengh, lengh, size), t = linspace(0, duration, time);
	c = wind(-lengh, lengh, size, speed, low_bound);
	dx = x[1]-x[0], dy = dx, dt = t[1]-t[0];

	// cout << "r = " << dt/((dx*dx)+(dy*dy) + dy) << endl;

	T.resize(time);
	T[0] = T_initial(-lengh, lengh, size, x, y);
	write_data(T[0], size, speed);
	system("python3 plot-a-frame.py");

	
	// calcualte
	
	for (int t_i = 1; t_i < time; t_i++){
		T[t_i].resize(size);
		T[t_i][0] = T[t_i - 1][0];
		for (int y_i = 1; y_i < (size - 1); y_i++){
			T[t_i][y_i].resize(size);
			T[t_i][y_i][0] = T[t_i - 1][y_i][0];
			for (int x_i = 1; x_i < (size - 1); x_i++){
				if (x_i < size/2 ) {
					T[t_i][y_i][x_i] = conduction(T, dx, dy, dt, D, t_i, y_i, x_i);
				}
				else {
					T[t_i][y_i][x_i] = convection(T, dx, dy, dt, D/4, c, t_i, y_i, x_i);
				}
				// if (T[t_i - 1][(int)round(size/2 - 1)][(int)round(size/2)] < 0.3) {
				if (T[t_i - 1][20][19] < 0.1) {
          cout << "value below 0.1" << endl;
					write(t[t_i], speed);
          system("kill $(pgrep wind)");
				}
			}
			T[t_i][y_i][size - 1] = T[t_i - 1][y_i][size - 1];
		}
		T[t_i][size - 1] = T[t_i - 1][size - 1];
		if (100*t_i%time == 0) {
			cout << 100*t_i/time << " % 	speed = " << speed << endl;
		}
		if (t_i%((int)round(time/frames)) == 0) {

			write_data(T[t_i], size, speed);
			system("python3 plot-a-frame.py");
			// cout << " image " << endl;
		}
		if (t_i > treshold) {
			// free memory
			vector<double> T[t_i - 5];
		}
	}
	return 0;
}
