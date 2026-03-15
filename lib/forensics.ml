module Tsk = Tsk
module Ewf = Ewf
module Registry = Registry

let image path =
  let image_handle_result = Tsk.img_open path in
  match image_handle_result with
  | Ok image_info -> image_info
  | Error msg ->
    print_endline msg;
    assert false
;;

type analysis_argument =
  | FileSystem of { start : int64 }
  | Filename of string

let analyse image fs path f =
  match fs with
  | FileSystem { start } ->
    let fs_info_result = Tsk.fs_open_img image ~offset:start in
    (match fs_info_result with
     | Ok fs_info ->
       (match path with
        | Filename p ->
          let file_result = Tsk.fs_file_open fs_info p in
          (match file_result with
           | Ok file -> f file
           | Error msg ->
             print_endline msg;
             assert false)
        | _ -> assert false)
     | Error msg ->
       print_endline msg;
       assert false)
  | _ -> assert false
;;
