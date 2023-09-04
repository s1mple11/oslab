#define STACK_SIZE 1024 //线程运行栈的最大深度
#define THR_TASKS 32    //最大线程数

#define THREAD_RUNNING 0//状态0:正在运行
#define THREAD_SLEEP 1  //状态1:休眠
#define THREAD_READY 2  //状态2:就绪
#define THREAD_EXIT 3   //状态3:退出

//线程结构体
struct thread_struct 
{
  int id; //线程id
  void (*thread_func)();//线程过程函数
  int esp; // 栈顶指针
  unsigned int wakeuptime; // 线程唤醒时间
  int status; // 线程状态
  int timeslice_counter; //时间片计数器
  int priority; // 优先级
  int stack[STACK_SIZE];//线程运行栈
};

int thread_create(int *thread_id,void (*start_routine)()); //创建线程
int thread_join(int thread_id);//强制调用
void thread_sleep(int seconds);//休眠