open Core

type tsk_img_info
type tsk_fs_info
type tsk_fs_file

external img_open
  :  string
  -> (tsk_img_info, string) result
  = "caml_tsk_img_open"

external fs_open_img
  :  tsk_img_info
  -> offset:int64
  -> (tsk_fs_info, string) result
  = "caml_tsk_fs_open_img"

external fs_file_open
  :  tsk_fs_info
  -> string
  -> (tsk_fs_file, string) result
  = "caml_tsk_fs_file_open"

external fs_file_read
  :  offset:int64
  -> size:int64
  -> tsk_fs_file
  -> bytes
  = "caml_tsk_fs_file_read"

type partition =
  { index : int64
  ; start : int64
  ; length : int64
  ; description : string
  }
[@@deriving sexp]

external get_partitions
  :  tsk_img_info
  -> (partition array, string) result
  = "caml_get_partitions"
