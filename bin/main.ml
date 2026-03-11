open Forensics
open Hex

let () =
  analyse
    (image "image.E01")
    (FileSystem { start = 217014272L })
    (Filename "/System Volume Information/WPSettings.dat")
    (fun file ->
       file
       |> Tsk.fs_file_read ~offset:0L ~size:256L
       |> Hex.of_bytes
       |> Hex.hexdump)
;;
