open Core
open Forensics
open Hex

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
      Out_channel.output_bytes oc raw);
    let%bind.Result registry = Registry.open_file "TEMP_REG.raw" in
    let%bind.Result key =
      Registry.libregf_file_get_key_by_utf8_path registry path
    in
    let%bind.Result name = Registry.libregf_key_get_utf8_name key in
    let%bind.Result subkey_names = Registry.get_subkey_names key in
    key
    |> Registry.get_value_names
    |> Result.iter
         ~f:
           (Array.iter ~f:(fun name ->
              print_endline name;
              Registry.get_value_by_name key ~name
              |> Result.iter ~f:(fun raw -> Hex.hexdump (Hex.of_bytes raw))))
    |> ignore;
    Ok ()
  ;;
end

let () =
  analyse
    (image "/mnt/d/Personal/Hunter XP.E01")
    (FileSystem { start = 63L })
    (Filename "/Windows/System32/Config/SOFTWARE")
    (RegistryAnalysis.show "\\Microsoft\\Windows NT\\CurrentVersion")
  |> ignore
;;
