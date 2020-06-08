include Utils;
include Ray;

type rgb = (int, int, int);

let color_to_string = (x: rgb): string =>
  switch (x) {
  | (r, g, b) =>
    string_of_int(r) ++ " " ++ string_of_int(g) ++ " " ++ string_of_int(b)
  };

let generate_color_string = (rgb_list: list(rgb)): string =>
  rgb_list
  |> List.rev_map(color_to_string)
  |> List.rev
  |> Array.of_list
  |> Js.Array.joinWith("\n");

let generate_ppm_from_color_string =
    (max_color: int, width: int, height: int, color_string: string): string => {j|P3
$width $height
$max_color
$color_string\n|j};

let generate_ppm =
    (width: int, height: int, max_color: int, rgb_list: list(rgb)): string =>
  generate_color_string(rgb_list)
  |> generate_ppm_from_color_string(max_color, width, height);

let generate_dynamic_ppm =
    (
      width: int,
      height: int,
      max_color: int,
      coord_to_color: (int, int) => rgb,
    )
    : string => {
  let width_numbers = range(width);
  let height_numbers = range(height);
  Utils.ordered_combinations(width_numbers, height_numbers)
  |> Utils.map(coord =>
       switch (coord) {
       | (x, y) => coord_to_color(x, y)
       }
     )
  |> generate_ppm(width, height, max_color);
};

let coord_to_rainbow =
    (width: int, height: int, max_color: int, x: int, y: int) => {
  let number_to_color_by_width = num =>
    int_of_float(
      float_of_int(num) /. float_of_int(width) *. float_of_int(max_color),
    );
  let number_to_color_by_height = num =>
    int_of_float(
      float_of_int(num) /. float_of_int(height) *. float_of_int(max_color),
    );
  (
    number_to_color_by_width(x),
    number_to_color_by_height(y),
    int_of_float(0.2 *. float_of_int(max_color)),
  );
};

let color_to_vec3 = (x: rgb): vec3 =>
  switch (x) {
  | (r, g, b) => (float_of_int(r), float_of_int(g), float_of_int(b))
  };

let vec3_to_color = (x: vec3): rgb =>
  switch (x) {
  | (a, b, c) => (int_of_float(a), int_of_float(b), int_of_float(c))
  };

let lerp_rgb = (start: rgb, _end: rgb, value: float): rgb => {
  let start = color_to_vec3(start);
  let _end = color_to_vec3(_end);
  vec3_to_color(Vec3.lerp_vector(start, _end, value));
};

let sky_color_from_ray = (x: ray): rgb => {
  let unit_direction = Vec3.unit(x.direction);
  let verticality =
    switch (unit_direction) {
    | (_, y, _) => 0.5 *. (y +. 1.0)
    };
  let white = (255, 255, 255);
  let blue = vec3_to_color((0.5 *. 255.0, 0.7 *. 255.0, 1.0 *. 255.0));
  lerp_rgb(white, blue, verticality);
};

let rec average_colors =
        (~acc: vec3=(0.0, 0.0, 0.0), ~n: int=0, colors: list(rgb)): rgb =>
  // Js.Console.log((n, acc, List.length(colors)));
  if (List.length(colors) == 0) {
    let (x, y, z) = acc;
    let divisor = float_of_int(n);
    if (n != 0) {
      let result = (
        int_of_float(x /. divisor),
        int_of_float(y /. divisor),
        int_of_float(z /. divisor),
      );
      // Js.Console.log("success!!!");
      // Js.Console.log(result);
      result;
    } else {
      Js.Console.log("ERROR!");
      (int_of_float(x), int_of_float(y), int_of_float(z));
    };
  } else {
    let (x, y, z) = List.hd(colors);
    let new_vec =
      add(acc, (float_of_int(x), float_of_int(y), float_of_int(z)));
    average_colors(~acc=new_vec, ~n=n + 1, List.tl(colors));
  };