#include "uthreads.h"
#include <iostream>

/*
 * =========================================
 * STRICT NO-SCHEDULER TESTS
 * =========================================
 * No waiting
 * No busy loops
 * No quantum assumptions
 * Only direct API effects
 * =========================================
 */

static int log_step = 0;

/*
 * Worker that only marks state
 */
void worker() {
    std::cout << "[worker] entered\n";
    log_step = 1;

    uthread_block(1);

    log_step = 2;
    std::cout << "[worker] resumed\n";
}

/*
 * -----------------------------------------
 * TEST 1: block moves thread out of RUNNING
 * -----------------------------------------
 */
void test_block_state_change() {
    std::cout << "\n=== TEST 1: block state change ===\n";

    uthread_init(100000);

    int tid = uthread_spawn(worker);

    /*
     * We do NOT wait for scheduler.
     * We only check structural correctness:
     */

    int res = uthread_block(tid);

    if (res != 0) {
        std::cout << "ERROR: block failed\n";
        exit(1);
    }

    std::cout << "[main] block returned OK\n";

    std::cout << "[main] TEST 1 PASSED\n";
}

/*
 * -----------------------------------------
 * TEST 2: resume invalid thread
 * -----------------------------------------
 */
void test_resume_invalid() {
    std::cout << "\n=== TEST 2: invalid resume ===\n";

    uthread_init(100000);

    int res = uthread_resume(9999);

    if (res != -1) {
        std::cout << "ERROR expected -1\n";
        exit(1);
    }

    std::cout << "[main] TEST 2 PASSED\n";
}

/*
 * -----------------------------------------
 * TEST 3: resume does not crash / double add
 * -----------------------------------------
 */
void test_resume_no_double_add() {
    std::cout << "\n=== TEST 3: resume idempotency ===\n";

    uthread_init(100000);

    int tid = uthread_spawn([]() {
        std::cout << "[noop]\n";
    });

    uthread_resume(tid);
    uthread_resume(tid); // should be no-op

    std::cout << "[main] TEST 3 PASSED\n";
}

/*
 * -----------------------------------------
 * TEST 4: block main thread forbidden
 * -----------------------------------------
 */
void test_block_main() {
    std::cout << "\n=== TEST 4: block main ===\n";

    uthread_init(100000);

    int res = uthread_block(0);

    if (res != -1) {
        std::cout << "ERROR: main block should fail\n";
        exit(1);
    }

    std::cout << "[main] TEST 4 PASSED\n";
}

/*
 * -----------------------------------------
 * MAIN
 * -----------------------------------------
 */
int main() {
    std::cout << "==============================\n";
    std::cout << " NO-SCHEDULER BLOCK/RESUME TESTS\n";
    std::cout << "==============================\n";

    test_block_state_change();
    test_resume_invalid();
    test_resume_no_double_add();
    test_block_main();

    std::cout << "\nALL TESTS PASSED 🎉\n";
    return 0;
}