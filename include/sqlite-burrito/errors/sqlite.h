/**
 * @file   sqlite.h
 * @author Dennis Sitelew
 * @date   Jul. 01, 2020
 */
#ifndef INCLUDE_SQLITE_BURRITO_ERRORS_SQLITE_H
#define INCLUDE_SQLITE_BURRITO_ERRORS_SQLITE_H

#include <sqlite-burrito/library.h>
#include <sqlite-burrito/isolation/sqlite3.h>

#include <system_error>

namespace sqlite_burrito {
namespace errors {
namespace sqlite3 {

SQLITE_BURRITO_EXPORT enum class condition {
   ok = isolation::sqlite_ok,

   error = isolation::sqlite_error,
   internal = isolation::sqlite_internal,
   perm = isolation::sqlite_perm,
   abort = isolation::sqlite_abort,
   busy = isolation::sqlite_busy,
   locked = isolation::sqlite_locked,
   nomem = isolation::sqlite_nomem,
   readonly = isolation::sqlite_readonly,
   interrupt = isolation::sqlite_interrupt,
   ioerr = isolation::sqlite_ioerr,
   corrupt = isolation::sqlite_corrupt,
   notfound = isolation::sqlite_notfound,
   full = isolation::sqlite_full,
   cantopen = isolation::sqlite_cantopen,
   protocol = isolation::sqlite_protocol,
   empty = isolation::sqlite_empty,
   schema = isolation::sqlite_schema,
   toobig = isolation::sqlite_toobig,
   constraint = isolation::sqlite_constraint,
   mismatch = isolation::sqlite_mismatch,
   misuse = isolation::sqlite_misuse,
   nolfs = isolation::sqlite_nolfs,
   auth = isolation::sqlite_auth,
   format = isolation::sqlite_format,
   range = isolation::sqlite_range,
   notadb = isolation::sqlite_notadb,
   notice = isolation::sqlite_notice,
   warning = isolation::sqlite_warning,
   row = isolation::sqlite_row,
   done = isolation::sqlite_done,
};

SQLITE_BURRITO_EXPORT enum class error {
   ok = isolation::sqlite_ok,

   error_missing_collseq = isolation::sqlite_error_missing_collseq,
   error_retry = isolation::sqlite_error_retry,
   error_snapshot = isolation::sqlite_error_snapshot,
   ioerr_read = isolation::sqlite_ioerr_read,
   ioerr_short_read = isolation::sqlite_ioerr_short_read,
   ioerr_write = isolation::sqlite_ioerr_write,
   ioerr_fsync = isolation::sqlite_ioerr_fsync,
   ioerr_dir_fsync = isolation::sqlite_ioerr_dir_fsync,
   ioerr_truncate = isolation::sqlite_ioerr_truncate,
   ioerr_fstat = isolation::sqlite_ioerr_fstat,
   ioerr_unlock = isolation::sqlite_ioerr_unlock,
   ioerr_rdlock = isolation::sqlite_ioerr_rdlock,
   ioerr_delete = isolation::sqlite_ioerr_delete,
   ioerr_blocked = isolation::sqlite_ioerr_blocked,
   ioerr_nomem = isolation::sqlite_ioerr_nomem,
   ioerr_access = isolation::sqlite_ioerr_access,
   ioerr_checkreservedlock = isolation::sqlite_ioerr_checkreservedlock,
   ioerr_lock = isolation::sqlite_ioerr_lock,
   ioerr_close = isolation::sqlite_ioerr_close,
   ioerr_dir_close = isolation::sqlite_ioerr_dir_close,
   ioerr_shmopen = isolation::sqlite_ioerr_shmopen,
   ioerr_shmsize = isolation::sqlite_ioerr_shmsize,
   ioerr_shmlock = isolation::sqlite_ioerr_shmlock,
   ioerr_shmmap = isolation::sqlite_ioerr_shmmap,
   ioerr_seek = isolation::sqlite_ioerr_seek,
   ioerr_delete_noent = isolation::sqlite_ioerr_delete_noent,
   ioerr_mmap = isolation::sqlite_ioerr_mmap,
   ioerr_gettemppath = isolation::sqlite_ioerr_gettemppath,
   ioerr_convpath = isolation::sqlite_ioerr_convpath,
   ioerr_vnode = isolation::sqlite_ioerr_vnode,
   ioerr_auth = isolation::sqlite_ioerr_auth,
   ioerr_begin_atomic = isolation::sqlite_ioerr_begin_atomic,
   ioerr_commit_atomic = isolation::sqlite_ioerr_commit_atomic,
   ioerr_rollback_atomic = isolation::sqlite_ioerr_rollback_atomic,
   locked_sharedcache = isolation::sqlite_locked_sharedcache,
   locked_vtab = isolation::sqlite_locked_vtab,
   busy_recovery = isolation::sqlite_busy_recovery,
   busy_snapshot = isolation::sqlite_busy_snapshot,
   cantopen_notempdir = isolation::sqlite_cantopen_notempdir,
   cantopen_isdir = isolation::sqlite_cantopen_isdir,
   cantopen_fullpath = isolation::sqlite_cantopen_fullpath,
   cantopen_convpath = isolation::sqlite_cantopen_convpath,
   cantopen_dirtywal = isolation::sqlite_cantopen_dirtywal,
   corrupt_vtab = isolation::sqlite_corrupt_vtab,
   corrupt_sequence = isolation::sqlite_corrupt_sequence,
   readonly_recovery = isolation::sqlite_readonly_recovery,
   readonly_cantlock = isolation::sqlite_readonly_cantlock,
   readonly_rollback = isolation::sqlite_readonly_rollback,
   readonly_dbmoved = isolation::sqlite_readonly_dbmoved,
   readonly_cantinit = isolation::sqlite_readonly_cantinit,
   readonly_directory = isolation::sqlite_readonly_directory,
   abort_rollback = isolation::sqlite_abort_rollback,
   constraint_check = isolation::sqlite_constraint_check,
   constraint_commithook = isolation::sqlite_constraint_commithook,
   constraint_foreignkey = isolation::sqlite_constraint_foreignkey,
   constraint_function = isolation::sqlite_constraint_function,
   constraint_notnull = isolation::sqlite_constraint_notnull,
   constraint_primarykey = isolation::sqlite_constraint_primarykey,
   constraint_trigger = isolation::sqlite_constraint_trigger,
   constraint_unique = isolation::sqlite_constraint_unique,
   constraint_vtab = isolation::sqlite_constraint_vtab,
   constraint_rowid = isolation::sqlite_constraint_rowid,
   notice_recover_wal = isolation::sqlite_notice_recover_wal,
   notice_recover_rollback = isolation::sqlite_notice_recover_rollback,
   warning_autoindex = isolation::sqlite_warning_autoindex,
   auth_user = isolation::sqlite_auth_user,
   ok_load_permanently = isolation::sqlite_ok_load_permanently,
};

SQLITE_BURRITO_EXPORT extern const std::error_category &
sqlite3_error_category() noexcept;

inline SQLITE_BURRITO_EXPORT std::error_code make_error_code(error ec) {
   return {static_cast<int>(ec), sqlite3_error_category()};
}

SQLITE_BURRITO_EXPORT extern const std::error_category &
sqlite3_condition_category() noexcept;

inline SQLITE_BURRITO_EXPORT std::error_condition make_error_condition(
    condition ec) {
   return {static_cast<int>(ec), sqlite3_condition_category()};
}

} // namespace sqlite3
} // namespace errors
} // namespace sqlite_burrito

namespace std {

template <>
SQLITE_BURRITO_EXPORT struct is_error_code_enum<
    sqlite_burrito::errors::sqlite3::error> : true_type {};

template <>
SQLITE_BURRITO_EXPORT struct is_error_condition_enum<
    sqlite_burrito::errors::sqlite3::condition> : true_type {};

} // namespace std

#endif /* INCLUDE_SQLITE_BURRITO_ERRORS_SQLITE_H */

