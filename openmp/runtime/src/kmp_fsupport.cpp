/*
 * kmp_fsupport.cpp
 */

/*!
 * @file   kmp_fsupport.cpp
 * @brief  wrapper for Fujitsu compiler
 * @author Fujitsu Limited.
 * @defgroup FJOMP_WRAPPER Fujitsu Wrapper
 */

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//

#include "kmp.h"
#include "kmp_atomic.h"

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*  __mpc* interfaces are called by Fortran
 *  __jwe* interfaces are called by C/C++
 */

/* ツールとの連携インタフェース */
/* __jwe_pversionが宣言されていない : 旧版 */
/* __jwe_pversion=1                 : PetaV1.0〜 */
/* バリアルーチンの増減がある場合、__jwe_pversionの値変更とツールへの通知を行う */
/* ツールは、__jwe_pversionの値でバリアルーチンテーブルを変更する */

int __jwe_pversion = 1 ;

#pragma weak __jwe_compiler_OMP
extern void __jwe_compiler_OMP() ;

/* Debug Code*/

//#define FJOMP_DEBUG
  
#if defined(FJOMP_DEBUG)
#define FJOMP_DEBUG_PRINT(...) FJOMP_DEBUG_PRINT_BODY(__VA_ARGS__, "")
#define FJOMP_DEBUG_PRINT_BODY(format, ...)                                    \
  fprintf(stderr, "%s:%d, MAX:%d, ID:%d, " format "%s\n", __func__, __LINE__,  \
          __kmp_FJOMP_get_num_threads(), __kmp_FJOMP_get_thread_num(),         \
          __VA_ARGS__)
#else
#define FJOMP_DEBUG_PRINT(...)
#endif

#if 0
#define FJOMP_DEBUG_PRINT_NO_THREADS_INFO(...)                                 \
  FJOMP_DEBUG_PRINT_NO_THREADS_INFO_BODY(__VA_ARGS__, "")
#define FJOMP_DEBUG_PRINT_NO_THREADS_INFO_BODY(format, ...)                    \
  fprintf(stderr, "%s:%d, " format "%s\n", __func__, __LINE__, __VA_ARGS__)
#else
#define FJOMP_DEBUG_PRINT_NO_THREADS_INFO(...)
#endif

#define MKLOC(loc, routine)                                                    \
  static ident_t(loc) = {0, KMP_IDENT_KMPC, 0, 0, ";unknown;unknown;0;0;;"};

/* prototype */

typedef void (*FJOMP_parallel_entry_t)(void *, long, long, void *, void *);

/* internal functions */

#ifndef KMP_DEBUG
static
#endif /* KMP_DEBUG */
    void
    __kmp_FJOMP_microtask_wrapper(int *, int *, FJOMP_parallel_entry_t, void *);

static int __kmp_FJOMP_get_num_threads(void);
static int __kmp_FJOMP_get_thread_num(void);

/* end prototype */

/*!
 * @brief   Fujitsu Profiler needs the weak symbol of
 *          __jwe_compiler_OMP, so use the symbol in
 *          this dummy routine
 * @ingroup FJOMP_WRAPPER
 * @return  nothing
 */

void __jwe_libomp_exist(void){
  if(__jwe_compiler_OMP)
    __jwe_compiler_OMP();
}

/*!
 * @brief   returns global thread id
 * @ingroup FJOMP_WRAPPER
 * @param   lib_inf    no use
 */
  
int __jwe_ptnm_(void) {
  FJOMP_DEBUG_PRINT("");
  int gtid = __kmp_entry_gtid();
  return gtid;
} /* end __jwe_ptnm_ */

int __jwe_ptnm(void) {
  FJOMP_DEBUG_PRINT("");
  int gtid = __kmp_entry_gtid();
  return gtid;
} /* end __jwe_ptnm */

