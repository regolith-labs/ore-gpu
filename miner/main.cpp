GPUSolver gpu_solver(1000); // Cache 1000 binaries

void mining_loop() {
    while(true) {
        auto challenges = get_challenges();
        equix_solution solutions;
        
        // Compile and cache HashX programs
        for(auto& chal : challenges) {
            hashx_program prog;
            hashx_make(ctx, chal.seed, &prog);
            auto sass = compiler.compile(prog);
            gpu_solver.add_binary(sass);
        }
        
        // Solve batch
        gpu_solver.solve_batch(challenges.data(), &solutions, challenges.size());
        
        // Process solutions
        submit_solutions(solutions);
    }
}
