static boolean parse_identifier( const char **pcur, char *ret )
{
const char *cur = *pcur;
int i = 0;
if (is_alpha_underscore( cur )) {
ret[i++] = *cur++;
      while (is_alpha_underscore( cur ) || is_digit( cur ))
ret[i++] = *cur++;
ret[i++] = '\0';
*pcur = cur;
return TRUE;

/* Parse floating point.
*/
static boolean parse_float( const char **pcur, float *val )
{
const char *cur = *pcur;
boolean integral_part = FALSE;
boolean fractional_part = FALSE;

if (*cur == '0' && *(cur + 1) == 'x') {
union fi fi;
fi.ui = strtoul(cur, NULL, 16);
*val = fi.f;
cur += 10;
goto out;
}

*val = (float) atof( cur );
if (*cur == '-' || *cur == '+')
cur++;
if (is_digit( cur )) {
cur++;
integral_part = TRUE;
while (is_digit( cur ))
cur++;
}
if (*cur == '.') {
cur++;
if (is_digit( cur )) {
cur++;
fractional_part = TRUE;
while (is_digit( cur ))
cur++;
}
}
if (!integral_part && !fractional_part)
return FALSE;
if (uprcase( *cur ) == 'E') {
cur++;
if (*cur == '-' || *cur == '+')
cur++;
if (is_digit( cur )) {
cur++;
while (is_digit( cur ))
cur++;
}
else
return FALSE;
}

out:
*pcur = cur;
return TRUE;
}

static boolean parse_double( const char **pcur, uint32_t *val0, uint32_t *val1)
{
const char *cur = *pcur;
union {
double dval;
uint32_t uval[2];
} v;

v.dval = strtod(cur, (char**)pcur);
if (*pcur == cur)
return FALSE;

*val0 = v.uval[0];
*val1 = v.uval[1];

return TRUE;
}

struct translate_ctx
{
const char *text;
const char *cur;
struct tgsi_token *tokens;
struct tgsi_token *tokens_cur;
struct tgsi_token *tokens_end;
struct tgsi_header *header;
unsigned processor : 4;
unsigned implied_array_size : 6;
unsigned num_immediates;
};

static void report_error(struct translate_ctx *ctx, const char *format, ...)
{
va_list args;
int line = 1;
int column = 1;
const char *itr = ctx->text;

debug_printf("\nTGSI asm error: ");

va_start(args, format);
_debug_vprintf(format, args);
va_end(args);

while (itr != ctx->cur) {
if (*itr == '\n') {
column = 1;
++line;
}
++column;
++itr;
}

debug_printf(" [%d : %d] \n", line, column);
}

/* Parse shader header.
* Return TRUE for one of the following headers.
*    FRAG
*    GEOM
*    VERT
*/
static boolean parse_header( struct translate_ctx *ctx )
{
uint processor;

if (str_match_nocase_whole( &ctx->cur, "FRAG" ))
processor = TGSI_PROCESSOR_FRAGMENT;
else if (str_match_nocase_whole( &ctx->cur, "VERT" ))
processor = TGSI_PROCESSOR_VERTEX;
else if (str_match_nocase_whole( &ctx->cur, "GEOM" ))
processor = TGSI_PROCESSOR_GEOMETRY;
else if (str_match_nocase_whole( &ctx->cur, "TESS_CTRL" ))
processor = TGSI_PROCESSOR_TESS_CTRL;
else if (str_match_nocase_whole( &ctx->cur, "TESS_EVAL" ))
processor = TGSI_PROCESSOR_TESS_EVAL;
else if (str_match_nocase_whole( &ctx->cur, "COMP" ))
processor = TGSI_PROCESSOR_COMPUTE;
else {
report_error( ctx, "Unknown header" );
return FALSE;
}

if (ctx->tokens_cur >= ctx->tokens_end)
return FALSE;
ctx->header = (struct tgsi_header *) ctx->tokens_cur++;
*ctx->header = tgsi_build_header();

if (ctx->tokens_cur >= ctx->tokens_end)
return FALSE;
*(struct tgsi_processor *) ctx->tokens_cur++ = tgsi_build_processor( processor, ctx->header );
ctx->processor = processor;

return TRUE;
}

static boolean parse_label( struct translate_ctx *ctx, uint *val )
{
const char *cur = ctx->cur;

if (parse_uint( &cur, val )) {
eat_opt_white( &cur );
if (*cur == ':') {
cur++;
ctx->cur = cur;
return TRUE;
}
}
return FALSE;
}

static boolean
parse_file( const char **pcur, uint *file )
{
uint i;

for (i = 0; i < TGSI_FILE_COUNT; i++) {
const char *cur = *pcur;

if (str_match_nocase_whole( &cur, tgsi_file_name(i) )) {
*pcur = cur;
*file = i;
return TRUE;
}
}
return FALSE;
}

static boolean
parse_opt_writemask(
struct translate_ctx *ctx,
uint *writemask )
{
const char *cur;

cur = ctx->cur;
eat_opt_white( &cur );
if (*cur == '.') {
cur++;
*writemask = TGSI_WRITEMASK_NONE;
eat_opt_white( &cur );
if (uprcase( *cur ) == 'X') {
cur++;
*writemask |= TGSI_WRITEMASK_X;
}
if (uprcase( *cur ) == 'Y') {
cur++;
*writemask |= TGSI_WRITEMASK_Y;
}
if (uprcase( *cur ) == 'Z') {
cur++;
*writemask |= TGSI_WRITEMASK_Z;
}
if (uprcase( *cur ) == 'W') {
cur++;
*writemask |= TGSI_WRITEMASK_W;
}

if (*writemask == TGSI_WRITEMASK_NONE) {
report_error( ctx, "Writemask expected" );
return FALSE;
}

ctx->cur = cur;
}
else {
*writemask = TGSI_WRITEMASK_XYZW;
}
return TRUE;
}


/* <register_file_bracket> ::= <file> `['
*/
static boolean
parse_register_file_bracket(
struct translate_ctx *ctx,
uint *file )
{
if (!parse_file( &ctx->cur, file )) {
report_error( ctx, "Unknown register file" );
return FALSE;
}
eat_opt_white( &ctx->cur );
if (*ctx->cur != '[') {
report_error( ctx, "Expected `['" );
return FALSE;
}
ctx->cur++;
return TRUE;
}

/* <register_file_bracket_index> ::= <register_file_bracket> <uint>
*/
static boolean
parse_register_file_bracket_index(
struct translate_ctx *ctx,
uint *file,
int *index )
{
uint uindex;

if (!parse_register_file_bracket( ctx, file ))
return FALSE;
eat_opt_white( &ctx->cur );
if (!parse_uint( &ctx->cur, &uindex )) {
report_error( ctx, "Expected literal unsigned integer" );
return FALSE;
}
*index = (int) uindex;
return TRUE;
}

/* Parse simple 1d register operand.
*    <register_dst> ::= <register_file_bracket_index> `]'
*/
static boolean
parse_register_1d(struct translate_ctx *ctx,
uint *file,
int *index )
{
if (!parse_register_file_bracket_index( ctx, file, index ))
return FALSE;
eat_opt_white( &ctx->cur );
if (*ctx->cur != ']') {
report_error( ctx, "Expected `]'" );
return FALSE;
}
ctx->cur++;
return TRUE;
}

struct parsed_bracket {
int index;

uint ind_file;
int ind_index;
uint ind_comp;
uint ind_array;
};


static boolean
parse_register_bracket(
struct translate_ctx *ctx,
struct parsed_bracket *brackets)
{
const char *cur;
uint uindex;

memset(brackets, 0, sizeof(struct parsed_bracket));

eat_opt_white( &ctx->cur );

cur = ctx->cur;
if (parse_file( &cur, &brackets->ind_file )) {
if (!parse_register_1d( ctx, &brackets->ind_file,
&brackets->ind_index ))
return FALSE;
eat_opt_white( &ctx->cur );

if (*ctx->cur == '.') {
ctx->cur++;
eat_opt_white(&ctx->cur);

switch (uprcase(*ctx->cur)) {
case 'X':
brackets->ind_comp = TGSI_SWIZZLE_X;
break;
case 'Y':
brackets->ind_comp = TGSI_SWIZZLE_Y;
break;
case 'Z':
brackets->ind_comp = TGSI_SWIZZLE_Z;
break;
case 'W':
brackets->ind_comp = TGSI_SWIZZLE_W;
break;
default:
report_error(ctx, "Expected indirect register swizzle component `x', `y', `z' or `w'");
return FALSE;
}
ctx->cur++;
eat_opt_white(&ctx->cur);
}

if (*ctx->cur == '+' || *ctx->cur == '-')
parse_int( &ctx->cur, &brackets->index );
else
brackets->index = 0;
}
else {
if (!parse_uint( &ctx->cur, &uindex )) {
report_error( ctx, "Expected literal unsigned integer" );
return FALSE;
}
brackets->index = (int) uindex;
brackets->ind_file = TGSI_FILE_NULL;
brackets->ind_index = 0;
}
eat_opt_white( &ctx->cur );
if (*ctx->cur != ']') {
report_error( ctx, "Expected `]'" );
return FALSE;
}
ctx->cur++;
if (*ctx->cur == '(') {
ctx->cur++;
eat_opt_white( &ctx->cur );
if (!parse_uint( &ctx->cur, &brackets->ind_array )) {
report_error( ctx, "Expected literal unsigned integer" );
return FALSE;
}
eat_opt_white( &ctx->cur );
if (*ctx->cur != ')') {
report_error( ctx, "Expected `)'" );
return FALSE;
}
ctx->cur++;
}
return TRUE;
}

static boolean
parse_opt_register_src_bracket(
struct translate_ctx *ctx,
struct parsed_bracket *brackets,
int *parsed_brackets)
{
const char *cur = ctx->cur;

*parsed_brackets = 0;

eat_opt_white( &cur );
if (cur[0] == '[') {
++cur;
ctx->cur = cur;

if (!parse_register_bracket(ctx, brackets))
return FALSE;

*parsed_brackets = 1;
}

return TRUE;
}


/* Parse source register operand.
*    <register_src> ::= <register_file_bracket_index> `]' |
*                       <register_file_bracket> <register_dst> [`.' (`x' | `y' | `z' | `w')] `]' |
*                       <register_file_bracket> <register_dst> [`.' (`x' | `y' | `z' | `w')] `+' <uint> `]' |
*                       <register_file_bracket> <register_dst> [`.' (`x' | `y' | `z' | `w')] `-' <uint> `]'
*/
static boolean
parse_register_src(
struct translate_ctx *ctx,
uint *file,
struct parsed_bracket *brackets)
{
brackets->ind_comp = TGSI_SWIZZLE_X;
if (!parse_register_file_bracket( ctx, file ))
return FALSE;
if (!parse_register_bracket( ctx, brackets ))
return FALSE;

return TRUE;
}

struct parsed_dcl_bracket {
uint first;
uint last;
};

static boolean
parse_register_dcl_bracket(
struct translate_ctx *ctx,
struct parsed_dcl_bracket *bracket)
{
uint uindex;
memset(bracket, 0, sizeof(struct parsed_dcl_bracket));

eat_opt_white( &ctx->cur );

if (!parse_uint( &ctx->cur, &uindex )) {
/* it can be an empty bracket [] which means its range
* is from 0 to some implied size */
if (ctx->cur[0] == ']' && ctx->implied_array_size != 0) {
bracket->first = 0;
bracket->last = ctx->implied_array_size - 1;
goto cleanup;
}
report_error( ctx, "Expected literal unsigned integer" );
return FALSE;
}
bracket->first = uindex;

eat_opt_white( &ctx->cur );

if (ctx->cur[0] == '.' && ctx->cur[1] == '.') {
uint uindex;

ctx->cur += 2;
eat_opt_white( &ctx->cur );
if (!parse_uint( &ctx->cur, &uindex )) {
report_error( ctx, "Expected literal integer" );
return FALSE;
}
bracket->last = (int) uindex;
eat_opt_white( &ctx->cur );
}
else {
bracket->last = bracket->first;
}

cleanup:
if (*ctx->cur != ']') {
report_error( ctx, "Expected `]' or `..'" );
return FALSE;
}
ctx->cur++;
return TRUE;
}

/* Parse register declaration.
*    <register_dcl> ::= <register_file_bracket_index> `]' |
*                       <register_file_bracket_index> `..' <index> `]'
*/
static boolean
parse_register_dcl(
struct translate_ctx *ctx,
uint *file,
struct parsed_dcl_bracket *brackets,
int *num_brackets)
{
const char *cur;

*num_brackets = 0;

if (!parse_register_file_bracket( ctx, file ))
return FALSE;
if (!parse_register_dcl_bracket( ctx, &brackets[0] ))
return FALSE;

*num_brackets = 1;

cur = ctx->cur;
eat_opt_white( &cur );

if (cur[0] == '[') {
bool is_in = *file == TGSI_FILE_INPUT;
bool is_out = *file == TGSI_FILE_OUTPUT;

++cur;
ctx->cur = cur;
if (!parse_register_dcl_bracket( ctx, &brackets[1] ))
return FALSE;
/* for geometry shader we don't really care about
* the first brackets it's always the size of the
* input primitive. so we want to declare just
* the index relevant to the semantics which is in
* the second bracket */

/* tessellation has similar constraints to geometry shader */
if ((ctx->processor == TGSI_PROCESSOR_GEOMETRY && is_in) ||
(ctx->processor == TGSI_PROCESSOR_TESS_EVAL && is_in) ||
(ctx->processor == TGSI_PROCESSOR_TESS_CTRL && (is_in || is_out))) {
brackets[0] = brackets[1];
*num_brackets = 1;
} else {
*num_brackets = 2;
}
}

return TRUE;
}


/* Parse destination register operand.*/
static boolean
parse_register_dst(
struct translate_ctx *ctx,
uint *file,
struct parsed_bracket *brackets)
{
brackets->ind_comp = TGSI_SWIZZLE_X;
if (!parse_register_file_bracket( ctx, file ))
return FALSE;
if (!parse_register_bracket( ctx, brackets ))
return FALSE;

return TRUE;
}

static boolean
parse_dst_operand(
struct translate_ctx *ctx,
struct tgsi_full_dst_register *dst )
{
uint file;
uint writemask;
const char *cur;
struct parsed_bracket bracket[2];
int parsed_opt_brackets;

if (!parse_register_dst( ctx, &file, &bracket[0] ))
return FALSE;
if (!parse_opt_register_src_bracket(ctx, &bracket[1], &parsed_opt_brackets))
return FALSE;

cur = ctx->cur;
eat_opt_white( &cur );

if (!parse_opt_writemask( ctx, &writemask ))
return FALSE;

dst->Register.File = file;
if (parsed_opt_brackets) {
dst->Register.Dimension = 1;
dst->Dimension.Indirect = 0;
dst->Dimension.Dimension = 0;
dst->Dimension.Index = bracket[0].index;

if (bracket[0].ind_file != TGSI_FILE_NULL) {
dst->Dimension.Indirect = 1;
dst->DimIndirect.File = bracket[0].ind_file;
dst->DimIndirect.Index = bracket[0].ind_index;
dst->DimIndirect.Swizzle = bracket[0].ind_comp;
dst->DimIndirect.ArrayID = bracket[0].ind_array;
}
bracket[0] = bracket[1];
}
dst->Register.Index = bracket[0].index;
dst->Register.WriteMask = writemask;
if (bracket[0].ind_file != TGSI_FILE_NULL) {
dst->Register.Indirect = 1;
dst->Indirect.File = bracket[0].ind_file;
dst->Indirect.Index = bracket[0].ind_index;
dst->Indirect.Swizzle = bracket[0].ind_comp;
dst->Indirect.ArrayID = bracket[0].ind_array;
}
return TRUE;
}

static boolean
parse_optional_swizzle(
struct translate_ctx *ctx,
uint *swizzle,
boolean *parsed_swizzle,
int components)
{
const char *cur = ctx->cur;

*parsed_swizzle = FALSE;

eat_opt_white( &cur );
if (*cur == '.') {
uint i;

cur++;
eat_opt_white( &cur );
for (i = 0; i < components; i++) {
if (uprcase( *cur ) == 'X')
swizzle[i] = TGSI_SWIZZLE_X;
else if (uprcase( *cur ) == 'Y')
swizzle[i] = TGSI_SWIZZLE_Y;
else if (uprcase( *cur ) == 'Z')
swizzle[i] = TGSI_SWIZZLE_Z;
else if (uprcase( *cur ) == 'W')
swizzle[i] = TGSI_SWIZZLE_W;
else {
report_error( ctx, "Expected register swizzle component `x', `y', `z' or `w'" );
return FALSE;
}
cur++;
}
*parsed_swizzle = TRUE;
ctx->cur = cur;
}
return TRUE;
}

static boolean
parse_src_operand(
struct translate_ctx *ctx,
struct tgsi_full_src_register *src )
{
uint file;
uint swizzle[4];
boolean parsed_swizzle;
struct parsed_bracket bracket[2];
int parsed_opt_brackets;

if (*ctx->cur == '-') {
ctx->cur++;
eat_opt_white( &ctx->cur );
src->Register.Negate = 1;
}

if (*ctx->cur == '|') {
ctx->cur++;
eat_opt_white( &ctx->cur );
src->Register.Absolute = 1;
}

if (!parse_register_src(ctx, &file, &bracket[0]))
return FALSE;
if (!parse_opt_register_src_bracket(ctx, &bracket[1], &parsed_opt_brackets))
return FALSE;

src->Register.File = file;
if (parsed_opt_brackets) {
src->Register.Dimension = 1;
src->Dimension.Indirect = 0;
src->Dimension.Dimension = 0;
src->Dimension.Index = bracket[0].index;
if (bracket[0].ind_file != TGSI_FILE_NULL) {
src->Dimension.Indirect = 1;
src->DimIndirect.File = bracket[0].ind_file;
src->DimIndirect.Index = bracket[0].ind_index;
src->DimIndirect.Swizzle = bracket[0].ind_comp;
src->DimIndirect.ArrayID = bracket[0].ind_array;
}
bracket[0] = bracket[1];
}
src->Register.Index = bracket[0].index;
if (bracket[0].ind_file != TGSI_FILE_NULL) {
src->Register.Indirect = 1;
src->Indirect.File = bracket[0].ind_file;
src->Indirect.Index = bracket[0].ind_index;
src->Indirect.Swizzle = bracket[0].ind_comp;
src->Indirect.ArrayID = bracket[0].ind_array;
}

/* Parse optional swizzle.
*/
if (parse_optional_swizzle( ctx, swizzle, &parsed_swizzle, 4 )) {
if (parsed_swizzle) {
src->Register.SwizzleX = swizzle[0];
src->Register.SwizzleY = swizzle[1];
src->Register.SwizzleZ = swizzle[2];
src->Register.SwizzleW = swizzle[3];
}
}

if (src->Register.Absolute) {
eat_opt_white( &ctx->cur );
if (*ctx->cur != '|') {
report_error( ctx, "Expected `|'" );
return FALSE;
}
ctx->cur++;
}


return TRUE;
}

static boolean
parse_texoffset_operand(
struct translate_ctx *ctx,
struct tgsi_texture_offset *src )
{
uint file;
uint swizzle[3];
boolean parsed_swizzle;
struct parsed_bracket bracket;

if (!parse_register_src(ctx, &file, &bracket))
return FALSE;

src->File = file;
src->Index = bracket.index;

/* Parse optional swizzle.
*/
if (parse_optional_swizzle( ctx, swizzle, &parsed_swizzle, 3 )) {
if (parsed_swizzle) {
src->SwizzleX = swizzle[0];
src->SwizzleY = swizzle[1];
src->SwizzleZ = swizzle[2];
}
}

return TRUE;
}

static boolean
match_inst(const char **pcur,
unsigned *saturate,
const struct tgsi_opcode_info *info)
{
const char *cur = *pcur;

/* simple case: the whole string matches the instruction name */
if (str_match_nocase_whole(&cur, info->mnemonic)) {
*pcur = cur;
*saturate = 0;
return TRUE;
}

if (str_match_no_case(&cur, info->mnemonic)) {
/* the instruction has a suffix, figure it out */
if (str_match_nocase_whole(&cur, "_SAT")) {
*pcur = cur;
*saturate = 1;
return TRUE;
}
}

return FALSE;
}

static boolean
parse_instruction(
struct translate_ctx *ctx,
boolean has_label )
{
uint i;
uint saturate = 0;
const struct tgsi_opcode_info *info;
struct tgsi_full_instruction inst;
const char *cur;
uint advance;

inst = tgsi_default_full_instruction();

/* Parse predicate.
*/
eat_opt_white( &ctx->cur );
if (*ctx->cur == '(') {
uint file;
int index;
uint swizzle[4];
boolean parsed_swizzle;

inst.Instruction.Predicate = 1;

ctx->cur++;
if (*ctx->cur == '!') {
ctx->cur++;
inst.Predicate.Negate = 1;
}

if (!parse_register_1d( ctx, &file, &index ))
return FALSE;

if (parse_optional_swizzle( ctx, swizzle, &parsed_swizzle, 4 )) {
if (parsed_swizzle) {
inst.Predicate.SwizzleX = swizzle[0];
inst.Predicate.SwizzleY = swizzle[1];
inst.Predicate.SwizzleZ = swizzle[2];
inst.Predicate.SwizzleW = swizzle[3];
}
}

if (*ctx->cur != ')') {
report_error( ctx, "Expected `)'" );
return FALSE;
}

ctx->cur++;
}

/* Parse instruction name.
*/
eat_opt_white( &ctx->cur );
for (i = 0; i < TGSI_OPCODE_LAST; i++) {
cur = ctx->cur;

info = tgsi_get_opcode_info( i );
if (match_inst(&cur, &saturate, info)) {
if (info->num_dst + info->num_src + info->is_tex == 0) {
ctx->cur = cur;
break;
}
else if (*cur == '\0' || eat_white( &cur )) {
ctx->cur = cur;
break;
}
}
}
if (i == TGSI_OPCODE_LAST) {
if (has_label)
report_error( ctx, "Unknown opcode" );
else
report_error( ctx, "Expected `DCL', `IMM' or a label" );
return FALSE;
}

inst.Instruction.Opcode = i;
inst.Instruction.Saturate = saturate;
inst.Instruction.NumDstRegs = info->num_dst;
inst.Instruction.NumSrcRegs = info->num_src;

if (i >= TGSI_OPCODE_SAMPLE && i <= TGSI_OPCODE_GATHER4) {
/*
* These are not considered tex opcodes here (no additional
* target argument) however we're required to set the Texture
* bit so we can set the number of tex offsets.
*/
inst.Instruction.Texture = 1;
inst.Texture.Texture = TGSI_TEXTURE_UNKNOWN;
}

/* Parse instruction operands.
*/
for (i = 0; i < info->num_dst + info->num_src + info->is_tex; i++) {
if (i > 0) {
eat_opt_white( &ctx->cur );
if (*ctx->cur != ',') {
report_error( ctx, "Expected `,'" );
return FALSE;
}
ctx->cur++;
eat_opt_white( &ctx->cur );
}

if (i < info->num_dst) {
if (!parse_dst_operand( ctx, &inst.Dst[i] ))
return FALSE;
}
else if (i < info->num_dst + info->num_src) {
if (!parse_src_operand( ctx, &inst.Src[i - info->num_dst] ))
return FALSE;
}
else {
uint j;

for (j = 0; j < TGSI_TEXTURE_COUNT; j++) {
if (str_match_nocase_whole( &ctx->cur, tgsi_texture_names[j] )) {
inst.Instruction.Texture = 1;
inst.Texture.Texture = j;
break;
}
}
if (j == TGSI_TEXTURE_COUNT) {
report_error( ctx, "Expected texture target" );
return FALSE;
}
}
}

cur = ctx->cur;
eat_opt_white( &cur );
for (i = 0; inst.Instruction.Texture && *cur == ','; i++) {
cur++;
eat_opt_white( &cur );
ctx->cur = cur;
if (!parse_texoffset_operand( ctx, &inst.TexOffsets[i] ))
return FALSE;
cur = ctx->cur;
eat_opt_white( &cur );
}
inst.Texture.NumOffsets = i;

cur = ctx->cur;
eat_opt_white( &cur );
if (info->is_branch && *cur == ':') {
uint target;

cur++;
eat_opt_white( &cur );
if (!parse_uint( &cur, &target )) {
report_error( ctx, "Expected a label" );
return FALSE;
}
inst.Instruction.Label = 1;
inst.Label.Label = target;
ctx->cur = cur;
}

advance = tgsi_build_full_instruction(
&inst,
ctx->tokens_cur,
ctx->header,
(uint) (ctx->tokens_end - ctx->tokens_cur) );
if (advance == 0)
return FALSE;
ctx->tokens_cur += advance;

return TRUE;
}

/* parses a 4-touple of the form {x, y, z, w}
* where x, y, z, w are numbers */
static boolean parse_immediate_data(struct translate_ctx *ctx, unsigned type,
union tgsi_immediate_data *values)
{
unsigned i;
int ret;

eat_opt_white( &ctx->cur );
if (*ctx->cur != '{') {
report_error( ctx, "Expected `{'" );
return FALSE;
}
ctx->cur++;
for (i = 0; i < 4; i++) {
eat_opt_white( &ctx->cur );
if (i > 0) {
if (*ctx->cur != ',') {
report_error( ctx, "Expected `,'" );
return FALSE;
}
ctx->cur++;
eat_opt_white( &ctx->cur );
}

switch (type) {
case TGSI_IMM_FLOAT64:
ret = parse_double(&ctx->cur, &values[i].Uint, &values[i+1].Uint);
i++;
break;
case TGSI_IMM_FLOAT32:
ret = parse_float(&ctx->cur, &values[i].Float);
break;
case TGSI_IMM_UINT32:
ret = parse_uint(&ctx->cur, &values[i].Uint);
break;
case TGSI_IMM_INT32:
ret = parse_int(&ctx->cur, &values[i].Int);
break;
default:
assert(0);
ret = FALSE;
break;
}

if (!ret) {
report_error( ctx, "Expected immediate constant" );
return FALSE;
}
}
eat_opt_white( &ctx->cur );
if (*ctx->cur != '}') {
report_error( ctx, "Expected `}'" );
return FALSE;
}
ctx->cur++;

return TRUE;
}

static boolean parse_declaration( struct translate_ctx *ctx )
{
struct tgsi_full_declaration decl;
uint file;
struct parsed_dcl_bracket brackets[2];
int num_brackets;
uint writemask;
const char *cur, *cur2;
uint advance;
boolean is_vs_input;

if (!eat_white( &ctx->cur )) {
report_error( ctx, "Syntax error" );
return FALSE;
}
if (!parse_register_dcl( ctx, &file, brackets, &num_brackets))
return FALSE;
if (!parse_opt_writemask( ctx, &writemask ))
return FALSE;

decl = tgsi_default_full_declaration();
decl.Declaration.File = file;
decl.Declaration.UsageMask = writemask;

if (num_brackets == 1) {
decl.Range.First = brackets[0].first;
decl.Range.Last = brackets[0].last;
} else {
decl.Range.First = brackets[1].first;
decl.Range.Last = brackets[1].last;

decl.Declaration.Dimension = 1;
decl.Dim.Index2D = brackets[0].first;
}

is_vs_input = (file == TGSI_FILE_INPUT &&
ctx->processor == TGSI_PROCESSOR_VERTEX);

cur = ctx->cur;
eat_opt_white( &cur );
if (*cur == ',') {
cur2 = cur;
cur2++;
eat_opt_white( &cur2 );
if (str_match_nocase_whole( &cur2, "ARRAY" )) {
int arrayid;
if (*cur2 != '(') {
report_error( ctx, "Expected `('" );
return FALSE;
}
cur2++;
eat_opt_white( &cur2 );
if (!parse_int( &cur2, &arrayid )) {
report_error( ctx, "Expected `,'" );
return FALSE;
}
eat_opt_white( &cur2 );
if (*cur2 != ')') {
report_error( ctx, "Expected `)'" );
return FALSE;
}
cur2++;
decl.Declaration.Array = 1;
decl.Array.ArrayID = arrayid;
ctx->cur = cur = cur2;
}
}

if (*cur == ',' && !is_vs_input) {
uint i, j;

cur++;
eat_opt_white( &cur );
if (file == TGSI_FILE_RESOURCE) {
for (i = 0; i < TGSI_TEXTURE_COUNT; i++) {
if (str_match_nocase_whole(&cur, tgsi_texture_names[i])) {
decl.Resource.Resource = i;
break;
}
}
if (i == TGSI_TEXTURE_COUNT) {
report_error(ctx, "Expected texture target");
return FALSE;
}

cur2 = cur;
eat_opt_white(&cur2);
while (*cur2 == ',') {
cur2++;
eat_opt_white(&cur2);
if (str_match_nocase_whole(&cur2, "RAW")) {
decl.Resource.Raw = 1;

} else if (str_match_nocase_whole(&cur2, "WR")) {
decl.Resource.Writable = 1;

} else {
break;
}
cur = cur2;
eat_opt_white(&cur2);
}

ctx->cur = cur;

} else if (file == TGSI_FILE_SAMPLER_VIEW) {
for (i = 0; i < TGSI_TEXTURE_COUNT; i++) {
if (str_match_nocase_whole(&cur, tgsi_texture_names[i])) {
decl.SamplerView.Resource = i;
break;
}
}
if (i == TGSI_TEXTURE_COUNT) {
report_error(ctx, "Expected texture target");
return FALSE;
}
eat_opt_white( &cur );
if (*cur != ',') {
report_error( ctx, "Expected `,'" );
return FALSE;
}
++cur;
eat_opt_white( &cur );
for (j = 0; j < 4; ++j) {
for (i = 0; i < TGSI_RETURN_TYPE_COUNT; ++i) {
if (str_match_nocase_whole(&cur, tgsi_return_type_names[i])) {
switch (j) {
case 0:
decl.SamplerView.ReturnTypeX = i;
break;
case 1:
decl.SamplerView.ReturnTypeY = i;
break;
case 2:
decl.SamplerView.ReturnTypeZ = i;
break;
case 3:
decl.SamplerView.ReturnTypeW = i;
break;
default:
assert(0);
}
break;
}
}
if (i == TGSI_RETURN_TYPE_COUNT) {
if (j == 0 || j >  2) {
report_error(ctx, "Expected type name");
return FALSE;
}
break;
} else {
cur2 = cur;
eat_opt_white( &cur2 );
if (*cur2 == ',') {
cur2++;
eat_opt_white( &cur2 );
cur = cur2;
continue;
} else
break;
}
}
if (j < 4) {
decl.SamplerView.ReturnTypeY =
decl.SamplerView.ReturnTypeZ =
decl.SamplerView.ReturnTypeW =
decl.SamplerView.ReturnTypeX;
}
ctx->cur = cur;
} else {
if (str_match_nocase_whole(&cur, "LOCAL")) {
decl.Declaration.Local = 1;
ctx->cur = cur;
}

cur = ctx->cur;
eat_opt_white( &cur );
if (*cur == ',') {
cur++;
eat_opt_white( &cur );

for (i = 0; i < TGSI_SEMANTIC_COUNT; i++) {
if (str_match_nocase_whole(&cur, tgsi_semantic_names[i])) {
uint index;

cur2 = cur;
eat_opt_white( &cur2 );
if (*cur2 == '[') {
cur2++;
eat_opt_white( &cur2 );
if (!parse_uint( &cur2, &index )) {
report_error( ctx, "Expected literal integer" );
return FALSE;
}
eat_opt_white( &cur2 );
if (*cur2 != ']') {
report_error( ctx, "Expected `]'" );
return FALSE;
}
cur2++;

decl.Semantic.Index = index;

cur = cur2;
}

decl.Declaration.Semantic = 1;
decl.Semantic.Name = i;

ctx->cur = cur;
break;
}
}
}
}
}

cur = ctx->cur;
eat_opt_white( &cur );
if (*cur == ',' && !is_vs_input) {
uint i;

cur++;
eat_opt_white( &cur );
for (i = 0; i < TGSI_INTERPOLATE_COUNT; i++) {
if (str_match_nocase_whole( &cur, tgsi_interpolate_names[i] )) {
decl.Declaration.Interpolate = 1;
decl.Interp.Interpolate = i;

ctx->cur = cur;
break;
}
}
if (i == TGSI_INTERPOLATE_COUNT) {
report_error( ctx, "Expected semantic or interpolate attribute" );
return FALSE;
}
}

cur = ctx->cur;
eat_opt_white( &cur );
if (*cur == ',' && !is_vs_input) {
uint i;

cur++;
eat_opt_white( &cur );
for (i = 0; i < TGSI_INTERPOLATE_LOC_COUNT; i++) {
if (str_match_nocase_whole( &cur, tgsi_interpolate_locations[i] )) {
decl.Interp.Location = i;

ctx->cur = cur;
break;
}
}
}

advance = tgsi_build_full_declaration(
&decl,
ctx->tokens_cur,
ctx->header,
(uint) (ctx->tokens_end - ctx->tokens_cur) );

if (advance == 0)
return FALSE;
ctx->tokens_cur += advance;

return TRUE;
}

static boolean parse_immediate( struct translate_ctx *ctx )
{
struct tgsi_full_immediate imm;
uint advance;
int type;

if (*ctx->cur == '[') {
uint uindex;

++ctx->cur;

eat_opt_white( &ctx->cur );
if (!parse_uint( &ctx->cur, &uindex )) {
report_error( ctx, "Expected literal unsigned integer" );
return FALSE;
}

if (uindex != ctx->num_immediates) {
report_error( ctx, "Immediates must be sorted" );
return FALSE;
}

eat_opt_white( &ctx->cur );
if (*ctx->cur != ']') {
report_error( ctx, "Expected `]'" );
return FALSE;
}

ctx->cur++;
}

if (!eat_white( &ctx->cur )) {
report_error( ctx, "Syntax error" );
return FALSE;
}
for (type = 0; type < Elements(tgsi_immediate_type_names); ++type) {
if (str_match_nocase_whole(&ctx->cur, tgsi_immediate_type_names[type]))
break;
}
if (type == Elements(tgsi_immediate_type_names)) {
report_error( ctx, "Expected immediate type" );
return FALSE;
}

imm = tgsi_default_full_immediate();
imm.Immediate.NrTokens += 4;
imm.Immediate.DataType = type;
parse_immediate_data(ctx, type, imm.u);

advance = tgsi_build_full_immediate(
&imm,
ctx->tokens_cur,
ctx->header,
(uint) (ctx->tokens_end - ctx->tokens_cur) );
if (advance == 0)
return FALSE;
ctx->tokens_cur += advance;

ctx->num_immediates++;

return TRUE;
}

static boolean
parse_primitive( const char **pcur, uint *primitive )
{
uint i;

for (i = 0; i < PIPE_PRIM_MAX; i++) {
const char *cur = *pcur;

if (str_match_nocase_whole( &cur, tgsi_primitive_names[i])) {
*primitive = i;
*pcur = cur;
return TRUE;
}
}
return FALSE;
}

static boolean
parse_fs_coord_origin( const char **pcur, uint *fs_coord_origin )
{
uint i;

for (i = 0; i < Elements(tgsi_fs_coord_origin_names); i++) {
const char *cur = *pcur;

if (str_match_nocase_whole( &cur, tgsi_fs_coord_origin_names[i])) {
*fs_coord_origin = i;
*pcur = cur;
return TRUE;
}
}
return FALSE;
}

static boolean
parse_fs_coord_pixel_center( const char **pcur, uint *fs_coord_pixel_center )
{
uint i;

for (i = 0; i < Elements(tgsi_fs_coord_pixel_center_names); i++) {
const char *cur = *pcur;

if (str_match_nocase_whole( &cur, tgsi_fs_coord_pixel_center_names[i])) {
*fs_coord_pixel_center = i;
*pcur = cur;
return TRUE;
}
}
return FALSE;
}


static boolean parse_property( struct translate_ctx *ctx )
{
struct tgsi_full_property prop;
uint property_name;
uint values[8];
uint advance;
char id[64];

if (!eat_white( &ctx->cur )) {
report_error( ctx, "Syntax error" );
return FALSE;
}
report_error( ctx, "Syntax error" );
return FALSE;
}
   if (!parse_identifier( &ctx->cur, id )) {
report_error( ctx, "Syntax error" );
return FALSE;
}
break;
}
}
