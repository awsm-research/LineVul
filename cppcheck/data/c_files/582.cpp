void VarianceTest<VarianceFunctionType>::ZeroTest() {
for (int i = 0; i <= 255; ++i) {
    memset(src_, i, block_size_);
for (int j = 0; j <= 255; ++j) {
      memset(ref_, j, block_size_);
unsigned int sse;
unsigned int var;
      REGISTER_STATE_CHECK(var = variance_(src_, width_, ref_, width_, &sse));
      EXPECT_EQ(0u, var) << "src values: " << i << "ref values: " << j;
}
}
}
