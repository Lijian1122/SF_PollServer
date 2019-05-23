#include <handy/handy.h>
using namespace handy;

int main(int argc, const char *argv[]) {
    EventBase base;
    Signal::signal(SIGINT, [&] { base.exit(); });
    //info("program begin");
    //TimerId ID1 =  base.runAfter(200, []() { info("a task in runAfter 200ms");  });
    TimerId ID1  = base.runAfter(100, []() { info("task100 in runAfter 100ms interval 1000ms"); }, 10000);
    TimerId ID2 =  base.runAfter(200, []() { info("task200 in runAfter 200ms interval 2000ms");  }, 20000 );
    //base.runAfter(300, []() { info("task300 in runAfter 100ms interval 3000ms"); }, 3000);
    //TimerId id = base.runAt(time(NULL) * 1000 + 300, []() { info("a task in runAt now+300 interval 500ms"); }, 500);
    //base.runAfter(2000, [&]() {
    //    info("cancel task of interval 500ms");
    //    base.cancel(ID1);
    //});

    base.runAfter(20000, [&]() {
        info("cancel task of interval 5000ms");
        base.cancel(ID1);
    });

    base.runAfter(40000, [&]() {
        info("cancel task of interval 8000ms");
        base.cancel(ID2);
    });
    base.runAfter(50000, [&]() { base.exit(); });
    base.loop();
}
