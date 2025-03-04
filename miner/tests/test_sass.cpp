#include "../sass/compiler.h"

TEST(SASSCompiler, BasicGeneration) {
    HashXCompiler compiler;
    hashx_program prog;
    // ... generate test program
    auto sass = compiler.compile(prog);
    ASSERT_GT(sass.size(), 1000);
}

TEST(HashXGPU, BasicExecution) {
    HashXCompiler compiler;
    // Create CPU context
    hashx_ctx* ctx = hashx_alloc(HASHX_COMPILED);
    const char* seed = "test";
    hashx_make(ctx, seed, strlen(seed));

    // Test nonce
    uint64_t nonce = 123456789;

    // Run test
    bool success = test_hashx_gpu(ctx, nonce);
    hashx_free(ctx);

    ASSERT_TRUE(success);
} 