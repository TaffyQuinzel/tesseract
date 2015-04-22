#include <iostream>
#include <cmath>
#include <unistd.h>
#include <cassert>

using namespace std;

//variables and structs
struct point4d{
	point4d(double _x, double _y, double _z, double _u) {x=_x; y=_y; z=_z; u=_u;}
	point4d();
	double x, y, z, u;
} hypercube[16]={
	//inner cube
	//back
	{-0.5, 0.5, -0.5, 0.5},
	{0.5, 0.5, -0.5, 0.5},
	{-0.5, 0.5, -0.5, -0.5},
	{0.5, 0.5, -0.5, -0.5},
	//front
	{-0.5, -0.5, -0.5, 0.5},
	{0.5, -0.5, -0.5, 0.5},
	{-0.5, -0.5, -0.5, -0.5},
	{0.5, -0.5, -0.5, -0.5},

	//outer cube
	//back
	{-0.5, 0.5, 0.5, 0.5},
	{0.5, 0.5, 0.5, 0.5},
	{-0.5, 0.5, 0.5, -0.5},
	{0.5, 0.5, 0.5, -0.5},
	//front
	{-0.5, -0.5, 0.5, 0.5},
	{0.5, -0.5, 0.5, 0.5},
	{-0.5, -0.5, 0.5, -0.5},
	{0.5, -0.5, 0.5, -0.5}
}, hypercubeOut[16]=hypercube;

int cubeConnections[32][2]={
	//inner cube
	//back
	{0,1}, {0,2},
	{3,1}, {3,2},
	//front
	{4,5}, {4,6},
	{7,5}, {7,6},
	//between
	{0,4}, {1,5},
	{2,6}, {3,7},

	//outer cube
	//back
	{8,9}, {8,10},
	{11,9}, {11,10},
	//front
	{12,13}, {12,14},
	{15,13}, {15,14},
	//between
	{8,12}, {9,13},
	{10,14}, {11,15},

	//between cubes
	//back
	{0,8}, {1,9},
	{2,10}, {3,11},
	//front
	{4,12}, {5,13},
	{6,14}, {7,15},
};

const int screenWidth=92,
		screenHeight=46;
string screendump[screenHeight][screenWidth];


//functions
void clearscreen();
void drawscreen();
void draw_point(double x, double y, string colour);
void draw_line(point4d one, point4d two, string colour);
void draw_cube(point4d cube[16]);
//rotation functions
void rotateCube(double xyAngle, double yzAngle, double zxAngle, double xuAngle, double yuAngle, double zuAngle);
point4d rotatePoint(point4d point, double xyAngle, double yzAngle, double zxAngle, double xuAngle, double yuAngle, double zuAngle);
point4d matrixMultiplicationAngle(point4d point, double matrix[4][4]);



int main(int argc, char *argv[])
{
	clearscreen();

	double counter=0.0;
	while(true)
	{
		rotateCube(0.5, 0.5, counter, counter, counter, counter);
		draw_cube(hypercubeOut);
		counter+=0.1;
		drawscreen();
		usleep(100000);
	}
	return 0;
}

void clearscreen()
{
	for(int i=0;i<screenHeight;i++)
		for(int j=0;j<screenWidth;j++)
			screendump[i][j]=" ";
}
void drawscreen()
{
	cout<<"\n\n\n";
	for(int i=0;i<screenHeight;i++)
	{
		for(int j=0;j<screenWidth;j++)
			cout<<screendump[i][j];
		cout<<"\n";
	}
	clearscreen();
}
void draw_point( double x, double y, string colour)
{
	if(y<0)y=0;
	if(y>45)y=45;
	int _x=x;
	int _y=y;
	screendump[_y][_x]=colour+"*\033[0m";
}
void draw_line(point4d one, point4d two, string colour)
{
	one.x=screenWidth/2+screenWidth/2*one.x;
	one.y=screenHeight/2+screenHeight/2*one.y;
	two.x=screenWidth/2+screenWidth/2*two.x;
	two.y=screenHeight/2+screenHeight/2*two.y;

	//check if the angle of the line is larger than 45 degrees
	const bool steep = fabs(two.y - one.y) > fabs(two.x - one.x);

	if(steep)
	{
		swap(one.x, one.y);
		swap(two.x, two.y);
	}

	if(one.x > two.x)
	{
		swap(one.x, two.x);
		swap(one.y, two.y);
	}

	const double dx = two.x - one.x;
	const double dy = fabs(two.y - one.y);

	double error = dx / 2.0;
	const int ystep = (one.y < two.y) ? 1 : -1;
	int y = (int)one.y;

	const int maxX = (int)two.x;
	for(int x=(int)one.x; x<maxX; x++)
	{
		if(steep) 
			draw_point(y,x,colour);
		else	 
			draw_point(x,y,colour);

		error -= dy;
		if(error < 0)
		{
			y += ystep;
			error += dx;
		}
	}
}
void draw_cube(point4d cubeToPrint[16])
{
	string colour="";
	for(int i=0;i<32;i++)
	{
		if(i<11) colour="\033[31m";
		if(i>11 && i<23) colour="\033[35m";
		if(i>23) colour="\033[32m";
		point4d tmpPoint[2]={
			{cubeToPrint[cubeConnections[i][0]].x, cubeToPrint[cubeConnections[i][0]].y, 0.0, 0.0},
			{cubeToPrint[cubeConnections[i][1]].x, cubeToPrint[cubeConnections[i][1]].y, 0.0, 0.0}
		};
		draw_line(tmpPoint[0], tmpPoint[1], colour);
	}
}

