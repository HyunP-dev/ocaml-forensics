open Core

type libregf_file_t
type libregf_key_t

external libregf_file_initialize
  :  unit
  -> (libregf_file_t, string) result
  = "caml_libregf_file_initialize"

external libregf_file_open
  :  libregf_file_t
  -> string
  -> (libregf_file_t, string) result
  = "caml_libregf_file_open"

external libregf_file_get_key_by_utf8_path
  :  libregf_file_t
  -> string
  -> (libregf_key_t, string) result
  = "caml_libregf_file_get_key_by_utf8_path"

external libregf_key_get_utf8_name
  :  libregf_key_t
  -> (string, string) result
  = "caml_libregf_key_get_utf8_name"

let open_file path =
  let%bind.Result handle = libregf_file_initialize () in
  libregf_file_open handle path
;;
