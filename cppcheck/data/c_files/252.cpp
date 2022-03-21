gamma_component_validate(PNG_CONST char *name, PNG_CONST validate_info *vi,
    PNG_CONST unsigned int id, PNG_CONST unsigned int od,
    PNG_CONST double alpha /* <0 for the alpha channel itself */,
    PNG_CONST double background /* component background value */)
{
   PNG_CONST unsigned int isbit = id >> vi->isbit_shift;
   PNG_CONST unsigned int sbit_max = vi->sbit_max;
   PNG_CONST unsigned int outmax = vi->outmax;
   PNG_CONST int do_background = vi->do_background;

double i;

/* First check on the 'perfect' result obtained from the digitized input
* value, id, and compare this against the actual digitized result, 'od'.
* 'i' is the input result in the range 0..1:
*/
i = isbit; i /= sbit_max;

/* Check for the fast route: if we don't do any background composition or if
* this is the alpha channel ('alpha' < 0) or if the pixel is opaque then
* just use the gamma_correction field to correct to the final output gamma.
*/
if (alpha == 1 /* opaque pixel component */ || !do_background
#ifdef PNG_READ_ALPHA_MODE_SUPPORTED
|| do_background == ALPHA_MODE_OFFSET + PNG_ALPHA_PNG
#endif
|| (alpha < 0 /* alpha channel */
#ifdef PNG_READ_ALPHA_MODE_SUPPORTED
&& do_background != ALPHA_MODE_OFFSET + PNG_ALPHA_BROKEN
#endif
))
{
/* Then get the gamma corrected version of 'i' and compare to 'od', any
* error less than .5 is insignificant - just quantization of the output
* value to the nearest digital value (nevertheless the error is still
* recorded - it's interesting ;-)
*/
double encoded_sample = i;
double encoded_error;

/* alpha less than 0 indicates the alpha channel, which is always linear
*/
if (alpha >= 0 && vi->gamma_correction > 0)
encoded_sample = pow(encoded_sample, vi->gamma_correction);
encoded_sample *= outmax;

encoded_error = fabs(od-encoded_sample);

if (encoded_error > vi->dp->maxerrout)
vi->dp->maxerrout = encoded_error;

if (encoded_error < vi->maxout_total && encoded_error < vi->outlog)
return i;
}

/* The slow route - attempt to do linear calculations. */
/* There may be an error, or background processing is required, so calculate
* the actual sample values - unencoded light intensity values.  Note that in
* practice these are not completely unencoded because they include a
* 'viewing correction' to decrease or (normally) increase the perceptual
* contrast of the image.  There's nothing we can do about this - we don't
* know what it is - so assume the unencoded value is perceptually linear.
*/
{
double input_sample = i; /* In range 0..1 */
double output, error, encoded_sample, encoded_error;
double es_lo, es_hi;
int compose = 0; /* Set to one if composition done */
int output_is_encoded; /* Set if encoded to screen gamma */
int log_max_error = 1; /* Check maximum error values */
png_const_charp pass = 0; /* Reason test passes (or 0 for fail) */

/* Convert to linear light (with the above caveat.)  The alpha channel is
* already linear.
*/
if (alpha >= 0)
{
int tcompose;

if (vi->file_inverse > 0)
input_sample = pow(input_sample, vi->file_inverse);

/* Handle the compose processing: */
tcompose = 0;
input_sample = gamma_component_compose(do_background, input_sample,
alpha, background, &tcompose);

if (tcompose)
compose = 1;
}

/* And similarly for the output value, but we need to check the background
* handling to linearize it correctly.
*/
output = od;
output /= outmax;

output_is_encoded = vi->screen_gamma > 0;

if (alpha < 0) /* The alpha channel */
{
#ifdef PNG_READ_ALPHA_MODE_SUPPORTED
if (do_background != ALPHA_MODE_OFFSET + PNG_ALPHA_BROKEN)
#endif
{
/* In all other cases the output alpha channel is linear already,
* don't log errors here, they are much larger in linear data.
*/
output_is_encoded = 0;
log_max_error = 0;
}
}

#ifdef PNG_READ_ALPHA_MODE_SUPPORTED
else /* A component */
{
if (do_background == ALPHA_MODE_OFFSET + PNG_ALPHA_OPTIMIZED &&
alpha < 1) /* the optimized case - linear output */
{
if (alpha > 0) log_max_error = 0;
output_is_encoded = 0;
}
}
#endif

if (output_is_encoded)
output = pow(output, vi->screen_gamma);

/* Calculate (or recalculate) the encoded_sample value and repeat the
* check above (unnecessary if we took the fast route, but harmless.)
*/
encoded_sample = input_sample;
if (output_is_encoded)
encoded_sample = pow(encoded_sample, vi->screen_inverse);
encoded_sample *= outmax;

encoded_error = fabs(od-encoded_sample);

/* Don't log errors in the alpha channel, or the 'optimized' case,
* neither are significant to the overall perception.
*/
if (log_max_error && encoded_error > vi->dp->maxerrout)
vi->dp->maxerrout = encoded_error;

if (encoded_error < vi->maxout_total)
{
if (encoded_error < vi->outlog)
return i;

/* Test passed but error is bigger than the log limit, record why the
* test passed:
*/
pass = "less than maxout:\n";
}

/* i: the original input value in the range 0..1
*
* pngvalid calculations:
*  input_sample: linear result; i linearized and composed, range 0..1
*  encoded_sample: encoded result; input_sample scaled to ouput bit depth
*
* libpng calculations:
*  output: linear result; od scaled to 0..1 and linearized
*  od: encoded result from libpng
*/

/* Now we have the numbers for real errors, both absolute values as as a
* percentage of the correct value (output):
*/
error = fabs(input_sample-output);

if (log_max_error && error > vi->dp->maxerrabs)
vi->dp->maxerrabs = error;

/* The following is an attempt to ignore the tendency of quantization to
* dominate the percentage errors for lower result values:
*/
if (log_max_error && input_sample > .5)
{
double percentage_error = error/input_sample;
if (percentage_error > vi->dp->maxerrpc)
vi->dp->maxerrpc = percentage_error;
}

/* Now calculate the digitization limits for 'encoded_sample' using the
* 'max' values.  Note that maxout is in the encoded space but maxpc and
* maxabs are in linear light space.
*
* First find the maximum error in linear light space, range 0..1:
*/
{
double tmp = input_sample * vi->maxpc;
if (tmp < vi->maxabs) tmp = vi->maxabs;
/* If 'compose' is true the composition was done in linear space using
* integer arithmetic.  This introduces an extra error of +/- 0.5 (at
* least) in the integer space used.  'maxcalc' records this, taking
* into account the possibility that even for 16 bit output 8 bit space
* may have been used.
*/
if (compose && tmp < vi->maxcalc) tmp = vi->maxcalc;

/* The 'maxout' value refers to the encoded result, to compare with
* this encode input_sample adjusted by the maximum error (tmp) above.
*/
es_lo = encoded_sample - vi->maxout;

if (es_lo > 0 && input_sample-tmp > 0)
{
double low_value = input_sample-tmp;
if (output_is_encoded)
low_value = pow(low_value, vi->screen_inverse);
low_value *= outmax;
if (low_value < es_lo) es_lo = low_value;

/* Quantize this appropriately: */
es_lo = ceil(es_lo / vi->outquant - .5) * vi->outquant;
}

else
es_lo = 0;

es_hi = encoded_sample + vi->maxout;

if (es_hi < outmax && input_sample+tmp < 1)
{
double high_value = input_sample+tmp;
if (output_is_encoded)
high_value = pow(high_value, vi->screen_inverse);
high_value *= outmax;
if (high_value > es_hi) es_hi = high_value;

es_hi = floor(es_hi / vi->outquant + .5) * vi->outquant;
}

else
es_hi = outmax;
}

/* The primary test is that the final encoded value returned by the
* library should be between the two limits (inclusive) that were
* calculated above.
*/
if (od >= es_lo && od <= es_hi)
{
/* The value passes, but we may need to log the information anyway. */
if (encoded_error < vi->outlog)
return i;

if (pass == 0)
pass = "within digitization limits:\n";
}

{
/* There has been an error in processing, or we need to log this
* value.
*/
double is_lo, is_hi;

/* pass is set at this point if either of the tests above would have
* passed.  Don't do these additional tests here - just log the
* original [es_lo..es_hi] values.
*/
if (pass == 0 && vi->use_input_precision && vi->dp->sbit)
{
/* Ok, something is wrong - this actually happens in current libpng
* 16-to-8 processing.  Assume that the input value (id, adjusted
* for sbit) can be anywhere between value-.5 and value+.5 - quite a
* large range if sbit is low.
*
* NOTE: at present because the libpng gamma table stuff has been
* changed to use a rounding algorithm to correct errors in 8-bit
* calculations the precise sbit calculation (a shift) has been
* lost.  This can result in up to a +/-1 error in the presence of
* an sbit less than the bit depth.
*/
#           if PNG_LIBPNG_VER < 10700
#              define SBIT_ERROR .5
#           else
#              define SBIT_ERROR 1.
#           endif
double tmp = (isbit - SBIT_ERROR)/sbit_max;

if (tmp <= 0)
tmp = 0;

else if (alpha >= 0 && vi->file_inverse > 0 && tmp < 1)
tmp = pow(tmp, vi->file_inverse);

tmp = gamma_component_compose(do_background, tmp, alpha, background,
NULL);

if (output_is_encoded && tmp > 0 && tmp < 1)
tmp = pow(tmp, vi->screen_inverse);

is_lo = ceil(outmax * tmp - vi->maxout_total);

if (is_lo < 0)
is_lo = 0;

tmp = (isbit + SBIT_ERROR)/sbit_max;

if (tmp >= 1)
tmp = 1;

else if (alpha >= 0 && vi->file_inverse > 0 && tmp < 1)
tmp = pow(tmp, vi->file_inverse);

tmp = gamma_component_compose(do_background, tmp, alpha, background,
NULL);

if (output_is_encoded && tmp > 0 && tmp < 1)
tmp = pow(tmp, vi->screen_inverse);

is_hi = floor(outmax * tmp + vi->maxout_total);

if (is_hi > outmax)
is_hi = outmax;

if (!(od < is_lo || od > is_hi))
{
if (encoded_error < vi->outlog)
return i;

pass = "within input precision limits:\n";
}

/* One last chance.  If this is an alpha channel and the 16to8
* option has been used and 'inaccurate' scaling is used then the
* bit reduction is obtained by simply using the top 8 bits of the
* value.
*
* This is only done for older libpng versions when the 'inaccurate'
* (chop) method of scaling was used.
*/
#           ifndef PNG_READ_16_TO_8_ACCURATE_SCALE_SUPPORTED
#              if PNG_LIBPNG_VER < 10504
/* This may be required for other components in the future,
* but at present the presence of gamma correction effectively
* prevents the errors in the component scaling (I don't quite
* understand why, but since it's better this way I care not
* to ask, JB 20110419.)
*/
if (pass == 0 && alpha < 0 && vi->scale16 && vi->sbit > 8 &&
vi->sbit + vi->isbit_shift == 16)
{
tmp = ((id >> 8) - .5)/255;

if (tmp > 0)
{
is_lo = ceil(outmax * tmp - vi->maxout_total);
if (is_lo < 0) is_lo = 0;
}

else
is_lo = 0;

tmp = ((id >> 8) + .5)/255;

if (tmp < 1)
{
is_hi = floor(outmax * tmp + vi->maxout_total);
if (is_hi > outmax) is_hi = outmax;
}

else
is_hi = outmax;

if (!(od < is_lo || od > is_hi))
{
if (encoded_error < vi->outlog)
return i;

pass = "within 8 bit limits:\n";
}
}
#              endif
#           endif
}
else /* !use_input_precision */
is_lo = es_lo, is_hi = es_hi;

/* Attempt to output a meaningful error/warning message: the message
* output depends on the background/composite operation being performed
* because this changes what parameters were actually used above.
*/
{
size_t pos = 0;
/* Need either 1/255 or 1/65535 precision here; 3 or 6 decimal
* places.  Just use outmax to work out which.
*/
int precision = (outmax >= 1000 ? 6 : 3);
int use_input=1, use_background=0, do_compose=0;
char msg[256];

if (pass != 0)
pos = safecat(msg, sizeof msg, pos, "\n\t");

/* Set up the various flags, the output_is_encoded flag above
* is also used below.  do_compose is just a double check.
*/
switch (do_background)
{
#           ifdef PNG_READ_BACKGROUND_SUPPORTED
case PNG_BACKGROUND_GAMMA_SCREEN:
case PNG_BACKGROUND_GAMMA_FILE:
case PNG_BACKGROUND_GAMMA_UNIQUE:
use_background = (alpha >= 0 && alpha < 1);
/*FALL THROUGH*/
#           endif
#           ifdef PNG_READ_ALPHA_MODE_SUPPORTED
case ALPHA_MODE_OFFSET + PNG_ALPHA_STANDARD:
case ALPHA_MODE_OFFSET + PNG_ALPHA_BROKEN:
case ALPHA_MODE_OFFSET + PNG_ALPHA_OPTIMIZED:
#           endif /* ALPHA_MODE_SUPPORTED */
do_compose = (alpha > 0 && alpha < 1);
use_input = (alpha != 0);
break;

default:
break;
}

/* Check the 'compose' flag */
if (compose != do_compose)
png_error(vi->pp, "internal error (compose)");

/* 'name' is the component name */
pos = safecat(msg, sizeof msg, pos, name);
pos = safecat(msg, sizeof msg, pos, "(");
pos = safecatn(msg, sizeof msg, pos, id);
if (use_input || pass != 0/*logging*/)
{
if (isbit != id)
{
/* sBIT has reduced the precision of the input: */
pos = safecat(msg, sizeof msg, pos, ", sbit(");
pos = safecatn(msg, sizeof msg, pos, vi->sbit);
pos = safecat(msg, sizeof msg, pos, "): ");
pos = safecatn(msg, sizeof msg, pos, isbit);
}
pos = safecat(msg, sizeof msg, pos, "/");
/* The output is either "id/max" or "id sbit(sbit): isbit/max" */
pos = safecatn(msg, sizeof msg, pos, vi->sbit_max);
}
pos = safecat(msg, sizeof msg, pos, ")");

/* A component may have been multiplied (in linear space) by the
* alpha value, 'compose' says whether this is relevant.
*/
if (compose || pass != 0)
{
/* If any form of composition is being done report our
* calculated linear value here (the code above doesn't record
* the input value before composition is performed, so what
* gets reported is the value after composition.)
*/
if (use_input || pass != 0)
{
if (vi->file_inverse > 0)
{
pos = safecat(msg, sizeof msg, pos, "^");
pos = safecatd(msg, sizeof msg, pos, vi->file_inverse, 2);
}

else
pos = safecat(msg, sizeof msg, pos, "[linear]");

pos = safecat(msg, sizeof msg, pos, "*(alpha)");
pos = safecatd(msg, sizeof msg, pos, alpha, precision);
}

/* Now record the *linear* background value if it was used
* (this function is not passed the original, non-linear,
* value but it is contained in the test name.)
*/
if (use_background)
{
pos = safecat(msg, sizeof msg, pos, use_input ? "+" : " ");
pos = safecat(msg, sizeof msg, pos, "(background)");
pos = safecatd(msg, sizeof msg, pos, background, precision);
pos = safecat(msg, sizeof msg, pos, "*");
pos = safecatd(msg, sizeof msg, pos, 1-alpha, precision);
}
}

/* Report the calculated value (input_sample) and the linearized
* libpng value (output) unless this is just a component gamma
* correction.
*/
if (compose || alpha < 0 || pass != 0)
{
pos = safecat(msg, sizeof msg, pos,
pass != 0 ? " =\n\t" : " = ");
pos = safecatd(msg, sizeof msg, pos, input_sample, precision);
pos = safecat(msg, sizeof msg, pos, " (libpng: ");
pos = safecatd(msg, sizeof msg, pos, output, precision);
pos = safecat(msg, sizeof msg, pos, ")");

/* Finally report the output gamma encoding, if any. */
if (output_is_encoded)
{
pos = safecat(msg, sizeof msg, pos, " ^");
pos = safecatd(msg, sizeof msg, pos, vi->screen_inverse, 2);
pos = safecat(msg, sizeof msg, pos, "(to screen) =");
}

else
pos = safecat(msg, sizeof msg, pos, " [screen is linear] =");
}

if ((!compose && alpha >= 0) || pass != 0)
{
if (pass != 0) /* logging */
pos = safecat(msg, sizeof msg, pos, "\n\t[overall:");

/* This is the non-composition case, the internal linear
* values are irrelevant (though the log below will reveal
* them.)  Output a much shorter warning/error message and report
* the overall gamma correction.
*/
if (vi->gamma_correction > 0)
{
pos = safecat(msg, sizeof msg, pos, " ^");
pos = safecatd(msg, sizeof msg, pos, vi->gamma_correction, 2);
pos = safecat(msg, sizeof msg, pos, "(gamma correction) =");
}

else
pos = safecat(msg, sizeof msg, pos,
" [no gamma correction] =");

if (pass != 0)
pos = safecat(msg, sizeof msg, pos, "]");
}

/* This is our calculated encoded_sample which should (but does
* not) match od:
*/
pos = safecat(msg, sizeof msg, pos, pass != 0 ? "\n\t" : " ");
pos = safecatd(msg, sizeof msg, pos, is_lo, 1);
pos = safecat(msg, sizeof msg, pos, " < ");
pos = safecatd(msg, sizeof msg, pos, encoded_sample, 1);
pos = safecat(msg, sizeof msg, pos, " (libpng: ");
pos = safecatn(msg, sizeof msg, pos, od);
pos = safecat(msg, sizeof msg, pos, ")");
pos = safecat(msg, sizeof msg, pos, "/");
pos = safecatn(msg, sizeof msg, pos, outmax);
pos = safecat(msg, sizeof msg, pos, " < ");
pos = safecatd(msg, sizeof msg, pos, is_hi, 1);

if (pass == 0) /* The error condition */
{
#              ifdef PNG_WARNINGS_SUPPORTED
png_warning(vi->pp, msg);
#              else
store_warning(vi->pp, msg);
#              endif
}

else /* logging this value */
store_verbose(&vi->dp->pm->this, vi->pp, pass, msg);
}
}
}

return i;
}
