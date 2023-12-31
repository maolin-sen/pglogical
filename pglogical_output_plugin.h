/*-------------------------------------------------------------------------
 *
 * pglogical_output_plugin.h
 *		pglogical output plugin
 *
 * Copyright (c) 2015, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *		pglogical_output_plugin.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_LOGICAL_OUTPUT_PLUGIN_H
#define PG_LOGICAL_OUTPUT_PLUGIN_H

#include "nodes/pg_list.h"
#include "nodes/primnodes.h"

/* summon cross-PG-version compatibility voodoo */
#include "pglogical_compat.h"
/*
pglogical 插件的内部数据结构 PGLogicalOutputData 的定义。这个结构用于存储 pglogical 插件的状态和配置信息。以下是每个字段的含义：

context: 这是一个内存上下文，用于分配临时内存。

api: 这是一个指向 PGLogicalProtoAPI 结构的指针，该结构提供了一组函数，用于处理逻辑复制协议的各种操作。

local_node_id: 这是本地节点的 ID。

allow_internal_basetypes, allow_binary_basetypes, forward_changeset_origins, field_datum_encoding: 这些是协议相关的配置选项。

client_pg_version, client_max_proto_version, client_min_proto_version, client_expected_encoding, client_protocol_format, client_binary_basetypes_major_version, client_want_internal_basetypes_set, client_want_internal_basetypes, client_want_binary_basetypes_set, client_want_binary_basetypes, client_binary_bigendian_set, client_binary_bigendian, client_binary_sizeofdatum, client_binary_sizeofint, client_binary_sizeoflong, client_binary_float4byval_set, client_binary_float4byval, client_binary_float8byval_set, client_binary_float8byval, client_binary_intdatetimes_set, client_binary_intdatetimes, client_no_txinfo: 这些是客户端提供的信息和请求的配置选项。

forward_origins: 这是一个列表，包含了要转发的源的名称。

replication_sets: 这是一个列表，包含了复制集的信息。每个复制集都是一个 PGLogicalRepSet 结构，包含了复制集的名称和其他信息。

replicate_only_table: 这是一个 RangeVar 结构，表示只复制的表的名称和模式。
*/
/* typedef appears in pglogical_output_plugin.h */
typedef struct PGLogicalOutputData
{
	MemoryContext context;

	struct PGLogicalProtoAPI *api;

	/* Cached node id */
	Oid			local_node_id;

	/* protocol */
	bool		allow_internal_basetypes;
	bool		allow_binary_basetypes;
	bool		forward_changeset_origins;
	int			field_datum_encoding;

	/*
	 * client info
	 *
	 * Lots of this should move to a separate shorter-lived struct used only
	 * during parameter reading, since it contains what the client asked for.
	 * Once we've processed this during startup we don't refer to it again.
	 */
	uint32		client_pg_version;
	uint32		client_max_proto_version;
	uint32		client_min_proto_version;
	const char *client_expected_encoding;
	const char *client_protocol_format;
	uint32		client_binary_basetypes_major_version;
	bool		client_want_internal_basetypes_set;
	bool		client_want_internal_basetypes;
	bool		client_want_binary_basetypes_set;
	bool		client_want_binary_basetypes;
	bool		client_binary_bigendian_set;
	bool		client_binary_bigendian;
	uint32		client_binary_sizeofdatum;
	uint32		client_binary_sizeofint;
	uint32		client_binary_sizeoflong;
	bool		client_binary_float4byval_set;
	bool		client_binary_float4byval;
	bool		client_binary_float8byval_set;
	bool		client_binary_float8byval;
	bool		client_binary_intdatetimes_set;
	bool		client_binary_intdatetimes;
	bool		client_no_txinfo;

	/* List of origin names */
    List	   *forward_origins;
	/* List of PGLogicalRepSet */
	List	   *replication_sets;
	RangeVar   *replicate_only_table;
} PGLogicalOutputData;

#endif /* PG_LOGICAL_OUTPUT_PLUGIN_H */
