static void calc_coeff(double mu[4], const int index[4], int prefilter, double r2, double mul)
{
double mul2 = mul * mul, mul3 = mul2 * mul;
double kernel[] = {
(5204 + 2520 * mul + 1092 * mul2 + 3280 * mul3) / 12096,
(2943 -  210 * mul -  273 * mul2 - 2460 * mul3) / 12096,
( 486 -  924 * mul -  546 * mul2 +  984 * mul3) / 12096,
(  17 -  126 * mul +  273 * mul2 -  164 * mul3) / 12096,
};

    double mat_freq[13];
memcpy(mat_freq, kernel, sizeof(kernel));
memset(mat_freq + 4, 0, sizeof(mat_freq) - sizeof(kernel));
int n = 6;
coeff_filter(mat_freq, n, kernel);
for (int k = 0; k < 2 * prefilter; ++k)
coeff_blur121(mat_freq, ++n);

double vec_freq[13];
n = index[3] + prefilter + 3;
calc_gauss(vec_freq, n, r2);
memset(vec_freq + n + 1, 0, sizeof(vec_freq) - (n + 1) * sizeof(vec_freq[0]));
n -= 3;
coeff_filter(vec_freq, n, kernel);
for (int k = 0; k < prefilter; ++k)
coeff_blur121(vec_freq, --n);

double mat[4][4];
calc_matrix(mat, mat_freq, index);

double vec[4];
for (int i = 0; i < 4; ++i)
vec[i] = mat_freq[0] - mat_freq[index[i]] - vec_freq[0] + vec_freq[index[i]];

for (int i = 0; i < 4; ++i) {
double res = 0;
for (int j = 0; j < 4; ++j)
res += mat[i][j] * vec[j];
mu[i] = FFMAX(0, res);
}
}
