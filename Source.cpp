#include <iostream>
#include <fstream>
#include "windows.h"

using namespace std;

#define IMAGE_SIZE 960
//used to be 256
//change image size to 1024 and dynamically allocate array or make array static to prevent stack overflow

int main(int argc, char* argv[])
{
	ifstream f("L2Data10K.dat", ios::binary);

	//get size of file and number of elements
	f.seekg(0, ios::end);
	long long size = f.tellg();
	size_t els = static_cast<unsigned>(size / sizeof(float));
	f.seekg(0);
	//cout << size << endl;
	//cout << els << endl;

	//create a dynamic alloc array of floats.
	float * arrayOfFloats = nullptr;
	try {
		arrayOfFloats = new float[els];
	}
	catch (const bad_alloc &) {
		cout << "alloc fail" << endl;
	}

	//place floats in dynamic alloc array
	f.read(reinterpret_cast<char *>(arrayOfFloats), size); // # bytes

	//print to console to check if array has coordinates
	//for (unsigned i = 0; i < els; i++)
	//	cout << "  " << arrayOfFloats[i];
	//cout << endl;
	
	BITMAPFILEHEADER bmfh;

	BITMAPINFOHEADER bmih;

	char colorTable[1024];
	// The following defines the array which holds the image.  
	// consider Dynamic allocations or static for below
	char bits[IMAGE_SIZE][IMAGE_SIZE] = { 0 };

	int i, j, k, l;

	// Define and open the output file. 
	ofstream bmpOut("manhat.bmp", ios::out + ios::binary);
	if (!bmpOut) {
		cout << "...could not open file, ending.";
		return -1;
	}

	// Initialize the bit map file header with static values.
	bmfh.bfType = 0x4d42;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(bmfh) + sizeof(bmih) + sizeof(colorTable);
	bmfh.bfSize = bmfh.bfOffBits + sizeof(bits);

	// Initialize the bit map information header with static values.
	bmih.biSize = 40;
	bmih.biWidth = IMAGE_SIZE;
	bmih.biHeight = IMAGE_SIZE;
	bmih.biPlanes = 1;
	bmih.biBitCount = 8;
	bmih.biCompression = 0;
	bmih.biSizeImage = IMAGE_SIZE * IMAGE_SIZE;
	bmih.biXPelsPerMeter = 2835;  // magic number, see Wikipedia entry
	bmih.biYPelsPerMeter = 2835;
	bmih.biClrUsed = 256;
	bmih.biClrImportant = 0;

	// Build color table.
	for (i = 0; i < 256; i++) {
		j = i * 4;
		colorTable[j] = colorTable[j + 1] = colorTable[j + 2] = colorTable[j + 3] = i;
		// for your edification, try setting one of the first three values to 255 instead of i
		// and see what it does to the generated bit map.
	}

	// Build gray scale array of bits in image, 
	/*for (i = 0; i < IMAGE_SIZE; i++) {
		for (j = 0; j < IMAGE_SIZE; j++) {
			//bits[i][j] = j; // displaying black to white left to right.
			//bits[i][j] = 255 - ((i + j) / 2); // diagonal fade white to black lower left to upper right
		}
		k = i;
		l = j;
	}*/

	//go through array and change bmp accordingly
	for (unsigned i = 0; i < els; i+=2)
	{
		int latitude = 0;
		int longitude = 0;

		//check latitude
		if (arrayOfFloats[i] > 40.830509 || arrayOfFloats[i] < 40.700455)
		{
			//cout << "out of bounds " << arrayOfFloats[i] << endl;
		}
		else
		{
			//find latitude
			latitude = int(((arrayOfFloats[i] - 40.700455) / 0.130054) * IMAGE_SIZE);
			//cout << arrayOfFloats[i] << " lat is " << latitude << endl;
		}

		//check longitude
		if (arrayOfFloats[i+1] > -73.914979 || arrayOfFloats[i + 1] < -74.045033)
		{
			//cout << "out of bounds " << arrayOfFloats[i + 1] << endl;
		}
		else
		{
			//find longitude
			longitude = int(((arrayOfFloats[i + 1] - -74.045033) / 0.130054) * IMAGE_SIZE);
			//cout << arrayOfFloats[i] << " long is " << latitude << endl;
		}

		bits[latitude][longitude] = 255;
		//cout << arrayOfFloats[i] << " lat is " << latitude << endl;
	}


	// Write out the bit map.  
	char* workPtr;
	workPtr = (char*)&bmfh;
	bmpOut.write(workPtr, 14);
	workPtr = (char*)&bmih;
	bmpOut.write(workPtr, 40);
	workPtr = &colorTable[0];
	bmpOut.write(workPtr, sizeof(colorTable));
	workPtr = &bits[0][0];
	bmpOut.write(workPtr, IMAGE_SIZE*IMAGE_SIZE);
	bmpOut.close();

	// Now let's look at our creation.
	system("mspaint manhat.bmp");

	return 0;
}
