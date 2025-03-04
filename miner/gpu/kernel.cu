#include <equix.h>
#include <hashx.h>
#include <rNdN.h>

__constant__ hashx_sass_binary* d_binaries; // Cache of compiled SASS
__constant__ uint32_t max_binaries;

__global__ void equix_gpu_solver(
    const equix_challenge* challenges,
    equix_solution* solutions,
    uint32_t num_challenges
) {
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx >= num_challenges) return;

    // Stage 0: Generate HashX function
    hashx_sass_binary* bin = &d_binaries[idx % max_binaries];
    uint64_t nonce = challenges[idx].nonce;
    uint64_t r[8];

    // Execute compiled HashX via SASS
    asm volatile (
        "{\n"
        ".reg .b32 %r_hashid;\n"
        "mov.u32 %r_hashid, %0;\n"
        "call.uni (%r_hashid), _hashx_sass_entry;\n"
        "}\n" 
        :: "r"(bin->id) 
        : "memory"
    );

    // Stage 1: Build hash table
    __shared__ struct {
        uint64_t indices[1 << 18]; // 1MB shared mem
        uint32_t count;
    } stage1;

    if(threadIdx.x == 0) stage1.count = 0;
    __syncthreads();

    for(uint32_t i = 0; i < (1 << 16); i++) {
        uint64_t h = hashx_exec_sass(bin, i);
        uint32_t bucket = (h >> 40) & 0xfff; // 12-bit bucket
        
        uint32_t pos = atomicAdd(&stage1.count, 1);
        stage1.indices[pos] = (bucket << 40) | (i & 0xfffff);
    }
    __syncthreads();

    // Stage 2: Find collisions
    __shared__ struct {
        uint64_t pairs[1 << 17];
        uint32_t count;
    } stage2;

    if(threadIdx.x == 0) stage2.count = 0;
    __syncthreads();

    for(uint32_t i = threadIdx.x; i < stage1.count; i += blockDim.x) {
        uint64_t entry = stage1.indices[i];
        uint32_t bucket = entry >> 40;
        
        // Linear probe matching (optimized for warp efficiency)
        for(uint32_t j = i + 1; j < stage1.count; j++) {
            if((stage1.indices[j] >> 40) == bucket) {
                uint32_t pos = atomicAdd(&stage2.count, 1);
                stage2.pairs[pos] = (entry << 20) | (stage1.indices[j] & 0xfffff);
            }
        }
    }
    __syncthreads();

    // Stage 3: Validate solutions
    for(uint32_t i = threadIdx.x; i < stage2.count; i += blockDim.x) {
        uint64_t pair = stage2.pairs[i];
        uint32_t idx1 = (pair >> 20) & 0xfffff;
        uint32_t idx2 = pair & 0xfffff;

        uint64_t h1 = hashx_exec_sass(bin, idx1);
        uint64_t h2 = hashx_exec_sass(bin, idx2);

        if((h1 + h2) & 0xffffffffffc00000) continue; // Partial sum check

        // Store valid solution
        uint32_t sol_idx = atomicAdd(&solutions->count, 1);
        if(sol_idx < EQUIX_MAX_SOLS) {
            solutions->sols[sol_idx].idx[0] = idx1;
            solutions->sols[sol_idx].idx[1] = idx2;
        }
    }
}

// Host-side management
struct GPUSolver {
    hashx_sass_binary* d_binaries;
    uint32_t bin_capacity;
    uint32_t bin_count;

    GPUSolver(size_t max_binaries) : bin_capacity(max_binaries) {
        cudaMalloc(&d_binaries, max_binaries * sizeof(hashx_sass_binary));
        cudaMemcpyToSymbol(::d_binaries, &d_binaries, sizeof(hashx_sass_binary*));
        cudaMemcpyToSymbol(::max_binaries, &max_binaries, sizeof(uint32_t));
    }

    void add_binary(const hashx_sass_binary& bin) {
        if(bin_count >= bin_capacity) {
            // LRU eviction
            cudaMemcpy(&d_binaries[bin_count % bin_capacity], &bin, 
                sizeof(hashx_sass_binary), cudaMemcpyHostToDevice);
        } else {
            cudaMemcpy(&d_binaries[bin_count], &bin, 
                sizeof(hashx_sass_binary), cudaMemcpyHostToDevice);
        }
        bin_count++;
    }

    void solve_batch(const equix_challenge* challenges, equix_solution* solutions, 
                     size_t count) {
        const int threads = 256;
        const int blocks = (count + threads - 1) / threads;
        
        equix_gpu_solver<<<blocks, threads>>>(challenges, solutions, count);
        cudaDeviceSynchronize();
    }

    ~GPUSolver() {
        cudaFree(d_binaries);
    }
};

// Test Kernel
__global__ void hashx_test_kernel(
    const hashx_sass_binary* bin,
    uint64_t nonce,
    uint8_t* output
) {
    // Execute compiled HashX
    asm volatile (
        "{\n"
        ".reg .b32 %r_hashid;\n"
        "mov.u32 %r_hashid, %0;\n"
        "call.uni (%r_hashid), _hashx_sass_entry;\n"
        "}\n" 
        :: "r"(bin->id) 
        : "memory"
    );

    // Store result
    if(threadIdx.x == 0) {
        *((uint64_t*)output) = nonce * 123; // Example hash computation
    }
}

// Verification Test
bool test_hashx_gpu(const hashx_ctx* cpu_ctx, uint64_t nonce) {
    // 1. Compile HashX program
    HashXCompiler compiler;
    auto sass_bin = compiler.compile(cpu_ctx->program);

    // 2. GPU Memory Setup
    hashx_sass_binary* d_bin;
    uint8_t* d_output;
    cudaMalloc(&d_bin, sizeof(hashx_sass_binary));
    cudaMalloc(&d_output, HASHX_SIZE);

    cudaMemcpy(d_bin, &sass_bin, sizeof(hashx_sass_binary), cudaMemcpyHostToDevice);

    // 3. Launch Kernel
    hashx_test_kernel<<<1, 1>>>(d_bin, nonce, d_output);
    cudaDeviceSynchronize();

    // 4. Get Result
    uint8_t gpu_hash[HASHX_SIZE];
    cudaMemcpy(gpu_hash, d_output, HASHX_SIZE, cudaMemcpyDeviceToHost);

    // 5. CPU Verification 
    uint8_t cpu_hash[HASHX_SIZE];
    hashx_exec(cpu_ctx, nonce, cpu_hash);

    // 6. Cleanup
    cudaFree(d_bin);
    cudaFree(d_output);

    return memcmp(gpu_hash, cpu_hash, HASHX_SIZE) == 0;
} 