void rotateCube(double xyAngle, double yzAngle, double zxAngle, double xuAngle, double yuAngle, double zuAngle)
{
	for(int i=0;i<16;i++)
		hypercubeOut[i]=rotatePoint(hypercube[i], xyAngle, yzAngle, zxAngle, xuAngle, yuAngle, zuAngle);
}

point4d rotatePoint(point4d point, double xyAngle, double yzAngle, double zxAngle, double xuAngle, double yuAngle, double zuAngle)
{
	point4d pointOut=point;
	double xyMatrix[4][4]={
		{cos(xyAngle),sin(xyAngle),0.0,0.0},
		{-sin(xyAngle),cos(xyAngle),0.0,0.0},
		{0.0,0.0,1.0,0.0},
		{0.0,0.0,0.0,1.0}
	},	yzMatrix[4][4]={
		{1.0,0.0,0.0,0.0},
		{0.0,cos(yzAngle),sin(yzAngle),0.0},
		{0.0,-sin(yzAngle),cos(yzAngle),0.0},
		{0.0,0.0,0.0,1.0}
	}, zxMatrix[4][4]={
		{cos(zxAngle),0.0,-sin(zxAngle),0.0},
		{0.0,1.0,0.0,0.0},
		{sin(zxAngle),0.0,cos(zxAngle),0.0},
		{0.0,0.0,0.0,1.0}
	}, xuMatrix[4][4]={
		{cos(xuAngle),0.0,0.0,sin(xuAngle)},
		{0.0,1.0,0.0,0.0},
		{0.0,0.0,1.0,0.0},
		{-sin(xuAngle),0.0,0.0,cos(xuAngle)}
	},	yuMatrix[4][4]={
		{1.0,0.0,0.0,0.0},
		{0.0,cos(yuAngle),0.0,-sin(yuAngle)},
		{0.0,0.0,1.0,0.0},
		{0.0,sin(yuAngle),0.0,cos(yuAngle)}
	}, zuMatrix[4][4]={
		{1.0,0.0,0.0,0.0},
		{0.0,1.0,0.0,0.0},
		{0.0,0.0,cos(zuAngle),-sin(zuAngle)},
		{0.0,0.0,sin(zuAngle),cos(zuAngle)}
	};	
	pointOut = matrixMultiplicationAngle(pointOut,xyMatrix);
	pointOut = matrixMultiplicationAngle(pointOut,yzMatrix);
	pointOut = matrixMultiplicationAngle(pointOut,zxMatrix);
	pointOut = matrixMultiplicationAngle(pointOut,xuMatrix);
	pointOut = matrixMultiplicationAngle(pointOut,yuMatrix);
	pointOut = matrixMultiplicationAngle(pointOut,zuMatrix);

	return pointOut;
}

point4d matrixMultiplicationAngle(point4d point, double matrix[4][4])
{
	double inMatrix[1][4]={ {point.x, point.y, point.z, point.u} };
	double outMatrix[1][4]={ {0,0,0,0} };

	for(int i=0;i<1;i++)
		for(int j=0;j<4;j++)
			for(int k=0;k<4;k++)
				outMatrix[i][j]+=inMatrix[i][k]*matrix[k][j];

	point4d out={outMatrix[0][0], outMatrix[0][1], outMatrix[0][2], outMatrix[0][3]};
	return out;
}
