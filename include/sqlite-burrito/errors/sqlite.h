/**
 * @file   sqlite.h
 * @author Dennis Sitelew
 * @date   Jul. 01, 2020
 */
#ifndef INCLUDE_SQLITE_BURRITO_ERRORS_SQLITE_H
#define INCLUDE_SQLITE_BURRITO_ERRORS_SQLITE_H

#include <sqlite-burrito/library.h>
#include <sqlite3.h>

#include <system_error>

namespace sqlite_burrito {
namespace errors {
namespace sqlite3 {

SQLITE_BURRITO_EXPORT enum class condition {
   ok = SQLITE_OK,

   error = SQLITE_ERROR,
   internal = SQLITE_INTERNAL,
   perm = SQLITE_PERM,
   abort = SQLITE_ABORT,
   busy = SQLITE_BUSY,
   locked = SQLITE_LOCKED,
   nomem = SQLITE_NOMEM,
   readonly = SQLITE_READONLY,
   interrupt = SQLITE_INTERRUPT,
   ioerr = SQLITE_IOERR,
   corrupt = SQLITE_CORRUPT,
   notfound = SQLITE_NOTFOUND,
   full = SQLITE_FULL,
   cantopen = SQLITE_CANTOPEN,
   protocol = SQLITE_PROTOCOL,
   empty = SQLITE_EMPTY,
   schema = SQLITE_SCHEMA,
   toobig = SQLITE_TOOBIG,
   constraint = SQLITE_CONSTRAINT,
   mismatch = SQLITE_MISMATCH,
   misuse = SQLITE_MISUSE,
   nolfs = SQLITE_NOLFS,
   auth = SQLITE_AUTH,
   format = SQLITE_FORMAT,
   range = SQLITE_RANGE,
   notadb = SQLITE_NOTADB,
   notice = SQLITE_NOTICE,
   warning = SQLITE_WARNING,
   row = SQLITE_ROW,
   done = SQLITE_DONE,
};

SQLITE_BURRITO_EXPORT enum class error {
   ok = SQLITE_OK,

   error_missing_collseq = SQLITE_ERROR_MISSING_COLLSEQ,
   error_retry = SQLITE_ERROR_RETRY,
   error_snapshot = SQLITE_ERROR_SNAPSHOT,
   ioerr_read = SQLITE_IOERR_READ,
   ioerr_short_read = SQLITE_IOERR_SHORT_READ,
   ioerr_write = SQLITE_IOERR_WRITE,
   ioerr_fsync = SQLITE_IOERR_FSYNC,
   ioerr_dir_fsync = SQLITE_IOERR_DIR_FSYNC,
   ioerr_truncate = SQLITE_IOERR_TRUNCATE,
   ioerr_fstat = SQLITE_IOERR_FSTAT,
   ioerr_unlock = SQLITE_IOERR_UNLOCK,
   ioerr_rdlock = SQLITE_IOERR_RDLOCK,
   ioerr_delete = SQLITE_IOERR_DELETE,
   ioerr_blocked = SQLITE_IOERR_BLOCKED,
   ioerr_nomem = SQLITE_IOERR_NOMEM,
   ioerr_access = SQLITE_IOERR_ACCESS,
   ioerr_checkreservedlock = SQLITE_IOERR_CHECKRESERVEDLOCK,
   ioerr_lock = SQLITE_IOERR_LOCK,
   ioerr_close = SQLITE_IOERR_CLOSE,
   ioerr_dir_close = SQLITE_IOERR_DIR_CLOSE,
   ioerr_shmopen = SQLITE_IOERR_SHMOPEN,
   ioerr_shmsize = SQLITE_IOERR_SHMSIZE,
   ioerr_shmlock = SQLITE_IOERR_SHMLOCK,
   ioerr_shmmap = SQLITE_IOERR_SHMMAP,
   ioerr_seek = SQLITE_IOERR_SEEK,
   ioerr_delete_noent = SQLITE_IOERR_DELETE_NOENT,
   ioerr_mmap = SQLITE_IOERR_MMAP,
   ioerr_gettemppath = SQLITE_IOERR_GETTEMPPATH,
   ioerr_convpath = SQLITE_IOERR_CONVPATH,
   ioerr_vnode = SQLITE_IOERR_VNODE,
   ioerr_auth = SQLITE_IOERR_AUTH,
   ioerr_begin_atomic = SQLITE_IOERR_BEGIN_ATOMIC,
   ioerr_commit_atomic = SQLITE_IOERR_COMMIT_ATOMIC,
   ioerr_rollback_atomic = SQLITE_IOERR_ROLLBACK_ATOMIC,
   locked_sharedcache = SQLITE_LOCKED_SHAREDCACHE,
   locked_vtab = SQLITE_LOCKED_VTAB,
   busy_recovery = SQLITE_BUSY_RECOVERY,
   busy_snapshot = SQLITE_BUSY_SNAPSHOT,
   cantopen_notempdir = SQLITE_CANTOPEN_NOTEMPDIR,
   cantopen_isdir = SQLITE_CANTOPEN_ISDIR,
   cantopen_fullpath = SQLITE_CANTOPEN_FULLPATH,
   cantopen_convpath = SQLITE_CANTOPEN_CONVPATH,
   cantopen_dirtywal = SQLITE_CANTOPEN_DIRTYWAL,
   corrupt_vtab = SQLITE_CORRUPT_VTAB,
   corrupt_sequence = SQLITE_CORRUPT_SEQUENCE,
   readonly_recovery = SQLITE_READONLY_RECOVERY,
   readonly_cantlock = SQLITE_READONLY_CANTLOCK,
   readonly_rollback = SQLITE_READONLY_ROLLBACK,
   readonly_dbmoved = SQLITE_READONLY_DBMOVED,
   readonly_cantinit = SQLITE_READONLY_CANTINIT,
   readonly_directory = SQLITE_READONLY_DIRECTORY,
   abort_rollback = SQLITE_ABORT_ROLLBACK,
   constraint_check = SQLITE_CONSTRAINT_CHECK,
   constraint_commithook = SQLITE_CONSTRAINT_COMMITHOOK,
   constraint_foreignkey = SQLITE_CONSTRAINT_FOREIGNKEY,
   constraint_function = SQLITE_CONSTRAINT_FUNCTION,
   constraint_notnull = SQLITE_CONSTRAINT_NOTNULL,
   constraint_primarykey = SQLITE_CONSTRAINT_PRIMARYKEY,
   constraint_trigger = SQLITE_CONSTRAINT_TRIGGER,
   constraint_unique = SQLITE_CONSTRAINT_UNIQUE,
   constraint_vtab = SQLITE_CONSTRAINT_VTAB,
   constraint_rowid = SQLITE_CONSTRAINT_ROWID,
   notice_recover_wal = SQLITE_NOTICE_RECOVER_WAL,
   notice_recover_rollback = SQLITE_NOTICE_RECOVER_ROLLBACK,
   warning_autoindex = SQLITE_WARNING_AUTOINDEX,
   auth_user = SQLITE_AUTH_USER,
   ok_load_permanently = SQLITE_OK_LOAD_PERMANENTLY,
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

