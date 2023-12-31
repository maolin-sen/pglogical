/*-------------------------------------------------------------------------
 *
 * pglogical_worker.h
 *              pglogical worker helper functions
 *
 * Copyright (c) 2015, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *              pglogical_worker.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PGLOGICAL_WORKER_H
#define PGLOGICAL_WORKER_H

#include "storage/lock.h"

#include "pglogical.h"


/*
用于表示 pglogical 扩展中的不同类型的后台工作进程。
PGLogicalWorkerType 枚举类型包含以下四个值和其对应的意义：

PGLOGICAL_WORKER_NONE：未使用的槽（slot）。这个值用于表示未被分配到任何后台工作进程的空闲槽。
PGLOGICAL_WORKER_MANAGER：管理者（Manager）。这个值用于表示执行与管理 pglogical 扩展相关的后台工作进程。
PGLOGICAL_WORKER_APPLY：应用（Apply）。这个值用于表示执行逻辑复制的后台工作进程，负责解析和应用逻辑复制消息以将数据从源数据库复制到目标数据库。
PGLOGICAL_WORKER_SYNC：特殊类型的应用（Apply），用于同步特定表的后台工作进程。这个值用于表示一个特殊的应用工作进程，专门用于同步特定的表。

通过使用这些不同的枚举值，可以标识和区分不同类型的 pglogical 后台工作进程，并在内部实现中进行相应的处理和调度。
*/
typedef enum {
	PGLOGICAL_WORKER_NONE,		/* Unused slot. */
	PGLOGICAL_WORKER_MANAGER,	/* Manager. */
	PGLOGICAL_WORKER_APPLY,		/* Apply. */
	PGLOGICAL_WORKER_SYNC		/* Special type of Apply that synchronizes one table. */
} PGLogicalWorkerType;

typedef struct PGLogicalApplyWorker
{
	Oid			subid;				/* Subscription id for apply worker. */
	bool		sync_pending;		/* Is there new synchronization info pending?. */
	XLogRecPtr	replay_stop_lsn;	/* Replay should stop here if defined. */
} PGLogicalApplyWorker;

typedef struct PGLogicalSyncWorker
{
	PGLogicalApplyWorker	apply; /* Apply worker info, must be first. */
	NameData	nspname;	/* Name of the schema of table to copy if any. */
	NameData	relname;	/* Name of the table to copy if any. */
} PGLogicalSyncWorker;

typedef struct PGLogicalWorker {
	PGLogicalWorkerType	worker_type;

	/* Generation counter incremented at each registration */
	uint16 generation;

	/* Pointer to proc array. NULL if not running. */
	PGPROC *proc;

	/* Time at which worker crashed (normally 0). */
	TimestampTz	crashed_at;

	/* Database id to connect to. */
	Oid		dboid;

	/* Type-specific worker info */
	union
	{
		PGLogicalApplyWorker apply;
		PGLogicalSyncWorker sync;
	} worker;

} PGLogicalWorker;

typedef struct PGLogicalContext {
	/* Write lock. */
	LWLock	   *lock;

	/* Supervisor process. */
	PGPROC	   *supervisor;

	/* Signal that subscription info have changed. */
	bool		subscriptions_changed;

	/* Background workers. */
	int			total_workers;
	PGLogicalWorker  workers[FLEXIBLE_ARRAY_MEMBER];
} PGLogicalContext;

extern PGLogicalContext		   *PGLogicalCtx;
extern PGLogicalWorker		   *MyPGLogicalWorker;
extern PGLogicalApplyWorker	   *MyApplyWorker;
extern PGLogicalSubscription   *MySubscription;

extern volatile sig_atomic_t got_SIGTERM;

extern void handle_sigterm(SIGNAL_ARGS);

extern void pglogical_subscription_changed(Oid subid, bool kill);

extern void pglogical_worker_shmem_init(void);

extern int pglogical_worker_register(PGLogicalWorker *worker);
extern void pglogical_worker_attach(int slot, PGLogicalWorkerType type);

extern PGLogicalWorker *pglogical_manager_find(Oid dboid);
extern PGLogicalWorker *pglogical_apply_find(Oid dboid, Oid subscriberid);
extern List *pglogical_apply_find_all(Oid dboid);

extern PGLogicalWorker *pglogical_sync_find(Oid dboid, Oid subid,
											const char *nspname, const char *relname);
extern List *pglogical_sync_find_all(Oid dboid, Oid subscriberid);

extern PGLogicalWorker *pglogical_get_worker(int slot);
extern bool pglogical_worker_running(PGLogicalWorker *w);
extern void pglogical_worker_kill(PGLogicalWorker *worker);

extern const char * pglogical_worker_type_name(PGLogicalWorkerType type);

#endif /* PGLOGICAL_WORKER_H */
