open Forensics
open Hex

let where = Tsk.fs_file_open
let from = Tsk.fs_file_read

let () =
  let path = "image.E01" in
  let img_info = Tsk.img_open path in
  let fs = Tsk.fs_open_img img_info ~offset:217014272L in
  let filepath = "/System Volume Information/WPSettings.dat" in
  filepath
  |> where fs
  |> from ~offset:0L ~size:256L
  |> Hex.of_bytes
  |> Hex.hexdump
;;
