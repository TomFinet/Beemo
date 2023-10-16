#include <httpserver/threadpool.h>

namespace httpserver {

threadpool::threadpool()
{

}

threadpool::~threadpool()
{

}

void threadpool::submit(std::function<void()> &job);
{

}

}
