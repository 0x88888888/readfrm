#include "defs.h"

unsigned int innodb_slot = 12;


char* legacy_db_types[128]= {
  "DB_TYPE_UNKNOWN",
  "DB_TYPE_DIAB_ISAM",
  "DB_TYPE_HASH",
  "DB_TYPE_MISAM",
  "DB_TYPE_PISAM",
  "DB_TYPE_RMS_ISAM", 
  "DB_TYPE_HEAP", 
  "DB_TYPE_ISAM",
  "DB_TYPE_MRG_ISAM", 
  "DB_TYPE_MYISAM", 
  "DB_TYPE_MRG_MYISAM",
  "DB_TYPE_BERKELEY_DB", 
  "DB_TYPE_INNODB",
  "DB_TYPE_GEMINI", 
  "DB_TYPE_NDBCLUSTER",
  "DB_TYPE_EXAMPLE_DB", 
  "DB_TYPE_ARCHIVE_DB", 
  "DB_TYPE_CSV_DB",
  "DB_TYPE_FEDERATED_DB",
  "DB_TYPE_BLACKHOLE_DB",
  "DB_TYPE_PARTITION_DB",
  "DB_TYPE_BINLOG",
  "DB_TYPE_SOLID",
  "DB_TYPE_PBXT",
  "DB_TYPE_TABLE_FUNCTION",
  "DB_TYPE_MEMCACHE",
  "DB_TYPE_FALCON",
  "DB_TYPE_MARIA",
  /** Performance schema engine. */
  "DB_TYPE_PERFORMANCE_SCHEMA",
  "DB_TYPE_FIRST_DYNAMIC=42",
  "DB_TYPE_DEFAULT=127", // Must be last
  NULL
};


char* row_type[8] = {   "ROW_TYPE_NOT_USED", /*-1*/
                         "ROW_TYPE_DEFAULT", /*0*/
                         "ROW_TYPE_FIXED", /*1*/
                         "ROW_TYPE_DYNAMIC",/*2*/
                         "ROW_TYPE_COMPRESSED", /*3*/
                         "ROW_TYPE_REDUNDANT", /*4*/
                         "ROW_TYPE_COMPACT", /*5*/
                /** Unused. Reserved for future versions. */
                         "ROW_TYPE_PAGE", /*6*/
                          };



