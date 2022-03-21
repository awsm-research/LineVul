IMPEG2D_ERROR_CODES_T impeg2d_vld_decode(
dec_state_t *ps_dec,
WORD16      *pi2_outAddr, /*!< Address where decoded symbols will be stored */
const UWORD8 *pu1_scan, /*!< Scan table to be used */
UWORD8      *pu1_pos, /*!< Scan table to be used */
UWORD16     u2_intra_flag, /*!< Intra Macroblock or not */
UWORD16     u2_chroma_flag, /*!< Chroma Block or not */
UWORD16     u2_d_picture, /*!< D Picture or not */
UWORD16     u2_intra_vlc_format, /*!< Intra VLC format */
UWORD16     u2_mpeg2, /*!< MPEG-2 or not */
WORD32      *pi4_num_coeffs /*!< Returns the number of coeffs in block */
)
{

UWORD32 u4_sym_len;

UWORD32 u4_decoded_value;
UWORD32 u4_level_first_byte;
WORD32  u4_level;
UWORD32 u4_run, u4_numCoeffs;
UWORD32 u4_buf;
UWORD32 u4_buf_nxt;
UWORD32 u4_offset;
UWORD32 *pu4_buf_aligned;
UWORD32 u4_bits;
stream_t *ps_stream = &ps_dec->s_bit_stream;
WORD32  u4_pos;
UWORD32 u4_nz_cols;
UWORD32 u4_nz_rows;

*pi4_num_coeffs = 0;

ps_dec->u4_non_zero_cols = 0;
ps_dec->u4_non_zero_rows = 0;
u4_nz_cols = ps_dec->u4_non_zero_cols;
u4_nz_rows = ps_dec->u4_non_zero_rows;

GET_TEMP_STREAM_DATA(u4_buf,u4_buf_nxt,u4_offset,pu4_buf_aligned,ps_stream)
/**************************************************************************/
/* Decode the DC coefficient in case of Intra block                       */
/**************************************************************************/
if(u2_intra_flag)
{
WORD32 dc_size;
WORD32 dc_diff;
WORD32 maxLen;
WORD32 idx;


maxLen = MPEG2_DCT_DC_SIZE_LEN;
idx = 0;
if(u2_chroma_flag != 0)
{
maxLen += 1;
idx++;
}


{
WORD16  end = 0;
UWORD32 maxLen_tmp = maxLen;
UWORD16 m_iBit;


/* Get the maximum number of bits needed to decode a symbol */
IBITS_NXT(u4_buf,u4_buf_nxt,u4_offset,u4_bits,maxLen)
do
{
maxLen_tmp--;
/* Read one bit at a time from the variable to decode the huffman code */
m_iBit = (UWORD8)((u4_bits >> maxLen_tmp) & 0x1);

/* Get the next node pointer or the symbol from the tree */
end = gai2_impeg2d_dct_dc_size[idx][end][m_iBit];
}while(end > 0);
dc_size = end + MPEG2_DCT_DC_SIZE_OFFSET;

/* Flush the appropriate number of bits from the stream */
FLUSH_BITS(u4_offset,u4_buf,u4_buf_nxt,(maxLen - maxLen_tmp),pu4_buf_aligned)

}



if (dc_size != 0)
{
UWORD32 u4_bits;

IBITS_GET(u4_buf,u4_buf_nxt,u4_offset,u4_bits,pu4_buf_aligned, dc_size)
dc_diff = u4_bits;

if ((dc_diff & (1 << (dc_size - 1))) == 0) //v Probably the prediction algo?
dc_diff -= (1 << dc_size) - 1;
}
else
{
dc_diff = 0;
}


pi2_outAddr[*pi4_num_coeffs] = dc_diff;
/* This indicates the position of the coefficient. Since this is the DC
* coefficient, we put the position as 0.
*/
pu1_pos[*pi4_num_coeffs] = pu1_scan[0];
(*pi4_num_coeffs)++;

if (0 != dc_diff)
{
u4_nz_cols |= 0x01;
u4_nz_rows |= 0x01;
}

u4_numCoeffs = 1;
}
/**************************************************************************/
/* Decoding of first AC coefficient in case of non Intra block            */
/**************************************************************************/
else
{
/* First symbol can be 1s */
UWORD32 u4_bits;

IBITS_NXT(u4_buf,u4_buf_nxt,u4_offset,u4_bits,1)

if(u4_bits == 1)
{

FLUSH_BITS(u4_offset,u4_buf,u4_buf_nxt,1, pu4_buf_aligned)
IBITS_GET(u4_buf,u4_buf_nxt,u4_offset,u4_bits,pu4_buf_aligned, 1)
if(u4_bits == 1)
{
pi2_outAddr[*pi4_num_coeffs] = -1;
}
else
{
pi2_outAddr[*pi4_num_coeffs] = 1;
}

/* This indicates the position of the coefficient. Since this is the DC
* coefficient, we put the position as 0.
*/
pu1_pos[*pi4_num_coeffs] = pu1_scan[0];
(*pi4_num_coeffs)++;
u4_numCoeffs = 1;

u4_nz_cols |= 0x01;
u4_nz_rows |= 0x01;
}
else
{
u4_numCoeffs = 0;
}
}
if (1 == u2_d_picture)
{
PUT_TEMP_STREAM_DATA(u4_buf, u4_buf_nxt, u4_offset, pu4_buf_aligned, ps_stream)
ps_dec->u4_non_zero_cols  = u4_nz_cols;
ps_dec->u4_non_zero_rows  = u4_nz_rows;
return ((IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE);
}



if (1 == u2_intra_vlc_format && u2_intra_flag)
{

while(1)
{
//Putting the impeg2d_dec_ac_coeff_one function inline.

UWORD32 lead_zeros;
WORD16 DecodedValue;

u4_sym_len = 17;
IBITS_NXT(u4_buf,u4_buf_nxt,u4_offset,u4_bits,u4_sym_len)

DecodedValue = gau2_impeg2d_tab_one_1_9[u4_bits >> 8];
u4_sym_len = (DecodedValue & 0xf);
u4_level = DecodedValue >> 9;
/* One table lookup */
if(0 != u4_level)
{
u4_run = ((DecodedValue >> 4) & 0x1f);
u4_numCoeffs       += u4_run;
u4_pos             = pu1_scan[u4_numCoeffs++ & 63];
pu1_pos[*pi4_num_coeffs] = u4_pos;

FLUSH_BITS(u4_offset,u4_buf,u4_buf_nxt,u4_sym_len,pu4_buf_aligned)
pi2_outAddr[*pi4_num_coeffs] = u4_level;

(*pi4_num_coeffs)++;
}
else
{
if (DecodedValue == END_OF_BLOCK_ONE)
{
u4_sym_len = 4;

break;
}
else
{
/*Second table lookup*/
lead_zeros = CLZ(u4_bits) - 20;/* -16 since we are dealing with WORD32 */
if (0 != lead_zeros)
{

u4_bits         = (u4_bits >> (6 - lead_zeros)) & 0x001F;

/* Flush the number of bits */
if (1 == lead_zeros)
{
u4_sym_len         = ((u4_bits & 0x18) >> 3) == 2 ? 11:10;
}
else
{
u4_sym_len         = 11 + lead_zeros;
}
/* flushing */
FLUSH_BITS(u4_offset,u4_buf,u4_buf_nxt,u4_sym_len,pu4_buf_aligned)

/* Calculate the address */
u4_bits         = ((lead_zeros - 1) << 5) + u4_bits;

DecodedValue = gau2_impeg2d_tab_one_10_16[u4_bits];

u4_run = BITS(DecodedValue, 8,4);
u4_level = ((WORD16) DecodedValue) >> 9;

u4_numCoeffs       += u4_run;
u4_pos             = pu1_scan[u4_numCoeffs++ & 63];
pu1_pos[*pi4_num_coeffs] = u4_pos;
pi2_outAddr[*pi4_num_coeffs] = u4_level;
(*pi4_num_coeffs)++;
}
/*********************************************************************/
/* MPEG2 Escape Code                                                 */
/*********************************************************************/
else if(u2_mpeg2 == 1)
{
u4_sym_len         = 6;
FLUSH_BITS(u4_offset,u4_buf,u4_buf_nxt,u4_sym_len,pu4_buf_aligned)
IBITS_GET(u4_buf,u4_buf_nxt,u4_offset,u4_bits,pu4_buf_aligned,18)
u4_decoded_value    = u4_bits;
u4_run             = (u4_decoded_value >> 12);
u4_level           = (u4_decoded_value & 0x0FFF);

if (u4_level)
u4_level = (u4_level - ((u4_level & 0x0800) << 1));

u4_numCoeffs       += u4_run;
u4_pos             = pu1_scan[u4_numCoeffs++ & 63];
pu1_pos[*pi4_num_coeffs] = u4_pos;
pi2_outAddr[*pi4_num_coeffs] = u4_level;
(*pi4_num_coeffs)++;
}
/*********************************************************************/
/* MPEG1 Escape Code                                                 */
/*********************************************************************/
else
{
/*-----------------------------------------------------------
* MPEG-1 Stream
*
* <See D.9.3 of MPEG-2> Run-level escape syntax
* Run-level values that cannot be coded with a VLC are coded
* by the escape code '0000 01' followed by
* either a 14-bit FLC (127 <= level <= 127),
* or a 22-bit FLC (255 <= level <= 255).
* This is described in Annex B,B.5f of MPEG-1.standard
*-----------------------------------------------------------*/

/*-----------------------------------------------------------
* First 6 bits are the value of the Run. Next is First 8 bits
* of Level. These bits decide whether it is 14 bit FLC or
* 22-bit FLC.
*
* If( first 8 bits of Level == '1000000' or '00000000')
*      then its is 22-bit FLC.
* else
*      it is 14-bit FLC.
*-----------------------------------------------------------*/
u4_sym_len         = 6;
FLUSH_BITS(u4_offset,u4_buf,u4_buf_nxt,u4_sym_len,pu4_buf_aligned)
IBITS_GET(u4_buf,u4_buf_nxt,u4_offset,u4_bits,pu4_buf_aligned,14)
u4_decoded_value     = u4_bits;
u4_run              = (u4_decoded_value >> 8);
u4_level_first_byte = (u4_decoded_value & 0x0FF);
if(u4_level_first_byte & 0x7F)
{
/*-------------------------------------------------------
* First 8 bits of level are neither 1000000 nor 00000000
* Hence 14-bit FLC (Last 8 bits are used to get level)
*
*  Level = (msb of Level_First_Byte is 1)?
*          Level_First_Byte - 256 : Level_First_Byte
*-------------------------------------------------------*/
u4_level = (u4_level_first_byte -
((u4_level_first_byte & 0x80) << 1));
}
else
{
/*-------------------------------------------------------
* Next 8 bits are either 1000000 or 00000000
* Hence 22-bit FLC (Last 16 bits are used to get level)
*
*  Level = (msb of Level_First_Byte is 1)?
*          Level_Second_Byte - 256 : Level_Second_Byte
*-------------------------------------------------------*/
IBITS_GET(u4_buf,u4_buf_nxt,u4_offset,u4_bits,pu4_buf_aligned,8)
u4_level = u4_bits;
u4_level = (u4_level - (u4_level_first_byte << 1));
}
u4_numCoeffs += u4_run;

u4_pos = pu1_scan[u4_numCoeffs++ & 63];

pu1_pos[*pi4_num_coeffs] = u4_pos;
pi2_outAddr[*pi4_num_coeffs] = u4_level;
(*pi4_num_coeffs)++;
}
}
}


u4_nz_cols |= 1 << (u4_pos & 0x7);
u4_nz_rows |= 1 << (u4_pos >> 0x3);


}
IBITS_GET(u4_buf,u4_buf_nxt,u4_offset,u4_bits,pu4_buf_aligned,u4_sym_len)
            if (u4_numCoeffs > 64)
            {
                return IMPEG2D_MB_TEX_DECODE_ERR;
            }
}
else
{
// Inline
while(1)
{

UWORD32 lead_zeros;
UWORD16 DecodedValue;

u4_sym_len = 17;
IBITS_NXT(u4_buf, u4_buf_nxt, u4_offset, u4_bits, u4_sym_len)


DecodedValue = gau2_impeg2d_tab_zero_1_9[u4_bits >> 8];
u4_sym_len = BITS(DecodedValue, 3, 0);
u4_level = ((WORD16) DecodedValue) >> 9;

if (0 != u4_level)
{
u4_run = BITS(DecodedValue, 8,4);

u4_numCoeffs       += u4_run;

u4_pos                 = pu1_scan[u4_numCoeffs++ & 63];
pu1_pos[*pi4_num_coeffs] = u4_pos;

FLUSH_BITS(u4_offset,u4_buf,u4_buf_nxt,u4_sym_len,pu4_buf_aligned)
pi2_outAddr[*pi4_num_coeffs] = u4_level;
(*pi4_num_coeffs)++;
}
else
{
if(DecodedValue == END_OF_BLOCK_ZERO)
{
u4_sym_len = 2;

break;
}
else
{
lead_zeros = CLZ(u4_bits) - 20;/* -15 since we are dealing with WORD32 */
/*Second table lookup*/
if (0 != lead_zeros)
{
u4_bits         = (u4_bits >> (6 - lead_zeros)) & 0x001F;

/* Flush the number of bits */
u4_sym_len         = 11 + lead_zeros;

/* Calculate the address */
u4_bits         = ((lead_zeros - 1) << 5) + u4_bits;

DecodedValue = gau2_impeg2d_tab_zero_10_16[u4_bits];

u4_run = BITS(DecodedValue, 8,4);
u4_level = ((WORD16) DecodedValue) >> 9;

u4_numCoeffs       += u4_run;

u4_pos                 = pu1_scan[u4_numCoeffs++ & 63];
pu1_pos[*pi4_num_coeffs] = u4_pos;
if (1 == lead_zeros)
u4_sym_len--;
/* flushing */
FLUSH_BITS(u4_offset,u4_buf,u4_buf_nxt,u4_sym_len,pu4_buf_aligned)
pi2_outAddr[*pi4_num_coeffs] = u4_level;

(*pi4_num_coeffs)++;
}
/*Escape Sequence*/
else if(u2_mpeg2 == 1)
{
u4_sym_len         = 6;
FLUSH_BITS(u4_offset,u4_buf,u4_buf_nxt,u4_sym_len,pu4_buf_aligned)
IBITS_GET(u4_buf,u4_buf_nxt,u4_offset,u4_bits,pu4_buf_aligned,18)
u4_decoded_value    = u4_bits;
u4_run             = (u4_decoded_value >> 12);
u4_level           = (u4_decoded_value & 0x0FFF);

if (u4_level)
u4_level = (u4_level - ((u4_level & 0x0800) << 1));

u4_numCoeffs           += u4_run;

u4_pos                 = pu1_scan[u4_numCoeffs++ & 63];
pu1_pos[*pi4_num_coeffs] = u4_pos;
pi2_outAddr[*pi4_num_coeffs] = u4_level;

(*pi4_num_coeffs)++;
}
/*********************************************************************/
/* MPEG1 Escape Code                                                 */
/*********************************************************************/
else
{
/*-----------------------------------------------------------
* MPEG-1 Stream
*
* <See D.9.3 of MPEG-2> Run-level escape syntax
* Run-level values that cannot be coded with a VLC are coded
* by the escape code '0000 01' followed by
* either a 14-bit FLC (127 <= level <= 127),
* or a 22-bit FLC (255 <= level <= 255).
* This is described in Annex B,B.5f of MPEG-1.standard
*-----------------------------------------------------------*/

/*-----------------------------------------------------------
* First 6 bits are the value of the Run. Next is First 8 bits
* of Level. These bits decide whether it is 14 bit FLC or
* 22-bit FLC.
*
* If( first 8 bits of Level == '1000000' or '00000000')
*      then its is 22-bit FLC.
* else
*      it is 14-bit FLC.
*-----------------------------------------------------------*/
u4_sym_len             = 6;
FLUSH_BITS(u4_offset,u4_buf,u4_buf_nxt,u4_sym_len,pu4_buf_aligned)
IBITS_GET(u4_buf,u4_buf_nxt,u4_offset,u4_bits,pu4_buf_aligned,14)
u4_decoded_value        = u4_bits;
u4_run                 = (u4_decoded_value >> 8);
u4_level_first_byte    = (u4_decoded_value & 0x0FF);
if(u4_level_first_byte & 0x7F)
{
/*-------------------------------------------------------
* First 8 bits of level are neither 1000000 nor 00000000
* Hence 14-bit FLC (Last 8 bits are used to get level)
*
*  Level = (msb of Level_First_Byte is 1)?
*          Level_First_Byte - 256 : Level_First_Byte
*-------------------------------------------------------*/
u4_level = (u4_level_first_byte -
((u4_level_first_byte & 0x80) << 1));
}
else
{
/*-------------------------------------------------------
* Next 8 bits are either 1000000 or 00000000
* Hence 22-bit FLC (Last 16 bits are used to get level)
*
*  Level = (msb of Level_First_Byte is 1)?
*          Level_Second_Byte - 256 : Level_Second_Byte
*-------------------------------------------------------*/
IBITS_GET(u4_buf,u4_buf_nxt,u4_offset,u4_bits,pu4_buf_aligned,8)
u4_level = u4_bits;
u4_level = (u4_level - (u4_level_first_byte << 1));
}
u4_numCoeffs           += u4_run;

u4_pos                 = pu1_scan[u4_numCoeffs++ & 63];
pu1_pos[*pi4_num_coeffs] = u4_pos;
pi2_outAddr[*pi4_num_coeffs] = u4_level;

(*pi4_num_coeffs)++;
}
}
}


u4_nz_cols |= 1 << (u4_pos & 0x7);
u4_nz_rows |= 1 << (u4_pos >> 0x3);
            }
            if (u4_numCoeffs > 64)
            {
                return IMPEG2D_MB_TEX_DECODE_ERR;
}

IBITS_GET(u4_buf,u4_buf_nxt,u4_offset,u4_bits,pu4_buf_aligned,u4_sym_len)

}

PUT_TEMP_STREAM_DATA(u4_buf, u4_buf_nxt, u4_offset, pu4_buf_aligned, ps_stream)

ps_dec->u4_non_zero_cols  = u4_nz_cols;
ps_dec->u4_non_zero_rows  = u4_nz_rows;

return (IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE;
}
