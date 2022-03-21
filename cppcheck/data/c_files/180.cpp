void jas_matrix_divpow2(jas_matrix_t *matrix, int n)
{
	int i;
	int j;
jas_seqent_t *rowstart;
	int rowstep;
jas_seqent_t *data;

if (jas_matrix_numrows(matrix) > 0 && jas_matrix_numcols(matrix) > 0) {
assert(matrix->rows_);
rowstep = jas_matrix_rowstep(matrix);
for (i = matrix->numrows_, rowstart = matrix->rows_[0]; i > 0; --i,
rowstart += rowstep) {
for (j = matrix->numcols_, data = rowstart; j > 0; --j,
++data) {
*data = (*data >= 0) ? ((*data) >> n) :
(-((-(*data)) >> n));
}
}
}
}