int jwe_ptnm_(void) {
  FJOMP_DEBUG_PRINT("");
  int gtid = __kmp_entry_gtid();
  return gtid;
} /* end jwe_ptnm */

/*!
 * @brief   returns global thread id
 * @ingroup FJOMP_WRAPPER
 * @param   lib_inf    no use
 */

int __mpc_ptnm(void) {
  int gtid = __kmp_entry_gtid();

  FJOMP_DEBUG_PRINT("");
  KA_TRACE(20, ("__mpc_ptnm: T#%d\n", gtid));

  return __jwe_ptnm_();
} /* end __mpc_ptnm*/

/*!
 * @brief   __jwe_opar is called from parallel construct and fork.
 * @ingroup FJOMP_WRAPPER
 * @param   task           the pointer to callback routine consisting of
 * outlined parallel construct
 * @param   data           the data to pass to the above callback routine.
 * @param   flag           the flag set when "if clause", "num_thread clause",and/or "proc_bind clause". 
 *                         0th-bit (least significant bit) is 1, if "if clause" is evaluates as false.
 *                         1st-bit (next to the least significant bit) is 1, if "num_threads clause" exists.
 *                         3rd to 2nd bit is 0x0, if no "proc_bind clause" exists.
 *                         3rd to 2nd bit is 0x1, if master is specified by "proc_bind clause".
 *                         3rd to 2nd bit is 0x2, if close is specified by "proc_bind clause".
 *                         3rd to 2nd bit is 0x3, if spread is specified by "proc_bind clause".
 * @param   in_num_threads the number of threads specified by "num_threads
 * clause"
 * @return  nothing
 */

void __jwe_opar(FJOMP_parallel_entry_t task, void *data, long flag,
                long in_num_threads) {

  kmp_proc_bind_t proc_bind_clause;
  unsigned num_threads_clause;
  int gtid = __kmp_entry_gtid();
  long num_threads, thread_id;
  //kmp_info_t *this_thr = __kmp_threads[gtid];

  FJOMP_DEBUG_PRINT("gtid = %d", gtid);

  MKLOC(loc, "__jwe_opar");
  KA_TRACE(20, ("__jwe_opar: T#%d\n", gtid));

  /*analyze "if clause"
    if(flag & 0x1 == 1)             "if clause" is evaluated as false
    if(flag & 0x2 == 1)             "num_threads clause" exists
  */
  if (flag & 0x2) {
    /* "num_threads clause" exists*/
    num_threads_clause = in_num_threads;
  } else {
    num_threads_clause = 0;
  }

  /*analyze "proc_bind clause"
    if((flag >> 2) & 0x03 == 0x0 )  no "proc_bind clause" exists.
    if((flag >> 2) & 0x03 == 0x1 )  master is specified by "proc_bind clause"
    if((flag >> 2) & 0x03 == 0x2 )  close  is specified by "proc_bind clause"
    if((flag >> 2) & 0x03 == 0x3 )  spread is specified by "proc_bind clause"
  */
  switch ((flag >> 2) & 0x03) {
  case 0:
    proc_bind_clause = proc_bind_default;
    break;

  case 1:
    proc_bind_clause = proc_bind_primary;
    break;

  case 2:
    proc_bind_clause = proc_bind_close;
    break;

  case 3:
    proc_bind_clause = proc_bind_spread;
    break;

  default:
    proc_bind_clause = proc_bind_default;
    break;
  }

  if ((flag & 0x1) == 0 && num_threads_clause != 1) {
    /* generate 2 or more threads*/

    if (num_threads_clause != 0) {
      __kmpc_push_num_threads(&loc, gtid, num_threads_clause);
    }

    if (proc_bind_clause != proc_bind_default) {
      __kmpc_push_proc_bind(&loc, gtid, (kmp_proc_bind_t)proc_bind_clause);
    }

    // __kmp_FJOMP_fork_call(&loc, gtid, task,
    // 					  (microtask_t)__kmp_FJOMP_microtask_wrapper, 2, task,
    // data);
    __kmpc_fork_call(&loc, 2, (microtask_t)__kmp_FJOMP_microtask_wrapper, task,
                     data);
  } else {
    /* serialized parallel */
    __kmpc_serialized_parallel(&loc, gtid);

    num_threads = __kmp_FJOMP_get_num_threads();
    thread_id = __kmp_FJOMP_get_thread_num();

    /* execute inside the parallel constructs by master threads */
    task(data, num_threads, thread_id, 0, 0);
    //task(data, num_threads, thread_id, this_thr, 0);

    __kmpc_end_serialized_parallel(&loc, gtid);
  }
} /* end __jwe_opar */

