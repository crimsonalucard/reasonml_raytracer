include Vec3;

//A + Bt
//A is origin, B is direction
type ray = {
  origin: vec3,
  direction: vec3,
};

let get_position = (x: ray, t: float): vec3 =>
  add(x.origin, scalar_mult(x.direction, t));

let generate_camera_ray =
    (
      x: int,
      y: int,
      width: int,
      height: int,
      origin: Vec3.vec3,
      start_vec: Vec3.vec3,
    )
    : ray => {
  let right_transform: vec3 =
    switch (start_vec) {
    | (x, _, _) => (Js.Math.abs_float(x) *. 2.0, 0.0, 0.0)
    };
  let down_transform: vec3 =
    switch (start_vec) {
    | (_, y, _) => (0.0, 0.0 -. y *. 2.0, 0.0)
    };
  let horizontal_increment = float_of_int(x) /. float_of_int(width);
  let vertical_increment = float_of_int(y) /. float_of_int(height);
  let new_vec =
    Vec3.scalar_mult(right_transform, horizontal_increment)
    |> Vec3.add(start_vec)
    |> Vec3.add(Vec3.scalar_mult(down_transform, vertical_increment));
  {origin, direction: new_vec};
};
type aliased_ray = list(ray);
Random.init(int_of_float(Js.Date.now()));

let rec get_aliased_rays =
        (
          ~acc: list(ray)=[],
          ~samples: int=200,
          width: int,
          height: int,
          original_ray: ray,
        )
        : aliased_ray =>
  if (samples == 0) {
    acc;
  } else {
    Random.init(int_of_float(Js.Date.now()));
    let randx = Random.float(1.0) *. (1.0 /. float_of_int(width));
    let randy = Random.float(1.0) *. (1.0 /. float_of_int(height));
    let (x, y, z) = original_ray.direction;
    let new_ray = {
      origin: original_ray.origin,
      direction: (x +. randx, y +. randy, z),
    };
    get_aliased_rays(
      ~acc=[new_ray, ...acc],
      ~samples=samples - 1,
      width,
      height,
      original_ray,
    );
  };

let rec generate_rays_for_camera =
        (
          ~tail: list(ray)=[],
          ~x: int=0,
          ~y: int=0,
          width: int,
          height: int,
          origin: vec3,
          start_vec: vec3,
        )
        : list(ray) => {
  let new_ray = generate_camera_ray(x, y, width, height, origin, start_vec);
  if (x == width && y == height) {
    List.rev([new_ray, ...tail]);
  } else if (x == width) {
    generate_rays_for_camera(
      ~tail=[new_ray, ...tail],
      ~x=0,
      ~y=y + 1,
      width,
      height,
      origin,
      start_vec,
    );
  } else {
    generate_rays_for_camera(
      ~tail=[new_ray, ...tail],
      ~x=x + 1,
      ~y,
      width,
      height,
      origin,
      start_vec,
    );
  };
};