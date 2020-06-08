include Shapes;

let width = 200;
let height = 100;
let samples_per_pixel = 100;
let max_color = 255;
let origin = (0.0, 0.0, 0.0);
let upper_left = ((-2.0), 1.0, (-1.0));

let print_percentage = (x: int, y: int, width: int, height: int) => {
  let percent_number =
    int_of_float(
      float_of_int(x + width * (y - 1))
      *. 100.0
      /. float_of_int(width * height),
    );

  let percent_string = string_of_int(percent_number) ++ "%";
  print_string(percent_string);
};

let coord_to_sky_color =
    (
      ~width=width,
      ~height=height,
      ~origin=origin,
      ~upper_left=upper_left,
      coord: (int, int),
    )
    : rgb =>
  switch (coord) {
  | (x, y) =>
    Ray.generate_camera_ray(x, y, width, height, origin, upper_left)
    |> sky_color_from_ray
  };

let main_sphere: sphere = {
  center: (0.0, 0.0, 0.0 -. 1.0),
  radius: 0.5,
  color: (255, 0, 0),
};

let ground: sphere = {
  center: (0.0, 0.0 -. 100.5, 0.0 -. 1.0),
  radius: 100.0,
  color: (0, 255, 0),
};

let shape_list = [Sphere(ground), Sphere(main_sphere)];

let ray_to_normal_color = (shapes: list(shape), a_ray: ray) => {
  //       Js.Console.log(main_ray);
  switch (resolve_hit_info_from_list(shapes, a_ray)) {
  | None => a_ray |> sky_color_from_ray
  | Just(hit_result) => hit_result.normal |> normal_to_color
  };
};

let aliased_ray_to_color =
    (
      shapes: list(shape),
      color_function: (list(shape), ray) => rgb,
      rays: aliased_ray,
    )
    : rgb => {
  rays |> List.rev_map(color_function(shapes)) |> average_colors;
};

let rec ray_to_random_bounce_color =
        (~depth: int=0, shapes: list(shape), main_ray: ray): rgb =>
  if (depth == (-1)) {
    (0, 0, 0);
  } else {
    switch (main_ray |> resolve_hit_info_from_list(shapes)) {
    | None =>
      let (r, g, b) = main_ray |> sky_color_from_ray;
      let multiplier = 0.5 ** float_of_int(depth);
      (
        int_of_float(float_of_int(r) *. multiplier),
        int_of_float(float_of_int(g) *. multiplier),
        int_of_float(float_of_int(b) *. multiplier),
      );
    | Just(hit_result) =>
      let random_vector = get_random_unit_sphere_vector();
      let target = add(hit_result.normal, random_vector);
      let bounce_ray: ray = {origin: hit_result.point, direction: target};
      ray_to_random_bounce_color(~depth=depth + 1, shapes, bounce_ray);
    };
  };

let coord_to_sky_color_or_sphere =
    (
      ~width=width,
      ~height=height,
      ~origin=origin,
      ~upper_left=upper_left,
      shapes: list(shape),
      coord: (int, int),
    )
    : rgb => {
  let (x, y) = coord;
  if (x == width - 1) {
    print_percentage(x, y, width, height);
  } else {
    ();
  };
  let main_ray = generate_camera_ray(x, y, width, height, origin, upper_left);
  let aliased_rays =
    main_ray |> get_aliased_rays(~samples=samples_per_pixel, width, height);
  aliased_rays |> aliased_ray_to_color(shapes, ray_to_random_bounce_color);
};

let coords = Utils.ordered_combinations(range(width), range(height));

let ppm_string =
  PpmGenerator.generate_dynamic_ppm(width, height, max_color, (x, y) =>
    coord_to_sky_color_or_sphere(
      ~width,
      ~height,
      ~origin,
      ~upper_left,
      shape_list,
      (x, y),
    )
  );

print_string(ppm_string);