/*!
 * @brief   wrapper for __jwe_opar called by C/C++ parallel construct.
 * @ingroup FJOMP_WRAPPER
 See @ref __jwe_opar
 */

void __mpc_opar(FJOMP_parallel_entry_t task, void *data, long flag,
                long in_num_threads) {

  kmp_proc_bind_t proc_bind_clause;
  unsigned num_threads_clause;
  int gtid = __kmp_entry_gtid();
  long num_threads, thread_id;

  FJOMP_DEBUG_PRINT("gtid = %d", gtid);

  MKLOC(loc, "__mpc_opar");
  KA_TRACE(20, ("__mpc_opar: T#%d\n", gtid));

  /*analyze "if clause"
    if(flag & 0x1 == 1)             "if clause" is evaluated as false
    if(flag & 0x2 == 1)             "num_threads clause" exists
  */
  if (flag & 0x2) {
    /* "num_threads clause" exists*/
    num_threads_clause = in_num_threads;
  } else {
    num_threads_clause = 0;
  }

  /*analyze "proc_bind clause"
    if((flag >> 2) & 0x03 == 0x0 )  no "proc_bind clause" exists.
    if((flag >> 2) & 0x03 == 0x1 )  master is specified by "proc_bind clause"
    if((flag >> 2) & 0x03 == 0x2 )  close  is specified by "proc_bind clause"
    if((flag >> 2) & 0x03 == 0x3 )  spread is specified by "proc_bind clause"
  */
  switch ((flag >> 2) & 0x03) {
  case 0:
    proc_bind_clause = proc_bind_default;
    break;

  case 1:
    proc_bind_clause = proc_bind_primary;
    break;

  case 2:
    proc_bind_clause = proc_bind_close;
    break;

  case 3:
    proc_bind_clause = proc_bind_spread;
    break;

  default:
    proc_bind_clause = proc_bind_default;
    break;
  }
  if ((flag & 0x1) == 0 && num_threads_clause != 1) {
    /* generate 2 or more threads*/

    if (num_threads_clause != 0) {
      __kmpc_push_num_threads(&loc, gtid, num_threads_clause);
    }

    if (proc_bind_clause != proc_bind_default) {
      __kmpc_push_proc_bind(&loc, gtid, (kmp_proc_bind_t)proc_bind_clause);
    }
    __kmpc_fork_call(&loc, 2, (microtask_t)__kmp_FJOMP_microtask_wrapper, task,
                     data);
  } else {
    /* serialized parallel */
    __kmpc_serialized_parallel(&loc, gtid);

    num_threads = __kmp_FJOMP_get_num_threads();
    thread_id = __kmp_FJOMP_get_thread_num();

    /* execute inside the parallel constructs by master threads */
    task(data, num_threads, thread_id, 0, 0);

    __kmpc_end_serialized_parallel(&loc, gtid);
  }
} /* end __mpc_opar  */


/*!
 * @brief   helper routine for microtask wrapper
 * @ingroup FJOMP_WRAPPER
 * @param   gtid           tne global thread number
 * @param   npr            no use
 * @param   task           the pointer to callback routine consisting of
 * outlined parallel construct
 * @param   data           the data to pass to the above routine
 * @return  nothing
 */

