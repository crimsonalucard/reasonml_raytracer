include PpmGenerator;

print_string(
  PpmGenerator.generate_dynamic_ppm(
    200,
    100,
    255,
    PpmGenerator.coord_to_rainbow(200, 100, 255),
  ),
);