type vec3 = (float, float, float);

let add = (x: vec3, y: vec3): vec3 =>
  switch (x, y) {
  | ((a, b, c), (d, e, f)) => (a +. d, b +. e, c +. f)
  };

let negate = (x: vec3): vec3 =>
  switch (x) {
  | (a, b, c) => (0.0 -. a, 0.0 -. b, 0.0 -. c)
  };

let sub = (x: vec3, y: vec3): vec3 => add(x, negate(y));

let scalar_div = (x: vec3, y: float): vec3 =>
  switch (x) {
  | (a, b, c) => (a /. y, b /. y, c /. y)
  };

let scalar_mult = (x: vec3, y: float): vec3 =>
  switch (x) {
  | (a, b, c) => (a *. y, b *. y, c *. y)
  };

let get_length_squared = (x: vec3): float =>
  switch (x) {
  | (a, b, c) => a *. a +. b *. b +. c *. c
  };

let get_length = (x: vec3): float => Js.Math.sqrt(get_length_squared(x));

let unit = (x: vec3): vec3 => scalar_div(x, get_length(x));

let dot = (x: vec3, y: vec3): float =>
  switch (x, y) {
  | ((a, b, c), (d, e, f)) => a *. d +. b *. e +. c *. f
  };

let cross = (x: vec3, y: vec3): vec3 =>
  switch (x, y) {
  | ((a, b, c), (d, e, f)) => (
      a *. e -. a *. f,
      b *. f -. b *. d,
      c *. d +. c *. f,
    )
  };

let lerp_vector = (start: vec3, _end: vec3, value: float): vec3 =>
  add(scalar_mult(start, 1.0 -. value), scalar_mult(_end, value));

let rec get_random_unit_sphere_vector = (): vec3 => {
  let result =
    sub(
      scalar_mult(
        (Random.float(1.0), Random.float(1.0), Random.float(1.0)),
        2.0,
      ),
      (1.0, 1.0, 1.0),
    );
  if (get_length_squared(result) >= 1.0) {
    get_random_unit_sphere_vector();
  } else {
    result;
  };
};