// 本文件由洪旭耀设计，可以通过QQ联系作者：26750452
#include "tickproc.h"
#include "target.h"

static TickNodeType* pList = 0;

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: TICK_Exec
//| 功能描述 |: 执行遍历
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |: 通常在tick中断内调用
//|          |:
////////////////////////////////////////////////////////////////////////////////
void TICK_Exec(void)
{
  TickNodeType* node;
  void (*pf)(void);
  CPU_SR_DECL;

  CPU_ENTER_CRITICAL();
  node = pList;
  while (node) {
    pf = node->callback;
    if (pf) {
      node = node->next;
      CPU_EXIT_CRITICAL();
      (*pf)();
      CPU_ENTER_CRITICAL();
    } else {
      node = node->next;
    }
  }
  CPU_EXIT_CRITICAL();
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: TICK_Add
//| 功能描述 |: 添加节点
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |: 千万不能重复添加同一个节点！
//|          |:
////////////////////////////////////////////////////////////////////////////////
void TICK_Add(TickNodeType* node)
{
  CPU_SR_DECL;

  CPU_ENTER_CRITICAL();
  node->next = pList;
  pList = node;
  CPU_EXIT_CRITICAL();
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: TICK_Del
//| 功能描述 |: 删除节点
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void TICK_Del(TickNodeType* node)
{
  TickNodeType* cur, *pre;
  CPU_SR_DECL;

  pre = NULL;
  CPU_ENTER_CRITICAL();
  cur = pList;
  while (cur) {
    if (cur == node) {
      if (pre) {
        pre->next = cur->next;
      } else {
        pList = cur->next;
      }
      cur->next = NULL;
      break;
    }
    pre = cur;
    cur = cur->next;
  }
  CPU_EXIT_CRITICAL();
}

////////////////////////////////////////////////////////////////////////////////

