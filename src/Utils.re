let rec range = (~start: int=0, _end: int): list(int) =>
  start >= _end ? [] : [start, ...range(~start=start + 1, _end)];

//tail recursive map
let map = (func, iterable) => iterable |> List.rev_map(func) |> List.rev;

//cannot use List.map in the definition, might blow up the stack
//less efficient to use this but no need to manage reversals.
let rec ordered_combinations = (a: list('a), b: list('b)): list(('a, 'b)) =>
  switch (b) {
  | [] => []
  | [y, ...ys] =>
    List.append(
      a |> map((a_value: 'a) => (a_value, y)),
      ordered_combinations(a, ys),
    )
  };

let generate_count_matrix = (width: int, height: int) => {
  range(height)
  |> List.map(height_index => {
       range(width) |> List.map(width_index => {(width_index, height_index)})
     });
};

type maybe('a) =
  | None
  | Just('a);