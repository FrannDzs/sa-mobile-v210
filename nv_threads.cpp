#include <mutex>

#include <nv_threads.h>
#include <log_client.h>

namespace saglobal {
    void (*g_NVThreadSpawnProc)(uintptr_t x0);
}

namespace samimic {
    // Count of created threads
    static uint8_t thCount{};
    static std::mutex nvLock{};

    using namespace saglobal;

    void NVThreadSpawnProc(uintptr_t x0)
    {
        std::unique_lock<std::mutex> nvWaitLocker(nvLock);
        salog::printFormat(salog::Info, "Threads: on (NVThreadSpawnProc: %u)", thCount);
        thCount++;

        nvWaitLocker.unlock();

        g_NVThreadSpawnProc(x0);
    }
}