#ifndef KMP_DEBUG
static
#endif /* KMP_DEBUG */
    void
    __kmp_FJOMP_microtask_wrapper(int *gtid, int *npr,
                                  FJOMP_parallel_entry_t task, void *data) {
  long num_threads, thread_id;
  int gtid_plus_1 = *gtid + 1;

  FJOMP_DEBUG_PRINT("");

  num_threads = __kmp_FJOMP_get_num_threads();
  thread_id = __kmp_FJOMP_get_thread_num();

  task(data, num_threads, thread_id, (void *)gtid_plus_1, 0);

} /* end __kmp_FJOMP_microtask_wrapper */

/*!
 * @brief   helper routine returns number of threads in the parallel region
 * @ingroup FJOMP_WRAPPER
 * @return  number of threads
 */

static int __kmp_FJOMP_get_num_threads(void) {
  FJOMP_DEBUG_PRINT_NO_THREADS_INFO("");
#ifdef KMP_STUB
  return 1;
#else
  // __kmpc_bound_num_threads initializes the library if needed
  return __kmpc_bound_num_threads(NULL);
#endif
} /* end __kmp_FJOMP_get_num_threads */

/*!
 * @brief   helper routine returns thread id in the parallel region
 * @ingroup FJOMP_WRAPPER
 * @return  thread id
 */

static int __kmp_FJOMP_get_thread_num(void) {
  FJOMP_DEBUG_PRINT_NO_THREADS_INFO("");
#ifdef KMP_STUB
  return 0;
#else
  int gtid;

#if KMP_OS_DARWIN || KMP_OS_FREEBSD || KMP_OS_NETBSD
  gtid = __kmp_entry_gtid();
#elif KMP_OS_WINDOWS
  if (!__kmp_init_parallel ||
      (gtid = (int)((kmp_intptr_t)TlsGetValue(__kmp_gtid_threadprivate_key))) ==
          0) {
    // Either library isn't initialized or thread is not registered
    // 0 is the correct TID in this case
    return 0;
  }
  --gtid; // We keep (gtid+1) in TLS
#elif KMP_OS_LINUX
#ifdef KMP_TDATA_GTID
  if (__kmp_gtid_mode >= 3) {
    if ((gtid = __kmp_gtid) == KMP_GTID_DNE) {
      return 0;
    }
  } else {
#endif
    if (!__kmp_init_parallel || (gtid = (kmp_intptr_t)(pthread_getspecific(
                                     __kmp_gtid_threadprivate_key))) == 0) {
      return 0;
    }
    --gtid;
#ifdef KMP_TDATA_GTID
  }
#endif
#else
#error Unknown or unsupported OS
#endif

  return __kmp_tid_from_gtid(gtid);
#endif
} /* end __kmp_FJOMP_get_thread_num */


/* __kmpから始まる同期待ち関数リスト */
const char *barrier_lists[] = {
  "__kmpc_barrier",
  "__kmp_barrier",
  "__kmp_linear_barrier_gather",
  "__kmp_linear_barrier_gather_cancellable",
  "__kmp_linear_barrier_release",
  "__kmp_linear_barrier_release_cancellable",
  "__kmp_tree_barrier_gather",
  "__kmp_tree_barrier_release",
  "__kmp_hyper_barrier_gather",
  "__kmp_hyper_barrier_release",
  "__kmp_init_hierarchical_barrier_thread",
  "__kmp_hierarchical_barrier_gather",
  "__kmp_hierarchical_barrier_release",
  "__kmp_end_split_barrier",
  "__kmp_fork_barrier",
  "__kmp_join_barrier",
  NULL
};

int __PROF_Get_BarrierName(const char ***bname_str, int *bname_num){
  *bname_str = barrier_lists;
  *bname_num = sizeof(barrier_lists) / sizeof(char *) - 1;
  if(*bname_num < 0){
    return 1;
  }
  return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
