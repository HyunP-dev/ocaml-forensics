type libregf_file_t

external libregf_file_initialize
  :  unit
  -> (libregf_file_t, string) result
  = "caml_libregf_file_initialize"

external libregf_file_open
  :  libregf_file_t
  -> string
  -> (libregf_file_t, string) result
  = "caml_libregf_file_open"
