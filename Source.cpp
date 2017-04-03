#include <iostream>
#include <fstream>
#include "windows.h"

using namespace std;

#define IMAGE_SIZE 1024
#define SouthernBound 40.700455
#define NorthernBound 40.830509
#define EasternBound -73.914979
#define WesternBound -74.045033
#define Map_SIZE 0.130054

int main(int argc, char* argv[])
{
	const float myIMAGE_SIZE = IMAGE_SIZE;
	const float mySouthernBound = SouthernBound;
	const float myNorthernBound = NorthernBound;
	const float myEasternBound = EasternBound;
	const float myWesternBound = WesternBound;
	const float myMap_SIZE = Map_SIZE;

	ifstream f("L2Data10K.dat", ios::binary);

	//get size of file and number of elements
	f.seekg(0, ios::end);
	long long size = f.tellg();
	size_t els = static_cast<unsigned>(size / sizeof(float));
	f.seekg(0);

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

	BITMAPFILEHEADER bmfh;

	BITMAPINFOHEADER bmih;

	char colorTable[1024];
	// The following defines the array which holds the image.  

	// static in order to prevent stack overflow
	static char bits[IMAGE_SIZE][IMAGE_SIZE] = { 0 };

	int i, j;

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
	}

	//go through array and change bmp accordingly
	for (unsigned i = 0; i < els; i += 2)
	{
		int latitude = 0;
		float flatit = 0;
		int longitude = 0;
		float flongit = 0;

		//check latitude
		if (arrayOfFloats[i] > NorthernBound || arrayOfFloats[i] < SouthernBound)
		{
			//cout << i << "is i and this is out of bounds " << arrayOfFloats[i] << endl;
		}
		else
		{
			//find latitude
			//latitude = int(((arrayOfFloats[i] - SouthernBound) / Map_SIZE) * IMAGE_SIZE);
			flatit = arrayOfFloats[i];
			__asm
			{
				FLD		flatit
				FSUB	mySouthernBound
				FDIV	myMap_SIZE
				FMUL	myIMAGE_SIZE
				FSTP	flatit
			}
			latitude = int(flatit);
		}

		//check longitude
		if (arrayOfFloats[i + 1] >  EasternBound || arrayOfFloats[i + 1] < WesternBound)
		{
			//cout << "out of bounds " << arrayOfFloats[i + 1] << endl;
		}
		else
		{
			//find longitude
			//longitude = int(((arrayOfFloats[i + 1] - EasternBound) / Map_SIZE) * IMAGE_SIZE);
			flongit = arrayOfFloats[i + 1];
			__asm
			{
				FLD		flongit
				FSUB	myWesternBound
				FDIV	myMap_SIZE
				FMUL	myIMAGE_SIZE
				FSTP	flongit
			}
			longitude = int(flongit);
		}
		//place white pixel at coordinate
		if (latitude > 0 && longitude > 0 && latitude < 1024 && latitude < 1024)
		{
			bits[latitude][longitude] = 255;
		}
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
