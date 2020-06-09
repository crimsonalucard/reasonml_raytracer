include PpmGenerator;

type hit_result = {
  point: vec3,
  t: float,
  normal: vec3,
};

module type Hittable = {
  type t;
  let get_hit_info: (t, float, ray) => maybe(hit_result);
};

type sphere = {
  center: vec3,
  radius: float,
  color: rgb,
};

type shape =
  | Sphere(sphere);

module SphereHittable: Hittable with type t = sphere = {
  type t = sphere;
  let get_hit_info =
      (input_sphere: sphere, t_min: float, input_ray: ray): maybe(hit_result) => {
    let a = dot(input_ray.direction, input_ray.direction);
    let origin_minus_center = sub(input_ray.origin, input_sphere.center);
    let b = 2.0 *. dot(input_ray.direction, origin_minus_center);
    let c =
      dot(origin_minus_center, origin_minus_center)
      -. input_sphere.radius
      *. input_sphere.radius;
    let b_squared_minus_four_ac = b *. b -. 4.0 *. a *. c;
    if (b_squared_minus_four_ac < 0.0) {
      None;
    } else {
      let t1 =
        (0.0 -. b -. Js.Math.sqrt(b_squared_minus_four_ac)) /. (2.0 *. a);
      let t2 =
        (0.0 -. b +. Js.Math.sqrt(b_squared_minus_four_ac)) /. (2.0 *. a);
      if (t1 < t_min && t2 < t_min) {
        None;
      } else if (t1 < t_min) {
        let point = get_position(input_ray, t2);
        Just({point, t: t2, normal: unit(sub(point, input_sphere.center))});
      } else {
        let point = get_position(input_ray, t1);
        Just({point, t: t1, normal: unit(sub(point, input_sphere.center))});
      };
    };
  };
};

let get_sphere_color_normal_from_point =
    (input_sphere: sphere, surface_point: vec3): vec3 => {
  let (x, y, z) = unit(sub(surface_point, input_sphere.center));
  scalar_mult((x +. 1.0, y +. 1.0, z +. 1.0), 0.5);
};

let normal_to_color = (~max_color=255, normal: vec3): rgb => {
  let (x, y, z) = scalar_mult(add(unit(normal), (1.0, 1.0, 1.0)), 0.5);
  let max_color_float = float_of_int(max_color);
  (
    int_of_float(max_color_float *. x),
    int_of_float(max_color_float *. y),
    int_of_float(max_color_float *. z),
  );
};

let sphere_normal_color_from_ray =
    (input_sphere: sphere, input_ray: ray): maybe(rgb) => {
  switch (SphereHittable.get_hit_info(input_sphere, 0.0, input_ray)) {
  | None => None
  | Just(hit_info) =>
    Just(
      hit_info.point
      |> get_sphere_color_normal_from_point(input_sphere)
      |> normal_to_color,
    )
  };
};

let rec resolve_hit_info_from_list =
        (
          ~acc: maybe(hit_result)=None,
          ~t_min: float=0.001,
          hittables: list(shape),
          input_ray: ray,
        )
        : maybe(hit_result) =>
  if (List.length(hittables) == 0) {
    acc;
  } else {
    let a_shape: shape = List.hd(hittables);
    let new_hit_info =
      switch (a_shape) {
      | Sphere(x) => SphereHittable.get_hit_info(x, t_min, input_ray)
      };
    let new_acc =
      switch (acc, new_hit_info) {
      | (None, maybe_new_hit_info) => maybe_new_hit_info
      | (maybe_old_hit_info, None) => maybe_old_hit_info
      | (Just(old_hit_info), Just(new_hit_info)) =>
        old_hit_info.t < new_hit_info.t
          ? Just(old_hit_info) : Just(new_hit_info)
      };
    resolve_hit_info_from_list(~acc=new_acc, List.tl(hittables), input_ray);
  };