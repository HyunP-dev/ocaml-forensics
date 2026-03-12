open Core
open Forensics
open Hex

type location =
  | Image of string
  | FileSystem of { start : int64 }
  | Filename of string
[@@deriving sexp]

module CommonAnalysis = struct
  let xxd file =
    file
    |> Tsk.fs_file_read ~offset:0L ~size:(-1L)
    |> Hex.of_bytes
    |> Hex.hexdump
  ;;
end

module RegistryAnalysis = struct
  let show path file = 
    let raw = Tsk.fs_file_read ~offset:0L ~size:(-1L) file in
    Out_channel.with_file "TEMP_REG.raw" ~binary:true ~f:(fun oc ->
      Out_channel.output_bytes oc raw
    );
    (* let%bind.Result registry = Registry.open_file "TEMP_REG.raw" in
    
    let name = Registry.libregf_key_get_utf8_name key in
    print_endline "Key name:";
    (match name with
    | Ok(n) -> print_endline n;
    | Error(msg) -> print_endline msg);
    name
    *)

    let%bind.Result registry = Registry.open_file "TEMP_REG.raw" in
    let%bind.Result key = Registry.libregf_file_get_key_by_utf8_path registry path in
    let%bind.Result name = Registry.libregf_key_get_utf8_name key in
    print_endline name;
    Ok(())
end

let () =
  analyse
    (image "/mnt/e/Hunter XP.E01")
    (FileSystem { start = 63L })
    (Filename "/Windows/System32/Config/SOFTWARE")
    (* (CommonAnalysis.xxd) *)
    (fun file -> RegistryAnalysis.show "\\Microsoft\\Windows NT\\CurrentVersion" file |> ignore)
;;

(* let locations =
    [ Image "image.E01"
    ; FileSystem { start = 256L }
    ; Filename "/System Volume Information/WPSettings.dat"
    ]
  in
  let sexp = sexp_of_list sexp_of_location locations in
  print_endline (Sexp.to_string_hum sexp) *)
