#ifndef OS_STDARG_H
#define OS_STDARG_H

/**
 * 可变参定义
 */
typedef char* va_list;
/**
 * 这一步先取到count的地址，再加上 sizeof(char *)，就是 p = 倒数第二个数据的地址
 */
#define va_start(p, count) (p = (va_list)&count + sizeof(char*))
/**
 * 因为  p = 倒数第二个数据的地址，故要取p指向的数据，需要将 p 的类型 char* 转换为 t*，再 * 取值
 * 再将 p = 上一个数据的地址
 * 这里的 sizeof(char *) 如果是 32 位系统就是 4 字节，这是正确的，因为在调用一个函数前要先将参数从右向左压栈
 * 无论传递的参数是 char 还是 int，push 操作都压入 4 字节
 */
#define va_arg(p, t) (*(t*)((p += sizeof(char*)) - sizeof(char*)))
#define va_end(p) (p = 0)

#endif //OS_STDARG_H
