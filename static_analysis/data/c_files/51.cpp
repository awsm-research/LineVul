static int jpeg_size(unsigned char* data, unsigned int data_size,
int *width, int *height)
{
int i = 0;
if (i + 3 < data_size && data[i] == 0xFF && data[i+1] == 0xD8 &&
data[i+2] == 0xFF && data[i+3] == 0xE0) {
i += 4;
if(i + 6 < data_size &&
data[i+2] == 'J' && data[i+3] == 'F' && data[i+4] == 'I' &&
data[i+5] == 'F' && data[i+6] == 0x00) {
unsigned short block_length = data[i] * 256 + data[i+1];
while(i<data_size) {
i+=block_length;
if((i + 1) >= data_size)
return -1;
if(data[i] != 0xFF)
return -1;
if(data[i+1] == 0xC0) {
*height = data[i+5]*256 + data[i+6];
*width = data[i+7]*256 + data[i+8];
return 0;
}
i+=2;
                block_length = data[i] * 256 + data[i+1];
}
}
}

return -1;
}
