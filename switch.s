.section .text
.global switch_to_next
switch_to_next:
	push %ebp
	mov %esp,%ebp /*����ջ֡��Ѱ�� */

	/* �����ֳ� */
	push %edi
	push %esi
	push %ebx
	push %edx
	push %ecx
	push %eax
	pushfl

	/* ׼���л�ջ*/
	mov current,%eax   /*���浱ǰ esp, eax �� current ��ַ */
	mov %esp,8(%eax)  
	mov 8(%ebp),%eax   /* ȡ��һ���߳̽ṹ���ַ*/
	mov %eax,current   /* ���� current*/
	mov 8(%eax),%esp   /* �л�����һ���̵߳�ջ*/


	/* �ѽ�����һ���̣߳��ָ��ֳ� */
	popfl
	popl %eax
	popl %edx
	popl %ecx
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret
