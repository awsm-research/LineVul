static inline ogg_uint32_t decode_packed_entry_number(codebook *book,
						      oggpack_buffer *b){
ogg_uint32_t chase=0;
int  read=book->dec_maxlength;
long lok = oggpack_look(b,read),i;

while(lok<0 && read>1)
lok = oggpack_look(b, --read);

if(lok<0){
oggpack_adv(b,1); /* force eop */
return -1;
}

/* chase the tree with the bits we got */
switch (book->dec_method)
{
case 0:
{
/* book->dec_nodeb==1, book->dec_leafw==1 */
/* 8/8 - Used */

unsigned char *t=(unsigned char *)book->dec_table;

for(i=0;i<read;i++){
	chase=t[chase*2+((lok>>i)&1)];
	if(chase&0x80UL)break;
}
chase&=0x7fUL;
break;
}
case 1:
{
/* book->dec_nodeb==1, book->dec_leafw!=1 */

/* 8/16 - Used by infile2 */
unsigned char *t=(unsigned char *)book->dec_table;
for(i=0;i<read;i++){
	int bit=(lok>>i)&1;
	int next=t[chase+bit];
	if(next&0x80){
	  chase= (next<<8) | t[chase+bit+1+(!bit || t[chase]&0x80)];
	  break;
	}
	chase=next;
}
//chase&=0x7fffUL;
chase&=~0x8000UL;
break;
}
case 2:
{

/* book->dec_nodeb==2, book->dec_leafw==1 */
/* 16/16 - Used */
for(i=0;i<read;i++){
	chase=((ogg_uint16_t *)(book->dec_table))[chase*2+((lok>>i)&1)];
	if(chase&0x8000UL)break;
}
//chase&=0x7fffUL;
chase&=~0x8000UL;
break;
}
case 3:
{
/* book->dec_nodeb==2, book->dec_leafw!=1 */

/* 16/32 - Used by infile2 */
ogg_uint16_t *t=(ogg_uint16_t *)book->dec_table;
for(i=0;i<read;i++){
	int bit=(lok>>i)&1;
	int next=t[chase+bit];
	if(next&0x8000){
	  chase= (next<<16) | t[chase+bit+1+(!bit || t[chase]&0x8000)];
	  break;
	}
	chase=next;
}
//chase&=0x7fffffffUL;
chase&=~0x80000000UL;
break;
}
case 4:

{
//Output("32/32");
for(i=0;i<read;i++){
	chase=((ogg_uint32_t *)(book->dec_table))[chase*2+((lok>>i)&1)];
	if(chase&0x80000000UL)break;
}
//chase&=0x7fffffffUL;
chase&=~0x80000000UL;
break;
}
}

if(i<read){
oggpack_adv(b,i+1);
return chase;
}
oggpack_adv(b,read+1);
return(-1);
}
