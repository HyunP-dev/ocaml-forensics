open Core
open Forensics
open Hex

type location =
  | Image of string
  | FileSystem of { start : int64 }
  | Filename of string
[@@deriving sexp]

let () =
  analyse
    (image "/Users/rpark/Downloads/Hunter\ XP.E01")
    (FileSystem { start = 63L })
    (Filename "/Windows/System32/Config/SOFTWARE")
    (fun file ->
       file
       |> Tsk.fs_file_read ~offset:0L ~size:256L
       |> Hex.of_bytes
       |> Hex.hexdump);
  (* let locations =
    [ Image "image.E01"
    ; FileSystem { start = 256L }
    ; Filename "/System Volume Information/WPSettings.dat"
    ]
  in
  let sexp = sexp_of_list sexp_of_location locations in
  print_endline (Sexp.to_string_hum sexp) *)
;;
