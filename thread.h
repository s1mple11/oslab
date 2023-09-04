#define STACK_SIZE 1024 //�߳�����ջ��������
#define THR_TASKS 32    //����߳���

#define THREAD_RUNNING 0//״̬0:��������
#define THREAD_SLEEP 1  //״̬1:����
#define THREAD_READY 2  //״̬2:����
#define THREAD_EXIT 3   //״̬3:�˳�

//�߳̽ṹ��
struct thread_struct 
{
  int id; //�߳�id
  void (*thread_func)();//�̹߳��̺���
  int esp; // ջ��ָ��
  unsigned int wakeuptime; // �̻߳���ʱ��
  int status; // �߳�״̬
  int timeslice_counter; //ʱ��Ƭ������
  int priority; // ���ȼ�
  int stack[STACK_SIZE];//�߳�����ջ
};

int thread_create(int *thread_id,void (*start_routine)()); //�����߳�
int thread_join(int thread_id);//ǿ�Ƶ���
void thread_sleep(int seconds);//����