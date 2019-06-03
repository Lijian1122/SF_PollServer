#include <string>
#include <string.h>
#include <stdio.h>
#include <ThreadPool.h>

using namespace std;

void add(int a ,int b)
{
   printf("a+b:%d %d %d  %ld\n",a, b,  a+b, pthread_self());
}

void sub(int a ,int b)
{
   printf("a-b:%d %d %d  %ld\n", a, b ,a-b, pthread_self());
}

int main()
{
   CThreadPool  pool;

   pool.Init(3);

   for(int i = 0 ; i< 5; i++)
   {
      pool.AddTask(std::bind(add, i, i+2));
   }

   for(int i = 0 ; i< 5; i++)
   {
      pool.AddTask(std::bind(sub, i+2, i));
   }

   sleep(10);
   pool.Destory();
   return 0;
}
