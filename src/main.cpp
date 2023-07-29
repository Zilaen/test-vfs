#include "VirtualFileSystem.h"

int main() {
    VirtualFileSystem vfs;

    std::thread thread1([&]() { // test our VFS
        vfs.Create("GameResources.pak", "Some Game resources.");

        size_t bytesWritten = vfs.Write("GameResources.pak", "Updated Game Resources.");
        std::cout << bytesWritten << " bytes written." << std::endl;

        size_t bytesRead = vfs.Read("GameResources.pak");
        std::cout << bytesRead << " bytes read." << std::endl;

        vfs.Close("GameResources.pak");
    });

    std::thread thread2([&]() {
        vfs.Create("Audio/GameAudio.pak", "Some Audio resources. \n");

        size_t bytesWritten = vfs.Write("Audio/GameAudio.pak", "Updated Audio Resources. \n");
        std::cout << bytesWritten << " bytes written." << std::endl;

        size_t bytesRead = vfs.Read("Audio/GameAudio.pak");
        std::cout << bytesRead << " bytes read." << std::endl;

        vfs.Close("Audio/GameAudio.pak");
    });

    thread1.join();
    thread2.join();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}