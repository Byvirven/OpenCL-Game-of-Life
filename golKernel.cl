int west(int position, int width) {
	if (position-1 < 0 or (position-1!=0 and
		(position-1)/width < position/width)) {
		return position-1+width;
	}
	return position-1;
}

int east(int position, int width, int size) {
	if (position+1 >= size or 
		(position+1)/width > position/width) {
		return position+1-width;
	}
	return position+1;
}

int north(int position, int width, int size) {
	if (position-width < 0) {
		return position-width+size;
	}
	return position-width;
}

int south(int position, int width, int size) {
	if (position+width >= size) {
		return (position+width)%size;
	}
	return position+width;
}

int southeast(int position, int width, int size) {
	if (position+width+1 >= size) {
		return (position+width+1)%size;
	}
	return position+width+1;
}
int southwest(int position, int width, int size) {
	if (position+width-1 >= size) {
		return (position+width-1)%size;
	}
	return position+width-1;
}
int northeast(int position, int width, int size) {
	if (position-width+1 < 0  or 
		(position-width+1)/width > (position-width)/width) {
		return position-width+1+size;
	}
	return position-width+1;
}
int northwest(int position, int width, int size) {
	if (position-width-1 < 0 or (position-width-1!=0 and
		(position-width-1)/width < (position-width)/width)) {
		return position-width-1+size;
	}
	return position-width-1;
}

__kernel void TephraTransport(__global const uchar *img, 
					__global const int *width, 
					__global const int *size, 
					__global uchar *imgtmp) {

	int i = get_global_id(0); // identifiant global
	int position = i*3;
	int alive = img[west(i, width[0])*3] + 
				img[east(i,width[0],size[0])*3] + 
				img[north(i,width[0],size[0])*3] + 
				img[south(i,width[0],size[0])*3] + 
				img[northeast(i,width[0],size[0])*3] + 
				img[northwest(i,width[0],size[0])*3] + 
				img[southeast(i,width[0],size[0])*3] + 
				img[southwest(i,width[0],size[0])*3];
	
	switch (img[position]) {
		case 0xFE : //#0000FE  --> bleu --> vivant
			if (alive == 2038 or alive == 2037) {
				imgtmp[position] = 0xFE;
				imgtmp[position+1] = 0;
				imgtmp[position+2] = 0;
			} else {
				imgtmp[position] = 0xFF;
				imgtmp[position+1] = 0xFF;
				imgtmp[position+2] = 0xFF;
			}
			break;
		default: //#FFFFFF --> blanc --> néant
			if (alive == 2037) {
				imgtmp[position] = 0xFE;
				imgtmp[position+1] = 0;
				imgtmp[position+2] = 0;
			} else {
				imgtmp[position] = 0xFF;
				imgtmp[position+1] = 0xFF;
				imgtmp[position+2] = 0xFF;
			}
	}
}
