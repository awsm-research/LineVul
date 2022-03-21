void RunMemCheck() {
ACMRandom rnd(ACMRandom::DeterministicSeed());
const int count_test_block = 5000;
    DECLARE_ALIGNED_ARRAY(16, int16_t, input_block, kNumCoeffs);
    DECLARE_ALIGNED_ARRAY(16, int16_t, input_extreme_block, kNumCoeffs);
    DECLARE_ALIGNED_ARRAY(16, int16_t, output_ref_block, kNumCoeffs);
    DECLARE_ALIGNED_ARRAY(16, int16_t, output_block, kNumCoeffs);

for (int i = 0; i < count_test_block; ++i) {
      // Initialize a test block with input range [-255, 255].
for (int j = 0; j < kNumCoeffs; ++j) {
        input_block[j] = rnd.Rand8() - rnd.Rand8();
        input_extreme_block[j] = rnd.Rand8() % 2 ? 255 : -255;
}
      if (i == 0)
for (int j = 0; j < kNumCoeffs; ++j)
          input_extreme_block[j] = 255;
      if (i == 1)
for (int j = 0; j < kNumCoeffs; ++j)
          input_extreme_block[j] = -255;

fwd_txfm_ref(input_extreme_block, output_ref_block, pitch_, tx_type_);
      REGISTER_STATE_CHECK(RunFwdTxfm(input_extreme_block,
                                      output_block, pitch_));

// The minimum quant value is 4.
for (int j = 0; j < kNumCoeffs; ++j) {
EXPECT_EQ(output_block[j], output_ref_block[j]);
        EXPECT_GE(4 * DCT_MAX_VALUE, abs(output_block[j]))
            << "Error: 16x16 FDCT has coefficient larger than 4*DCT_MAX_VALUE";
}
}
}
