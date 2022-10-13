#include <mutex>
#include <condition_variable>

std::condition_variable cv;
std::mutex mtx;
char cur_char = 'a';

void func(char ch) {
    for (int i = 0; i < 100; ++i) {
        std::unique_lock lock(mtx);
        cv.wait(lock, [&]() { return cur_char == ch; });
        putchar(ch);
        if (cur_char == 'z') cur_char = 'a'; else ++cur_char;
        cv.notify_all();
    }
}
